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
#include <math.h>
#include <LSM303.h>
#include <Adafruit_NeoPixel.h>

#define STRIP_PIN 6
#define STRIP_LENGTH 20
Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, STRIP_PIN, NEO_GRB + NEO_KHZ800);

LSM303 compass;

#define MIN_SPACING 300
#define FLASH_TIME 200
long lastFiredAt = 0;

#define FLASH_R 200
#define FLASH_G 70
#define FLASH_B 70
#define TAIL_LENGTH 5
#define TAIL_FALL_OFF 0.4

void setup() {
  Serial.begin(9600);
  Wire.begin();
  compass.init();
  compass.enableDefault();

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

void loop() {
  compass.read();

  runTotem(compass.a.x > 28000);

  //delay(50);
}

void runTotem(bool fired) {
  long interval = millis() - lastFiredAt;
  if (interval < FLASH_TIME) {
    wavefront((float)interval/FLASH_TIME);
  } else if (interval > MIN_SPACING && fired) {
    lastFiredAt = millis();
    wavefront(0.0);
  }
}

void wavefront(float progress) {
  int frontPixel = progress*(STRIP_LENGTH+TAIL_LENGTH+1);
  Serial.println(frontPixel);
  for (int i = 0; i < STRIP_LENGTH; i++) {
    if (i <= frontPixel) {
      int numBack = frontPixel - i;
      if (numBack > TAIL_LENGTH) {
        strip.setPixelColor(i, 0);
      } else {
        float fadeFactor = pow(TAIL_FALL_OFF, numBack);
        strip.setPixelColor(
          i,
          strip.Color(FLASH_R*fadeFactor, FLASH_G*fadeFactor, FLASH_B*fadeFactor)
        );
      }
    } else {
      strip.setPixelColor(i, 0);
    }
  }
  
  strip.show();
}
