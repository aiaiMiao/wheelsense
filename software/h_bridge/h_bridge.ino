/*
* Arduino code for SN754410 H-bridge
* motor driver control.
* copyleft Feb. 2010, Fabian Winkler *
*/

enum dir {
  FORWARD,
  BACKWARD
};

enum motor {
  M1,
  M2
};

void setDirection(dir newDir);

int speed1Pin = 12;
int speed2Pin = 13;
int speeds[] = {speed1Pin, speed2Pin};

int motor1APin = 8;
int motor1BPin = 9;
int motor2APin = 10;
int motor2BPin = 11;
int motor1[2] = {motor1APin, motor1BPin};
int motor2[2] = {motor2APin, motor2BPin};
int* motors[2] = {motor1, motor2};

int values[] = {0, 0};

void setup() {
  Serial.begin(9600);
// set digital i/o pins as outputs: 
  pinMode(motor1[0], OUTPUT);
  pinMode(motor1[1], OUTPUT);
  pinMode(motor2[0], OUTPUT);
  pinMode(motor2[1], OUTPUT);
  pinMode(speeds[0], OUTPUT); 
  pinMode(speeds[1], OUTPUT); 
  setDirection(M1, FORWARD); 
  setDirection(M2, FORWARD); 
}

void setDirection(motor mtr, dir newDir) {
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

void loop() {
//  // put motor in forward motion
  int newDir = 1;
  if (newDir == FORWARD) {
    Serial.println("Forward");
    setDirection(M1, FORWARD);
  } else {
    Serial.println("Backward");
    setDirection(M1, BACKWARD);
  }
  // just invert the above values for reverse motion, 
  // i.e. motor1APin = HIGH and motor1BPin = LOW
  // control the speed 0- 255
  values[0] = 255; // half speed
  analogWrite(speeds[0], values[0]); // output speed as
}
