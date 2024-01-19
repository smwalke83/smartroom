/* 
 * Project Smart Porch
 * Author: Scott Walker
 * Date: 1/19/2024
 */

#include "Particle.h"             // header files
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
const int SERVOPIN = A2;          // lines 18-23 define pins for components
const int BLACKBUTTONPIN = D9; 
const int GREENBUTTONPIN = D8; 
const int MOTIONPIN = D16; 
const int RAINSENSOR = D15; 
const int PHOTOSENSOR = A5;
const int PIXELCOUNT = 2;         // neopixels
const int ANGLE1 = 180;           // lines 25-26 are constants for the servo motors
const int ANGLE2 = 90;
const int OLED_RESET = -1;        // lines 27-30 are constants for the BME and OLED display
const int OLEDADDRESS = 0x3C;
const int BMEADDRESS = 0x76;
const int DEGREE = 0xF8;
const int WEMO1 = 3;              // lines 31-41 are wemo and hue constants
const int WEMO2 = 4;
const int BULB1 = 1;
const int BULB2 = 2;
const int BULB3 = 3;
const int BULB4 = 4;
const int BULB5 = 5;
const int BULB6 = 6;
const int hueColor = 0;
const int hueBrightness = 150;
const int saturation = 0;
int photoValues[30];              // array that fills with data from the photodiode, to be averaged out for more consistant data
const int neoColors[7] = {blue, navy, white, pink, purple, maroon, red};  // colors for neopixels, based on temperature
int neoColorTemp;                 // lines 44-47 are variables to help determine brightness and color of neopixels/hue lights
int neoBrightness;
int photoSum;
int photoAVG;
int currentTime;                  // lines 48-49 are variables used for a simple timer
int lastSecond;
int n; int x; int y; int z;       // variables used for 'for' loops and iteration
float tempC;                      // lines 51-55 are variables for the BME readings and OLED display
float tempF;
float pressPA;
float pressHG;
float humidRH;
bool onOff;                       // these bool states affect what mode the controller is in - hot/cold, rain/dry, on/off, manual/auto
bool rain;
bool hot;
bool wemo1On;
bool wemo2On;
bool hueisOn;
bool manual;
bool motion;
const int manualModes[6] = {0, 1, 2, 3, 4, 5}; 
Adafruit_BME280 bme;                                  // objects
Adafruit_SSD1306 display(OLED_RESET);
Adafruit_NeoPixel pixel(PIXELCOUNT, SPI1, WS2812B);
Servo myServo;
IoTTimer rainTimer;
IoTTimer wiperTimer;
IoTTimer motionTimer;
Button blackButton(BLACKBUTTONPIN);
Button greenButton(GREENBUTTONPIN);
Wemo wemo1(WEMO1);
Wemo wemo2(WEMO2);

SYSTEM_MODE(MANUAL);

void setup() {

pixel.begin();                                          // pixel setup
pixel.show();
display.begin(SSD1306_SWITCHCAPVCC, OLEDADDRESS);       // OLED setup
display.clearDisplay();
display.display();
bme.begin(BMEADDRESS);                                  // BME280 setup
WiFi.on();                                              // connecting to network for hue and wemo
WiFi.clearCredentials();
WiFi.setCredentials("IoTNetwork");
WiFi.connect();
pinMode(RAINSENSOR, INPUT);                             // pinmodes for relevant components
pinMode(PHOTOSENSOR, INPUT);
pinMode(MOTIONPIN, INPUT);
myServo.attach(SERVOPIN);                               // servo setup
myServo.write(ANGLE1);
x = 0;                                                  // presetting bools and variables
z = 0;
lastSecond = 0;
onOff = false;
wemo1On = false;
wemo2On = false;
hueisOn = true;
manual = false;

}

void loop() {

motion = digitalRead(MOTIONPIN);                        // assigning variables for the loop
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
neoBrightness = (750 - (5*photoAVG))/7;
if(neoBrightness < 0){
  neoBrightness = 0;
}
pixel.setBrightness(neoBrightness);
if(!manual){                                                      // code for auto mode, lighting pixels first
  pixel.clear();
  for(n = 0; n < PIXELCOUNT; n++){
    pixel.setPixelColor(n, neoColors[neoColorTemp]);
  }
  pixel.show();
  if(hueisOn == false && photoAVG < 30){                          // turning the hue on/off based on ambient light, bulbs not in use will be commented out
    //setHue(BULB1, HIGH, hueColor, hueBrightness, saturation);
    //setHue(BULB2, HIGH, hueColor, hueBrightness, saturation);
    //setHue(BULB3, HIGH, hueColor, hueBrightness, saturation);
    //setHue(BULB4, HIGH, hueColor, hueBrightness, saturation);
    //setHue(BULB5, HIGH, hueColor, hueBrightness, saturation);
    //setHue(BULB6, HIGH, hueColor, hueBrightness, saturation);
    hueisOn = true;
  }
  if(hueisOn == true && photoAVG >= 30){
    //setHue(BULB1, LOW, hueColor, hueBrightness, saturation);
    //setHue(BULB2, LOW, hueColor, hueBrightness, saturation);
    //setHue(BULB3, LOW, hueColor, hueBrightness, saturation);
    //setHue(BULB4, LOW, hueColor, hueBrightness, saturation);
    //setHue(BULB5, LOW, hueColor, hueBrightness, saturation);
    //setHue(BULB6, LOW, hueColor, hueBrightness, saturation);
    hueisOn = false;
  }
  if(digitalRead(RAINSENSOR)){                                    // determining whether it's in rain mode or not
    rain = true;
    rainTimer.startTimer(10000);
  }
  if(tempF > 75.0){                                               // determining whether it's in warm or cold mode
    hot = true;
  }
  if(tempF <= 75.0){
    hot = false;
  }
  if(rain){                                                       // what to do in rain mode (windshied wipers)
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
    if(hot){                                                      // what to display in rain mode (warm)
      display.clearDisplay();
      display.drawBitmap(32, 0, STORMCLOUDBMP, 128, 64, 1);
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(5, 10);
      display.printf("T: %0.2f%cF\n\n P: %0.0finHG\n\n H: %0.2f%%", tempF, DEGREE, pressHG, humidRH);
      display.display();
    }
    if(!hot){                                                     // what to display in rain mode (cold)
      display.clearDisplay();
      display.drawBitmap(32, 0, SNOWFLAKEBMP, 128, 64, 1);
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(5, 10);
      display.printf("T: %0.2f%cF\n\n P: %0.0finHG\n\n H: %0.2f%%", tempF, DEGREE, pressHG, humidRH);
      display.display();
    }
  }
  if(!rain){                                                      // what to do in dry mode (no wipers)
    if(hot){                                                      // display for warm/dry
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
    if(!hot){                                                     // display for cold/dry
      display.clearDisplay();
      display.drawBitmap(32, 0, RAINCOATBMP, 128, 64, 1);
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(5, 10);
      display.printf("T: %0.2f%cF\n\n P: %0.0finHG\n\n H: %0.2f%%", tempF, DEGREE, pressHG, humidRH);
      display.display();
    }
  }
  if(rainTimer.isTimerReady()){                                   // exiting rain mode and resetting wipers
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
  if(tempF > 80 && !wemo1On){                                     // when to turn on the fan
    wemo1.wemoWrite(HIGH);
    wemo1On = true;
  }
  if(tempF < 70 && !wemo2On){                                     // when to turn on the heat
    wemo2.wemoWrite(HIGH);
    wemo2On = true;
  }
  if(tempF <= 80 && tempF >= 70){                                 // when to turn off fan and heat
    if(wemo1On){
      wemo1.wemoWrite(LOW);
      wemo1On = false;
    }
    if(wemo2On){
      wemo2.wemoWrite(LOW);
      wemo2On = false;
    }
  }
}
if(blackButton.isClicked())                                       // enter manual mode
  manual = !manual;
if(manual){                                                       // increment manual settings
  if(greenButton.isClicked()){
    z++;
  }
  if(manualModes[z%6] == 0){                                      // manual mode 1 - all off
    display.clearDisplay();
    display.display();
    pixel.clear();
    pixel.show();
    myServo.write(ANGLE1);
    if(hueisOn == true){
      //setHue(BULB1, LOW, hueColor, hueBrightness, saturation);
      //setHue(BULB2, LOW, hueColor, hueBrightness, saturation);
      //setHue(BULB3, LOW, hueColor, hueBrightness, saturation);
      //setHue(BULB4, LOW, hueColor, hueBrightness, saturation);
      //setHue(BULB5, LOW, hueColor, hueBrightness, saturation);
      //setHue(BULB6, LOW, hueColor, hueBrightness, saturation);
      hueisOn = false;
    }
  }
  if(manualModes[z%6] == 1){                                      // manual mode 2 - hot/dry w/ fan
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
    pixel.clear();
    for(n = 0; n < PIXELCOUNT; n++){
      pixel.setPixelColor(n, neoColors[neoColorTemp]);
    }
    pixel.show();
    if(hueisOn == false){
      //setHue(BULB1, HIGH, hueColor, hueBrightness, saturation);
      //setHue(BULB2, HIGH, hueColor, hueBrightness, saturation);
      //setHue(BULB3, HIGH, hueColor, hueBrightness, saturation);
      //setHue(BULB4, HIGH, hueColor, hueBrightness, saturation);
      //setHue(BULB5, HIGH, hueColor, hueBrightness, saturation);
      //setHue(BULB6, HIGH, hueColor, hueBrightness, saturation);
      hueisOn = true;
    }
    if(!wemo1On){
      wemo1.wemoWrite(HIGH);
      wemo1On = true;
    }
    if(wemo2On){
      wemo2.wemoWrite(LOW);
      wemo2On = false;
    }
  }
  if(manualModes[z%6] == 2){                                      // manual mode 3 - cold/dry, w/ heat
    display.clearDisplay();
    display.drawBitmap(32, 0, RAINCOATBMP, 128, 64, 1);
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(5, 10);
    display.printf("T: %0.2f%cF\n\n P: %0.0finHG\n\n H: %0.2f%%", tempF, DEGREE, pressHG, humidRH);
    display.display();
    pixel.clear();
    for(n = 0; n < PIXELCOUNT; n++){
      pixel.setPixelColor(n, neoColors[neoColorTemp]);
    }
    pixel.show();
    if(hueisOn == false){
      //setHue(BULB1, HIGH, hueColor, hueBrightness, saturation);
      //setHue(BULB2, HIGH, hueColor, hueBrightness, saturation);
      //setHue(BULB3, HIGH, hueColor, hueBrightness, saturation);
      //setHue(BULB4, HIGH, hueColor, hueBrightness, saturation);
      //setHue(BULB5, HIGH, hueColor, hueBrightness, saturation);
      //setHue(BULB6, HIGH, hueColor, hueBrightness, saturation);
      hueisOn = true;
    }
    if(wemo1On){
      wemo1.wemoWrite(LOW);
      wemo1On = false;
    }
    if(!wemo2On){
      wemo2.wemoWrite(HIGH);
      wemo2On = true;
    }
  }
  if(manualModes[z%6] == 3){                                      // manual mode 4 - hot/wet w/ fan
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
    display.clearDisplay();
    display.drawBitmap(32, 0, STORMCLOUDBMP, 128, 64, 1);
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(5, 10);
    display.printf("T: %0.2f%cF\n\n P: %0.0finHG\n\n H: %0.2f%%", tempF, DEGREE, pressHG, humidRH);
    display.display();
    pixel.clear();
    for(n = 0; n < PIXELCOUNT; n++){
      pixel.setPixelColor(n, neoColors[neoColorTemp]);
    }
    pixel.show();
    if(hueisOn == false){
      //setHue(BULB1, HIGH, hueColor, hueBrightness, saturation);
      //setHue(BULB2, HIGH, hueColor, hueBrightness, saturation);
      //setHue(BULB3, HIGH, hueColor, hueBrightness, saturation);
      //setHue(BULB4, HIGH, hueColor, hueBrightness, saturation);
      //setHue(BULB5, HIGH, hueColor, hueBrightness, saturation);
      //setHue(BULB6, HIGH, hueColor, hueBrightness, saturation);
      hueisOn = true;
    }
    if(!wemo1On){
      wemo1.wemoWrite(HIGH);
      wemo1On = true;
    }
    if(wemo2On){
      wemo2.wemoWrite(LOW);
      wemo2On = false;
    }
  }
  if(manualModes[z%6] == 4){                                         // manual mode 5 - cold/wet w/ heater
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
    display.clearDisplay();
    display.drawBitmap(32, 0, SNOWFLAKEBMP, 128, 64, 1);
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(5, 10);
    display.printf("T: %0.2f%cF\n\n P: %0.0finHG\n\n H: %0.2f%%", tempF, DEGREE, pressHG, humidRH);
    display.display();
    pixel.clear();
    for(n = 0; n < PIXELCOUNT; n++){
      pixel.setPixelColor(n, neoColors[neoColorTemp]);
    }
    pixel.show();
    if(hueisOn == false){
      //setHue(BULB1, HIGH, hueColor, hueBrightness, saturation);
      //setHue(BULB2, HIGH, hueColor, hueBrightness, saturation);
      //setHue(BULB3, HIGH, hueColor, hueBrightness, saturation);
      //setHue(BULB4, HIGH, hueColor, hueBrightness, saturation);
      //setHue(BULB5, HIGH, hueColor, hueBrightness, saturation);
      //setHue(BULB6, HIGH, hueColor, hueBrightness, saturation);
      hueisOn = true;
    }
    if(wemo1On){
      wemo1.wemoWrite(LOW);
      wemo1On = false;
    }
    if(!wemo2On){
      wemo2.wemoWrite(HIGH);
      wemo2On = true;
    }
  }
  if(manualModes[z%6] == 5){                                        // manual mode 6 - motion lights
    motion = digitalRead(MOTIONPIN);
    if(!motion && motionTimer.isTimerReady()){
      display.clearDisplay();
      display.display();
      pixel.clear();
      pixel.show();
      myServo.write(ANGLE1);
      if(wemo1On){
        wemo1.wemoWrite(LOW);
        wemo1On = false;
      }
      if(wemo2On){
        wemo2.wemoWrite(LOW);
        wemo2On = false;
      }
      if(hueisOn == true){
        //setHue(BULB1, LOW, hueColor, hueBrightness, saturation);
        //setHue(BULB2, LOW, hueColor, hueBrightness, saturation);
        //setHue(BULB3, LOW, hueColor, hueBrightness, saturation);
        //setHue(BULB4, LOW, hueColor, hueBrightness, saturation);
        //setHue(BULB5, LOW, hueColor, hueBrightness, saturation);
        //setHue(BULB6, LOW, hueColor, hueBrightness, saturation);
        hueisOn = false;
      }
    }
    if(motion){
      motionTimer.startTimer(5000);
      pixel.clear();
      for(n = 0; n < PIXELCOUNT; n++){
        pixel.setPixelColor(n, neoColors[neoColorTemp]);
      }
      pixel.show();
      if(wemo1On){
        wemo1.wemoWrite(LOW);
        wemo1On = false;
      }
      if(wemo2On){
        wemo2.wemoWrite(LOW);
        wemo2On = false;
      }
      if(hueisOn == false){
        //setHue(BULB1, HIGH, hueColor, hueBrightness, saturation);
        //setHue(BULB2, HIGH, hueColor, hueBrightness, saturation);
        //setHue(BULB3, HIGH, hueColor, hueBrightness, saturation);
        //setHue(BULB4, HIGH, hueColor, hueBrightness, saturation);
        //setHue(BULB5, HIGH, hueColor, hueBrightness, saturation);
        //setHue(BULB6, HIGH, hueColor, hueBrightness, saturation);
        hueisOn = true;
      }
      myServo.write(ANGLE1);
    }
  }
}

}
