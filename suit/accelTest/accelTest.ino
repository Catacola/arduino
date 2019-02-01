#include <Arduino.h>
#include <Wire.h>
#include <LSM303.h>
//#include <Adafruit_NeoPixel.h>

LSM303 compassR, compassL;
//Adafruit_NeoPixel strip;

#define STRIP_PIN 10
#define STRIP_LENGTH 60

void setup() {
  Serial.begin(9600);
  Wire.begin();
  compassR.init(LSM303::device_auto, LSM303::sa0_high);
  compassR.enableDefault();
  compassL.init(LSM303::device_auto, LSM303::sa0_low);
  compassL.enableDefault();

  //strip = Adafruit_NeoPixel(STRIP_LENGTH, STRIP_PIN, NEO_GRB + NEO_KHZ800);
  //strip.begin();
  //strip.show();
}

void loop() {
  Serial.println("foo");

  compassL.read();
  compassR.read();
  

  float radialAR = sqrt(pow(compassR.a.x, 2) + pow(compassR.a.z, 2));
  float radialAL = sqrt(pow(compassL.a.x, 2) + pow(compassL.a.z, 2));

  //char out[50];
  //sprintf(out,"%d, %d, %d", compass.a.x, compass.a.y, compass.a.z);
  //sprintf(out,"%f, %d", radialA, compass.a.y);
  // Serial.print(radialA);
  // Serial.print(", ");
  // Serial.println(compass.a.y);
  //  Serial.println(out);
  // delay(50);

  //for (size_t i = 0; i < STRIP_LENGTH; i++) {
    //strip.setPixelColor(i, 0);
  //}

  if (radialAL > 14500) {
    Serial.println("left level");
    //for (size_t i = 0; i < STRIP_LENGTH; i++) {
      //strip.setPixelColor(i, 0xFF0000);
    //}
  }

  if (compassL.a.y < -14000) {
    Serial.println("left down");
    //for (size_t i = 0; i < STRIP_LENGTH; i++) {
      //strip.setPixelColor(i, 0x00FF00);
    //}
  }

  if (compassL.a.y > 14000) {
    Serial.println("left up");
    //for (size_t i = 0; i < STRIP_LENGTH; i++) {
      //strip.setPixelColor(i, 0x0000FF);
    //}
  }

  if (radialAR > 14500) {
    Serial.println("right level");
    //for (size_t i = 0; i < STRIP_LENGTH; i++) {
      //strip.setPixelColor(i, 0xFF0000);
    //}
  }

  if (compassR.a.y < -14000) {
    Serial.println("right down");
    //for (size_t i = 0; i < STRIP_LENGTH; i++) {
      //strip.setPixelColor(i, 0x00FF00);
    //}
  }

  if (compassR.a.y > 14000) {
    Serial.println("right up");
    //for (size_t i = 0; i < STRIP_LENGTH; i++) {
      //strip.setPixelColor(i, 0x0000FF);
    //}
  }

  //strip.show();
}
