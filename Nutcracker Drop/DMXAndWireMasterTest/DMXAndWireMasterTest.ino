#include <DMXSerial.h>
#include <Wire.h>

#define START_CHANNEL 190
#define N_CHANNELS 6

const int ledPin = 13;

void setup() {
  Wire.begin();
  DMXSerial.init(DMXReceiver);
}

// the loop routine runs over and over again forever:
void loop() 
{
  Wire.beginTransmission(8); // transmit to device #8
  for (int i=0; i< N_CHANNELS; i++) {
  //    Wire.beginTransmission(8); // transmit to device #8
      Wire.write(i);
      Wire.write(DMXSerial.read(i + START_CHANNEL));
      //Wire.endTransmission();
  }
  Wire.endTransmission();

  delay(100);
}
