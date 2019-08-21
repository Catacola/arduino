#include <FastLED.h>

FASTLED_USING_NAMESPACE

const int flicker_intervals[] = {10, 20, 20, 240, 20, 40, 20, 100, 20, 20, 20, 260, 80, 20, 240, 60, 160, 20, 240, 20, 1000, 20, 20, 40, 100, 20}; //, 2740, 340, 860, 20, 1400, 20, 60, 20};

#define DATA_PIN    6
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS    150
CRGB leds[NUM_LEDS];

#define BRIGHTNESS          64 // out of 255
#define FRAMES_PER_SECOND  120

#define MIN_EVENT_SPACING 25 //45  // seconds
#define MAX_EVENT_SPACING 35 //150 // seconds

#define FUCK_YOU_DURATION 8 // seconds
#define HEART_YOU_DURATION 8 // seconds
#define FUCK_OUR_BURN_DURATION 8 // seconds

#define MAX_SPARKLE_CHANCE 90
#define MAX_SPARKLE_DISTANCE 5

typedef struct {
  uint8_t x;
  uint8_t y;
  uint8_t word_num;
  uint8_t let_num;
} SignLED;

const SignLED ledInfo[NUM_LEDS] = {
  // Fuck
  {10, 43, 0, 0},
  {10, 35, 0, 0},
  {28, 27, 0, 0},
  {19, 27, 0, 0},
  {10, 27, 0, 0},
  {10, 18, 0, 0},
  {10, 9, 0, 0},
  {19, 9, 0, 0},
  {28, 9, 0, 0},

  // fUck
  {43, 9, 0, 1},
  {43, 18, 0, 1},
  {44, 27, 0, 1},
  {47, 36, 0, 1},
  {51, 44, 0, 1},
  {58, 46, 0, 1},
  {65, 45, 0, 1},
  {66, 36, 0, 1},
  {66, 27, 0, 1},
  {66, 18, 0, 1},
  {66, 10, 0, 1},

  // fuCk
  {90, 10, 0, 2},
  {99, 13, 0, 2},
  {82, 11, 0, 2},
  {79, 20, 0, 2},
  {79, 29, 0, 2},
  {81, 38, 0, 2},
  {86, 45, 0, 2},
  {95, 46, 0, 2},
  {101, 42, 0, 2},

  // fucK
  {113, 29, 0, 3},
  {113, 19, 0, 3},
  {113, 10, 0, 3},
  {135, 10, 0, 3},
  {129, 16, 0, 3},
  {121, 24, 0, 3},
  {112, 39, 0, 3},
  {112, 47, 0, 3},
  {121, 34, 0, 3},
  {127, 40, 0, 3},
  {132, 47, 0, 3},

  // youR
  {148, 61, 1, 3},
  {156, 62, 1, 3},
  {163, 69, 1, 3},
  {157, 76, 1, 3},
  {149, 77, 1, 3},
  {161, 96, 1, 3},
  {155, 90, 1, 3},
  {149, 85, 1, 3},
  {140, 96, 1, 3},
  {140, 87, 1, 3},
  {140, 78, 1, 3},
  {140, 70, 1, 3},
  {140, 61, 1, 3},

  // yoUr
  {128, 61, 1, 2},
  {127, 68, 1, 2},
  {127, 77, 1, 2},
  {126, 86, 1, 2},
  {125, 94, 1, 2},
  {119, 98, 1, 2},
  {114, 93, 1, 2},
  {111, 87, 1, 2},
  {108, 78, 1, 2},
  {108, 70, 1, 2},
  {108, 61, 1, 2},

  // yOur
  {88, 61, 1, 1},
  {95, 66, 1, 1},
  {97, 75, 1, 1},
  {97, 83, 1, 1},
  {95, 92, 1, 1},
  {88, 97, 1, 1},
  {81, 91, 1, 1},
  {79, 82, 1, 1},
  {78, 74, 1, 1},
  {80, 66, 1, 1},

  // Your
  {66, 60, 1, 0},
  {62, 66, 1, 0},
  {60, 72, 1, 0},
  {40, 60, 1, 0},
  {46, 66, 1, 0},
  {50, 73, 1, 0},
  {54, 79, 1, 0},
  {54, 88, 1, 0},
  {54, 97, 1, 0},

  // Heart
  {49, 105, 3, 0},
  {40, 107, 3, 0},
  {34, 114, 3, 0},
  {28, 108, 3, 0},
  {19, 105, 3, 0},
  {12, 112, 3, 0},
  {12, 121, 3, 0},
  {15, 129, 3, 0},
  {21, 138, 3, 0},
  {27, 145, 3, 0},
  {33, 151, 3, 0},
  {38, 143, 3, 0},
  {44, 135, 3, 0},
  {49, 129, 3, 0},
  {54, 121, 3, 0},
  {54, 113, 3, 0},

  // Burn
  {71, 112, 2, 0},
  {71, 119, 2, 0},
  {71, 128, 2, 0},
  {71, 137, 2, 0},
  {71, 146, 2, 0},
  {81, 146, 2, 0},
  {89, 144, 2, 0},
  {93, 136, 2, 0},
  {94, 122, 2, 0},
  {88, 128, 2, 0},
  {80, 128, 2, 0},
  {80, 112, 2, 0},
  {91, 113, 2, 0},

  // bUrn
  {105, 110, 2, 1},
  {105, 118, 2, 1},
  {105, 126, 2, 1},
  {106, 132, 2, 1},
  {109, 141, 2, 1},
  {115, 146, 2, 1},
  {121, 141, 2, 1},
  {124, 133, 2, 1},
  {124, 126, 2, 1},
  {124, 117, 2, 1},
  {124, 110, 2, 1},

  // buRn
  {136, 111, 2, 2},
  {144, 111, 2, 2},
  {136, 120, 2, 2},
  {136, 129, 2, 2},
  {136, 138, 2, 2},
  {136, 146, 2, 2},
  {156, 146, 2, 2},
  {152, 141, 2, 2},
  {147, 135, 2, 2},
  {149, 127, 2, 2},
  {155, 126, 2, 2},
  {158, 118, 2, 2},
  {152, 112, 2, 2},

  // burN
  {166, 111, 2, 3},
  {166, 119, 2, 3},
  {166, 128, 2, 3},
  {166, 137, 2, 3},
  {166, 146, 2, 3},
  {183, 140, 2, 3},
  {181, 132, 2, 3},
  {176, 123, 2, 3},
  {174, 117, 2, 3},
  {191, 111, 2, 3},
  {191, 119, 2, 3},
  {191, 128, 2, 3},
  {191, 137, 2, 3},
  {191, 146, 2, 3},

};

void setup() {
  delay(3000); // 3 second delay for recovery

  Serial.begin(57600);
  
  // set a random random seed
  random16_set_seed(analogRead(0));
  
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
}

uint8_t gCurrentEffect = 0;
uint8_t gHue = 0; // rotating "base color" used by many of the patterns

uint32_t gEffectSpacing = 0;
uint32_t gLastEffectStart = 0;

void loop()
{
  basePattern();

  runEffect();

  checkForNewEffect();
  
  // send the 'leds' array out to the actual LED strip
  FastLED.show();  
  // insert a delay to keep the framerate modest
  FastLED.delay(1000/FRAMES_PER_SECOND); 

  // do some periodic updates
  EVERY_N_MILLISECONDS( 50 ) { gHue++; } // slowly cycle the "base color" through the rainbow
}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void basePattern() 
{
  for(int i=0; i< NUM_LEDS; i++) {
    uint8_t hue = gHue + globalLetterNum(i) * 256 / 6;
    leds[i] = CHSV(hue, 255, 255);
  }
}

/*********************************
 * Effects
 *********************************/

void fuckYou() { 
  if (flickerState(FUCK_YOU_DURATION * 1000)) {
    for(int i=0; i< NUM_LEDS; i++) {
      SignLED myLED = ledInfo[i];
      if( myLED.word_num > 1 || (myLED.word_num == 1 && myLED.let_num == 3) ) {
        leds[i] = 0;
      }
    }
  }
}

void heartYou() {
  if (flickerState(HEART_YOU_DURATION * 1000)) {
    for(int i=0; i< NUM_LEDS; i++) {
      SignLED myLED = ledInfo[i];
      if( myLED.word_num % 2 == 0  || myLED.let_num == 3 ) {
        leds[i] = 0;
      }
    }
  }
}

void fuckOurBurn() {
  if (flickerState(FUCK_OUR_BURN_DURATION * 1000)) {
    for(int i=0; i< NUM_LEDS; i++) {
      SignLED myLED = ledInfo[i];
      if(( myLED.word_num == 1  && myLED.let_num == 0 ) || myLED.word_num == 3 ) {
        leds[i] = 0;
      }
    }
  }
}

void fuckYouBurn() {
  if (flickerState(FUCK_OUR_BURN_DURATION * 1000)) {
    for(int i=0; i< NUM_LEDS; i++) {
      SignLED myLED = ledInfo[i];
      if(( myLED.word_num == 1  && myLED.let_num == 0 ) || myLED.word_num == 3 ) {
        leds[i] = 0;
      }
    }
  }
}

void sparkleWave() {
  // line of form a*x + b*y - c = 0
  const uint8_t a = 40;
  const uint8_t b = 50;

  uint16_t c = currentEffectTime();
  
  for(int i=0; i< NUM_LEDS; i++) {
    SignLED myLED = ledInfo[i];
    uint16_t distance = abs(a * myLED.x + b * myLED.y - c)/64;
    uint8_t sparkleChance = MAX_SPARKLE_CHANCE * (MAX_SPARKLE_DISTANCE - distance) / MAX_SPARKLE_DISTANCE;
    if (sparkleChance < random8(100)) {
      leds[i] = CRGB::White;
    }
  }
}

/**************************************
 * Effect control and timing
 ****************************************/

void runEffect() {
  Serial.print("effect ");
  Serial.println(gCurrentEffect);
  switch (gCurrentEffect) {
    case 0:
      fuckYou();
      return;
    case 1:
      heartYou();
      return;
    case 2:
      fuckOurBurn();
      return;
    case 3:
      sparkleWave();
      return;
  }
}

void checkForNewEffect() {
  uint32_t curTime = currentEffectTime();
  if (curTime < gEffectSpacing) {
    return;
  }
  
  chooseNewPattern();
  chooseEffectSpacing();
  gLastEffectStart = millis();
}

void chooseNewPattern() {
  uint8_t roll = random8(100);

  if (roll < 60) {
    gCurrentEffect = 0;
  } else if (roll < 80) {
    gCurrentEffect = 3;
  } else if (roll < 95) {
    gCurrentEffect = 1;
  } else {
    gCurrentEffect = 2;
  }
}

void chooseEffectSpacing() {
  gEffectSpacing = random16(MIN_EVENT_SPACING * 10, MAX_EVENT_SPACING * 10) * 100;
}


/**************************************
 * Helpers
 ****************************************/

bool flickerState(uint32_t effectTimeout) {
  // returns true when flickered off
  uint32_t total_time = 0;
  int flicker_step;

  uint32_t effectTime = currentEffectTime();
  bool invert = effectTime > effectTimeout;
  effectTime -= invert ? effectTimeout : 0;

  Serial.println(invert ? "inverted" : "not");
  
  for(flicker_step = 0; flicker_step<ARRAY_SIZE(flicker_intervals); flicker_step++) {
    total_time += flicker_intervals[flicker_step];
    if ( total_time > effectTime ) {
      Serial.println(flicker_step);
      return flicker_step % 2 == (invert ? 1 : 0);
    }
  }
  return !invert;
}

uint32_t currentEffectTime() {
  return millis() - gLastEffectStart;
}

uint8_t globalLetterNum(uint8_t index) {
  SignLED myLED = ledInfo[index];
  return myLED.let_num + myLED.word_num * 4;
//  if(index < 9) {
//    return 0;
//  } else if(index < 20) {
//    return 1;
//  } else if(index < 29) {
//    return 2;
//  } else if(index < 40) {
//    return 3;
//  } else if(index < 53) {
//    return 7;
//  } else if(index < 64) {
//    return 6;
//  } else if(index < 74) {
//    return 5;
//  } else if(index < 83) {
//    return 4;
//  } else if(index < 99) {
//    return 12;
//  } else if(index < 112) {
//    return 8;
//  } else if(index < 123) {
//    return 9;
//  } else if(index < 136) {
//    return 10;
//  } else {
//    return 11;  
//  }
}
