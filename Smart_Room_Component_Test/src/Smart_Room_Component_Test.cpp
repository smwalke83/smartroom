/* 
 * Project Test Motion and Rain Sensor
 * Author: Scott Walker
 * Date: 12/15/2023

 */

#include "Particle.h"
const int TESTPIN = D15;
bool test;

SYSTEM_MODE(SEMI_AUTOMATIC);

void setup() {

pinMode(TESTPIN, INPUT);
Serial.begin(9600);
waitFor(Serial.isConnected, 10000);

}

void loop() {

test = digitalRead(TESTPIN);
Serial.printf("The pin reads %i\n", test);

}
