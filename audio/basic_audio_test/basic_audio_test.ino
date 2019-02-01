#include <FastLED.h>

#define LED_PIN     5
#define COLOR_ORDER RGB
#define CHIPSET     WS2811
#define NUM_LEDS    50
#define BRIGHTNESS  200

#define MIC_PIN     A0

CRGB leds[NUM_LEDS];

void setup() {
  delay(3000);
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.show();

  Serial.begin(9600);

}

void loop() {
  int level = analogRead(MIC_PIN);
  Serial.println(level);

  for(int i = 0; i < NUM_LEDS; i++) {
    leds[i] = i * 30 < level ? CRGB::Blue : 0;
  }

  FastLED.show();

}
