#include <Adafruit_NeoPixel.h>
#include <Keypad.h>

#define MY_NEO_PIN 30

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(300, MY_NEO_PIN, NEO_GRB + NEO_KHZ800);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

const byte ROWS = 4; // Four rows
const byte COLS = 4; // Three columns
// Define the Keymap
char keys[ROWS][COLS] = {
  {'0','1','2','3'},
  {'4','5','6','7'},
  {'8','9','A','B'},
  {'C','D','E','F'}
};
// Connect keypad ROW0, ROW1, ROW2 and ROW3 to these Arduino pins.
byte rowPins[ROWS] = { 47, 49, 51, 53 };
// Connect keypad COL0, COL1 and COL2 to these Arduino pins.
byte colPins[COLS] = { 45, 43, 41, 39 };
// Create the Keypad
Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );


class ChargingEffect {
  public:
    inline ChargingEffect(Adafruit_NeoPixel s, uint32_t c) {
      chargeColor = c;
      chargeStrip = s;
      isOn = false;
      running = false;
    }
    void start() {
      nextStepTime = millis();
      running = true;
    }
    void stop() {
      running = false;
    }
    virtual bool run();
  private:
    Adafruit_NeoPixel chargeStrip;
    uint32_t chargeColor;
    uint32_t nextStepTime;
    bool isOn;
    bool running;
};

bool ChargingEffect::run() {
  if (!running || millis() < nextStepTime) {
    return false;
  } else {
    if (isOn) {
      chargeStrip.setPixelColor(0,0);
      chargeStrip.setPixelColor(1,0);
      chargeStrip.setPixelColor(2,0);
    } else {
      chargeStrip.setPixelColor(0,chargeColor);
      chargeStrip.setPixelColor(1,chargeColor);
      chargeStrip.setPixelColor(2,chargeColor);
    }
    isOn = !isOn;
    chargeStrip.show();
    nextStepTime = millis() + 1000;
    return true;
  }
}

ChargingEffect charge = ChargingEffect(strip,strip.Color(0,0,250));

void setup() {
  //kpd.setDebounceTime(50);
  kpd.addEventListener(handleKeypadEvent);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  Serial.begin(9600);
}

void loop() {
  kpd.getKey(); //needed to fire handler
  charge.run();
  //Serial.println(kpd.getState());
}

// handle keypand events
void handleKeypadEvent(KeypadEvent key){
  switch (kpd.getState()){
    case PRESSED:
      switch (key){
        case '0': 
        case '1':
        case '2':
        case '3':
      
        case '4': 
        case '5':
        case '6':
        case '7':
        
        case '8': 
        case '9':
        case 'A':
        case 'B':
        
        case 'C': 
        case 'D':
        case 'E':
        case 'F':
          //ChargingEffect charge = ChargingEffect(strip, strip.Color(0,0,250));
        break;
      }
      break;
    case RELEASED:
      charge.stop();
    break;
    case HOLD:
      charge.start();
    break;
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

//class Effect {
//  public:
//    inline virtual void begin(){ /*nothing*/ };
//    void start() = 0;
//    void stop() = 0;
//    void run() = 0;
//    bool step() = 0;
//  private:
//    uint16_t stepInterval;
//    uint32_t nextStepTime;
//}

