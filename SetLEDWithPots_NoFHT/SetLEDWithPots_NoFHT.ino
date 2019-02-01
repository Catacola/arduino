// color swirl! connect an RGB LED to the PWM pins as indicated
// in the #defines
// public domain, enjoy!

#define POT0_PIN 5
#define POT1_PIN 2
#define POT2_PIN 4
 
#define RED_POT 0
#define GREEN_POT 1
#define BLUE_POT 2

#define HUE_POT 0
#define SAT_POT 1
#define VAL_POT 2

#define STEP_POT 0
#define RATE_POT 1

#define DELAY_POT 1

#define THRESH_POT 1

#define MODE0_PIN 8 //digital
#define MODE1_PIN 10 //digital
#define MODE2_PIN 11 //digital
#define MODE3_PIN 12 //digital

#define RED_LED_PIN 5 //digital
#define GREEN_LED_PIN 6 //digital
#define BLUE_LED_PIN 9 //digital

#define AUDIO_PIN 0 //analog

#define HISTORYWEIGHT 0.9988 //decay to 10% after 2000 samples (~1 second) (0.1^(1/2000))
#define INSTANTWEIGHT 0.977 //decay to 10% after 100 samples (~50 ms)
#define BEATPOWERTHRESHOLD 1.6
#define NBINS_INSTPOWER 50 //50 bins per power sample (~25 ms)
#define NBINS_AVGPOWER 40 //20 avg power bins (40x25ms=1 s)

struct sensorData_t {
  float pot[3];
  bool mode_pin[4];
} controls;

float instPower[NBINS_INSTPOWER] = {0};
float avgPower[NBINS_AVGPOWER] = {0};
float totalAvgPower=0;
int instPowerIndex=0;
int avgPowerIndex=0;

float avgLevel = 512;

#define MIDLEVEL 85

//for arrays
#define R 0
#define RED 0
#define G 1
#define GREEN 1
#define B 2
#define BLUE 2

#define DEBUG 1
     
void setup() {
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(BLUE_LED_PIN, OUTPUT);
  
  pinMode(MODE0_PIN, INPUT_PULLUP);
  pinMode(MODE1_PIN, INPUT_PULLUP);
  pinMode(MODE2_PIN, INPUT_PULLUP);
  pinMode(MODE3_PIN, INPUT_PULLUP);
  
  noInterrupts();
#if 0
  //set timer2 interrupt at 8kHz
  TCCR2A = 0;// set entire TCCR2A register to 0
  TCCR2B = 0;// same for TCCR2B
  TCNT2  = 0;//initialize counter value to 0
  // set compare match register for 8khz increments
  OCR2A = 124;// = (16*10^6) / (2000*64) - 1 (must be <256)
  // turn on CTC mode
  TCCR2A |= (1 << WGM21);
  // Set CS22 bit for 64 prescaler
  TCCR2B |= (1 << CS22);   
  // enable timer compare interrupt
  TIMSK2 |= (1 << OCIE2A);
#endif
  interrupts();
  
  #if DEBUG
  Serial.begin(9600);
  #endif
}
 
void loop() {
  
  readSensors();
  
  if(!controls.mode_pin[0]){
    //put 1st mode here
    setHSVFromPots();
    Serial.println("Mode 1");
  } else if(!controls.mode_pin[1]){
    //put 2nd mode here
    cycleHueSmooth();
    Serial.println("Mode 2");
  } else if(!controls.mode_pin[2]){
    //put 3rd mode here
    stepColors();
    Serial.println("Mode 3");
  } else if(!controls.mode_pin[3]){
    //put 4th mode here
    stepOnBeat();
    Serial.println("Mode 4");
  } else {
    //mode0
    setRGBFromPots();
    Serial.println("Mode 0");
  }
}


/*
Steps the colors around the hue wheel bases on pots
*/
float currentHue=0.0;

void cycleHueSmooth(){
  int stepLevel = controls.pot[STEP_POT]-512;
  float divisor = 20000.0 - (abs(stepLevel)/512.0)*19900.0;
  currentHue += stepLevel/divisor;
  
  currentHue = currentHue > 360.0 ? currentHue - 360.0 : currentHue;
  currentHue = currentHue < 0.0 ? currentHue + 360.0 : currentHue;
  float valLevel = correctForNoise(controls.pot[VAL_POT]/1024.0);
  setLevelHSV(currentHue,1,valLevel);
  delay(1);
}

unsigned long timeOfLastStep=0;

void stepColors(){

  int delayTime = 333 + controls.pot[DELAY_POT]*10;
  
  if(millis()-timeOfLastStep > delayTime){
    float hueStepSize = (controls.pot[STEP_POT]-512)/512.0*180;
    currentHue += hueStepSize;
    currentHue = currentHue > 360.0 ? currentHue - 360.0 : currentHue;
    currentHue = currentHue < 0.0 ? currentHue + 360.0 : currentHue;
    timeOfLastStep=millis();
  }
  
  float valLevel = correctForNoise(controls.pot[VAL_POT]/1024.0);
  setLevelHSV(currentHue,1,valLevel);

}

boolean computeEnergy = false;
boolean inABeat = false;
int cyclesGoingDown = 0;
float peakPower = 0.0;

void stepOnBeat(){
  if(computeEnergy){
    //compute average power for these bins and update total average

    //remove old average from the total
    totalAvgPower -= avgPower[avgPowerIndex]/NBINS_AVGPOWER;
    //reset the bin
    avgPower[avgPowerIndex] = 0;
    for(int i=0; i<NBINS_INSTPOWER; i++){
      avgPower[avgPowerIndex]+=instPower[i];
    }
    avgPower[avgPowerIndex] /= NBINS_INSTPOWER;
    //update the total average
    totalAvgPower += avgPower[avgPowerIndex]/NBINS_AVGPOWER;
    
    float threshCoeff = 1.3 + 9.0*controls.pot[THRESH_POT]/1024.0;
    
    if(avgPower[avgPowerIndex] > threshCoeff*totalAvgPower){
      if(!inABeat){
        cycleColor_beatHelper();
        inABeat=true;
        peakPower = avgPower[avgPowerIndex];
      } else {
        if(avgPower[avgPowerIndex] > peakPower){
          peakPower=avgPower[avgPowerIndex];
          if(cyclesGoingDown>5){
            cycleColor_beatHelper(); 
          }
          cyclesGoingDown=0;
        } else {
          cyclesGoingDown++;
          peakPower = avgPower[avgPowerIndex];
        }
      }
    } else {
      inABeat=false;
      cyclesGoingDown=0;
    }

    ++avgPowerIndex;
    avgPowerIndex%=NBINS_AVGPOWER;
    computeEnergy=false;
 
  }
}

void cycleColor_beatHelper(){
  float hueStepSize = (controls.pot[STEP_POT]-512)/512.0*180;
  currentHue += hueStepSize;
  
  currentHue = currentHue > 360.0 ? currentHue - 360.0 : currentHue;
  currentHue = currentHue < 0.0 ? currentHue + 360.0 : currentHue;
  float valLevel = correctForNoise(controls.pot[VAL_POT]/1024.0);
  setLevelHSV(currentHue,1,valLevel);
}

/*
Interrupt funtion for beat detection
*/
ISR(TIMER2_COMPA_vect){
  int analogAudioLevel = analogRead(AUDIO_PIN);

  //no audio signal should be 127, so we subtract the average level
  float instSignal = analogAudioLevel-avgLevel;
  //and update the avg level
  avgLevel = avgLevel*HISTORYWEIGHT + analogAudioLevel*(1-HISTORYWEIGHT);

  //now compute the power
  instPower[instPowerIndex] = instSignal*instSignal;
  instPowerIndex++;
  if(instPowerIndex >= NBINS_INSTPOWER){
    computeEnergy=true;
    instPowerIndex=0;
  }
}

/*
Sets lights using the pots for an HSV input
*/
void setHSVFromPots(){
  float hueLevel = controls.pot[HUE_POT]/1024.0*360;
  float satLevel = controls.pot[SAT_POT]/1024.0;
  float valLevel = controls.pot[VAL_POT]/1024.0;
  
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
#define NOISE_THRESH 0.10

void setRGBFromPots(){
  float newRedLevel = controls.pot[RED_POT]/1024.0;
  float newGreenLevel = controls.pot[GREEN_POT]/1024.0;
  float newBlueLevel = controls.pot[BLUE_POT]/1024.0;

  newRedLevel=correctForNoise(newRedLevel);
  newGreenLevel=correctForNoise(newGreenLevel);
  newBlueLevel=correctForNoise(newBlueLevel);
    
  setLevelRGB(newRedLevel,newGreenLevel,newBlueLevel);

}

float correctForNoise(float rawMean){
  if(rawMean < NOISE_THRESH){
    return 0.0;
  } else {
  return (rawMean-NOISE_THRESH)/(1.0-NOISE_THRESH);
  }
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
#define INT_THRESH 0
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

/*
  Read sensors and populate controls structure
*/
#define ANALOG_SAMPLES_PER_AVG 100
float pot0Mean=0.0, pot1Mean=0.0, pot2Mean=0.0;
int nAnalogSamples=0;

void readSensors(){
  
  controls.mode_pin[0] = digitalRead(MODE0_PIN);
  controls.mode_pin[1] = digitalRead(MODE1_PIN);
  controls.mode_pin[2] = digitalRead(MODE2_PIN);
  controls.mode_pin[3] = digitalRead(MODE3_PIN);

  
  pot0Mean+=analogRead(POT0_PIN);
  pot1Mean+=analogRead(POT1_PIN);
  pot2Mean+=analogRead(POT2_PIN);
  nAnalogSamples++;
  
  if(nAnalogSamples >= ANALOG_SAMPLES_PER_AVG){
    controls.pot[0]=pot0Mean/nAnalogSamples;
    controls.pot[1]=pot1Mean/nAnalogSamples;
    controls.pot[2]=pot2Mean/nAnalogSamples;
    
    pot0Mean=pot1Mean=pot2Mean=0.0;
    
    nAnalogSamples=0;
  } 
}
