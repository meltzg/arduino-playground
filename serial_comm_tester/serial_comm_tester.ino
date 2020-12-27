#include <Wire.h>
#include "CommonMessaging.h"
#include "PathFinder.h"

void setup() {
  Serial.begin(9600);
  Wire.begin();
}

void loop() {
  while (Serial.available()) {
    String str = Serial.readStringUntil('\n');
    Wire.beginTransmission(FINDER_I2C_ADDR);
    Wire.write(str.c_str());
    Wire.endTransmission();

    Wire.requestFrom(FINDER_I2C_ADDR, str.length());    // request 6 bytes from slave device #8

    while (Wire.available()) { // slave may send less than requested
      char c = Wire.read(); // receive a byte as character
      Serial.print(c);         // print the character
    }
    Serial.println();
  }
}
