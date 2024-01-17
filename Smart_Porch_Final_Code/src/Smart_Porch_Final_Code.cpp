/* 
 * Project Smart Porch
 * Author: Scott Walker
 * Date: 
 */

#include "Particle.h"
#include "Adafruit_BME280.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"
#include "IoTTimer.h"
#include "neopixel.h"
#include "Button.h"
#include "Colors.h"
#include "Bitmap.h"
#include "hue.h"
#include "wemoObj.h"
const int SERVOPIN = A2; 
const int BLACKBUTTONPIN = D9; 
const int GREENBUTTONPIN = D8; 
const int MOTIONPIN = D16; 
const int RAINSENSOR = D15; 
const int PHOTOSENSOR = A5;
const int PIXELCOUNT = 2;
const int ANGLE1 = 180;
const int ANGLE2 = 90;
const int OLED_RESET = -1;
const int OLEDADDRESS = 0x3C;
const int BMEADDRESS = 0x76;
const int DEGREE = 0xF8;
const int WEMO1 = 3;
const int WEMO2 = 4;
int photoValues[30];
int neoColors[7] = {blue, navy, white, pink, purple, maroon, red};
int neoColorTemp;
int photoSum;
int neoBrightness;
int photoAVG;
int currentTime;
int lastSecond;
int n; int x; int y;
float tempC;
float tempF;
float pressPA;
float pressHG;
float humidRH;
bool onOff;
bool rain;
bool hot;
Adafruit_BME280 bme;
Adafruit_SSD1306 display(OLED_RESET);
Adafruit_NeoPixel pixel(PIXELCOUNT, SPI1, WS2812B);
Servo myServo;
IoTTimer rainTimer;
IoTTimer wiperTimer;
Button blackButton(BLACKBUTTONPIN);
Button greenButton(GREENBUTTONPIN);
Wemo wemo1(WEMO1);
Wemo wemo2(WEMO2);

SYSTEM_MODE(SEMI_AUTOMATIC);

void setup() {

pixel.begin();
pixel.show();
display.begin(SSD1306_SWITCHCAPVCC, OLEDADDRESS);
display.clearDisplay();
display.display();
bme.begin(BMEADDRESS);
pinMode(RAINSENSOR, INPUT);
pinMode(PHOTOSENSOR, INPUT);
pinMode(MOTIONPIN, INPUT);
myServo.attach(SERVOPIN);
onOff = false;
myServo.write(ANGLE1);
x = 0;
lastSecond = 0;

}

void loop() {

tempC = bme.readTemperature();
pressPA = bme.readPressure();
humidRH = bme.readHumidity();
tempF = (9.0/5.0)*tempC + 32;
pressHG = pressPA/3386.0;
neoColorTemp = map(tempF, 65.0, 85.0, 0.0, 6.0);
photoValues[x%30] = analogRead(PHOTOSENSOR);
x++;
photoSum = 0;
for(y = 0; y < 30; y++){
    photoSum = photoValues[y] + photoSum;
}
photoAVG = photoSum/30;
neoBrightness = 70 - photoAVG;
if(neoBrightness < 0){
  neoBrightness = 0;
}
pixel.setBrightness(neoBrightness);
for(n = 0; n < PIXELCOUNT; n++){
  pixel.setPixelColor(n, neoColors[neoColorTemp]);
}
pixel.show();
if(digitalRead(RAINSENSOR)){
  rain = true;
  rainTimer.startTimer(10000);
}
if(tempF > 72.0){
  hot = true;
}
if(tempF <= 72.0){
  hot = false;
}
if(rain){
  if(wiperTimer.isTimerReady()){
    onOff = !onOff;
    wiperTimer.startTimer(1000);
  }
  if(onOff){
    myServo.write(ANGLE2);
  }
  if(!onOff){
    myServo.write(ANGLE1);
  }
  if(hot){
    display.clearDisplay();
    display.drawBitmap(32, 0, STORMCLOUDBMP, 128, 64, 1);
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(5, 10);
    display.printf("T: %0.2f%cF\n\n P: %0.0finHG\n\n H: %0.2f%%", tempF, DEGREE, pressHG, humidRH);
    display.display();
  }
  if(!hot){
    display.clearDisplay();
    display.drawBitmap(32, 0, SNOWFLAKEBMP, 128, 64, 1);
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(5, 10);
    display.printf("T: %0.2f%cF\n\n P: %0.0finHG\n\n H: %0.2f%%", tempF, DEGREE, pressHG, humidRH);
    display.display();
  }
}
if(!rain){
  if(hot){
    display.clearDisplay();
    display.fillCircle(96, 32, 5, WHITE);
    display.drawLine(96, 22, 96, 17, WHITE);
    display.drawLine(96, 42, 96, 47, WHITE);
    display.drawLine(106, 32, 111, 32, WHITE);
    display.drawLine(86, 32, 81, 32, WHITE);
    display.drawLine(103, 39, 106, 42, WHITE);
    display.drawLine(103, 25, 106, 22, WHITE);
    display.drawLine(89, 25, 86, 22, WHITE);
    display.drawLine(89, 39, 86, 42, WHITE);
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(5, 10);
    display.printf("T: %0.2f%cF\n\n P: %0.0finHG\n\n H: %0.2f%%", tempF, DEGREE, pressHG, humidRH);
    display.display();
  }
  if(!hot){
    display.clearDisplay();
    display.drawBitmap(32, 0, RAINCOATBMP, 128, 64, 1);
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(5, 10);
    display.printf("T: %0.2f%cF\n\n P: %0.0finHG\n\n H: %0.2f%%", tempF, DEGREE, pressHG, humidRH);
    display.display();
  }
}
if(rainTimer.isTimerReady()){
  rain = false;
  currentTime = millis();
  if(lastSecond == 0){
    lastSecond = millis();
  }
  if(currentTime - lastSecond > 1000){
    myServo.write(ANGLE1);
    lastSecond = millis();
  }
}

}
