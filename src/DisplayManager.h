#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Screen dimensions for standard I2C OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1 // Share reset pin with ESP32

// Global visibility for the display object instance
extern Adafruit_SSD1306 display;

// Display interface functions
bool initDisplay();
void showBootSplash(String version);
void drawNetworkStatus(String mode, String ssid, String ip, String statusText);

#endif
