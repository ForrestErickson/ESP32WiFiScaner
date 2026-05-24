#include "DisplayManager.h"

// Initialize display over default I2C pins (GPIO 21 SDA, GPIO 22 SCL on standard ESP32)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

bool initDisplay() {
  // Address 0x3C is standard for most 128x64 I2C OLED modules
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("Error: SSD1306 OLED allocation failed!");
    return false;
  }
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.display();
  return true;
}

void showBootSplash(String version) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("====================");
  display.println("  AMUSED SCIENTIST  ");
  display.println("====================");
  display.setCursor(0, 30);
  display.print("Firmware Ver: "); display.println(version);
  display.print("Date: "); display.println(__DATE__);
  display.println("\nInitializing systems...");
  display.display();
}

void drawNetworkStatus(String mode, String ssid, String ip, String statusText) {
  display.clearDisplay();
  
  // Header Banner Block
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("[MODE]: "); display.println(mode);
  display.println("--------------------");
  
  // Data Body Block
  display.setCursor(0, 20);
  display.print("SSID: "); 
  // Truncate long SSIDs so they stay beautifully on screen
  if(ssid.length() > 14) {
    display.println(ssid.substring(0, 12) + "..");
  } else {
    display.println(ssid);
  }
  
  display.print("IP:   "); display.println(ip);
  
  // Dynamic bottom status readout ticker line
  display.setCursor(0, 50);
  display.print("> "); display.println(statusText);
  
  display.display();
}
