#include <Arduino.h>
#include "WiFi.h"

#define LED_BUILTIN 2
// Changed from GPIO_NUM_0 to GPIO_NUM_35
#define SCAN_BUTTON GPIO_NUM_35 

// Variables for the non-blocking LED timer
unsigned long lastLedUpdate = 0;
bool ledState = false;

// Volatile flag for the button interrupt
volatile bool buttonPressed = false;

// Function declarations
void scanNetworks();
void IRAM_ATTR handleButtonPress();

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  
  Serial.begin(115200);
  while (!Serial) { ; }

  // Changed to use the new SCAN_BUTTON define
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
    if (currentMillis - lastLedUpdate >= 100) {
      ledState = false;
      digitalWrite(LED_BUILTIN, LOW);
      lastLedUpdate = currentMillis;
    }
  } else {
    if (currentMillis - lastLedUpdate >= 900) {
      ledState = true;
      digitalWrite(LED_BUILTIN, HIGH);
      lastLedUpdate = currentMillis;
    }
  }

  // --- BUTTON CHECK ROUTINE ---
  if (buttonPressed) {
    buttonPressed = false; 

    // Call the Wi-Fi scan function
    scanNetworks(); 
    
    // Cooldown to avoid mechanical button bounce
    delay(500); 
    buttonPressed = false;
    
    // Reset LED timer after the blocking Wi-Fi scan
    lastLedUpdate = millis(); 
  }
}

// --- WI-FI SCAN FUNCTION ---
void scanNetworks() {
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
}

// Interrupt Service Routine for the button
void IRAM_ATTR handleButtonPress() {
  buttonPressed = true;
}
