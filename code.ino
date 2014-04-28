//-----------------------------------------------------------
// ---created by MiCoDevelopers (micodevelopers@gmail.com)---
// ------------- Dominik Jurečko, Lukáš Slivko---------------
//-----------------------SLOVAKIA----------------------------

#include <Servo.h>

//Declaring variables
int input[4],charsCount = 0,percentage;
int lastFLights=0,lastBLights=0,lastMove=0,lastDirection=0;
char inputChar;
boolean writeIt = false;
//---------------------------------------------------------
int fLights = 2;
int bLights = 3;
int enableA = 4;
int motor1 = 5;
int motor2 = 6;
int servoControlPin = 9;
Servo tilt;

void brake(){
   digitalWrite(motor1, LOW);
   digitalWrite(motor2, LOW);
   digitalWrite(enableA, LOW);
}

void frontLights(int percentage){
  if(lastFLights != percentage){
    if(percentage == 1)
      digitalWrite(fLights, HIGH);
    else
      digitalWrite(fLights, LOW);
    lastFLights = percentage;
  }
  //If value is same - do nothing
}

void backLights(int percentage){
  if(lastBLights != percentage){
    if(percentage == 1)
      digitalWrite(bLights, HIGH);
    else
      digitalWrite(bLights, LOW);
    lastBLights = percentage;
  }
  //If value is same - do nothing
}

void motorSet(int percentage){
  if(lastMove != percentage){
    if(percentage>0){
        //You can't connect motor direct to arduino becaouse you can damage it,
        //you must use H bridge - for example
        //http://www.ebay.com/itm/Dual-H-Bridge-DC-Stepper-Motor-Drive-Controller-Board-Module-Arduino-L298N-T1K-/360786451706?pt=LH_DefaultDomain_2&hash=item54008c5cfa
        //(Give down jumper from enableA, front pin connect to enableA on arduino)
        digitalWrite(motor1, HIGH);
        digitalWrite(motor2, LOW);
        analogWrite(enableA, 255*((float)percentage/100));
    }
    else{
        digitalWrite(motor2, HIGH);
        digitalWrite(motor1, LOW);
        analogWrite(enableA, 255*(-((float)percentage/100)));
    }
  }
  //If value is same - do nothing
}

void directionSet(int percentage){
  if(lastDirection != percentage){
    lastDirection = percentage;
    //We want positive percentages
    percentage /= 1.1;
    percentage += 90;
    tilt.write(percentage);
  }
  //If value is same - do nothing
}

void setup() {
  //Set pins mode
  pinMode(bLights, OUTPUT);
  pinMode(fLights, OUTPUT);
  pinMode(enableA, OUTPUT);
  pinMode(motor1, OUTPUT);
  pinMode(motor2, OUTPUT);
  // Opens serial port, sets data rate to 9600 bps,
  // set your value
  Serial1.begin(9600);
  //Set servo signal pin(middle wire), center servo to middle
  tilt.attach(servoControlPin);
  tilt.write(90);
}

void loop() {
  //If application is sending data
  if(Serial1.available()){
    //We read one character
    inputChar = Serial1.read();
    //If it is ending char we calculate percentages
    if(inputChar == '!'){
      //If lenght of data between first and last char is 4,
      //it must be three-digit number so
      // it can be only 100 percent
       if(charsCount == 4) percentage = 100;
       //if lenght is 3 - percentage is two-digit number
       else if(charsCount == 3){
         percentage = (input[1]-'0')*10+(input[2]-'0');
       }
       //If length is 2 - there is only one-digit number
       else if(charsCount == 2) percentage = input[1]-'0';
       writeIt = false;
       charsCount = 0;
       //Call relevant function according to first character
       switch(input[0]){
         case 'w':
           if(percentage == 0) 
             //Set brake
             brake();
           else
             motorSet(percentage);
           break;
         case 's':
           if(percentage == 0) 
             brake();
           else
             motorSet(-percentage);
           break;
         case 'f':
           frontLights(percentage);
           break;
         case 'b':
           backLights(percentage);
           break;
         case 'l':
           directionSet(-percentage);
           break;
         case 'r':
           directionSet(percentage);
           break;
       }
       
    }
    //If first character was read, we read next char,
    //else we wait on first character
    else if(writeIt == true){
      input[charsCount] = inputChar;
      charsCount++;
    }
    //If first character was read, we allow read next char
    else if(inputChar == '$'){
      writeIt = true;
    }
  }
}
