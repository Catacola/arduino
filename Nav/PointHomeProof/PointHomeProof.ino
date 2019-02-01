#include <Wire.h>
#include <LSM303.h>
#include <Adafruit_NeoPixel.h>

#define N_PIXELS 13
#define N_ROWS 4
#define PIN 3

LSM303 compass;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(48, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(9600);
  Wire.begin();
  compass.init(LSM303::device_D, LSM303::sa0_auto);
  compass.enableDefault();
  
  /*
  Calibration values; the default values of +/-32767 for each axis
  lead to an assumed magnetometer bias of 0. Use the Calibrate example
  program to determine appropriate values for your particular unit.
  */
  compass.m_min = (LSM303::vector<int16_t>){ -3553,  -3772,  -3035};
  compass.m_max = (LSM303::vector<int16_t>){ +2979,  +2618,  +3034};

  strip.begin();
  strip.show();
}

void loop() {
  compass.read();
  
  /*
  When given no arguments, the heading() function returns the angular
  difference in the horizontal plane between a default vector and
  north, in degrees.
  
  The default vector is chosen by the library to point along the
  surface of the PCB, in the direction of the top of the text on the
  silkscreen. This is the +X axis on the Pololu LSM303D carrier and
  the -Y axis on the Pololu LSM303DLHC, LSM303DLM, and LSM303DLH
  carriers.
  
  To use a different vector as a reference, use the version of heading()
  that takes a vector argument; for example, use
  
    compass.heading((LSM303::vector<int>){0, 0, 1});
  
  to use the +Z axis as a reference.
  */
  setPixels(compass);
  strip.show();
  
  delay(100);
}

#define MASK_WIDTH 4
#define MASK_SCALE 30.0

float linearMask(float c, float n) {
  n = fmod(n, N_PIXELS);
  float dist = min(abs(c-n),abs(c-n-N_PIXELS));
  if (dist/MASK_WIDTH < 0.5) {
    return MASK_SCALE * (1 - 2*dist/MASK_WIDTH);
  } else {
    return 0;
  }
};

int levelFromMask(int pixel, float heading) {
  float center = N_PIXELS * heading / 360.0;
  float pixelLo = fmod(pixel - 0.5, N_PIXELS);
  float pixelHi = fmod(pixel + 0.5, N_PIXELS);
  float fLo = linearMask(center, pixelLo);
  float fHi = linearMask(center, pixelHi);
  float loBound = fmod(center - MASK_WIDTH / 2, N_PIXELS);
  float hiBound = fmod(center + MASK_WIDTH / 2, N_PIXELS);
  
  if (fLo == 0 && fHi == 0) {
    return 0;
  } else if (fLo == 0) {
    return (int)(fHi * (pixelHi - loBound));
  } else if (fHi == 0) {
    return (int)(fLo * (hiBound - pixelLo));
  } else if (abs(center - pixel) > 0.5) {
    return (int)((fLo + fHi)/2);
  } else {
    float fCenter = linearMask(center, center);
    return (int)((center - pixelLo)*(fLo + fCenter)/2 +
                 (pixelHi - center)*(fHi+fCenter)/2);
  }
}

void setPixels(LSM303 compass) {
  float heading = 360.0 - compass.heading(LSM303::vector<int>{0 , 0, -1});
  Serial.println(heading);
  uint32_t onColor;
  
  for (int i=0; i< N_PIXELS*N_ROWS; i++) {
    onColor = strip.Color(levelFromMask(i,heading),0,0);
    strip.setPixelColor(i,onColor);
  }
}
