/* @file CustomKeypad.pde
|| @version 1.0
|| @author Alexander Brevig
|| @contact alexanderbrevig@gmail.com
||
|| @description
|| | Demonstrates changing the keypad size and key values.
|| #
*/
#include <Keypad.h>

extern const uint8_t gamma[];

const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
//define the cymbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {
  {'F','E','D','C'},
  {'B','A','9','8'},
  {'7','6','5','4'},
  {'3','2','1','0'}
};

//char hexaKeys[ROWS][COLS] = {
//  {'F','B','7','3'},
//  {'E','A','6','2'},
//  {'D','9','5','1'},
//  {'C','8','4','0'}
//};
byte rowPins[ROWS] = {7, 6, 4, 2}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {8, 10, 11, 12}; //connect to the column pinouts of the keypad

//initialize an instance of class NewKeypad
Keypad kpd = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

// led strand
#define RED_LED_PIN 9 //digital
#define GREEN_LED_PIN 5 //digital
#define BLUE_LED_PIN 3 //digital

//globals
#define R 0
#define G 1
#define B 2
uint8_t color[3] = {0, 0, 0};

void setup(){
  //set led strand pins to output mode
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(BLUE_LED_PIN, OUTPUT);
  
  analogWrite(RED_LED_PIN, 0);
  analogWrite(GREEN_LED_PIN, 0);
  analogWrite(BLUE_LED_PIN, 0);
  
  kpd.addEventListener(handleKeypadEvent);
  Serial.begin(9600);
}
  
void loop(){
  kpd.getKey(); //needed to fire handler
  if (millis()%1000 == 0) {
    char line[20];
    sprintf(line,"%d, %d, %d", color[R], color[G], color[B]);
    Serial.println(line);
  }
}

// handle keypand events
void handleKeypadEvent(KeypadEvent key){
  switch (kpd.getState()){
    case PRESSED:
      switch (key){
   // ROW 0
        case '0': 
          setColor(0, 0, 0);
          break;
        case '1': 
          setColor(64, 156, 255);
          break;
        case '2':
          setColor(201, 226, 255);
          break;
        case '3':
          setColor(255, 255, 255);
          break;
   // ROW 1
        case '4':
          break; 
        case '5':
          setColor(255, 147, 41);
          break;
        case '6':
          setColor(255, 197, 143);
          break;
        case '7':
          setColor(255, 214, 170);
          break;
   // ROW 2
        case '8':
          scaleColor(1.1, 1.1, 1.1);
          break;
        case '9':
          // increment red
          changeColor(5, 0, 0);
          break;
        case 'A':
          // increment green
          changeColor(0, 5, 0);
          break;
        case 'B':
          // increment blue
          changeColor(0, 0, 5);
          break;
   // ROW 3
        case 'C':
          scaleColor(0.9, 0.9, 0.9);
          break;
        case 'D':
          // decrement red
          changeColor(-5, 0, 0);
          break;
        case 'E':
          // decrement red
          changeColor(0, -5, 0);
          break;
        case 'F':
          // decrement blue
          changeColor(0, 0, -5);
          break;
          //ChargingEffect charge = ChargingEffect(strip, strip.Color(0,0,250));
      }
      break;
    case RELEASED:
      //charge.stop();
    break;
    case HOLD:
      //charge.start();
    break;
  }
}

void setColor(uint8_t red, uint8_t green, uint8_t blue) {
  color[R] = red;
  color[G] = green;
  color[B] = blue;
  displayColor();
}

uint8_t limitToByte(int16_t x) {
  return max(min(x, 255), 0);
}

void changeColor(int8_t dRed, int8_t dGreen, int8_t dBlue) {
  color[R] = limitToByte(color[R] + dRed);
  color[G] = limitToByte(color[G] + dGreen);
  color[B] = limitToByte(color[B] + dBlue);
  displayColor();
}

void scaleColor(float cRed, float cGreen, float cBlue) {
  color[R] = limitToByte(color[R] * cRed);
  color[G] = limitToByte(color[G] * cGreen);
  color[B] = limitToByte(color[B] * cBlue);
  displayColor();
}

void displayColor() {
  analogWrite(RED_LED_PIN, pgm_read_byte(&gamma[color[R]]));
  analogWrite(GREEN_LED_PIN, pgm_read_byte(&gamma[color[G]]));
  analogWrite(BLUE_LED_PIN, pgm_read_byte(&gamma[color[B]]));
}

#define THRESH 5
uint8_t modifyColorInt(uint8_t c) {
  if (c < THRESH) {
    return 0;
  } else {
    return c;
  }
}

const uint8_t PROGMEM gamma[] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
    2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
    5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
   10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
   17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
   25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
   37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
   51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
   69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
   90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
  115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
  144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
  177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
  215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255 };
