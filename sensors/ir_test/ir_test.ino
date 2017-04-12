

/* ReadSharpIR.pde
*  simple sketch to read values from a IR Distance Sensor
*/ 
#define N_SENSORS 4

int value;
int sensors[] = {A2, A3, A4, A5};
int i;

void setup()
{
    Serial.begin(9600);            // setup serial port to laptop to print values
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

