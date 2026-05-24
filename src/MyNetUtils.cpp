#include "MyNetUtils.h"

void startScan(unsigned long &scanStartTime, bool &isScanning, unsigned long &ledOnTime, unsigned long &ledOffTime) {
  Serial.println("Button pressed! Starting async scan...");
  
  delay(5); // Prevents serial data pin corruption from radio power draw

  ledOnTime = 50;
  ledOffTime = 450;
  isScanning = true;
  scanStartTime = millis(); 

  WiFi.scanNetworks(true); 
}

void checkScanStatus(unsigned long scanStartTime, bool &isScanning, unsigned long &ledOnTime, unsigned long &ledOffTime, volatile bool &buttonPressed) {
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
