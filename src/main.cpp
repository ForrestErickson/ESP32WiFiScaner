#include <Arduino.h>
#include "WiFi.h"
#include "Splash.h"
#include "MyNetUtils.h"
#include "ConfigManager.h"

// --- SEMANTIC VERSIONING ---
#define VERSION_MAJOR 0
#define VERSION_MINOR 0
#define VERSION_PATCH 8 // Added ElegantOTA Over-The-Air Update Engine



#define LED_BUILTIN 2
#define SCAN_BUTTON GPIO_NUM_35 

// Variables for the non-blocking LED timer
unsigned long lastLedUpdate = 0;
bool ledState = false;
unsigned long ledOnTime = 100;   
unsigned long ledOffTime = 900;  

// Volatile variables for the button interrupt and debounce
volatile bool buttonPressed = false;
volatile unsigned long lastInterruptTime = 0; 

// State flag to safely schedule a factory reset from outside the ISR thread
bool processFactoryReset = false;

// Runtime variable holding loaded flash credentials in RAM
WifiConfig currentCredentials;

// ISR Declaration
void IRAM_ATTR handleButtonPress();

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  
  Serial.begin(115200);
  while (!Serial) { ; }

  // Prints updated 0.0.5 modular boot metadata
  printSplash(VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);

  // Initialize LittleFS storage partition
  if (initStorage()) {
    loadConfig(currentCredentials);
  }

  // Set up physical hardware override button on GPIO 35
  pinMode(SCAN_BUTTON, INPUT_PULLUP); 
  attachInterrupt(digitalPinToInterrupt(SCAN_BUTTON), handleButtonPress, FALLING);

  // Route state machine to either connect to Wi-Fi or spin up the SoftAP hotspot
  initNetwork(currentCredentials);
}

void loop() {
  // --- NON-BLOCKING DYNAMIC LED CADENCE SELECTOR ---
  unsigned long currentMillis = millis();
  
  if (currentNetState == STATE_CONNECTED) {
    ledOnTime = 1000; ledOffTime = 0;   // Solid ON: Connected to local internet router
  } else if (currentNetState == STATE_CONNECTING) {
    ledOnTime = 100;  ledOffTime = 100; // Fast Flash: Searching/authenticating with AP
  } else if (currentNetState == STATE_AP_MODE) {
    ledOnTime = 500;  ledOffTime = 500; // Steady Breathing: Hosting hotspot web page
  }

  // Non-blocking blink actuator engine
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

  // --- BACKGROUND STATE TRACKING POLLER ---
  checkNetworkStatus();

  // --- SAFE INTERRUPT TRIGGER CHECKER ---
  if (buttonPressed) {
    buttonPressed = false;
    processFactoryReset = true; // Safely defer execution out of execution bottlenecks
  }

  // --- SAFE STORAGE RESET WORKFLOW ---
  if (processFactoryReset) {
    processFactoryReset = false;
    delay(10); // Clear remaining TX buffers completely

    Serial.println("\nButton override triggered! Erasing flash configuration and restarting chip...");
    clearConfig();
    delay(500);    // Give the user console time to finish rendering the output string
    ESP.restart(); // Software resets the CPU core down to base boot instructions
  }
}

// Interrupt Service Routine for the button
void IRAM_ATTR handleButtonPress() {
  unsigned long interruptTime = millis();
  if (interruptTime - lastInterruptTime > 250) {
    buttonPressed = true;
  }
  lastInterruptTime = interruptTime;
}
