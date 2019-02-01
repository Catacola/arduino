#include <Conceptinetics.h>
#include <FastLED.h>

#define DMX_SLAVE_CHANNELS   10
DMX_Slave dmx_slave ( DMX_SLAVE_CHANNELS );


#define LED_PIN     5
#define COLOR_ORDER RGB
#define CHIPSET     WS2811
#define NUM_LEDS    420

#define BRIGHTNESS  255
#define FRAMES_PER_SECOND 60

CRGB leds[NUM_LEDS];

void setup() {
  // put your setup code here, to run once:
  delay(3000); // sanity delay
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness( BRIGHTNESS );

  dmx_slave.enable();
  //dmx_slave.setStartAddress(1);
}

void loop() {
  // put your main code here, to run repeatedly:
  //CRGB color = CHSV( random8(25, 40), 240, random8(165, 205));
  //CRGB color = CHSV( 35, 255, 255);
//  uint8_t scale = dmx_slave.getChannelValue(1);
//
//  for (uint8_t i = 0; i < NUM_LEDS; i++) {
//    if (i % 3 != 0) continue;
//    uint8_t val = random8();
//    if (val > 127) {
//      leds[i] = CHSV( 30, 240, 220);
//      continue;
//    }
//    leds[i] = CHSV( 30, 240, scale);
//  }

  for(int i=0; i<NUM_LEDS; i++) {
    if (i%3 != 0) continue;
    char val = random8();
    if (val > 127) {
      leds[i] = CHSV( 30, 240, 220);
      continue;
    }
    leds[i] = CHSV( 30, 240, 220 - (val/4));
  }

  //leds(0, NUM_LEDS-1) = color;
  FastLED.show();
  //delay(random8(20, 180));
  FastLED.delay(100);
}
