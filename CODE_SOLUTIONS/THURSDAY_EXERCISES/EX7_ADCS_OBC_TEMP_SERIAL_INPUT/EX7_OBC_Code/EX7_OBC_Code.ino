#include <SoftwareSerial.h>

// Initialize SoftwareSerial object
SoftwareSerial ADCSSerial(3, 4);

void setup() {
  // Start serial communication
  Serial.begin(9600);
  ADCSSerial.begin(9600);
}

void loop() {
  // Check if data is available to read
  if (Serial.available() > 0) {
    // Read command from Serial monitor
    String command = Serial.readStringUntil('\n');
    command.trim(); // Remove leading/trailing whitespace
    
    // Check if the command is 'TEMPADCS'
    if (command == "TEMPADCS") {
      // Send command to ADCS to request temperature
      ADCSSerial.println("TEMPADCS");
      
      // Read and print response from ADCS
      if (ADCSSerial.available() > 0) {
        String response = ADCSSerial.readStringUntil('\n');
        Serial.println("Received from ADCS: " + response);
      }
    }
  }
}
