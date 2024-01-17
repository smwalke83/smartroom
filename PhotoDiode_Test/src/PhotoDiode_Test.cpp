/* 
 * Project Photosensor Test
 * Author: Scott Walker
 * Date: 
 */
#include "Particle.h"
const int PHOTOPIN = A5;
int pinValue;
int photoValues[30];
int photoSum;
int photoAVG;
int brightness;
int x;
int y;
int currentTime;
int lastSecond;

SYSTEM_MODE(SEMI_AUTOMATIC);

void setup() {

pinMode(PHOTOPIN, INPUT);
Serial.begin(9600);
waitFor(Serial.isConnected, 10000);
x = 0;
lastSecond = 0;

}

void loop() {

currentTime = millis();
photoValues[x%30] = analogRead(PHOTOPIN);
x++;
photoSum = 0;
for(y = 0; y < 30; y++){
    photoSum = photoValues[y] + photoSum;
    photoAVG = photoSum/y;
}
if(currentTime - lastSecond > 500){
    Serial.printf("The photoAVG is %d\n", photoAVG);
    lastSecond = millis();
}

}
