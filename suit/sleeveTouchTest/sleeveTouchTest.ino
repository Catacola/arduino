
#include <Wire.h>
#include "Adafruit_MPR121.h"
#include <Adafruit_NeoPixel.h>

#define N_PIXELS 13
#define N_ROWS 4
#define PIN 3

Adafruit_MPR121 touch = Adafruit_MPR121();

void setup() {
  while (!Serial);        // needed to keep leonardo/micro from starting too fast!

  Serial.begin(9600);
  Serial.println("Adafruit MPR121 Capacitive Touch sensor test"); 
  
  // Default address is 0x5A, if tied to 3.3V its 0x5B
  // If tied to SDA its 0x5C and if SCL then 0x5D
  if (!touch.begin(0x5A)) {
    Serial.println("MPR121 not found, check wiring?");
    while (1);
  }
  Serial.println("MPR121 found!");

}



void loop() {

  char out[100];
  
  //Serial.print(touch.filteredData(0));
  //Serial.print(", ");
  //Serial.println(touch.filteredData(3));
  
  if (touch.filteredData(0) < 155) {
    Serial.println("left");
  } else if (touch.filteredData(3) < 155) {
    Serial.println("right");
  }
}
