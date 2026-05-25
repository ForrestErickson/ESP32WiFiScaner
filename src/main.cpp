#include <Arduino.h>
#include "WiFi.h"
#include "Splash.h"
#include "MyNetUtils.h"
#include "ConfigManager.h"
#include "DisplayManager.h" 

// --- SEMANTIC VERSIONING ---
#define VERSION_MAJOR 0
#define VERSION_MINOR 0
#define VERSION_PATCH 14 // Multi-profile sequential retry before SoftAP fallback

#define LED_BUILTIN 2
#define SCAN_BUTTON GPIO_NUM_35 

unsigned long lastLedUpdate = 0;
bool ledState = false;
unsigned long ledOnTime = 100;   
unsigned long ledOffTime = 900;  

volatile bool buttonPressed = false;
volatile unsigned long lastInterruptTime = 0; 
bool processFactoryReset = false;

WifiConfig currentCredentials;

void IRAM_ATTR handleButtonPress();

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  
  Serial.begin(115200);
  while (!Serial) { ; }

  printSplash(VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);

  if (initDisplay()) {
    String verStr = String(VERSION_MAJOR) + "." + String(VERSION_MINOR) + "." + String(VERSION_PATCH);
    showBootSplash(verStr);
    delay(2000); 
  }

  if (initStorage()) {
    loadConfig(currentCredentials);
  }

  pinMode(SCAN_BUTTON, INPUT_PULLUP); 
  attachInterrupt(digitalPinToInterrupt(SCAN_BUTTON), handleButtonPress, FALLING);

  initNetwork(currentCredentials);
}

void loop() {
  unsigned long currentMillis = millis();
  
  if (currentNetState == STATE_CONNECTED) {
    ledOnTime = 100;  ledOffTime = 900; 
  } else if (currentNetState == STATE_CONNECTING) {
    ledOnTime = 100;  ledOffTime = 100; 
  } else if (currentNetState == STATE_AP_MODE) {
    ledOnTime = 500;  ledOffTime = 500; 
  }

  if (ledOnTime > 0 && ledOffTime > 0) {
    if (ledState == true) {
      if (currentMillis - lastLedUpdate >= ledOnTime) {
        ledState = false;
        digitalWrite(LED_BUILTIN, LOW);
        lastLedUpdate = currentMillis;
      }
    } else {
      if (currentMillis - lastLedUpdate >= ledOffTime) {
        ledState = true;
        digitalWrite(LED_BUILTIN, HIGH);
        lastLedUpdate = currentMillis;
      }
    }
  } else if (ledOnTime > 0 && ledOffTime == 0) {
    digitalWrite(LED_BUILTIN, HIGH);
  }

  checkNetworkStatus();

  if (buttonPressed) {
    buttonPressed = false;
    processFactoryReset = true; 
  }

  if (processFactoryReset) {
    processFactoryReset = false;
    delay(10); 

    drawNetworkStatus("FACTORY RESET", "Wiping...", "0.0.0.0", "Restarting...");
    
    Serial.println("\nButton override triggered! Erasing flash configuration and restarting chip...");
    clearConfig();
    delay(500);    
    ESP.restart(); 
  }
}

void IRAM_ATTR handleButtonPress() {
  unsigned long interruptTime = millis();
  if (interruptTime - lastInterruptTime > 250) {
    buttonPressed = true;
  }
  lastInterruptTime = interruptTime;
}
