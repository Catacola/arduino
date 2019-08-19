#define OCTAVE 1 // use the octave output function
#define OCT_NORM 1 //renorm please
#define LIN_OUT8 0
#define SCALE 1
#define FHT_N 256 // set to 256 point fht

#include <FHT.h> // include the library

#include <FastLED.h>

#define DATA_PIN    7
#define CLK_PIN   6
#define LED_TYPE    APA102
#define COLOR_ORDER RGB
#define NUM_LEDS    30
CRGB leds[NUM_LEDS];

#define BRIGHTNESS          96
#define FRAMES_PER_SECOND  120

void setup() {
  delay(3000); // 3 second delay for recovery

  // initialize serial communications at 9600 bps:
  Serial.begin(9600);

//  FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
//
//  // set master brightness control
//  FastLED.setBrightness(BRIGHTNESS);

  // setup FHT on ADC0
  TIMSK0 = 0; // turn off timer0 for lower jitter
  ADCSRA = 0xe5; // set the adc to free running mode
  ADMUX = 0x40; // use adc0
  DIDR0 = 0x01; // turn off the digital input for adc0
}

void loop() {

  cli();  // UDRE interrupt slows this way down on arduino1.0
  for (int i = 0 ; i < FHT_N ; i++) { // save 256 samples
    while(!(ADCSRA & 0x10)); // wait for adc to be ready
    ADCSRA = 0xf5; // restart adc
    byte m = ADCL; // fetch adc data
    byte j = ADCH;
    int k = (j << 8) | m; // form into an int
    k -= 0x0200; // form into a signed int
    k <<= 6; // form into a 16b signed int
    fht_input[i] = k; // put real data into bins
  }
  fht_window(); // window the data for better frequency response
  fht_reorder(); // reorder the data before doing the fht
  fht_run(); // process the data in the fht
  fht_mag_log(); // take the output of the fht
  sei();

  int lows = fht_oct_out[0];
  int mids = fht_oct_out[4];
  int highs = fht_oct_out[7];
  
  Serial.print("lows=");
  Serial.print(lows);
  Serial.print(" mids=");
  Serial.print(mids);
  Serial.print(" highs=");
  Serial.println(highs);

//  for(int i=0; i< NUM_LEDS; i++) {
//    leds[i] = 0;
//    if (i * 1.0 / NUM_LEDS  < lows * 1.0 / 256) {
//      leds[i] += CRGB::Red;
//    }
//    if (i * 1.0 / NUM_LEDS  < mids * 1.0 / 256) {
//      leds[i] += CRGB::Blue;
//    }
//    if (i * 1.0 / NUM_LEDS  < highs * 1.0 / 256) {
//      leds[i] += CRGB::Green;
//    }
//  }
//
//  FastLED.show();
//  FastLED.delay(1000/FRAMES_PER_SECOND); 
}
