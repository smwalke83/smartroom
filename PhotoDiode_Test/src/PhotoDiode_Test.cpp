/* 
 * Project Photosensor Test
 * Author: Scott Walker
 * Date: 
 */
#include "Particle.h"
const int PHOTOPIN = A5;
int pinValue;

SYSTEM_MODE(SEMI_AUTOMATIC);

void setup() {

pinMode(PHOTOPIN, INPUT);
Serial.begin(9600);
waitFor(Serial.isConnected, 10000);

}

void loop() {

pinValue = analogRead(PHOTOPIN);
Serial.printf("The value is %d\n", pinValue);

}
