#ifndef MY_NET_UTILS_H
#define MY_NET_UTILS_H

#include <Arduino.h>
#include "WiFi.h"

// Struct structure ready to store up to 4 access points later
struct WifiCredential {
  String ssid;
  String password;
};

// Network function declarations
void startScan(unsigned long &scanStartTime, bool &isScanning, unsigned long &ledOnTime, unsigned long &ledOffTime);
void checkScanStatus(unsigned long scanStartTime, bool &isScanning, unsigned long &ledOnTime, unsigned long &ledOffTime, volatile bool &buttonPressed);
String getEncryptionName(wifi_auth_mode_t authMode);

#endif
