#include <Arduino.h>

void setup() {
  Serial.begin(115200);
  Serial.println(F("\nI2C PINS"));
  Serial.print(F("\tSDA = ")); Serial.println(SDA);
  Serial.print(F("\tSCL = ")); Serial.println(SCL);
}

void loop() {
  Serial.print(F("\tSDA = ")); Serial.println(SDA); #//21
  Serial.print(F("\tSCL = ")); Serial.println(SCL); #//22
}