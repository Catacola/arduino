

// color swirl! connect an RGB LED to the PWM pins as indicated
// in the #defines
// public domain, enjoy!

#define OCTAVE 1 // use the octave output function
#define OCT_NORM 1 //renorm please
#define LIN_OUT8 0
#define SCALE 1
#define FHT_N 256 // set to 256 point fht

#include <FHT.h> // include the library
 
#define REDPIN 5
#define GREENPIN 6
#define BLUEPIN 9

#define AUDIOPIN 0
#define LEVELSAMPLES 1
#define BRIGHTNESSSCALE 3

#define TOTALDUTYSAMPLES 1000
#define MINDUTYCYCLE 0.40
#define MAXDUTYCYCLE 0.60

//for arrays
#define R 0
#define RED 0
#define G 1
#define GREEN 1
#define B 2
#define BLUE 2

float dutyCycle[3] = {0.0, 0.0, 0.0};
int colorOctaves[3] = {7,4,0};
int nDutySamples = 0;
int fht_oct_offsets[8] = {102, 89, 78, 111,  95, 75, 74, 61};
int fht_oct_offset_rms[8] = {2, 2, 2, 1, 1, 1, 1, 1};


void setup() {
  pinMode(REDPIN, OUTPUT);
  pinMode(GREENPIN, OUTPUT);
  pinMode(BLUEPIN, OUTPUT);
 
  //Serial.begin(9600);
  //setup analog pin0 for fast reading
  TIMSK0 = 0; // turn off timer0 for lower jitter
  ADCSRA = 0xe5; // set the adc to free running mode
  ADMUX = 0x40; // use adc0
  DIDR0 = 0x01; // turn off the digital input for adc0
}

void checkDynamicThresholds(int rSignal, int gSignal, int bSignal){

  dutyCycle[R] = (nDutySamples*dutyCycle[R]+(rSignal>0?1:0))/(nDutySamples+1);
  dutyCycle[G] = (nDutySamples*dutyCycle[G]+(gSignal>0?1:0))/(nDutySamples+1);
  dutyCycle[B] = (nDutySamples*dutyCycle[B]+(bSignal>0?1:0))/(nDutySamples+1);
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

#define OUTPUTATMID 85

int remapLevel(int inLevel){
   if(inLevel< 127){
    return inLevel*OUTPUTATMID/127;
   } else {
    return OUTPUTATMID + (inLevel-127)*(255-OUTPUTATMID)/128; 
   }
}

void setColorRGB(int rLevel, int gLevel, int bLevel){
  int mappedRed=remapLevel(rLevel);
  int mappedGreen=remapLevel(gLevel);
  int mappedBlue=remapLevel(bLevel);
  analogWrite(REDPIN, remapLevel(mappedRed));
  analogWrite(GREENPIN, remapLevel(mappedGreen));
  analogWrite(BLUEPIN, remapLevel(mappedBlue));
  
  checkDynamicThresholds(mappedRed,mappedGreen,mappedBlue);
}

int getOctLEDLevel(int octBin){
  int signal=fht_oct_out[octBin]-fht_oct_offsets[octBin];
  if(signal>3*fht_oct_offset_rms[octBin]){
   return signal/BRIGHTNESSSCALE;
  } else {
   return 0; 
  }
}

int newRedLevel=0;
int newGreenLevel=0;
int newBlueLevel=0;
 
void loop() { 
  cli();  // UDRE interrupt slows this way down on arduino1.0
  for (int i = 0 ; i < FHT_N ; i++) { // save 256 samples
    while(!(ADCSRA & 0x10)); // wait for adc to be ready
    ADCSRA = 0xf5; // restart adc
    byte m = ADCL; // fetch adc data
    byte j = ADCH;
    int k = (j << 8) | m; // form into an int
    k -= 0x0200; // form into a signed int
    k <<= 6; // form into a 16b signed int
    fht_input[i] = k; // put real data into bins
  }
  fht_window(); // window the data for better frequency response
  fht_reorder(); // reorder the data before doing the fht
  fht_run(); // process the data in the fht
  //fht_mag_log(); // take the output of the fht
  fht_mag_octave();
  //fht_mag_lin8();  
  sei();
  
  newBlueLevel = getOctLEDLevel(colorOctaves[B]);
  newGreenLevel = getOctLEDLevel(colorOctaves[G]);
  newRedLevel = getOctLEDLevel(colorOctaves[R]);
  
  setColorRGB(newRedLevel, newGreenLevel, newBlueLevel);
  
  
     
/*    Serial.print(sampleLoops);
    Serial.print(" samples. RGB levels: ");
    Serial.print(newRedLevel/(sampleLoops));
    Serial.print(", ");
    Serial.print(newGreenLevel/(sampleLoops));
    Serial.print(", ");
    Serial.print(newBlueLevel/(sampleLoops));
    Serial.println("");*/
     

}

