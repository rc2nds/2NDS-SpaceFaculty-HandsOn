//THIS ADCS CODE SENDS THE TEMPERATURE READ ON THE ADCS TO THE OBC
#include <SoftwareSerial.h>

// Define the pin for sensor data input
#define PIN_ADCSTEMP A6

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

  // Print sensor data to Serial monitor
  Serial.print("Temp is: ");
  Serial.print(temp);
  Serial.println("°C");

  // Send sensor data to receiver Arduino
  OBCSerial.print("TEMPADCS: ");
  OBCSerial.print(temp);
  OBCSerial.println("°C");

  // Delay for stability
  delay(2000);
}
