#include <FastLED.h>

#define LED_PIN     5
#define COLOR_ORDER RGB
#define CHIPSET     WS2811
#define NUM_LEDS    50
#define BRIGHTNESS  200

#define MAX_ON          100
#define FPS             45
#define CHANCE_ON       30
#define CHANCE_REVERSE  5

CRGB leds[NUM_LEDS];
int num_on = 0;

void setup() {
  delay(3000);
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.show();

  randomSeed(analogRead(4));
}

void loop() {

  if (num_on < MAX_ON && random(100) > 99 - CHANCE_ON) {
    int new_light = random(NUM_LEDS);
    if (leds[new_light].b == 0) {
      leds[new_light].b = 1;
      num_on++;
    }
  }

  for (int i = 0; i < NUM_LEDS; i++) {
    if (leds[i].b > 0) {
      if(leds[i].b % 2 == 1) {
        leds[i].b = qadd8(leds[i].b, 2);
        if (random(1000) > 999 - CHANCE_REVERSE) {
          leds[i].b--;
        }
      } else {
        leds[i].b -= 2;
        if (leds[i].b == 0) {
          num_on--;
        }
      }
    }
  }

  FastLED.show();
  FastLED.delay(1000/FPS);
}
