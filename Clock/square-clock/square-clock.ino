#include "FastLED.h"

#include <TinyGPS++.h>
#include <SoftwareSerial.h>

static const int RXPin = 10, TXPin = 11;
static const uint32_t GPSBaud = 9600;

// The TinyGPS++ object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);

#define LED_PIN 3
#define N_LEDS 15
#define LOG 0

CRGB strip[N_LEDS];
unsigned long lastUpdated = 0;

void setup() {
  FastLED.addLeds<WS2812B, LED_PIN, RGB>(strip, N_LEDS);
  //Serial.begin(115200);

  ss.begin(GPSBaud);
}

void loop() {
  while (ss.available() > 0)
    gps.encode(ss.read());
    
  if (millis() - lastUpdated > 150) {
    displayTime();
    lastUpdated = millis();
  }
}

CRGB getTimeUnitColor(CRGB unitColor, int pixelIndex, float timeUnitPos) {
  float spacing = 1.0/N_LEDS;
  float pixelPos = pixelIndex*1.0/N_LEDS;

  if (fabs(pixelPos - timeUnitPos) < spacing) {
    return unitColor % (255 * (1 - fabs(pixelPos - timeUnitPos) / spacing));
  } else {
    return CRGB(0, 0, 0);
  }
  //return (fabs(pixelIndex*1.0/N_LEDS - timeUnitPos) < 0.04) ? unitColor : CHSV(0, 0, 0);
}

void displayTime() {
  if (gps.time.isValid()) {
    //Serial.println("valid time");
    float hourPos = gps.time.hour()/12.0 + gps.time.minute()/720.0;
    float minPos = gps.time.minute()/60.0 + gps.time.second()/3600.0;
    float secPos = gps.time.second()/60.0 + gps.time.centisecond()/6000.0;

    for (int i = 0; i < N_LEDS; i++) {
      strip[i] = getTimeUnitColor(CHSV(HUE_BLUE, 255, 200), i, hourPos);
      strip[i] += getTimeUnitColor(CHSV(HUE_GREEN, 255, 200), i, minPos);
      //strip[i] += getTimeUnitColor(CHSV(HUE_RED, 255, 200), i, secPos);
    }

    /*Serial.print(hourPos);
    Serial.print(F(", "));
    Serial.print(minPos);
    Serial.print(F(", "));
    Serial.println(secPos);*/
  } else {
    //Serial.println("invalid time");
    CRGB color = CHSV(0 , 0, sin8(millis()/100));
    for (auto& led: strip) {
      led = color;
    }
  }

  FastLED.show();
}
