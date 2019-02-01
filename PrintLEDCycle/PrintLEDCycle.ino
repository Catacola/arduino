// color swirl! connect an RGB LED to the PWM pins as indicated
// in the #defines
// public domain, enjoy!
 
#define REDPIN 5
#define GREENPIN 6
#define BLUEPIN 9

#define STEPSIZE 0.01
#define MIDLEVEL 85
 
#define DELAYTIME 3     // make this higher to slow down
 
void setup() {
  pinMode(REDPIN, OUTPUT);
  pinMode(GREENPIN, OUTPUT);
  pinMode(BLUEPIN, OUTPUT);
  
  //Serial.begin(9600);
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
 
 
void loop() {
  float r, g, b;
  
//  analogWrite(REDPIN,0);
//  delay(DELAYTIME);
//  analogWrite(REDPIN,MIDLEVEL);
//  delay(DELAYTIME);
//  analogWrite(REDPIN,255);
//  delay(DELAYTIME);
//  analogWrite(REDPIN,0);
//  
//  analogWrite(GREENPIN,0);
//  delay(DELAYTIME);
//  analogWrite(GREENPIN,MIDLEVEL);
//  delay(DELAYTIME);
//  analogWrite(GREENPIN,255);
//  delay(DELAYTIME);
//  analogWrite(GREENPIN,0);  
//  
//  analogWrite(BLUEPIN,0);
//  delay(DELAYTIME);
//  analogWrite(BLUEPIN,MIDLEVEL);
//  delay(DELAYTIME);
//  analogWrite(BLUEPIN,255);
//  delay(DELAYTIME);
//  analogWrite(BLUEPIN,0);
//  
//  return;
 
  for(r=0.0; r<=1.0; r+=STEPSIZE){
    setLevelRGB(r,0,1.0-r);
    Serial.print("Red level: ");
    Serial.println(r);
    delay(DELAYTIME); 
  }
   
  for(g=0.0; g<=1.0; g+=STEPSIZE){
    setLevelRGB(1.0-g,g,0);
    Serial.print("Green level: ");
    Serial.println(g);
    delay(DELAYTIME); 
  }
  
  for(b=0.0; b<=1.0; b+=STEPSIZE){
    setLevelRGB(0,1.0-b,b);
    Serial.print("Blue level: ");
    Serial.println(b);
    delay(DELAYTIME); 
  }

  
}
