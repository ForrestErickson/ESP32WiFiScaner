#ifndef MY_NET_UTILS_H
#define MY_NET_UTILS_H

#include <Arduino.h>
#include "WiFi.h"
#include <ESPAsyncWebServer.h>
#include "ConfigManager.h"

// --- BRANDING CONFIGURATION ---
#define COMPANY_NAME "Amused_Scientist"

enum NetworkState {
  STATE_IDLE,
  STATE_CONNECTING,
  STATE_CONNECTED,
  STATE_AP_MODE
};

// Global visibility allocations
extern NetworkState currentNetState;
extern AsyncWebServer server;

// Network function declarations
void initNetwork(const WifiConfig &config);
void checkNetworkStatus();
void startSoftAP();
void startWebServer();
String getEncryptionName(wifi_auth_mode_t authMode);

#endif // MY_NET_UTILS_H
