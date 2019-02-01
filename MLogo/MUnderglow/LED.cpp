#include "LED.h"
#include <Adafruit_NeoPixel.h>

LED::LED(Adafruit_NeoPixel strip, uint8_t index, uint8_t absIdx):
  _strip(strip),
  _ledIndex(index),
  absoluteIndex(absIdx)
{
  red = 0;
  green = 0;
  blue = 0;
}

void LED::setRGB(uint8_t iRed, uint8_t iGreen, uint8_t iBlue) {
  red = iRed;
  green = iGreen;
  blue = iBlue;
  _strip.setPixelColor(
    _ledIndex,
    _strip.Color(red, green, blue)
  );
}
