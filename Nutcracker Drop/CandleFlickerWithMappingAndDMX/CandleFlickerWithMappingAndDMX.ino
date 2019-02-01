#include <FastLED.h>

#define LED_PIN     5
#define COLOR_ORDER RGB
#define CHIPSET     WS2811
#define NUM_LEDS    500
#define NUM_LEDS_A  15

#define BRIGHTNESS  255
#define FRAMES_PER_SECOND 60

typedef struct {
  uint8_t index;
  int8_t x;
  uint8_t y;
  //uint8_t strand;
} TreeLED;

TreeLED strandA[NUM_LEDS_A] = {
  { 0,   0,  0},
  { 4,  -3, 11},
  { 7,  -5, 22},
  {10,   3, 22},
  {13,   8, 27},
  {15,   7, 32},
  {19,  -3, 33},
  {22, -11, 33},
  {27, -10, 47},
  {31,   3, 47},
  {35,  15, 47},
  {39,  25, 47},
  {44,  30, 57},
  {48,  22, 60},
  {49,  18, 60}
};

CRGB leds[NUM_LEDS];

void setup() {
  // put your setup code here, to run once:
  delay(3000); // sanity delay
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness( BRIGHTNESS );
}

void loop() {
  int16_t center = (millis()/20) % 120 - 60;
  for(int i=0; i<NUM_LEDS_A; i++) {
//    uint8_t base_level = max(120, 255 - 4*abs((long)center - strandA[i].y));
    uint8_t base_level = max(120, 255 - 4*abs((long)center - strandA[i].x));

    uint8_t val = random8();
    if (val > 127) {
      setLED(i, CHSV( 30, 240, base_level));
      continue;
    }
    setLED(i, CHSV( 30, 240, qsub8(base_level, val/4)));
  }

  FastLED.show();
  FastLED.delay(75);
}

void setLED(uint8_t index, CRGB color) {
  leds[strandA[index].index] = color;
}
