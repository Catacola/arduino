// color swirl! connect an RGB LED to the PWM pins as indicated
// in the #defines
// public domain, enjoy!
 
#define POTPIN 0

#define STEPSIZE 0.01
#define MIDLEVEL 85
 
#define DELAYTIME 3     // make this higher to slow down
 
void setup() {
  
  Serial.begin(9600);
}
 
void loop() {

  Serial.println(analogRead(POTPIN));
  delay(500);
  
}
