#include "Splash.h"

const char* FILE_NAME = "ESP23WiFiScaner, ";

void printSplash(int major, int minor, int patch) {
  Serial.print("==== ");
  Serial.print(FILE_NAME);
  Serial.print("Ver: "); 
  Serial.print(major); Serial.print(".");
  Serial.print(minor); Serial.print(".");
  Serial.print(patch);
  Serial.println(" ====");
  Serial.print("Built: "); Serial.print(__DATE__);
  Serial.print(" "); Serial.print(__TIME__);
  Serial.println("");
  Serial.println("========================================");
  Serial.println("");
}
