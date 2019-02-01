
#include <Wire.h>
#include "Adafruit_MPR121.h"
#include <Adafruit_NeoPixel.h>

#define N_PIXELS 13
#define N_ROWS 4
#define PIN 3

Adafruit_MPR121 touch = Adafruit_MPR121();
Adafruit_NeoPixel strip = Adafruit_NeoPixel(48, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  while (!Serial);        // needed to keep leonardo/micro from starting too fast!

  Serial.begin(9600);
  Serial.println("Adafruit MPR121 Capacitive Touch sensor test"); 
  
  // Default address is 0x5A, if tied to 3.3V its 0x5B
  // If tied to SDA its 0x5C and if SCL then 0x5D
  if (!touch.begin(0x5A)) {
    Serial.println("MPR121 not found, check wiring?");
    while (1);
  }
  Serial.println("MPR121 found!");
  
  strip.begin();
  strip.show();
}



void loop() {
  
  for (int i=0; i< N_PIXELS; i++) {
    if (touch.filteredData(11) < 160) {
      double level = (160 - touch.filteredData(0))/160.0;
      strip.setPixelColor(i,strip.Color(256*level,256*level,84*level));
    } else {
      strip.setPixelColor(i,strip.Color(0,0,0));
    }
  }
  
  for (int i=N_PIXELS; i< 3*N_PIXELS; i++) {
    if (touch.filteredData(5) < 160) {
      double level = (160 - touch.filteredData(0))/160.0;
      strip.setPixelColor(i,strip.Color(84*level,256*level,256*level));
    } else {
      strip.setPixelColor(i,strip.Color(0,0,0));
    }
  }
  
  for (int i=3*N_PIXELS; i< 4*N_PIXELS; i++) {
    if (touch.filteredData(0) < 130) {
      double level = (130 - touch.filteredData(0))/130.0;
      strip.setPixelColor(i,strip.Color(256*level,84*level,256*level));
    } else {
      strip.setPixelColor(i,strip.Color(0,0,0));
    }
  }

  strip.show();
}
