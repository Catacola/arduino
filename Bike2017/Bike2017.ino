#include "FastLED.h"

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

#define DATA_PIN_DENSE    3
#define CLK_PIN_DENSE   4
#define DATA_PIN_SPARSE    7
#define CLK_PIN_SPARSE   8
#define LED_TYPE    APA102
#define COLOR_ORDER BGR
#define NUM_LEDS_DENSE    144
#define NUM_LEDS_SPARSE    30
CRGB sparse_leds[NUM_LEDS_SPARSE];
CRGB dense_leds[NUM_LEDS_DENSE];

#define DATA_PIN_HEADLIGHT    12
#define NUM_LEDS_HEADLIGHT    93
#define HEADLIGHT_LED_TYPE    WS2812B
#define COLOR_ORDER_HEADLIGHT RGB
CRGB headlight_leds[NUM_LEDS_HEADLIGHT];

#define BRIGHTNESS         80
#define FRAMES_PER_SECOND  120

#define L_BLUE_HUE 140
#define D_BLUE_HUE 160
#define PINK_HUE 224

void setup() {
  delay(3000); // 3 second delay for recovery
  
  // tell FastLED about the LED strip configuration
  //FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE,DATA_PIN_DENSE,CLK_PIN_DENSE,COLOR_ORDER>(dense_leds, NUM_LEDS_DENSE).setCorrection(TypicalLEDStrip);

  FastLED.addLeds<LED_TYPE,DATA_PIN_SPARSE,CLK_PIN_SPARSE,COLOR_ORDER>(sparse_leds, NUM_LEDS_SPARSE).setCorrection(TypicalLEDStrip);

  FastLED.addLeds<HEADLIGHT_LED_TYPE,DATA_PIN_HEADLIGHT,COLOR_ORDER_HEADLIGHT>(headlight_leds, NUM_LEDS_HEADLIGHT).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
}


// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = {
//  rainbow,
//  rainbowWithGlitter,
  confetti,
  //sinelon,
  juggle
  //bpm
};

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0;
uint8_t gLerpVal = 0;
uint8_t headlightLerpVal = 0;


void loop()
{
  // Call the current pattern function once, updating the 'leds' array
  gPatterns[gCurrentPatternNumber]();
  headlightPattern();

  // send the 'leds' array out to the actual LED strip
  FastLED.show();
  // insert a delay to keep the framerate modest
  FastLED.delay(1000/FRAMES_PER_SECOND); 

  // do some periodic updates
  EVERY_N_MILLISECONDS( 20 ) {
    gLerpVal++;
    headlightLerpVal++;
    gHue = calcHue(gLerpVal);
  }

  EVERY_N_SECONDS( 600 ) { nextPattern(); } // change patterns periodically
}

uint8_t calcHue(uint8_t param) {
  if (param < 64) {
   return lerp8by8( D_BLUE_HUE, L_BLUE_HUE, param * 4); 
  } else if (param < 128) {
    return lerp8by8( L_BLUE_HUE, D_BLUE_HUE, (param - 64) * 4);
  } else if (param < 192) {
    return lerp8by8( D_BLUE_HUE, PINK_HUE, (param - 128) * 4);
  } else {
    return lerp8by8( PINK_HUE, D_BLUE_HUE, (param - 192) * 4);
  }
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
  fill_rainbow( sparse_leds, NUM_LEDS_SPARSE, gHue, 7);
  fill_rainbow( dense_leds, NUM_LEDS_DENSE, gHue, 7);
}

void rainbowWithGlitter() 
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}

void addGlitter( fract8 chanceOfGlitter) 
{
  if( random8() < chanceOfGlitter) {
    sparse_leds[ random16(NUM_LEDS_SPARSE) ] += CRGB::White;
    dense_leds[ random16(NUM_LEDS_DENSE) ] += CRGB::White;
  }
}

void confetti() 
{
  gLerpVal += 16;
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( dense_leds, NUM_LEDS_DENSE, 10);
  fadeToBlackBy( sparse_leds, NUM_LEDS_SPARSE, 10);

  dense_leds[random16(NUM_LEDS_DENSE)] += CHSV( gHue, 200, 255);
  dense_leds[random16(NUM_LEDS_DENSE)] += CHSV( gHue, 200, 255);
  dense_leds[random16(NUM_LEDS_DENSE)] += CHSV( gHue, 200, 255);

  
  sparse_leds[random16(NUM_LEDS_SPARSE)] += CHSV( gHue, 200, 255);
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( dense_leds, NUM_LEDS_DENSE, 20);
  fadeToBlackBy( sparse_leds, NUM_LEDS_SPARSE, 20);
  dense_leds[beatsin16(13,0,NUM_LEDS_DENSE - 1)] += CHSV( gHue, 255, 192);
  sparse_leds[beatsin16(13,0,NUM_LEDS_SPARSE - 1)] += CHSV( gHue, 255, 192);
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS_DENSE; i++) { //9948
    dense_leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
  for( int i = 0; i < NUM_LEDS_SPARSE; i++) { //9948
    sparse_leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( dense_leds, NUM_LEDS_DENSE, 20);
  fadeToBlackBy( sparse_leds, NUM_LEDS_SPARSE, 20);
  uint8_t dothue = gHue;
  uint8_t dotLerp = gLerpVal;
  for( int i = 0; i < 8; i++) {
    dothue = calcHue(dotLerp);
    dense_leds[beatsin16(i+7,0,NUM_LEDS_DENSE-1)] |= CHSV(dothue, 200, 255);
    i|1 && (sparse_leds[beatsin16(i+7,0,NUM_LEDS_SPARSE-1)] |= CHSV(dothue, 200, 255));
    dotLerp += 32;
  }
}

void headlightPattern() {
  //fill_rainbow( headlight_leds, NUM_LEDS_HEADLIGHT, gHue, 7);
  uint8_t localLerp = headlightLerpVal;

  for(int i=0; i< NUM_LEDS_HEADLIGHT; i++) {
    headlight_leds[i].setHue(calcHue(localLerp++));
    i|1 && localLerp++;
  }
}
