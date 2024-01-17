/* 
 * Project Wipers/Rain test
 * Author: Scott Walker
 * Date: 1/5/2024
 */

#include "Particle.h"
#include "IoTTimer.h"
const int SERVOPIN = A2;
const int ANGLE1 = 180;
const int ANGLE2 = 90;
const int RAINSENSOR = D15;
int currentTime;
int lastSecond;
bool onOff;
bool rain;
Servo myServo;
IoTTimer rainTimer;

SYSTEM_MODE(SEMI_AUTOMATIC);

void setup() {

pinMode(RAINSENSOR, INPUT);
myServo.attach(SERVOPIN);
lastSecond = 0;
onOff = true;
myServo.write(ANGLE1);

}

void loop() {

currentTime = millis();
if(digitalRead(RAINSENSOR)){
  rain = true;
  rainTimer.startTimer(10000);
}
if(rain){
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
if(rainTimer.isTimerReady()){
  rain = false;
  myServo.write(ANGLE1);
}

}

