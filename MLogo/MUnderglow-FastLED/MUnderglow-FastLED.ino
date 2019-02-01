#include "FastLED.h"

#define PIN0 2
#define PIN1 4
#define PIN2 6
#define PIN3 8

CRGB strip0[21];
CRGB strip1[57];
CRGB strip2[57];
CRGB strip3[21];

void setup() {
  FastLED.addLeds<WS2812B, PIN0, RGB>(strip0, 21);
  FastLED.addLeds<WS2812B, PIN1, RGB>(strip1, 57);
  FastLED.addLeds<WS2812B, PIN2, RGB>(strip2, 57);
  FastLED.addLeds<WS2812B, PIN3, RGB>(strip3, 21);
}

void loop() {
  unsigned long t = millis();

  for (int i = 0; i < 156; i++) {
    int norm = i * 65535 / 156;
    setLED(
      i,
      10 * pow(sin16(t * 5 / 7) / 32767.0, 10)*pow(sin16(norm / 2 + t * 5 / 11) / 32767.0, 2),
      20 * pow(sin16(norm + t * 8 / 5) / 32767.0, 4),
      min(255, 165 * pow(cos(5 * norm - t * 53 / 50) / 32767.0 + sin16(norm + t * 4) / 32767.0, 2))
    );
  }

  FastLED.show();
}

void setLED(uint8_t idx, int r, int g, int b) {
  CRGB color = CRGB(r, g, b);
  if (idx < 21) {
    strip0[idx] = color;
  } else if (idx < 78) {
    strip1[77 - idx] = color;
  } else if (idx < 135) {
    strip2[idx - 78] = color;
  } else {
    strip3[155 - idx] = color;
  }
}
