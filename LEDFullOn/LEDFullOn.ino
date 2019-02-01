// color swirl! connect an RGB LED to the PWM pins as indicated
// in the #defines
// public domain, enjoy!
 
#define REDPIN 5
#define GREENPIN 6
#define BLUEPIN 9

#define WHITE    0
#define RED      1
#define GREEN    2
#define BLUE     3
#define YELLOW   4
#define VIOLET   5
#define TEAL     6
#define MAGENTA  7
#define WARM     8

static const int redLevels[]   = {255, 255,   0,   0, 255, 180,   0, 255, 255};
static const int greenLevels[] = {255,   0, 255,   0, 100,   0, 255,   0,  65};
static const int blueLevels[]  = {255,   0,   0, 255,   0, 255, 160, 160,  10};
 
#define FADESPEED 20     // make this higher to slow down
int color = MAGENTA;
float currentIntensity = 0.4;
 
void setup() {
  pinMode(REDPIN, OUTPUT);
  pinMode(GREENPIN, OUTPUT);
  pinMode(BLUEPIN, OUTPUT);

  setColorByCode(color,currentIntensity);

}
 

void setColorRGB(int rLevel, int gLevel, int bLevel){
  analogWrite(REDPIN, rLevel);
  analogWrite(GREENPIN, gLevel);
  analogWrite(BLUEPIN, bLevel);
}


void setColorByCode(int colorCode, float intensity){
  intensity = (intensity > 1.0) ? 1.0 : intensity;
  intensity = (intensity < 0.0) ? 0.0 : intensity;
  
  setColorRGB(redLevels[colorCode]*intensity,
              greenLevels[colorCode]*intensity,
              blueLevels[colorCode]*intensity);
}


void loop() {


}
