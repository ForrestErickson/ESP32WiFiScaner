#include <Arduino.h>
#include "WiFi.h"

#define FILE_NAME "ESP23WiFiScaner, "

// --- SEMANTIC VERSIONING ---
#define VERSION_MAJOR 0
#define VERSION_MINOR 0
#define VERSION_PATCH 2 // Bumped version for serial glitch fix

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

// Function declarations
void startScan();
void checkScanStatus();
String getEncryptionName(wifi_auth_mode_t authMode);
void IRAM_ATTR handleButtonPress();

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  
  Serial.begin(115200);
  while (!Serial) { ; }

  // --- SERIAL SPLASH MESSAGE ---
  Serial.print("==== ");
  Serial.print(FILE_NAME);
  Serial.print("Ver: "); 
  Serial.print(VERSION_MAJOR); Serial.print(".");
  Serial.print(VERSION_MINOR); Serial.print(".");
  Serial.print(VERSION_PATCH);
  Serial.println(" ====");
  Serial.print("Built: "); Serial.print(__DATE__);
  Serial.print(" "); Serial.print(__TIME__);
  Serial.println("");
  Serial.println("========================================");
  Serial.println("");

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
  
  // Safe 5ms delay allows the serial chip to finish transmitting 
  // the text block before the Wi-Fi radio task takes over resources.
  delay(5); 

  ledOnTime = 50;
  ledOffTime = 450;
  isScanning = true;
  scanStartTime = millis(); 

  WiFi.scanNetworks(true); 
}

// --- CHECK BACKGROUND SCAN PROGRESS ---
void checkScanStatus() {
  int n = WiFi.scanComplete(); 
  
  if (n >= 0) { 
    Serial.print(n);
    Serial.println(" networks found:");
    for (int i = 0; i < n; ++i) {
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" [Ch: ");
      Serial.print(WiFi.channel(i)); 
      Serial.print("] (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(" dBm) - ");
      Serial.println(getEncryptionName(WiFi.encryptionType(i))); 
    }
    Serial.println();

    WiFi.scanDelete(); 
    
    ledOnTime = 100;
    ledOffTime = 900;
    isScanning = false;
    buttonPressed = false; 
  } 
  else {
    if (millis() - scanStartTime > 10000) {
      Serial.println("Actual Scan Timeout/Failure! Resetting interface.");
      WiFi.scanDelete();
      WiFi.disconnect();
      ledOnTime = 100;
      ledOffTime = 900;
      isScanning = false;
    }
  }
}

// --- HELPER FUNCTION TO TRANSLATE ENCRYPTION TYPES ---
String getEncryptionName(wifi_auth_mode_t authMode) {
  switch (authMode) {
    case WIFI_AUTH_OPEN:            return "Open";
    case WIFI_AUTH_WEP:             return "WEP";
    case WIFI_AUTH_WPA_PSK:         return "WPA";
    case WIFI_AUTH_WPA2_PSK:        return "WPA2";
    case WIFI_AUTH_WPA_WPA2_PSK:    return "WPA/WPA2";
    case WIFI_AUTH_WPA2_ENTERPRISE: return "WPA2-Enterprise";
    case WIFI_AUTH_WPA3_PSK:        return "WPA3";
    case WIFI_AUTH_WPA2_WPA3_PSK:   return "WPA2/WPA3";
    default:                        return "Unknown";
  }
}

// --- DEBOUNCED INTERRUPT SERVICE ROUTINE ---
void IRAM_ATTR handleButtonPress() {
  unsigned long interruptTime = millis();
  
  if (interruptTime - lastInterruptTime > 250) {
    buttonPressed = true;
  }
  lastInterruptTime = interruptTime;
}
