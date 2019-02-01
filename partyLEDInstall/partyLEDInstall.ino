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

//for beat detection
#define HISTORYWEIGHT 0.9988 //decay to 10% after 2000 samples (~1 second) (0.1^(1/2000))
#define INSTANTWEIGHT 0.977 //decay to 10% after 100 samples (~50 ms)
#define BEATPOWERTHRESHOLD 1.5
#define NBINS_INSTPOWER 50 //50 bins per power sample (~25 ms)
#define NBINS_AVGPOWER 40 //20 avg power bins (40x25ms=1 s)

//for fourier
#define TOTALDUTYSAMPLES 1000
#define MINDUTYCYCLE 0.05
#define MAXDUTYCYCLE 0.80

//for intersong detection
#define INTERSONG_POWER_THRESHOLD 35
#define MIN_INTERSONG_COUNTS 30

//for arrays
#define R 0
#define RED 0
#define G 1
#define GREEN 1
#define B 2
#define BLUE 2

int colorBases[3][3]={{255,0,0},
                      {0,255,0},
                      {0,0,255}};

float instPower[NBINS_INSTPOWER] = {0};
float avgPower[NBINS_AVGPOWER] = {0};
float totalAvgPower = 0;
int instPowerIndex=0;
int avgPowerIndex=0;

float avgLevel = 127;
int fadeStep=0;

float dutyCycle[3] = {0.0, 0.0, 0.0};
int basisOctaves[3] = {7,4,0};
int nDutySamples = 0;
int fht_oct_offsets[8] = {102, 89, 78, 111,  95, 75, 74, 61};
int fht_oct_offset_rms[8] = {2, 2, 2, 1, 1, 1, 1, 1};

boolean beatMode = false;


void turnOnFourierMode(){
  TIMSK0 = 0; // turn off timer0 for lower jitter
  ADCSRA = 0xe5; // set the adc to free running mode
  ADMUX = 0x40; // use adc0
  DIDR0 = 0x01; // turn off the digital input for adc0
  
  noInterrupts();
  //set timer0 interrupt at 10kHz
  TCCR0A = 0;// set entire TCCR2A register to 0
  TCCR0B = 0;// same for TCCR2B
  TCNT0  = 0;//initialize counter value to 0
  // set compare match register for 2khz increments
  OCR0A = 24;// = (16*10^6) / (10000*64) - 1 (must be <256)
  // turn on CTC mode
  TCCR0A |= (1 << WGM01);
  // Set CS01 and CS00 bits for 64 prescaler
  TCCR0B |= (1 << CS01) | (1 << CS00);   
  // enable timer compare interrupt
  TIMSK0 |= (1 << OCIE0A);
  
  interrupts();
}

void turnOffFourierMode(){
  noInterrupts();
  // disable timer0 compare interrupt
  TIMSK0 &= (0 << OCIE0A);
  interrupts();
}

void turnOnBeatMode(){
  beatMode=true;
}

void turnOffBeatMode(){
  beatMode=false;
}

void setup() {

  randomSeed(analogRead(3)+analogRead(5));
  
  pinMode(REDPIN, OUTPUT);
  pinMode(GREENPIN, OUTPUT);
  pinMode(BLUEPIN, OUTPUT);
  
  noInterrupts();

  //set timer2 interrupt at 2kHz
  // used for computing average energy and beat detection
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

  interrupts();
  
  changeMode();
  
}

boolean computeEnergy = false;
boolean inABeat = false;
boolean analyzeFHT = false;
int cyclesGoingDown = 0;
float peakPower = 0.0;
int consecutiveQuiet = 0;
boolean alreadyChangedSong = false;

void loop() {
  
  if(analyzeFHT){
    fht_window(); // window the data for better frequency response
    fht_reorder(); // reorder the data before doing the fht
    fht_run(); // process the data in the fht
    fht_mag_octave(); // take the output of the fht
    
    float newBasis0Level = getOctBasisLevel(basisOctaves[0]);
    float newBasis1Level = getOctBasisLevel(basisOctaves[1]);
    float newBasis2Level = getOctBasisLevel(basisOctaves[2]);
    setColorByBases(newBasis0Level, newBasis1Level, newBasis2Level);
  }

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
    
    if(totalAvgPower < INTERSONG_POWER_THRESHOLD){
      consecutiveQuiet++;
      if(consecutiveQuiet > MIN_INTERSONG_COUNTS){
        if(!alreadyChangedSong){
          changeMode();
          alreadyChangedSong=true;
        } 
      }
    } else {
      consecutiveQuiet = 0;
     alreadyChangedSong=false; 
    }
    
    #if 0
    Serial.print("Inst power: ");
    Serial.print(avgPower[avgPowerIndex]);
    Serial.print(", avg power: ");
    Serial.println(totalAvgPower);
    #endif
    
    if(beatMode){
      if(avgPower[avgPowerIndex] > BEATPOWERTHRESHOLD*totalAvgPower){
        if(!inABeat){
          cycleColor();
          inABeat=true;
          peakPower = avgPower[avgPowerIndex];
        } else {
          if(avgPower[avgPowerIndex] > peakPower){
            peakPower=avgPower[avgPowerIndex];
            if(cyclesGoingDown>5){
              cycleColor(); 
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
    }

    ++avgPowerIndex;
    avgPowerIndex%=NBINS_AVGPOWER;
    computeEnergy=false;
 
  }
}

void changeMode(){
 
  //choose flashy or fourier
  if(random(2)){
    turnOffBeatMode();
    turnOnFourierMode();
  } else {
    turnOffFourierMode();
    turnOnBeatMode();
  }
  
  //choose new bases
  if(random(2)){
    //random bases
    float angle1 = random(628)/100.0;
    float angle2 = random(628)/100.0;
    float angle3 = random(628)/100.0;

    setOrthoBasisEuler(angle1, angle2, angle3);
  } else {
    //rbg bases
    int firstBasis = random(3);
    setBasisRGB(firstBasis,255,0,0);
    
    int secondBasis = random(3);
    while(firstBasis == secondBasis){
      secondBasis = random(3);
    }
    setBasisRGB(secondBasis,0,255,0);
    setBasisRGB(3-firstBasis-secondBasis,0,0,255);
  }
}

int fourierIndex = 0;

//fourier recording at 10khz
ISR(TIMER0_COMPA_vect){
  while(!(ADCSRA & 0x10)); // wait for adc to be ready
  ADCSRA = 0xf5; // restart adc
  byte m = ADCL; // fetch adc data
  byte j = ADCH;
  int k = (j << 8) | m; // form into an int
  k -= 0x0200; // form into a signed int
  k <<= 6; // form into a 16b signed int
  fht_input[fourierIndex] = k; // put real data into bins
  fourierIndex++;
  if(fourierIndex >= FHT_N){
    analyzeFHT=true;
    fourierIndex = 0;
  }
}


//beat detection
ISR(TIMER2_COMPA_vect){
  int analogAudioLevel = analogRead(AUDIOPIN);

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

char colorState = 0;

void cycleColor(){
  ++colorState %= 3;
  if(colorState==0){
    setColorByBases(1.0,0,0);
  } 
  else if (colorState==1){
    setColorByBases(0,1.0,0);
  } 
  else {
    setColorByBases(0,0,1.0);
  }
}

void setColorByBases(float level0, float level1, float level2){
  int redLevel = sqrt(pow(colorBases[0][R]*level0,2) + pow(colorBases[1][R]*level1,2) + pow(colorBases[2][R]*level2,2)); 
  int greenLevel = sqrt(pow(colorBases[0][G]*level0,2) + pow(colorBases[1][G]*level1,2) + pow(colorBases[2][G]*level2,2)); 
  int blueLevel = sqrt(pow(colorBases[0][B]*level0,2) + pow(colorBases[1][B]*level1,2) + pow(colorBases[2][B]*level2,2));
  
  redLevel = (redLevel>255)?255:redLevel;
  greenLevel = (greenLevel>255)?255:greenLevel;
  blueLevel = (blueLevel>255)?255:blueLevel;
  
  setColorRGB(redLevel, greenLevel, blueLevel);
}

#define INPUTFORMID 220

int remapLevel(int inLevel){
   if(inLevel< INPUTFORMID){
    return inLevel*127/INPUTFORMID;
   } else {
    return 127 + (inLevel-INPUTFORMID)*128/(100-INPUTFORMID); 
   }
}

void setColorRGB(int rLevel, int gLevel, int bLevel){
  int mappedRed=remapLevel(rLevel);
  int mappedGreen=remapLevel(gLevel);
  int mappedBlue=remapLevel(bLevel);
  analogWrite(REDPIN, remapLevel(mappedRed));
  analogWrite(GREENPIN, remapLevel(mappedGreen));
  analogWrite(BLUEPIN, remapLevel(mappedBlue));
  
  //Serial.print(rLevel);
  //Serial.print(", ");
  //Serial.print(gLevel);
  //Serial.print(", ");
  //Serial.println(bLevel);
}

void setBasisRGB(int basis, int basisR, int basisG, int basisB){
#if 0
  Serial.print("Basis ");
  Serial.print(basis);
  Serial.print("{");
  Serial.print(basisR);
  Serial.print(", ");
  Serial.print(basisG);
  Serial.print(", ");
  Serial.print(basisB);
  Serial.println("}");
#endif
  
  colorBases[basis][R] = (int)abs(basisR);
  colorBases[basis][G] = (int)abs(basisG);
  colorBases[basis][B] = (int)abs(basisB);
}

void setOrthoBasisEuler(float alpha, float beta, float gamma){
  float c1 = cos(alpha);
  float c2 = cos(beta);
  float c3 = cos(gamma);
  float s1 = sin(alpha);
  float s2 = sin(beta);
  float s3 = sin(gamma);
  
  setBasisRGB(0, 255*c2, 255*(c1*s2), 255*(s1*s2));
  setBasisRGB(1, 255*(-c3*s2), 255*(c1*c2*c3-s1*s3), 255*(c1*s3+c2*c3*s1));
  setBasisRGB(2, 255*(s2*s3), 255*(-c3*s1-c1*c2*s3), 255*(c1*c3-c2*s1*s3));
  
}


float getOctBasisLevel(int octBin){
  int signal=fht_oct_out[octBin]-fht_oct_offsets[octBin];
  if(signal>3*fht_oct_offset_rms[octBin]){
   return signal/1024.0;
  } else {
   return 0; 
  }
}


