#include "ConfigManager.h"
#include <LittleFS.h>
#include <ArduinoJson.h>

const char* CONFIG_FILE = "/config.json";

bool initStorage() {
  if (!LittleFS.begin(true)) { // true allows formatting the partition if it's corrupted
    Serial.println("Error: LittleFS mount failed!");
    return false;
  }
  Serial.println("LittleFS mounted successfully.");
  return true;
}

bool loadConfig(WifiConfig &config) {
  if (!LittleFS.exists(CONFIG_FILE)) {
    Serial.println("No configuration file found. Fresh boot assumed.");
    return false;
  }

  File configFile = LittleFS.open(CONFIG_FILE, "r");
  if (!configFile) {
    Serial.println("Failed to open configuration file for reading.");
    return false;
  }

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, configFile);
  configFile.close();

  if (error) {
    Serial.println("Failed to parse config JSON.");
    return false;
  }

  config.ssid = doc["ssid"] | "";
  config.password = doc["password"] | "";
  
  Serial.println("Configuration loaded from flash storage.");
  return true;
}

bool saveConfig(const WifiConfig &config) {
  File configFile = LittleFS.open(CONFIG_FILE, "w");
  if (!configFile) {
    Serial.println("Failed to open configuration file for writing.");
    return false;
  }

  JsonDocument doc;
  doc["ssid"] = config.ssid;
  doc["password"] = config.password;

  if (serializeJson(doc, configFile) == 0) {
    Serial.println("Failed to write config data to file.");
    configFile.close();
    return false;
  }

  configFile.close();
  Serial.println("Configuration successfully written to flash memory.");
  return true;
}

bool clearConfig() {
  if (LittleFS.exists(CONFIG_FILE)) {
    if (LittleFS.remove(CONFIG_FILE)) {
      Serial.println("Configuration file deleted successfully.");
      return true;
    }
  }
  return false;
}
