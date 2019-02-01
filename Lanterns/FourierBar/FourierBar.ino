#include <Adafruit_NeoPixel.h>

//for arrays
#define R 0
#define RED 0
#define G 1
#define GREEN 1
#define B 2
#define BLUE 2

//physical things
#define N_PIXELS 13
#define N_ROWS 4
#define PIN 3 //digital
#define AUDIO_PIN 0 //analog

//FHT library defines
#define OCTAVE 1 // use the octave output function
#define OCT_NORM 1 //renorm please
#define LIN_OUT8 0
#define SCALE 1
#define FHT_N 256 // set to 256 point fht

#include <FHT.h> //must come after FHT library defines

//my FHT defines
#define MIDLEVEL 85
#define TOTALDUTYSAMPLES 1000
#define MINDUTYCYCLE 0.15
#define MAXDUTYCYCLE 0.70

#define MINAVG 0.35
#define MAXAVG 0.65

//FHT variables
float dutyCycle[3] = {0.0, 0.0, 0.0};
float avgLevel[3] = {0.0, 0.0, 0.0};
int colorOctaves[3] = {7,4,0};
int nDutySamples = 0;
int fht_oct_offsets[8] = {152, 140, 130, 121,  110, 95, 80, 71};
int fht_oct_offset_rms[8] = {2, 2, 2, 1, 1, 1, 1, 1};
int fht_oct_scale[8] = {96, 96, 96, 96, 96, 96, 96, 96};

Adafruit_NeoPixel strip = Adafruit_NeoPixel(N_PIXELS*N_ROWS, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  TIMSK0 = 0; // turn off timer0 for lower jitter
  ADCSRA = 0xe5; // set the adc to free running mode
  ADMUX = 0x40; // use adc0
  DIDR0 = 0x01; // turn off the digital input for adc0
  
  Serial.begin(9600);
  strip.begin();
  strip.show();
}

void loop() {
  
  setPixels();
 
}

void setPixels() {
  while(1) { // reduces jitter
    cli();  // UDRE interrupt slows this way down on arduino1.0
    for (int i = 0 ; i < FHT_N ; i++) {
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
    
    Serial.print("analig: ");
    Serial.println(fht_input[0]);
    
    float newBlueLevel = getOctLEDLevel(colorOctaves[B]);
    float newGreenLevel = getOctLEDLevel(colorOctaves[G]);
    float newRedLevel = getOctLEDLevel(colorOctaves[R]);

    Serial.print("octaves in: ");  
    Serial.print(fht_oct_out[colorOctaves[R]]);
    Serial.print(", ");
    Serial.print(fht_oct_out[colorOctaves[G]]);
    Serial.print(", ");
    Serial.print(fht_oct_out[colorOctaves[B]]);
//  
//    Serial.print(" :::: levels in: ");  
//    Serial.print(newRedLevel);
//    Serial.print(", ");
//    Serial.print(newGreenLevel);
//    Serial.print(", ");
//    Serial.println(newBlueLevel);
  
    setLevelRGB(newRedLevel, newGreenLevel, newBlueLevel);
    updateDynamicThresholds(newRedLevel, newGreenLevel, newBlueLevel);
    
    strip.show();
    //delay(100);
  }
}

/*
Returns a float specifying the level of the channel of the octave
*/
float getOctLEDLevel(int octBin){
  int signal=fht_oct_out[octBin]-fht_oct_offsets[octBin];

  if(signal>3*fht_oct_offset_rms[octBin]){
    return (float)signal/fht_oct_scale[octBin];
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
  
  avgLevel[R] = (nDutySamples*avgLevel[R]+(rSignal>0.05?rSignal:0))/(nDutySamples+1);
  avgLevel[G] = (nDutySamples*avgLevel[G]+(gSignal>0.05?gSignal:0))/(nDutySamples+1);
  avgLevel[B] = (nDutySamples*avgLevel[B]+(bSignal>0.05?bSignal:0))/(nDutySamples+1);

  nDutySamples++;
  
  if(nDutySamples>=TOTALDUTYSAMPLES){
    for(int c=0; c<3; c++){
      if(dutyCycle[c] < MINDUTYCYCLE){
        fht_oct_offsets[colorOctaves[c]] -= 5;
      } else if(dutyCycle[c] > MAXDUTYCYCLE){
        fht_oct_offsets[colorOctaves[c]] += 5; 
      }
      dutyCycle[c]=0.0;
      
      Serial.print("Color Octave ");
      Serial.print(colorOctaves[c]);
      Serial.print(" is ");
      Serial.print(fht_oct_offsets[colorOctaves[c]]);
      
      if(avgLevel[c]/dutyCycle[c] < MINAVG){
        fht_oct_scale[colorOctaves[c]] -= 5;
      } else if(avgLevel[c]/dutyCycle[c] > MAXAVG){
        fht_oct_scale[colorOctaves[c]] += 5;
      }
      avgLevel[c]=0.0;
      
      Serial.print(", Scale is ");
      Serial.println(fht_oct_scale[colorOctaves[c]]);
    }
    nDutySamples=0;
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
#define INT_THRESH 5
#define R_MAX 60
#define G_MAX 60
#define B_MAX 60
void setLevelRGB(float rLevel, float gLevel, float bLevel){
  #if 0
  Serial.print("RGB func: ");
  Serial.print(pixelLevel(0, rLevel, 150));
  Serial.print(",");
  Serial.print(pixelLevel(0, gLevel, 150));
  Serial.print(",");
  Serial.println(pixelLevel(0, bLevel, 150));
  #endif
  for(int i=0; i<N_PIXELS*N_ROWS; i++) {
    strip.setPixelColor(
      i,
      pixelLevel(i, rLevel, R_MAX),
      pixelLevel(i, gLevel, G_MAX),
      pixelLevel(i, bLevel, B_MAX)
    );
  }
}

int pixelLevel(int pixel, float level, float maxLevel) {
  //Serial.println(getIntLevel(max(0, min(1, level*N_PIXELS*N_ROWS - pixel))*maxLevel));
  return max(0, min(1, level*N_PIXELS*N_ROWS - pixel))*maxLevel;
}
