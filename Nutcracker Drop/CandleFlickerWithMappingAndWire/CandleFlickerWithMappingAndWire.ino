#include <FastLED.h>
#include <Wire.h>

#define LED_PIN     5
#define COLOR_ORDER RGB
#define CHIPSET     WS2811
#define NUM_LEDS    500
#define NUM_LEDS_A  96

#define BRIGHTNESS  255
#define FRAMES_PER_SECOND 60

#define CANDLE_HUE 30
#define DEFAULT_MIN_BASE_LEVEL 120

uint8_t gLevel = 180;
uint8_t gBoostCenterXParam = 0;
uint8_t gBoostCenterYParam = 0;
uint8_t gHueMixingParam = 0;

typedef struct {
  uint16_t index;
  int8_t x;
  uint8_t y;
  //uint8_t strand;
} TreeLED;

TreeLED strandA[NUM_LEDS_A] = {
  { 1,   0,  0},
  { 5,  -3, 11},
  { 8,  -5, 22},
  {11,   3, 22},
  {14,   8, 27},
  {16,   7, 32},
  {20,  -3, 33},
  {23, -11, 33},
  {28, -10, 47},
  {32,   3, 47},
  {36,  15, 47},
  {40,  25, 47},
  {45,  30, 57},
  {49,  22, 60},
  {50,  18, 60},
  
  // Strand B
  {51,  12,  63},
  {54,   4,  61},
  {58,  -8,  61},
  {62, -19,  62},
  {65, -28,  60},
  {73, -12,  75},
  {78,  -1,  84},
  {83,  11,  80},
  {87,  22,  78},
  {91,  35,  85},
  {96,  40, 100},
  {100,  46, 108},

  // Strand C
  {105,  64, 103},
  {110,  54,  96},
  {116,  51,  79},
  {121,  41,  72},
  {128,  37,  58},
  {134,  28,  43},
  {140,  21,  31},
  {142,  17,  30},
  {146,  15,  20},
  {150,  10,  11},
  
  // Strand F
  {151,  -9,  12},
  {154, -11,  19},
  {156, -14,  21},
  {161, -19,  32},
  {168, -28,  47},
  {174, -38,  58},
  {181, -43,  77},
  {185, -51,  82},
  {191, -52,  96},
  {195, -61, 101},
  {198, -59, 108},
  
  // Strand G
  {203, -56, 126},
  {215, -75, 156},
  {224, -52, 159},
  {229, -39, 165},
  {235, -23, 168},
  {240, -25, 156},
  {246, -21, 141},
  
  // Strand H
  {251,  -6, 141},
  {256, -15, 133},
  {261, -30, 131},
  {267, -41, 141},
  {274, -57, 137},
  {278, -54, 128},
  {282, -45, 125},
  {286, -42, 115},
  {290, -44, 106},
  {295, -35, 109},
  {300, -36,  98},
  
  // Strand D
  {301, -22,  81},
  {305, -12,  88},
  {313,   6, 104},
  {317,  14, 114},
  {326,  14,  89},
  {330,  23, 100},
  {334,  28, 109},
  {340,  30, 127},
  {347,  48, 124},
  
  // Strand E
  {351,  68, 122},
  {355,  78, 127},
  {358,  71, 133},
  {362,  58, 136},
  {369,  37, 138},
  {370,  35, 137},
  {375,  46, 150},
  {381,  61, 149},
  {387,  79, 148},
  {392,  85, 160},
  {399,  66, 162},
  
  // Strand I
  {401,  53, 161},
  {409,  34, 163},
  {414,  24, 155},
  {421,  12, 142},
  {426,  11, 130},
  {431,   3, 119},
  {436,  -9, 117},
  {442, -24, 117},
  {445, -22, 110},
  {450, -12, 102},
};

CRGB leds[NUM_LEDS];

void setup() {
  // put your setup code here, to run once:
  delay(3000); // sanity delay
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness( BRIGHTNESS );

  Wire.begin(8);                // join i2c bus with address #8
  Wire.onReceive(receiveEvent); // register event
  Serial.begin(9600);           // start serial for output
  Serial.println("starting");
}

void loop() {
//  int16_t center = (millis()/20) % 120 - 60;
  int8_t centerX = gBoostCenterXParam - 127;
  int8_t centerY = gBoostCenterYParam - 40;

  uint8_t min_base_level = lerp8by8(0, DEFAULT_MIN_BASE_LEVEL, gLevel);
  uint8_t max_base_level = gLevel;

  for(int i=0; i<NUM_LEDS_A; i++) {
//    uint8_t base_level = max(120, 255 - 4*abs((long)center - strandA[i].y));
    uint8_t base_level = max(min_base_level, max_base_level - 4*abs((long)centerX - strandA[i].x));
    base_level = max(base_level, max_base_level - 4*abs((long)centerY - strandA[i].y));
    uint8_t hue = getHue(gHueMixingParam, strandA[i]);

    uint8_t val = random8();
    if (val > 127) {
      setLED(i, CHSV( hue, 240, base_level));
      continue;
    }
    setLED(i, CHSV( hue, 240, qsub8(base_level, val/4)));
  }

  FastLED.show();
  FastLED.delay(75);
}

void setLED(uint8_t index, CRGB color) {
  leds[strandA[index].index] = color;
}

void receiveEvent(int howMany) {
  while (Wire.available()) {
    int channel = Wire.read();
    int level = Wire.read();
    processChannel(channel, level);    
  }
}

uint8_t getHue(uint8_t mix_param, TreeLED led) {
  uint8_t hue;
  switch (led.index % 6) {
    case 0:
      hue = 0; //red
      break;
    case 1:
      hue = 32; //orange
      break;
    case 2:
      hue = 96; //green
      break;
    case 3:
      hue = 160; //blue
      break;
    case 4:
      hue = 192; //purple
      break;
    case 5:
      hue = 224; //pink
      break;
  }

  if (led.index % 31 < mix_param / 8) {
    return hue;
  } else {
    return CANDLE_HUE;
  }
}

void processChannel(uint8_t channel, uint8_t level) {
  switch (channel) {
    case 0:
      //gLevel = level;
      break;
    case 1:
      gBoostCenterXParam = level;
      break;
    case 2:
      gBoostCenterYParam = level;
      break;
    case 3:
      gHueMixingParam = level;
      break;
    default:
      break;
  }
     
}
