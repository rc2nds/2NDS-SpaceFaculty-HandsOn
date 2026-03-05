//THIS OBC CODE RECEIVES A MESSAGE FROM THE ADCS

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
  if (ADCSSerial.available() > 0) {
    // Read data from SoftwareSerial and print to Serial monitor
    String data = ADCSSerial.readStringUntil('\n');
    Serial.println("Received from ADCS: " + data);
  }
}
