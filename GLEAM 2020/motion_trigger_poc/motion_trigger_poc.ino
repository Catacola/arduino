#include <FastLED.h>

FASTLED_USING_NAMESPACE

#define FASTLED_USE_GLOBAL_BRIGHTNESS 1
 
#define LED_DATA_PIN 10                // choose the pin for the LED
#define LED_CLK_PIN 11
#define LED_TYPE    APA102
#define COLOR_ORDER BGR
#define NUM_LEDS    60
CRGB leds[NUM_LEDS];

#define BRIGHTNESS          80
#define FRAMES_PER_SECOND  120

#define PIR_DEBOUNCE_ON 3000
#define PIR_DEBOUNCE_OFF 15000

#define PIR_PIN 6               // choose the input pin (for PIR sensor)

int pirState = LOW;             // we start, assuming no motion detected
uint16_t lastPirChange = 0;
 
void setup() {
  pinMode(LED_CLK_PIN, OUTPUT);
  pinMode(LED_DATA_PIN, OUTPUT);
  pinMode(PIR_PIN, INPUT);

  FastLED.addLeds<LED_TYPE,LED_DATA_PIN,LED_CLK_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
  
  Serial.begin(9600);
}
 
void loop(){
  checkPIR();
  updateLEDs();
  FastLED.show();
  FastLED.delay(1000/FRAMES_PER_SECOND);
}

void updateLEDs() {
  uint8_t ease_frac = easeFracFromPIR();
  uint8_t brightness = 135 + (40 * ease_frac/255) + beatsin8(16, 0, 40 + (40 * ease_frac/255));
  CRGB color = CHSV(205 + (30 * ease_frac/255), 255, brightness);
  fill_solid(leds, NUM_LEDS, color);
}

uint8_t easeFracFromPIR() {
  uint16_t time_since_trigger = millis() - lastPirChange;
  if (time_since_trigger > 5120) {
    return pirState == LOW ? 0 : 255;
  }
  if (pirState == HIGH) {
    return ease8InOutApprox(time_since_trigger / 20);
  } else {
    return ease8InOutApprox((5120 - time_since_trigger) / 20);
  }
}

uint16_t lastPirValChange = 0;
uint8_t lastPirVal = LOW;

void checkPIR() {
  uint8_t val = digitalRead(PIR_PIN);
  uint16_t mils = millis();

  /*Serial.print("val: ");
  Serial.print(val);
  Serial.print(", lastPirVal: ");
  Serial.print(lastPirVal);
  Serial.print(", lastPirValChange: ");
  Serial.print(lastPirValChange);
  Serial.print(", millis(): ");
  Serial.print(mils);
  Serial.print(", time since change: ");
  Serial.println((mils - lastPirValChange));*/
  

  // Debounce
  if (val != lastPirVal) {
    lastPirValChange = mils;
    lastPirVal = val;
    return;
  }

  if ((mils - lastPirValChange) < (pirState == LOW ? PIR_DEBOUNCE_ON : PIR_DEBOUNCE_OFF)) {
    return;
  }
  

  if (val == HIGH) {  
    if (pirState == LOW) {
      printTime();
      Serial.println("Motion detected!");
      pirState = HIGH;
      updatePirChangeTime(mils);
      
    }
  } else {
    if (pirState == HIGH){
      printTime();
      Serial.println("Motion ended!");
      pirState = LOW;
      updatePirChangeTime(mils); 
    }
  }
}

void updatePirChangeTime(uint16_t mils) {
  lastPirChange = (lastPirChange - mils > 5120) ? mils : mils - (5120 - lastPirChange);
}

void printTime() {
  unsigned long t = millis()/1000;
  Serial.print(t/60);
  Serial.print("m ");
  Serial.print(t%60);
  Serial.print("s: ");
}
