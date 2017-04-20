#include <Wire.h>

int value;

//#define N_SENSORS 4
//#define THRESHOLD_RAMP 180
//#define THRESHOLD_OBJ 150
//#define THRESHOLD_OBJ_RAMP 350

enum dir {
  FORWARD,
  BACKWARD
};

enum motor {
  M1,
  M2
};

#define N_SENSORS 4
#define THRESHOLD_RAMP 60
#define THRESHOLD_OBJ 150
//#define THRESHOLD_OBJ_RAMP 160
#define THRESHOLD_OBJ_RAMP 170

int sensors[] = {A5, A2, A0, A3};
//int sensors[] = {LB, RB, LT, RT};
int hapticPins[] = {6, 10}; // left, right
int sensorValues[N_SENSORS];
int pinOn[] = {false, false};
int motor1APin = 8;
int motor1BPin = 9;
int motor2APin = 11;
int motor2BPin = 12;
int motor1[2] = {motor1APin, motor1BPin};
int motor2[2] = {motor2APin, motor2BPin};
int* motors[2] = {motor1, motor2};


// FRONT AND BACK
#define NOTE_C5  523
#define NOTE_C7  2093
#define buzzerPin 3
#define botDistPin A1
#define topDistPin 13
#define frontDistPin A4

#define FRONT_DIST_THRESHOLD 115

#define BACK_FEEDBACK_SOUND NOTE_C7
#define FRONT_FEEDBACK_SOUND NOTE_C5

#define VOLTS_PER_UNIT    .0049F        // (.0049 for 10 bit A-D) 
const int sample_size = 5;
int topData[sample_size];
int botData[sample_size];
int idx = 0;
bool sensorReady = false;
int topDistAvg = 0;
int botDistAvg = 0;


void setup() {
    Serial.begin(9600);

    pinMode(hapticPins[0], OUTPUT);
    pinMode(hapticPins[1], OUTPUT);

    pinMode(motor1[0], OUTPUT);
    pinMode(motor1[1], OUTPUT);
    pinMode(motor2[0], OUTPUT);
    pinMode(motor2[1], OUTPUT);
    
    for(int i = 0; i < N_SENSORS; i++) {
      pinMode(sensors[i], INPUT);
    }

    pinMode(buzzerPin, OUTPUT);
    pinMode(botDistPin, INPUT);

    delay(200);
    setDirection(M1, FORWARD); 
    setDirection(M2, FORWARD);
    engageHaptics(false, false); 
}

void engageHaptics(boolean left, boolean right) {

    Serial.print(left);
    Serial.print(right);
    Serial.println("\n");
    if(!left && !right) {
        Serial.println("neither");
        digitalWrite(hapticPins[0], LOW);
        digitalWrite(hapticPins[1], LOW);
        return;
    } else if(left && right) {
        Serial.println("both");
        digitalWrite(hapticPins[0], HIGH);
        digitalWrite(hapticPins[1], HIGH);
        return;
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
        delay(200);
        digitalWrite(hapticPins[0], LOW);
    }

    if(right) {
        Serial.println("right");
        digitalWrite(hapticPins[1], HIGH);
        delay(200);
        digitalWrite(hapticPins[1], LOW);
    }


}

int getdistAvg() {
  int topTotal = 0;
  int botTotal = 0;
  for (int i = 0; i < sample_size; i++) {
    topTotal += topData[i];
    botTotal += botData[i];
  }
  int topDistAvg = topTotal / sample_size;
  int botDistAvg = botTotal / sample_size;
  Serial.print("topDistAvg: ");
  Serial.println(topDistAvg);
  Serial.print("botDistAvg: ");
  Serial.println(botDistAvg);
  return min(topDistAvg, botDistAvg);
//  return botDistAvg;
}

void giveFeedback(int distance, int sound) {
  Serial.println("feedback");
  if (distance < 50) {
    if (distance < 15) {
      distance = 15;
    }
    int duration = map(distance, 15, 50, 10, 250);
    tone(buzzerPin, sound, 20);
    delay(duration);
    noTone(buzzerPin);
  }
}

void giveBackFeedback(int distance) {
  giveFeedback(distance, BACK_FEEDBACK_SOUND);
}

void giveFrontFeedback() {
  giveFeedback(25, FRONT_FEEDBACK_SOUND);
}


bool updateDistance() {
  int topProx = pingIt(topDistPin);
  int botProx = pingIt(botDistPin);
//  Serial.print("topProx: ");
//  Serial.println(topProx);
//  Serial.print("botProx: ");
//  Serial.println(botProx);
  
//  float topV = (float)topProx * VOLTS_PER_UNIT;
//  float botV = (float)botProx * VOLTS_PER_UNIT;
//  if (topV < .2 && botV < .2) {
  if (botProx < 3 || topProx < 3) {
    return false;
  }
  
//  int topCm = 60.495 * pow(topV,-1.1904);
//  int botCm = 60.495 * pow(botV,-1.1904);

  topData[idx] = topProx;
  botData[idx++] = botProx;
  if (!sensorReady && idx == 5) {
    sensorReady = true;
  } 
  idx = idx % sample_size;
  return true;
}

void handleBack() {
  int inRange = updateDistance();
  int minDist = getdistAvg();
  Serial.print("minDist: ");
  Serial.println(minDist);
  if (sensorReady && inRange) {
    giveBackFeedback(minDist);
  } else {
    Serial.println("Not in range");
  }
  Serial.println("------------");
}

double frontDist = 300;

boolean handleFront() {
  double dist = analogRead(frontDistPin);
  const double f = 0.3;
  frontDist = f * dist + (1-f) * frontDist;
  
  Serial.print("frontDist:");
  Serial.println(dist);
  if(dist < FRONT_DIST_THRESHOLD) {
    giveFrontFeedback();
    return true;
  } else {
    return false;
  }
}

void setDirection(motor mtr, dir newDir) {
  Serial.println("setDirection");
  int* m = motors[mtr];
  Serial.println(m[0]);
  Serial.println(m[1]);
  if (newDir == FORWARD) {
    Serial.println("Forward");
    digitalWrite(m[0], LOW);
    digitalWrite(m[1], HIGH);
  } else {
    Serial.println("Backward");
    digitalWrite(m[0], HIGH);
    digitalWrite(m[1], LOW);
  }
}

long microsecondsToInches(long microseconds) {
  // According to Parallax's datasheet for the PING))), there are
  // 73.746 microseconds per inch (i.e. sound travels at 1130 feet per
  // second).  This gives the distance travelled by the ping, outbound
  // and return, so we divide by 2 to get the distance of the obstacle.
  // See: http://www.parallax.com/dl/docs/prod/acc/28015-PING-v1.3.pdf
  return microseconds / 74 / 2;
}

long microsecondsToCentimeters(long microseconds) {
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the
  // object we take half of the distance travelled.
  return microseconds / 29 / 2;
}

int pingIt(int pingPin) {
    // establish variables for duration of the ping,
  // and the distance result in inches and centimeters:
  long duration, inches, cm;

  // The PING))) is triggered by a HIGH pulse of 2 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  pinMode(pingPin, OUTPUT);
  digitalWrite(pingPin, LOW);
  delayMicroseconds(2);
  digitalWrite(pingPin, HIGH);
  delayMicroseconds(5);
  digitalWrite(pingPin, LOW);

  // The same pin is used to read the signal from the PING))): a HIGH
  // pulse whose duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  pinMode(pingPin, INPUT);
  duration = pulseIn(pingPin, HIGH);

  // convert the time into a distance
//  inches = microsecondsToInches(duration);
  cm = microsecondsToCentimeters(duration);
  return cm;
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
//int sensors[] = {LB, RB, LT, RT};
    boolean leftBottomObs = sensorValues[0] > THRESHOLD_OBJ_RAMP;
    boolean leftBottomRamp = sensorValues[0] < THRESHOLD_RAMP;
    boolean leftBottom = leftBottomObs || leftBottomRamp;
    boolean leftTop =  sensorValues[2] > THRESHOLD_OBJ;
    boolean rightBottomObs = sensorValues[1] > THRESHOLD_OBJ_RAMP;
    boolean rightBottomRamp = sensorValues[1] < THRESHOLD_RAMP;
    boolean rightBottom = rightBottomObs || rightBottomRamp;
    boolean rightTop = sensorValues[3] > THRESHOLD_OBJ;
    boolean left = leftBottom || leftTop;
    boolean right = rightBottom || rightTop;
//    if (leftBottomRamp) {
//      Serial.println("leftBottomRamp");
//    }
//    if (leftBottomObs) {
//      Serial.println("leftBottomObs");
//    }
//    if (leftTop) {
//      Serial.println("leftTop");
//    }
//    if (rightBottomRamp) {
//      Serial.println("rightBottomRamp");
//    }
//    if (rightBottomObs) {
//      Serial.println("rightBottomObs");
//    }
//    if (rightTop) {
//      Serial.println("rightTop");
//    }
    engageHaptics(left, right);

    boolean frontBeep = handleFront();
    if(!frontBeep) {
      handleBack();
    }

    delay(100);
}
