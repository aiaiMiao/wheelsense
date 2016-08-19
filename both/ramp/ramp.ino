#include <Wire.h>
#include "Adafruit_DRV2605.h"

Adafruit_DRV2605 drv;
//int ledPin = 3;
int value;
int ledVal;

int sensors[] = {A0, A1};
int hapticPins[] = {40, 52};

int hapticCount = 0;
int hapticIndex = 0; 


void setup() {
  Serial.begin(9600);
  Serial.println("DRV test");
  digitalWrite(hapticPins[0], HIGH);
  digitalWrite(hapticPins[1], HIGH);
  delay(100);

  drv.begin();
    
  // I2C trigger by sending 'go' command 
  drv.setMode(DRV2605_MODE_INTTRIG); // default, internal trigger when sending GO command

  drv.selectLibrary(1);
  drv.setWaveform(0, 58);  // ramp up medium 1, see datasheet part 11.2
  drv.setWaveform(1, 1);  // strong click 100%, see datasheet part 11.2
  drv.setWaveform(2, 0);  // end of waveforms

  delay(100);
}

void loop() {
  if(hapticIndex == 0) {
    digitalWrite(hapticPins[0], HIGH);
    digitalWrite(hapticPins[1], LOW);
  } else {
    digitalWrite(hapticPins[0], LOW);
    digitalWrite(hapticPins[1], HIGH);
  }  
  delay(100);
//  value = analogRead(sensors[0]);
value = 0;
  Serial.println(value); 
  
//  ledVal = map(value, 0, 1023, 0, 255);
//  analogWrite(ledPin, ledVal);
  if(value < 90) {
    drv.go();
//    Serial.println("trigger");
  } else {
    drv.stop();
  }
  delay(100);

  hapticCount++;
  if(hapticCount > 2) {
    hapticCount = 0;
    hapticIndex = 1 - hapticIndex;
  }
}
