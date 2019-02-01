// color swirl! connect an RGB LED to the PWM pins as indicated
// in the #defines
// public domain, enjoy!
 
#define REDPIN 5
#define GREENPIN 6
#define BLUEPIN 3

#define TOTALBRIGHTNESS 50
 
#define FADESPEED 65     // time between steps in ms

int fadeStep=0;
 
void setup() {
  pinMode(REDPIN, OUTPUT);
  pinMode(GREENPIN, OUTPUT);
  pinMode(BLUEPIN, OUTPUT);
  
  noInterrupts();
  
  //set timer1 interrupt at 1Hz
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  OCR1A = 2000;
  // = (16*10^6) * FADESPEED / (1000*1024) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS10 and CS12 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);  
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);

  interrupts();

}
 
 
void loop() { 
 
}

ISR(TIMER1_COMPA_vect){
  if(fadeStep <= TOTALBRIGHTNESS) {
  // fade from blue to red
    int level = fadeStep; 
    analogWrite(REDPIN, level);
    analogWrite(BLUEPIN, TOTALBRIGHTNESS - level);
  } else if(fadeStep <= 2*TOTALBRIGHTNESS) {  
  // fade from red to green
    int level = fadeStep - TOTALBRIGHTNESS;
    analogWrite(GREENPIN, level);
    analogWrite(REDPIN, TOTALBRIGHTNESS - level);
  } else if(fadeStep <= 3*TOTALBRIGHTNESS) {
  // fade from green to blue 
    int level = fadeStep - 2*TOTALBRIGHTNESS;
    analogWrite(BLUEPIN, level);
    analogWrite(GREENPIN, TOTALBRIGHTNESS - level);
  }
  fadeStep = ++fadeStep % (TOTALBRIGHTNESS*3+1);
}

