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
#define TOTALDUTYSAMPLES 500
#define MINDUTYCYCLE 0.15
#define MAXDUTYCYCLE 0.80

#define MINAVG 0.20
#define MAXAVG 0.45

//FHT variables
float dutyCycle[4] = {0.0, 0.0, 0.0, 0.0};
float avgLevel[4] = {0.0, 0.0, 0.0, 0.0};
float colors[4][3] = { {0, 40, 40},
                       {20, 0, 60},
                       {0, 0, 80},
                       {0, 40, 40}};
int colorOctaves[4] = {7,4,2,0};
int nDutySamples = 0;
int fht_oct_offsets[8] = {100, 100, 100, 100,  100, 95, 80, 70};
int fht_oct_offset_rms[8] = {2, 2, 2, 1, 1, 1, 1, 1};
int fht_oct_scale[8] = {56, 56, 56, 56, 56, 56, 56, 56};

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
    
//    Serial.print("analig: ");
//    Serial.println(fht_input[0]);

    for(int i = 0; i<4; i++){
      setRowLevel(i, getOctLEDLevel(colorOctaves[i]));
    }
    
//    Serial.print("octaves in: ");  
//    Serial.print(fht_oct_out[colorOctaves[R]]);
//    Serial.print(", ");
//    Serial.print(fht_oct_out[colorOctaves[G]]);
//    Serial.print(", ");
//    Serial.print(fht_oct_out[colorOctaves[B]]);
//  
//    Serial.print(" :::: levels in: ");  
//    Serial.print(newRedLevel);
//    Serial.print(", ");
//    Serial.print(newGreenLevel);
//    Serial.print(", ");
//    Serial.println(newBlueLevel);
  
    //setLevelRGB(newRedLevel, newGreenLevel, newBlueLevel);
    updateDynamicThresholds();
    
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
void updateDynamicThresholds(){

  for(int i=0; i<4; i++){
    float level = getOctLEDLevel(colorOctaves[i]);
    dutyCycle[i] = (nDutySamples*dutyCycle[i]+(level>0.05?1:0))/(nDutySamples+1);
    avgLevel[i] = (nDutySamples*avgLevel[i]+(level>0.05?level:0))/(nDutySamples+1);
  }

  nDutySamples++;
  
  if(nDutySamples>=TOTALDUTYSAMPLES){
    for(int c=0; c<4; c++){
      if(dutyCycle[c] < MINDUTYCYCLE){
        fht_oct_offsets[colorOctaves[c]] -= 5;
      } else if(dutyCycle[c] > MAXDUTYCYCLE){
        fht_oct_offsets[colorOctaves[c]] += 5; 
      }
      
      Serial.print("Color Octave ");
      Serial.print(colorOctaves[c]);
      Serial.print(" is ");
      Serial.print(fht_oct_offsets[colorOctaves[c]]);
      
      if(avgLevel[c]/dutyCycle[c] < MINAVG){
        fht_oct_scale[colorOctaves[c]] -= 5;
      } else if(avgLevel[c]/dutyCycle[c] > MAXAVG){
        fht_oct_scale[colorOctaves[c]] += 5;
      }
      
      Serial.print(", Scale is ");
      Serial.println(fht_oct_scale[colorOctaves[c]]);

      dutyCycle[c]=0.0;
      avgLevel[c]=0.0;
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
Sets the a bar on each row based on colors
*/
void setRowLevel(int row, float level) {
  for(int i = 0; i < N_PIXELS; i++) {
    strip.setPixelColor(
      i + row*N_PIXELS,
      pixelLevel(i, level, colors[row][R]),
      pixelLevel(i, level, colors[row][G]),
      pixelLevel(i, level, colors[row][B])
    );
  }
}

void setCylPixel(int theta, int z, int r, int g, int b) {
  int n = ((theta % 360)/360.0 + z * N_ROWS) * N_PIXELS;
  strip.setPixelColor(n, r, g, b);
}

int pixelLevel(int pixel, float level, float maxLevel) {
  //Serial.println(getIntLevel(max(0, min(1, level*N_PIXELS*N_ROWS - pixel))*maxLevel));
  return max(0, min(1, level*N_PIXELS - pixel))*maxLevel;
}
