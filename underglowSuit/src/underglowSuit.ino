#include <Arduino.h>
#include <Wire.h>
#include <LSM303.h>
#include <Adafruit_NeoPixel.h>

LSM303 compass;
Adafruit_NeoPixel strip;

#define STRIP_PIN 10
#define STRIP_LENGTH 60

void setup() {
  Serial.begin(9600);
  Wire.begin();
  compass.init();
  compass.enableDefault();

  strip = Adafruit_NeoPixel(STRIP_LENGTH, STRIP_PIN, NEO_GRB + NEO_KHZ800);
  strip.show();
}

void loop() {
  compass.read();

  Serial.printf("%d, %d, %d\n", compass.a.x, compass.a.y, compass.a.z);
  sleep(50);
}
