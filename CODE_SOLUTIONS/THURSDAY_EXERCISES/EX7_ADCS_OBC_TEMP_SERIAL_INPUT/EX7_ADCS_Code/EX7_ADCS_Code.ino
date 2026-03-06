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

// Function to read temperature
float readTemperature() {
  float reading = analogRead(PIN_ADCSTEMP);
  float temp = (reading * (5100.00 / 1024.00)) / 10.00;
  return temp;
}

void loop() {
  // Check if data is available from OBC
  if (OBCSerial.available() > 0) {
    // Read data from SoftwareSerial
    String command = OBCSerial.readStringUntil('\n');
    command.trim(); // Remove leading/trailing whitespace
    
    // Check if the command is 'TEMPADCS'
    if (command == "TEMPADCS") {
      // Call the function to read temperature
      float temp = readTemperature();
      
      // Send sensor data to receiver Arduino
      OBCSerial.print("TEMPADCS: ");
      OBCSerial.print(temp);
      OBCSerial.println("°C");
    }
  }

  // Delay for stability
  delay(1000);
}

