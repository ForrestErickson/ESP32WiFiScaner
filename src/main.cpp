#include <Arduino.h>
#include "WiFi.h"

#define LED_BUILTIN 2
#define SCAN_BUTTON GPIO_NUM_35 

// Variables for the non-blocking LED timer
unsigned long lastLedUpdate = 0;
bool ledState = false;
unsigned long ledOnTime = 100;   
unsigned long ledOffTime = 900;  

// Asynchronous scan state tracking
bool isScanning = false;
unsigned long scanStartTime = 0; // Tracks exactly when the button kicked off the scan

// Volatile flag for the button interrupt
volatile bool buttonPressed = false;

// Function declarations
void startScan();
void checkScanStatus();
void IRAM_ATTR handleButtonPress();

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  
  Serial.begin(115200);
  while (!Serial) { ; }

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
    startScan(); 
  } else if (buttonPressed && isScanning) {
    buttonPressed = false; 
  }

  // --- NON-BLOCKING STATUS MONITOR ---
  if (isScanning) {
    checkScanStatus();
  }
}

// --- START THE ASYNCHRONOUS SCAN ---
void startScan() {
  Serial.println("Button pressed! Starting async scan...");
  
  ledOnTime = 50;
  ledOffTime = 450;
  isScanning = true;
  scanStartTime = millis(); 

  // Initiate background scan
  WiFi.scanNetworks(true); 
}

// --- CHECK BACKGROUND SCAN PROGRESS ---
void checkScanStatus() {
  int n = WiFi.scanComplete(); 
  
  // Case 1: Scan successfully found 0 or more networks!
  if (n >= 0) { 
    Serial.print(n);
    Serial.println(" networks found:");
    for (int i = 0; i < n; ++i) {
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.println(")");
    }
    Serial.println();

    WiFi.scanDelete(); 
    
    // Reset back to standard slow-blink settings
    ledOnTime = 100;
    ledOffTime = 900;
    isScanning = false;
    buttonPressed = false; 
  } 
  // Case 2: Scan is returning an ongoing status code (-1 or transient -2)
  else {
    // A scan shouldn't take longer than 10 seconds. 
    // If it exceeds this, the hardware radio has encountered a genuine error.
    if (millis() - scanStartTime > 10000) {
      Serial.println("Actual Scan Timeout/Failure! Resetting interface.");
      WiFi.scanDelete();
      
      // Re-initialize the WiFi stack to clear any deep hardware hang-ups
      WiFi.disconnect();
      
      ledOnTime = 100;
      ledOffTime = 900;
      isScanning = false;
    }
  }
}

// Interrupt Service Routine for the button
void IRAM_ATTR handleButtonPress() {
  buttonPressed = true;
}
