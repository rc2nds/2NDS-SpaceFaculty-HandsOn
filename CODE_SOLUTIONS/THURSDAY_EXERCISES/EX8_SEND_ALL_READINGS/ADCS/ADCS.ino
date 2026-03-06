//THIS ADCS CODE SENDS THE TEMPERATURE READ ON THE ADCS TO THE OBC
#include <SoftwareSerial.h>

// Define the pin for sensor data input
#define PIN_ADCSTEMP A6
#define SUN_SENSOR_1 A0
#define SUN_SENSOR_2 A1
#define SUN_SENSOR_3 A2


// Initialize SoftwareSerial object
SoftwareSerial OBCSerial(3, 2);

void setup() {
  // Start serial communication
  Serial.begin(9600);
  OBCSerial.begin(9600);
}

void loop() {
  // Read sensor data
  float reading = analogRead(PIN_ADCSTEMP);
  float temp = (reading * (5100.00 / 1024.00)) / 10.00;
  int ss1 = analogRead(SUN_SENSOR_1);
  int ss2 = analogRead(SUN_SENSOR_2);
  int ss3 = analogRead(SUN_SENSOR_3);

  // Send sensor data to receiver Arduino
  OBCSerial.print(" | TEMPADCS: ");
  OBCSerial.print(temp);
  OBCSerial.print("°C | ");
  if (ss2 > ss3 && ss2 > ss1) {
    OBCSerial.println("THE CAMERA IS OBSERVING EARTH");
  } else {
    OBCSerial.println("THE SATELLITE POSITION NEEDS TO BE ADJUSTED");
  }

  // Delay for stability
  delay(2000);
}
