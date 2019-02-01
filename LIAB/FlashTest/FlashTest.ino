#include "FastLED.h"

#define NUM_LEDS 8
#define PIN 10

CRGB leds[NUM_LEDS];

void setup() {
  // put your setup code here, to run once:
    delay(2000);
    FastLED.addLeds<WS2812B, PIN, RGB>(leds, NUM_LEDS);

    Serial.begin(9600);
}

unsigned long last_fired = 0;
void loop() {
  // put your main code here, to run repeatedly:
  last_fired = millis();
  char out[15];

  unsigned long t = 0;
  while (t < 10000) {
    t = millis() - last_fired;
    // int amp = sin8(t);
    int amp = pow(10000.0 - t, 2)/100000000*sin8(t);
    sprintf(out, "amp: %d", amp);
    Serial.println(out);
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CHSV(0, 0, (int)amp);
      
    }
    FastLED.show();
  }

  delay(5000);

}
