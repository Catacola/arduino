#include <Adafruit_NeoPixel.h>
#include "LED.h"

#define PIN0 2
#define PIN1 4
#define PIN2 6
#define PIN3 8

//Adafruit_NeoPixel strip[4];

Adafruit_NeoPixel strip0 = Adafruit_NeoPixel(21, PIN0, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip1 = Adafruit_NeoPixel(57, PIN1, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip2 = Adafruit_NeoPixel(57, PIN2, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip3 = Adafruit_NeoPixel(21, PIN3, NEO_GRB + NEO_KHZ800);

//strip[0] = Adafruit_NeoPixel(21, PIN1, NEO_GRB + NEO_KHZ800);
//strip[1] = Adafruit_NeoPixel(57, PIN2, NEO_GRB + NEO_KHZ800);
//strip[2] = Adafruit_NeoPixel(57, PIN3, NEO_GRB + NEO_KHZ800);
//strip[3] = Adafruit_NeoPixel(21, PIN4, NEO_GRB + NEO_KHZ800);

//LED leds[156];

void setup() {
  strip0.begin();
  strip1.begin();
  strip2.begin();
  strip3.begin();

  strip0.show();
  strip1.show();
  strip2.show();
  strip3.show();
  
  Serial.begin(9600);
}

void loop() {
  unsigned long t = millis();

  for (int i = 0; i < 156; i++) {
    float norm = i / 156.0 * 6.28;

    int red = 10 * pow(sin(t / 14700.0), 10)*pow(sin(0.5 * norm + t / 23000.0), 2);
    int blue = 20 * pow(sin(norm + t / 6500.0), 4);
    int green = min(255, 165 * pow(cos(5 * norm - t / 9840.0) + sin(norm + t / 2643.0), 2));

    setLED(
      i,
      strip0.Color(
        red,
        blue,
        green
      )
    );
  }

  strip0.show();
  strip1.show();
  strip2.show();
  strip3.show();
  //  for(int i = 0; i < 4; i++) {
  //    strip[i].show();
  //  }

}

void setLED(uint8_t idx, uint32_t color) {
  if (idx < 21) {
    strip0.setPixelColor(idx, color);
  } else if (idx < 78) {
    strip1.setPixelColor(77 - idx, color);
  } else if (idx < 135) {
    strip2.setPixelColor(idx - 78, color);
  } else {
    strip3.setPixelColor(155 - idx, color);
  }
}
