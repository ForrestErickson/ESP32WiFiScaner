#include <Arduino.h>
#include "WiFi.h"
#include "Splash.h"
#include "MyNetUtils.h"

// --- SEMANTIC VERSIONING ---
#define VERSION_MAJOR 0
#define VERSION_MINOR 0
#define VERSION_PATCH 3 // Refactored to modular architecture

#define LED_BUILTIN 2
#define SCAN_BUTTON GPIO_NUM_35 

// Variables for the non-blocking LED timer
unsigned long lastLedUpdate = 0;
bool ledState = false;
unsigned long ledOnTime = 100;   
unsigned long ledOffTime = 900;  

// Asynchronous scan state tracking
bool isScanning = false;
unsigned long scanStartTime = 0; 

// Volatile variables for the button interrupt and debounce
volatile bool buttonPressed = false;
volatile unsigned long lastInterruptTime = 0; 

// ISR Declaration
void IRAM_ATTR handleButtonPress();

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  
  Serial.begin(115200);
  while (!Serial) { ; }

  // Modular Splash Screen call
  printSplash(VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);

  pinMode(SCAN_BUTTON, INPUT_PULLUP); 
  attachInterrupt(digitalPinToInterrupt(SCAN_BUTTON), handleButtonPress, FALLING);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  
  Serial.println("Setup done. Ready.");
}

void loop() {
  // --- NON-BLOCKING DYNAMIC LED BLINK ROUTINE ---
  unsigned long currentMillis = millis();
  
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

  // --- BUTTON CHECK ROUTINE ---
  if (buttonPressed && !isScanning) {
    buttonPressed = false; 
    startScan(scanStartTime, isScanning, ledOnTime, ledOffTime); 
  } else if (buttonPressed && isScanning) {
    buttonPressed = false; 
  }

  // --- NON-BLOCKING STATUS MONITOR ---
  if (isScanning) {
    checkScanStatus(scanStartTime, isScanning, ledOnTime, ledOffTime, buttonPressed);
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
