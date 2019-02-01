// color swirl! connect an RGB LED to the PWM pins as indicated
// in the #defines
// public domain, enjoy!
 
#define REDPIN 5
#define GREENPIN 6
#define BLUEPIN 3

#define TOTALBRIGHTNESS 120
 
#define FADESPEED 65     // make this higher to slow down
 
void setup() {
  pinMode(REDPIN, OUTPUT);
  pinMode(GREENPIN, OUTPUT);
  pinMode(BLUEPIN, OUTPUT);
}
 
 
void loop() {
  int r, g, b;
 
  // fade from blue to red
  for (r = 0; r <= TOTALBRIGHTNESS; r++) { 
    analogWrite(REDPIN, r);
    analogWrite(BLUEPIN, TOTALBRIGHTNESS - r);
    delay(FADESPEED);
  } 
  // fade from red to green
  for (g = 0; g <= TOTALBRIGHTNESS; g++) { 
    analogWrite(GREENPIN, g);
    analogWrite(REDPIN, TOTALBRIGHTNESS - g);
    delay(FADESPEED);
  } 
  // fade from green to blue
  for (b = 0; b <= TOTALBRIGHTNESS; b++) { 
    analogWrite(BLUEPIN, b);
    analogWrite(GREENPIN, TOTALBRIGHTNESS - b);
    delay(FADESPEED);
  } 
}
