#include <Arduino.h>
#include "WiFi.h"

#define LED_BUILTIN 2
#define SCAN_BUTTON GPIO_NUM_35

// Variables for the non-blocking LED timer
unsigned long lastLedUpdate = 0;
bool ledState = false;

// Volatile flag for the button interrupt
volatile bool buttonPressed = false;

// Interrupt Service Routine for the button
void IRAM_ATTR handleButtonPress() {
  buttonPressed = true;
}

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
  // --- NON-BLOCKING LED BLINK ROUTINE ---
  unsigned long currentMillis = millis();
  
  if (ledState == true) {
    // If LED is ON, wait 100ms to turn it OFF
    if (currentMillis - lastLedUpdate >= 100) {
      ledState = false;
      digitalWrite(LED_BUILTIN, LOW);
      lastLedUpdate = currentMillis;
    }
  } else {
    // If LED is OFF, wait 900ms to turn it ON
    if (currentMillis - lastLedUpdate >= 900) {
      ledState = true;
      digitalWrite(LED_BUILTIN, HIGH);
      lastLedUpdate = currentMillis;
    }
  }

  // --- BUTTON & WI-FI SCAN ROUTINE ---
  if (buttonPressed) {
    buttonPressed = false; 

    Serial.println("Button pressed! Starting scan...");
    
    int n = WiFi.scanNetworks();
    if (n == 0) {
      Serial.println("No networks found.");
    } else {
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
    }
    Serial.println();
    
    // Cooldown to avoid mechanical button bounce
    delay(500); 
    buttonPressed = false;
    
    // Reset LED timer after the blocking Wi-Fi scan so the rhythm stays clean
    lastLedUpdate = millis(); 
  }
}
