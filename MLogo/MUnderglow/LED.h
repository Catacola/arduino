#ifndef LED_H
#define LED_H

#include <Adafruit_NeoPixel.h>
#include <Arduino.h>

class LED
{
    private:
          Adafruit_NeoPixel _strip;
          uint8_t _ledIndex;

    public:
          uint8_t red, blue, green, absoluteIndex;
          LED(Adafruit_NeoPixel strip, uint8_t index, uint8_t absIdx);
          void setRGB(uint8_t iRed, uint8_t iGreen, uint8_t iBlue);
};

#endif
