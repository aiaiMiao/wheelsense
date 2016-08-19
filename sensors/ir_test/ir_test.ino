

/* ReadSharpIR.pde
*  simple sketch to read values from a IR Distance Sensor
*/ 
#define N_SENSORS 3

int value;
int sensors[] = {A0, A1, A2};
int i;

void setup()
{
    Serial.begin(115200);            // setup serial port to laptop to print values
}

void loop() 
{
  for(i=0; i<N_SENSORS; i++) {
    value = analogRead(sensors[i]);
    Serial.print(value);
    if(i != N_SENSORS - 1) {
      Serial.print(", ");
    } else {
      Serial.println();
    }
  }

    delay(200);                                               // short delay 
}

