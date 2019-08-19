#include <FastLED.h>

FASTLED_USING_NAMESPACE

// FastLED "100-lines-of-code" demo reel, showing just a few 
// of the kinds of animation patterns you can quickly and easily 
// compose using FastLED.  
//
// This example also shows one easy way to define multiple 
// animations patterns and have them automatically rotate.
//
// -Mark Kriegsman, December 2014

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define DATA_PIN_BOTTOM    9
#define DATA_PIN_TOP_OUTER    A1
#define DATA_PIN_TOP_INNER    7
#define CLK_PIN_TOP_OUTER     A0
#define LED_TYPE_BOTTOM    WS2812B
#define LED_TYPE_TOP_OUTER    APA102
#define LED_TYPE_TOP_INNER    WS2812B
#define COLOR_ORDER_2812 GRB
#define COLOR_ORDER_102 BGR
#define NUM_LEDS_BOTTOM    69
#define NUM_LEDS_TOP    60

CRGB leds_top[NUM_LEDS_TOP];
CRGB leds_bottom[NUM_LEDS_BOTTOM];

#define BRIGHTNESS         200
#define FRAMES_PER_SECOND  120

void setup() {
  delay(3000); // 3 second delay for recovery

  pinMode(DATA_PIN_TOP_OUTER, OUTPUT);
  pinMode(CLK_PIN_TOP_OUTER, OUTPUT);
  
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE_TOP_INNER,DATA_PIN_TOP_INNER,COLOR_ORDER_2812>(leds_top, NUM_LEDS_TOP).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE_TOP_OUTER,DATA_PIN_TOP_OUTER,CLK_PIN_TOP_OUTER,COLOR_ORDER_102>(leds_top, NUM_LEDS_TOP).setCorrection(TypicalLEDStrip);

  FastLED.addLeds<LED_TYPE_BOTTOM,DATA_PIN_BOTTOM,COLOR_ORDER_2812>(leds_bottom, NUM_LEDS_BOTTOM).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
}


// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
//SimplePatternList gPatterns = { rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm };
SimplePatternList gPatterns = { rainbow };

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns
  
void loop()
{
  // Call the current pattern function once, updating the 'leds' array
  gPatterns[gCurrentPatternNumber]();

  // send the 'leds' array out to the actual LED strip
  FastLED.show();  
  // insert a delay to keep the framerate modest
  FastLED.delay(1000/FRAMES_PER_SECOND); 

  // do some periodic updates
  EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
  EVERY_N_SECONDS( 10 ) { nextPattern(); } // change patterns periodically
}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
}

void rainbow() 
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds_bottom, NUM_LEDS_BOTTOM, gHue, 7);
  fill_rainbow( leds_top, NUM_LEDS_TOP, gHue, 7);
}

//void rainbowWithGlitter() 
//{
//  // built-in FastLED rainbow, plus some random sparkly glitter
//  rainbow();
//  addGlitter(80);
//}
//
//void addGlitter( fract8 chanceOfGlitter) 
//{
//  if( random8() < chanceOfGlitter) {
//    leds[ random16(NUM_LEDS) ] += CRGB::White;
//  }
//}
//
//void confetti() 
//{
//  // random colored speckles that blink in and fade smoothly
//  fadeToBlackBy( leds, NUM_LEDS, 10);
//  int pos = random16(NUM_LEDS);
//  leds[pos] += CHSV( gHue + random8(64), 200, 255);
//}
//
//void sinelon()
//{
//  // a colored dot sweeping back and forth, with fading trails
//  fadeToBlackBy( leds, NUM_LEDS, 20);
//  int pos = beatsin16( 13, 0, NUM_LEDS-1 );
//  leds[pos] += CHSV( gHue, 255, 192);
//}
//
//void bpm()
//{
//  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
//  uint8_t BeatsPerMinute = 62;
//  CRGBPalette16 palette = PartyColors_p;
//  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
//  for( int i = 0; i < NUM_LEDS; i++) { //9948
//    leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
//  }
//}
//
//void juggle() {
//  // eight colored dots, weaving in and out of sync with each other
//  fadeToBlackBy( leds, NUM_LEDS, 20);
//  byte dothue = 0;
//  for( int i = 0; i < 8; i++) {
//    leds[beatsin16( i+7, 0, NUM_LEDS-1 )] |= CHSV(dothue, 200, 255);
//    dothue += 32;
//  }
//}
