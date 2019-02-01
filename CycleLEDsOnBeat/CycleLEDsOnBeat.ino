// color swirl! connect an RGB LED to the PWM pins as indicated
// in the #defines
// public domain, enjoy!

#define REDPIN 5
#define GREENPIN 6
#define BLUEPIN 9

#define AUDIOPIN 0

#define HISTORYWEIGHT 0.9988 //decay to 10% after 2000 samples (~1 second) (0.1^(1/2000))
#define INSTANTWEIGHT 0.977 //decay to 10% after 100 samples (~50 ms)
#define BEATPOWERTHRESHOLD 1.6
#define NBINS_INSTPOWER 50 //50 bins per power sample (~25 ms)
#define NBINS_AVGPOWER 40 //20 avg power bins (40x25ms=1 s)

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

void setup() {

  randomSeed(analogRead(3)+analogRead(5));
  
  float angle1 = random(628)/100.0;
  float angle2 = random(628)/100.0;
  float angle3 = random(628)/100.0;

  Serial.begin(9600);
    
  setOrthoBasisEuler(angle1, angle2, angle3); 
  
  pinMode(REDPIN, OUTPUT);
  pinMode(GREENPIN, OUTPUT);
  pinMode(BLUEPIN, OUTPUT);
  
  noInterrupts();
#if 1
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
  
}

boolean computeEnergy = false;
boolean inABeat = false;
int cyclesGoingDown = 0;
float peakPower = 0.0;

void loop() {

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
    
    Serial.print("Inst power: ");
    Serial.print(avgPower[avgPowerIndex]);
    Serial.print(", avg power: ");
    Serial.println(totalAvgPower);
    
    
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

    ++avgPowerIndex;
    avgPowerIndex%=NBINS_AVGPOWER;
    computeEnergy=false;
 
  }
}

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
  int redLevel = colorBases[0][R]*level0 + colorBases[1][R]*level1 + colorBases[2][R]*level2; 
  int greenLevel = colorBases[0][G]*level0 + colorBases[1][G]*level1 + colorBases[2][G]*level2; 
  int blueLevel = colorBases[0][B]*level0 + colorBases[1][B]*level1 + colorBases[2][B]*level2;
  
  redLevel = (redLevel>255)?255:redLevel;
  greenLevel = (greenLevel>255)?255:greenLevel;
  blueLevel = (blueLevel>255)?255:blueLevel;
  
  setColorRGB(redLevel, greenLevel, blueLevel);
}

#define INPUTFORMID 201

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


