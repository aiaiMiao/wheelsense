#include <Wire.h>
#include "Adafruit_DRV2605.h"

Adafruit_DRV2605 drv;
int potPin = A0;
int ledPin = 10;
int potVal = 0;
int ledVal = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("DRV test");
  drv.begin();
    
  // I2C trigger by sending 'go' command 
  drv.setMode(DRV2605_MODE_INTTRIG); // default, internal trigger when sending GO command

  drv.selectLibrary(1);
  drv.setWaveform(0, 58);  // ramp up medium 1, see datasheet part 11.2
  drv.setWaveform(1, 1);  // strong click 100%, see datasheet part 11.2
  drv.setWaveform(2, 0);  // end of waveforms
}

void loop() {
  potVal = analogRead(potPin);
  ledVal = map(potVal, 0, 1023, 0, 255);
  Serial.print("Pot: "); Serial.println(potVal); 
  analogWrite(ledPin, ledVal);
  drv.go();
  delay(potVal);
}
