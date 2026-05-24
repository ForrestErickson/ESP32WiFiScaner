#ifndef MY_NET_UTILS_H
#define MY_NET_UTILS_H

#include <Arduino.h>
#include "WiFi.h"
#include "ConfigManager.h"

// --- BRANDING CONFIGURATION ---
#define COMPANY_NAME "Amused_Scientist"

// Defines the operational states of our network stack
enum NetworkState {
  STATE_IDLE,
  STATE_CONNECTING,
  STATE_CONNECTED,
  STATE_AP_MODE
};

// Global variable tracking the real-time network state
extern NetworkState currentNetState;

// Network function declarations
void initNetwork(const WifiConfig &config);
void checkNetworkStatus();
void startSoftAP();
String getEncryptionName(wifi_auth_mode_t authMode);

#endif
