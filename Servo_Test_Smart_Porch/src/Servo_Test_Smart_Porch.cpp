/* 
 * Project Wiper Servos
 * Author: Scott Walker
 * Date: 1/5/2024
 */

#include "Particle.h"
const int SERVOPIN = A2;
const int ANGLE1 = 180;
const int ANGLE2 = 90;
int currentTime;
int lastSecond;
bool onOff;
Servo myServo;

SYSTEM_MODE(SEMI_AUTOMATIC);

void setup() {

myServo.attach(SERVOPIN);
lastSecond = 0;
onOff = true;
myServo.write(ANGLE1);

}

void loop() {

currentTime = millis();
if(currentTime < 10000){
  if(onOff){
    myServo.write(ANGLE2);
    }
  if(!onOff){
    myServo.write(ANGLE1);
    }
  if(currentTime - lastSecond > 1000){
    onOff = !onOff;
    lastSecond = millis();
    }
  }
if(currentTime > 10000){
  myServo.write(ANGLE1);
  }

}

