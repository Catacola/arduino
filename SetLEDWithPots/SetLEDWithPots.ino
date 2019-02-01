// color swirl! connect an RGB LED to the PWM pins as indicated
// in the #defines
// public domain, enjoy!
 
#define RED_POT 5 //analog
#define GREEN_POT 2 //analog
#define BLUE_POT 4 //analog

#define HUE_POT 5 //analog
#define SAT_POT 2 //analog
#define VAL_POT 4 //analog

#define STEP_POT 5 //analog
#define RATE_POT 2 //analog

#define DELAY_POT 2 //analog

#define MODE1_PIN 8 //digital
#define MODE2_PIN 10 //digital
#define MODE3_PIN 11 //digital
#define MODE4_PIN 12 //digital

#define RED_LED_PIN 5 //digital
#define GREEN_LED_PIN 6 //digital
#define BLUE_LED_PIN 9 //digital

#define AUDIO_PIN 0 //analog

#define MIDLEVEL 85

//for arrays
#define R 0
#define RED 0
#define G 1
#define GREEN 1
#define B 2
#define BLUE 2

//FHT variables
#define OCTAVE 1 // use the octave output function
#define OCT_NORM 1 //renorm please
#define LIN_OUT8 0
#define SCALE 1
#define FHT_N 256 // set to 256 point fht

#include <FHT.h>

#define TOTALDUTYSAMPLES 1000
#define MINDUTYCYCLE 0.33
#define MAXDUTYCYCLE 0.66

#define DEBUG 0

float dutyCycle[3] = {0.0, 0.0, 0.0};
int colorOctaves[3] = {7,4,0};
int nDutySamples = 0;
int fht_oct_offsets[8] = {102, 89, 78, 111,  95, 75, 74, 61};
int fht_oct_offset_rms[8] = {2, 2, 2, 1, 1, 1, 1, 1};
     
void setup() {
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(BLUE_LED_PIN, OUTPUT);
  
  pinMode(MODE1_PIN, INPUT_PULLUP);
  pinMode(MODE2_PIN, INPUT_PULLUP);
  pinMode(MODE3_PIN, INPUT_PULLUP);
  pinMode(MODE4_PIN, INPUT_PULLUP);
  
  //setup analog pin0 for fast reading
  //TIMSK0 = 0; // turn off timer0 for lower jitter
  //ADCSRA = 0xe5; // set the adc to free running mode
  //ADMUX = 0x40; // use adc0
  //DIDR0 = 0x01; // turn off the digital input for adc0
  
  #if DEBUG
  Serial.begin(9600);
  #endif
}
 
void loop() {
  #if DEBUG
  Serial.print("Pot 1: ");
  Serial.print(analogRead(RED_POT));
  Serial.print(", Pot 2: ");
  Serial.print(analogRead(GREEN_POT));
  Serial.print(", Pot 3: ");
  Serial.println(analogRead(BLUE_POT));
  #endif
  if(!digitalRead(MODE1_PIN)){
    //put 1st mode here
    setHSVFromPots();
    Serial.println("Mode 1");
  } else if(!digitalRead(MODE2_PIN)){
    //put 2nd mode here
    stepHueSmooth();
    Serial.println("Mode 2");
  } else if(!digitalRead(MODE3_PIN)){
    //put 3rd mode here
    cycleColors();
    Serial.println("Mode 3");
  } else if(!digitalRead(MODE4_PIN)){
    //put 4th mode here
    setFromFHT();
    Serial.println("Mode 4");
  } else {
    //mode0
    setRGBFromPots();
    Serial.println("Mode 0");
  }
}

/*
Sets LED levels based on audio input
*/
void setFromFHT(){
  //cli();  // UDRE interrupt slows this way down on arduino1.0
  for (int i = 0 ; i < FHT_N ; i++) { // save 256 samples
    //while(!(ADCSRA & 0x10)); // wait for adc to be ready
    //ADCSRA = 0xf5; // restart adc
    //byte m = ADCL; // fetch adc data
    //byte j = ADCH;
    //int k = (j << 8) | m; // form into an int
    //k -= 0x0200; // form into a signed int
    //k <<= 6; // form into a 16b signed int
    //fht_input[i] = k; // put real data into bins
    fht_input[i] = analogRead(AUDIO_PIN); // put real data into bins
  }
  fht_window(); // window the data for better frequency response
  fht_reorder(); // reorder the data before doing the fht
  fht_run(); // process the data in the fht
  //fht_mag_log(); // take the output of the fht
  fht_mag_octave();
  //fht_mag_lin8();  
  //sei();
  
  float newBlueLevel = getOctLEDLevel(colorOctaves[B]);
  float newGreenLevel = getOctLEDLevel(colorOctaves[G]);
  float newRedLevel = getOctLEDLevel(colorOctaves[R]);
  
  float redBoost = pow(analogRead(RED_POT)/512.0,2);
  float greenBoost = pow(analogRead(GREEN_POT)/512.0,2);
  float blueBoost = pow(analogRead(BLUE_POT)/512.0,2);
  
  setLevelRGB(newRedLevel*redBoost, newGreenLevel*greenBoost, newBlueLevel*blueBoost);
  updateDynamicThresholds(newRedLevel, newGreenLevel, newBlueLevel);
}

/*
Returns a float specifying the level of the channel of the octave
*/

float getOctLEDLevel(int octBin){
  int signal=fht_oct_out[octBin]-fht_oct_offsets[octBin];
  if(signal>3*fht_oct_offset_rms[octBin]){
   return signal/256.0;
  } else {
   return 0; 
  }
}

/*
Updates dynamic thresholds if need be
*/
void updateDynamicThresholds(float rSignal, float gSignal, float bSignal){

  dutyCycle[R] = (nDutySamples*dutyCycle[R]+(rSignal>0.05?1:0))/(nDutySamples+1);
  dutyCycle[G] = (nDutySamples*dutyCycle[G]+(gSignal>0.05?1:0))/(nDutySamples+1);
  dutyCycle[B] = (nDutySamples*dutyCycle[B]+(bSignal>0.05?1:0))/(nDutySamples+1);
  nDutySamples++;
  
  if(nDutySamples>=TOTALDUTYSAMPLES){
    for(int c=0; c<3; c++){
      if(dutyCycle[c] < MINDUTYCYCLE){
        fht_oct_offsets[colorOctaves[c]] -=5;
      } else if(dutyCycle[c] > MAXDUTYCYCLE){
        fht_oct_offsets[colorOctaves[c]] +=5; 
      }
      dutyCycle[c]=0.0;
    }
    nDutySamples=0;
  }
}

/*
Steps the colors around the hue wheel bases on pots
*/
float currentHue=0.0;

void stepHueSmooth(){
  int stepLevel = analogRead(STEP_POT)-512;
  float divisor = 4096.0 - (abs(stepLevel)/512.0)*3692;
  currentHue += stepLevel/divisor;
  
  currentHue = currentHue > 360.0 ? currentHue - 360.0 : currentHue;
  currentHue = currentHue < 0.0 ? currentHue + 360.0 : currentHue;
  float valLevel = analogRead(VAL_POT)/1024.0;
  setLevelHSV(currentHue,1,valLevel);
  delay(1);
}

void cycleColors(){
  float hueStepSize = (analogRead(STEP_POT)-512)/512.0*180;
  int delayTime = 333 + analogRead(DELAY_POT)*10;
  currentHue += hueStepSize;
  
  currentHue = currentHue > 360.0 ? currentHue - 360.0 : currentHue;
  currentHue = currentHue < 0.0 ? currentHue + 360.0 : currentHue;
  float valLevel = analogRead(VAL_POT)/1024.0;
  setLevelHSV(currentHue,1,valLevel);
  delay(delayTime);
}

/*
Sets lights using the pots for an HSV input
*/
void setHSVFromPots(){
  float hueLevel = analogRead(HUE_POT)/1024.0*360;
  float satLevel = analogRead(SAT_POT)/1024.0;
  float valLevel = analogRead(VAL_POT)/1024.0;
  
  setLevelHSV(hueLevel,satLevel,valLevel);
}

/*
Sets colors based on hue (0-360), saturation (0-1), and value (0-1)
*/
void setLevelHSV(float hueLevel, float satLevel, float valLevel){
  float redLevel, greenLevel, blueLevel;
  
  HSVtoRGB(&redLevel, &greenLevel, &blueLevel, hueLevel, satLevel, valLevel);
  setLevelRGB(redLevel,blueLevel,greenLevel);
}

/*
Converts h(0-360), s(0-1), and v(0-1) to r(0-1), g(0-1), and b(0-1)
*/
void HSVtoRGB( float *r, float *g, float *b, float h, float s, float v )
{
	int i;
	float f, p, q, t;
	if( s == 0 ) {
		// achromatic (grey)
		*r = *g = *b = v;
		return;
	}
	h /= 60;			// sector 0 to 5
	i = floor( h );
	f = h - i;			// factorial part of h
	p = v * ( 1 - s );
	q = v * ( 1 - s * f );
	t = v * ( 1 - s * ( 1 - f ) );
	switch( i ) {
		case 0:
			*r = v;
			*g = t;
			*b = p;
			break;
		case 1:
			*r = q;
			*g = v;
			*b = p;
			break;
		case 2:
			*r = p;
			*g = v;
			*b = t;
			break;
		case 3:
			*r = p;
			*g = q;
			*b = v;
			break;
		case 4:
			*r = t;
			*g = p;
			*b = v;
			break;
		default:		// case 5:
			*r = v;
			*g = p;
			*b = q;
			break;
	}
}

/*
Sets RBG of lights directly from pot levels
*/
void setRGBFromPots(){
  float redLevel = analogRead(RED_POT)/1024.0;
  float greenLevel = analogRead(GREEN_POT)/1024.0;
  float blueLevel = analogRead(BLUE_POT)/1024.0;
  
  //Serial.print(redLevel);
  //Serial.print(" ");
  //Serial.print(greenLevel);
  //Serial.print(" ");
  //Serial.println(blueLevel);

  setLevelRGB(redLevel,greenLevel,blueLevel);
}

/*
Turns a float level (0-1) into an int level (0-255)
remapped so brightness is semi-linear in float level
*/
int getIntLevel(float fLevel){
  if(fLevel < 0.0) {
    return 0;
  } else if(fLevel< 0.5){
    return (int)(fLevel/0.5*MIDLEVEL);
  } else if(fLevel<1.0){
    return (int)(MIDLEVEL + (fLevel-0.5)/0.5*(255-MIDLEVEL));
  } else {
    return 255;
  }
}

/*
Sets the RBG level from floats (0-1)
*/
#define INT_THRESH 5
void setLevelRGB(float rLevel, float gLevel, float bLevel){
  int redInt = getIntLevel(rLevel);
  int greenInt = getIntLevel(gLevel);
  int blueInt = getIntLevel(bLevel);

  if(redInt > INT_THRESH){
    analogWrite(RED_LED_PIN, redInt);
  } else {
    analogWrite(RED_LED_PIN, 0);
  }
  
  if(greenInt > INT_THRESH){
    analogWrite(GREEN_LED_PIN, greenInt);
  } else {
    analogWrite(GREEN_LED_PIN, 0);
  }
  
  if(blueInt > INT_THRESH){
    analogWrite(BLUE_LED_PIN, blueInt);
  } else {
    analogWrite(BLUE_LED_PIN, 0);
  }

}
