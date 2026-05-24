#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <Arduino.h>

// Struct to hold active configuration data
struct WifiConfig {
  String ssid = "";
  String password = "";
};

// Configuration management functions
bool initStorage();
bool loadConfig(WifiConfig &config);
bool saveConfig(const WifiConfig &config);
bool clearConfig();

#endif
