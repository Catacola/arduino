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

#define DATA_PIN    2
//#define CLK_PIN   4
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS    58
CRGB leds[NUM_LEDS];

#define BRIGHTNESS          96
#define FRAMES_PER_SECOND  120

typedef struct {
  uint8_t letter;
  int8_t x;
  uint8_t y;
} SignLED;

SignLED mapping[NUM_LEDS]= {
  // O
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},

  // P
  {1, 0, 0},
  {1, 0, 0},
  {1, 0, 0},
  {1, 0, 0},
  {1, 0, 0},
  {1, 0, 0},
  {1, 0, 0},
  {1, 0, 0},
  {1, 0, 0},
  {1, 0, 0},
  {1, 0, 0},

  //E
  {2, 0, 0},
  {2, 0, 0},
  {2, 0, 0},
  {2, 0, 0},
  {2, 0, 0},
  {2, 0, 0},
  {2, 0, 0},
  {2, 0, 0},
  {2, 0, 0},
  {2, 0, 0},
  {2, 0, 0},
  {2, 0, 0},

  //N
  {3, 0, 0},
  {3, 0, 0},
  {3, 0, 0},
  {3, 0, 0},
  {3, 0, 0},
  {3, 0, 0},
  {3, 0, 0},
  {3, 0, 0},
  {3, 0, 0},
  {3, 0, 0},
  {3, 0, 0},
  {3, 0, 0},
  {3, 0, 0},
  {3, 0, 0},
  {3, 0, 0},

  //?
  {4, 0, 0},
  {4, 0, 0},
  {4, 0, 0},
  {4, 0, 0},
  {4, 0, 0},
  {4, 0, 0},
  {4, 0, 0},
  {4, 0, 0},
  {4, 0, 0},
};

const int flicker_intervals[] = {10, 20, 20, 240, 20, 40, 20, 100, 20, 20, 20, 260, 80, 20, 240, 60, 160, 20, 240, 20, 1000, 20, 20, 40, 100, 20}; //, 2740, 340, 860, 20, 1400, 20, 60, 20};

void setup() {
  delay(3000); // 3 second delay for recovery
  
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  //FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
}


// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { rainbow };
//SimplePatternList gPatterns = { rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm };


uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns
bool qMarkOn = false;
unsigned long qMarkStart = 0;
  
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
  EVERY_N_SECONDS( 30 ) { cycleQMark(); }
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
  //fill_rainbow( leds, NUM_LEDS, gHue, 7);
  for(int i=0; i< NUM_LEDS; i++) {
    int hueOffset = letterToHueOffset( mapping[i].letter );
    uint8_t hue = gHue + hueOffset;
    leds[i] = CHSV(hue, 255, 255);
  }
  blankQMark();
}

void cycleQMark() {
  qMarkOn = !qMarkOn;
  qMarkStart = millis();
}

void blankQMark() {
  long total_time = 0;
  int flicker_step;
  for(flicker_step = 0; flicker_step<ARRAY_SIZE(flicker_intervals); flicker_step++) {
    total_time += flicker_intervals[flicker_step];
    if ( total_time > millis() - qMarkStart ) {
      break;
    }
  }
  if (flicker_step % 2 == (qMarkOn ? 1 : 0)) {
    for(int i=0; i< NUM_LEDS; i++) {
      if( mapping[i].letter == 4 ) {
        leds[i] = CHSV(0, 255, 0);
      }
    }
  }
}

uint8_t letterToHueOffset(uint8_t letNum) {
  uint8_t mapped_num = 0;
  switch(letNum) {
    case 0:
      mapped_num = 0;
      break;
    case 1:
      mapped_num = 2;
      break;
    case 2:
      mapped_num = 4;
      break;
    case 3:
      mapped_num = 1;
      break;
    case 4:
      mapped_num = 3;
  }
  return gHue + ( mapped_num * 256 / 5);
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
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

void confetti() 
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16( 13, 0, NUM_LEDS-1 );
  leds[pos] += CHSV( gHue, 255, 192);
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
    leds[beatsin16( i+7, 0, NUM_LEDS-1 )] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}
