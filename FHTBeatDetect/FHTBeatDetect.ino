

// color swirl! connect an RGB LED to the PWM pins as indicated
// in the #defines
// public domain, enjoy!

#define OCTAVE 1 // use the octave output function
#define OCT_NORM 1 //renorm please
#define FHT_N 256 // set to 256 point fht

#include <FHT.h> // include the library
 
#define REDPIN 5
#define GREENPIN 6
#define BLUEPIN 9

#define AUDIOPIN 0

#define NBINS_AVGPOWER 40

#define MIDLEVEL 85


//for arrays
#define R 0
#define RED 0
#define G 1
#define GREEN 1
#define B 2
#define BLUE 2

float colorBases[3][3]={{1.0,   0,   0},
                        {  0, 1.0,   0},
                        {  0,   0, 1.0}};

//float dutyCycle[3] = {0.0, 0.0, 0.0};
//int colorOctaves[3] = {7,4,0};
//int nDutySamples = 0;
//int fht_oct_offsets[8] = {102, 89, 78, 111,  95, 75, 74, 61};
//int fht_oct_offset_rms[8] = {2, 2, 2, 1, 1, 1, 1, 1};

void setup() {
  pinMode(REDPIN, OUTPUT);
  pinMode(GREENPIN, OUTPUT);
  pinMode(BLUEPIN, OUTPUT);
 
  Serial.begin(9600);
  //setup analog pin0 for fast reading
  //TIMSK0 = 0; // turn off timer0 for lower jitter
  ADCSRA = 0xe5; // set the adc to free running mode
  ADMUX = 0x40; // use adc0
  DIDR0 = 0x01; // turn off the digital input for adc0
  
  initializeArrays();
}

unsigned char powerPerBand[8][NBINS_AVGPOWER];
float avgPowerPerBand[8];
int avgPowerIndex = 0;

void initializeArrays(){
  for(int i=0;i<8;i++){
    avgPowerPerBand[i]=0;
    for(int j=0; j<NBINS_AVGPOWER; j++){
      powerPerBand[i][j]=0;
    }
  }
}

void calculatePowerPerBand(){
  for(int i=0; i<8; i++){
    avgPowerPerBand[i] -= powerPerBand[i][avgPowerIndex]*1.0/NBINS_AVGPOWER;
    powerPerBand[i][avgPowerIndex] = fht_oct_out[i];
    avgPowerPerBand[i] += powerPerBand[i][avgPowerIndex]*1.0/NBINS_AVGPOWER;
    Serial.print("Bin: ");
    Serial.print(i);
    Serial.print(", level: ");
    Serial.print(fht_oct_out[i]);
    Serial.print(", avg level: ");
    Serial.println(avgPowerPerBand[i]);
  }
  ++avgPowerIndex%=NBINS_AVGPOWER;
  delay(1000);
}

void detectBeat(){
  for(int i=0; i<8; i++){
    if(fht_oct_out[i] > 6,0*avgPowerPerBand[i]){
      cycleColor();
      break;
    }
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
  float redLevel = colorBases[0][R]*level0 + colorBases[1][R]*level1 + colorBases[2][R]*level2; 
  float greenLevel = colorBases[0][G]*level0 + colorBases[1][G]*level1 + colorBases[2][G]*level2; 
  float blueLevel = colorBases[0][B]*level0 + colorBases[1][B]*level1 + colorBases[2][B]*level2;
  
//  redLevel = (redLevel>1.0)?1.0:redLevel;
//  greenLevel = (greenLevel>1.0)?1.0:greenLevel;
//  blueLevel = (blueLevel>1.0)?1.0:blueLevel;
  
  setLevelRGB(redLevel, greenLevel, blueLevel);
}
 
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
  fht_mag_octave(); // take the output of the fht
  sei();
    
  calculatePowerPerBand();
  detectBeat();

}

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

void setLevelRGB(float rLevel, float gLevel, float bLevel){
  analogWrite(REDPIN,getIntLevel(rLevel));
  analogWrite(GREENPIN,getIntLevel(gLevel));
  analogWrite(BLUEPIN,getIntLevel(bLevel));
}

