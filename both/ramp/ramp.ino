#include <Wire.h>
#include "Adafruit_DRV2605.h"


Adafruit_DRV2605 drv;
int value;

#define N_SENSORS 4
#define THRESHOLD_RAMP 100
#define THRESHOLD_OBJ 150
#define THRESHOLD_OBJ_RAMP 350

int sensors[] = {A5, A6,A0,A1};
int hapticPins[] = {43, 39}; // left, right
int sensorValues[N_SENSORS];
int pinOn[] = {false, false};

// obstactle sensors
// A5, A6 = left, right

int mode = DRV2605_MODE_EXTTRIGLVL;

void setup() {
    Serial.begin(9600);
    Serial.println("DRV test");

    pinMode(hapticPins[0], OUTPUT);
    pinMode(hapticPins[1], OUTPUT);

    digitalWrite(hapticPins[0], LOW);
    digitalWrite(hapticPins[1], LOW);
    delay(100);

    drv.begin();

    delay(100);

    drv.setMode(DRV2605_MODE_INTTRIG);
    drv.selectLibrary(1);
    drv.setWaveform(0, 58);  // ramp up medium 1, see datasheet part 11.2
    /* drv.setWaveform(1, 1);  // strong click 100%, see datasheet part 11.2 */
    drv.setWaveform(1, 0);  // end of waveforms

    delay(200);

    drv.stop();
    mode = DRV2605_MODE_INTTRIG;

    delay(200);
}

void engageHaptics(boolean left, boolean right) {

    Serial.print(left);
    Serial.print(right);
    Serial.println("\n");

    if(!left && !right) {
        Serial.println("neither");
        drv.setMode(DRV2605_MODE_INTTRIG);
        drv.stop();
        mode = DRV2605_MODE_INTTRIG;
        return;
    } else if(left && right) {
        Serial.println("both");
        drv.setMode(DRV2605_MODE_INTTRIG);
        drv.go();
        delay(400);
        mode = DRV2605_MODE_INTTRIG;
        return;
    }

    if(mode == DRV2605_MODE_INTTRIG) {
        drv.setMode(DRV2605_MODE_INTTRIG);
        drv.stop();
        delay(50);

        digitalWrite(hapticPins[0], LOW);
        digitalWrite(hapticPins[1], LOW);

        drv.setMode(DRV2605_MODE_EXTTRIGEDGE);
        digitalWrite(hapticPins[0], LOW);
        digitalWrite(hapticPins[1], LOW);

        mode = DRV2605_MODE_EXTTRIGEDGE;
        delay(50);
    }

    if(!left) {
        digitalWrite(hapticPins[0], LOW);
    }
    if(!right) {
        digitalWrite(hapticPins[1], LOW);
    }

    if(left) {
        Serial.println("left");
        digitalWrite(hapticPins[0], HIGH);
        digitalWrite(hapticPins[1], LOW);
        delay(400);
        digitalWrite(hapticPins[0], LOW);
    }

    if(right) {
        Serial.println("right");
        digitalWrite(hapticPins[1], HIGH);
        delay(400);
        digitalWrite(hapticPins[1], LOW);
    }


}

void loop() {

  for(int i=0;i<N_SENSORS;i++){
     sensorValues[i] = analogRead(sensors[i])*0.3 + sensorValues[i]*0.7;
    }

    Serial.print(sensorValues[0]);
    Serial.print(", ");
    Serial.print(sensorValues[1]);
    Serial.print(", ");

    Serial.print(sensorValues[2]);
    Serial.print(", ");
    Serial.print(sensorValues[3]);
    Serial.println();

    boolean left = sensorValues[0] < THRESHOLD_RAMP || sensorValues[2] > THRESHOLD_OBJ || sensorValues[0] > THRESHOLD_OBJ_RAMP  ;
    boolean right = sensorValues[1] < THRESHOLD_RAMP || sensorValues[3] > THRESHOLD_OBJ || sensorValues[1] > THRESHOLD_OBJ_RAMP ;

    engageHaptics(left, right);

    delay(100);
}
