#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978

#define NOTE_C7  2093
#define buzzerPin 3
#define botDistPin A1
//#define topDistPin A3
#define frontDistPin A0

#define FRONT_DIST_THRESHOLD 115

#define BACK_FEEDBACK_SOUND NOTE_C7
#define FRONT_FEEDBACK_SOUND NOTE_C5

#define VOLTS_PER_UNIT    .0049F        // (.0049 for 10 bit A-D) 
const int sample_size = 5;
//int topData[sample_size];
int botData[sample_size];
int idx = 0;
bool sensorReady = false;
//int topDistAvg = 0;
int botDistAvg = 0;

void setup() {
  Serial.begin (9600);
  pinMode(buzzerPin, OUTPUT);
  pinMode(botDistPin, INPUT);
//  pinMode(topDistPin, INPUT);
}

int getdistAvg() {
//  int topTotal = 0;
  int botTotal = 0;
  for (int i = 0; i < sample_size; i++) {
//    topTotal += topData[i];
    botTotal += botData[i];
  }
//  int topDistAvg = topTotal / sample_size;
  int botDistAvg = botTotal / sample_size;
//  Serial.print("topDistAvg: ");
//  Serial.println(topDistAvg);
  Serial.print("botDistAvg: ");
  Serial.println(botDistAvg);
//  return min(topDistAvg, botDistAvg);
  return botDistAvg;
}

void giveFeedback(int distance, int sound) {
  if (distance < 50) {
    if (distance < 15) {
      distance = 15;
    }
    int duration = map(distance, 15, 50, 10, 300);
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
//  int topProx = analogRead(topDistPin);
  int botProx = analogRead(botDistPin);
//  Serial.print("topProx: ");
//  Serial.println(topProx);
  Serial.print("botProx: ");
  Serial.println(botProx);
  
//  float topV = (float)topProx * VOLTS_PER_UNIT;
  float botV = (float)botProx * VOLTS_PER_UNIT;
//  if (topV < .2 && botV < .2) {
  if (botV < .2) {
    return false;
  }
  
//  int topCm = 60.495 * pow(topV,-1.1904);
  int botCm = 60.495 * pow(botV,-1.1904);

//  topData[idx] = topCm;
  botData[idx++] = botCm;
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

void loop() {
  boolean frontBeep = handleFront();
  if(!frontBeep) {
    handleBack();
  }
  delay(50);
}

