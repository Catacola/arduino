#include <FastLED.h>

#define DATA_PIN    7
#define CLK_PIN   6
#define LED_TYPE    APA102
#define COLOR_ORDER BGR
#define NUM_LEDS    30
CRGB leds[NUM_LEDS];

#define BRIGHTNESS          96
#define FRAMES_PER_SECOND  120

void setup() {
  delay(3000); // 3 second delay for recovery

  // initialize serial communications at 9600 bps:
  //Serial.begin(9600);

  FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
}

void loop() {

  int high = 0;

  for(int i=0; i< 100; i++) {
    high = max(high, analogRead(A0));
    FastLED.delay(2);
  }

  for(int i=0; i< NUM_LEDS; i++) {
    if (i * 1.0 / NUM_LEDS  < high * 1.0 / 1024) {
      leds[i] = CRGB::MidnightBlue;
    } else {
      leds[i] = 0;    
    }
  }

  FastLED.show();
}
