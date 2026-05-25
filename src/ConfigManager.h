#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <Arduino.h>

#define MAX_WIFI_PROFILES 4

struct WifiProfile {
  String ssid = "";
  String password = "";
};

struct WifiConfig {
  WifiProfile profiles[MAX_WIFI_PROFILES];
};

// Configuration management functions
bool initStorage();
bool loadConfig(WifiConfig &config);
bool saveConfig(const WifiConfig &config);
bool clearConfig();

#endif // CONFIG_MANAGER_H
