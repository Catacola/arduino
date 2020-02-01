/*
The sensor outputs provided by the library are the raw 16-bit values
obtained by concatenating the 8-bit high and low accelerometer and
magnetometer data registers. They can be converted to units of g and
gauss using the conversion factors specified in the datasheet for your
particular device and full scale setting (gain).

Example: An LSM303D gives a magnetometer X axis reading of 1982 with
its default full scale setting of +/- 4 gauss. The M_GN specification
in the LSM303D datasheet (page 10) states a conversion factor of 0.160
mgauss/LSB (least significant bit) at this FS setting, so the raw
reading of -1982 corresponds to 1982 * 0.160 = 317.1 mgauss =
0.3171 gauss.

In the LSM303DLHC, LSM303DLM, and LSM303DLH, the acceleration data
registers actually contain a left-aligned 12-bit number, so the lowest
4 bits are always 0, and the values should be shifted right by 4 bits
(divided by 16) to be consistent with the conversion factors specified
in the datasheets.

Example: An LSM303DLH gives an accelerometer Z axis reading of -16144
with its default full scale setting of +/- 2 g. Dropping the lowest 4
bits gives a 12-bit raw value of -1009. The LA_So specification in the
LSM303DLH datasheet (page 11) states a conversion factor of 1 mg/digit
at this FS setting, so the value of -1009 corresponds to -1009 * 1 =
1009 mg = 1.009 g.
*/

#include <Wire.h>
#include <LSM303.h>
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

#define N_CALIBRATION_SAMPLES 100

LSM303 compass;

char report[80];
int16_t prevX = 0, prevY = 0, prevZ = 0;

void setup()
{
  Serial.begin(9600);
  Wire.begin();
  compass.init();
  compass.enableDefault();

  pinMode(LED_CLK_PIN, OUTPUT);
  pinMode(LED_DATA_PIN, OUTPUT);
  
  FastLED.addLeds<LED_TYPE,LED_DATA_PIN,LED_CLK_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
}

void loop()
{ 
  compass.read();

  /*snprintf(report, sizeof(report), "A: %6d %6d %6d    M: %6d %6d %6d",
    compass.a.x, compass.a.y, compass.a.z,
    prevX, prevY, prevZ);
  Serial.println(report);*/

  uint16_t mag = abs(compass.a.x - prevX)
               + abs(compass.a.y - prevY)
               + abs(compass.a.z - prevZ);

  //Serial.print("mag: ");
  Serial.println(mag);
  Serial.print(" ");

  CRGB color = CHSV(mag < 1000 ? 220 : 30, 255, 80);
  fill_solid(leds, NUM_LEDS, color);

  prevX = compass.a.x;
  prevY = compass.a.y;
  prevZ = compass.a.z;

  FastLED.delay(100);
}
