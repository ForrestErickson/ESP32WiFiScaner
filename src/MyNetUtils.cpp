#include "MyNetUtils.h"

NetworkState currentNetState = STATE_IDLE;
unsigned long connectionStartTime = 0;
const unsigned long CONNECTION_TIMEOUT = 10000; 

void initNetwork(const WifiConfig &config) {
  if (config.ssid == "") {
    Serial.println("No saved credentials found. Booting directly into Access Point mode...");
    startSoftAP();
    return;
  }

  Serial.print("Attempting to connect to SSID: ");
  Serial.println(config.ssid);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(config.ssid.c_str(), config.password.c_str());
  
  currentNetState = STATE_CONNECTING;
  connectionStartTime = millis();
}

void checkNetworkStatus() {
  if (currentNetState == STATE_CONNECTING) {
    if (WiFi.status() == WL_CONNECTED) {
      currentNetState = STATE_CONNECTED;
      Serial.println("\n>>> SUCCESS: Connected to home network! <<<");
      Serial.print("IP Address allocated: ");
      Serial.println(WiFi.localIP());
      Serial.println("========================================\n");
    } 
    else if (millis() - connectionStartTime > CONNECTION_TIMEOUT) {
      Serial.println("\n>>> Connection Timeout! Falling back to SoftAP Mode... <<<");
      WiFi.disconnect();
      startSoftAP();
    }
  }
}

void startSoftAP() {
  WiFi.mode(WIFI_AP);
  
  // 1. Fetch the raw hardware MAC address from the network card (Format: AA:BB:CC:DD:EE:FF)
  String macStr = WiFi.softAPmacAddress();
  
  // 2. Strip out all colons to make the name look clean and professional
  macStr.replace(":", "");
  
  // 3. Dynamically compose SSID using the COMPANY_NAME macro and appending the MAC address
  String uniqueSSID = String(COMPANY_NAME) + macStr;
  
  // 4. Broadcast the unique open hotspot network
  bool success = WiFi.softAP(uniqueSSID.c_str(), NULL); 
  
  if (success) {
    currentNetState = STATE_AP_MODE;
    Serial.println("========================================");
    Serial.println("Access Point Hotspot Active!");
    Serial.print("SSID:       "); Serial.println(uniqueSSID);
    Serial.print("IP Address: "); Serial.println(WiFi.softAPIP());
    Serial.println("========================================");
    Serial.println("");
  } else {
    Serial.println("Error: SoftAP Hotspot initialization failed!");
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
