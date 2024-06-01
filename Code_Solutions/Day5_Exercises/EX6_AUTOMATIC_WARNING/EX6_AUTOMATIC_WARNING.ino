#include <SPI.h>
#include <LoRa.h>

#define PIN_OBCTEMP A1
#define PIN_EPSTEMP A2
#define PIN_VBAT A0

#define TEMPERATURE_THRESHOLD 30
#define VOLTAGE_THRESHOLD 3.6

unsigned long previousMillis = 0;
unsigned long interval = 1000; // Check every 1 second
unsigned long loraInterval = 60000; // LoRa transmission interval (1 minute)

bool obcTempWarning = false;
bool epsTempWarning = false;
bool vbatWarning = false;

void setup() {
  Serial.begin(9600);
  Serial.println("LoRa Sender");

  if (!LoRa.begin(434E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
}

void loop() {
  unsigned long currentMillis = millis();
  
  // Check for warnings every second
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    checkWarnings();
  }

  // Send warning message via LoRa if any warning is detected
  if (obcTempWarning || epsTempWarning || vbatWarning) {
    sendWarning();
  }

  // Handle incoming commands from serial if available
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    
    if (input == "TEMPOBC") {
      readTEMPOBC();
    } else if (input == "TEMPEPS") {
      readTEMPEPS();
    } else if (input == "VBAT") {
      readVBAT();
    } else if (input == "ALL") {
      sendAll();
    } else if (input == "STATUS") {
      printStatus();
    }
  }
}

void readTEMPOBC() {
  float obcReading = analogRead(PIN_OBCTEMP);
  float obcTemp = (obcReading * (5.0 / 1024.0)) * 100.0; // Convert to Celsius
  
  LoRa.beginPacket();
  LoRa.print("[Team Name] TEMPOBC = ");
  LoRa.print(obcTemp);
  LoRa.println(" deg");
  LoRa.endPacket();
}

void readTEMPEPS() {
  float epsReading = analogRead(PIN_EPSTEMP);
  float epsTemp = (epsReading * (5.0 / 1024.0)) * 100.0; // Convert to Celsius
  
  LoRa.beginPacket();
  LoRa.print("[Team Name] TEMPEPS = ");
  LoRa.print(epsTemp);
  LoRa.println(" deg");
  LoRa.endPacket();
}

void readVBAT() {
  float vbat = analogRead(PIN_VBAT) * (5.0 / 1023.0);
  
  if (vbat < VOLTAGE_THRESHOLD) {
    vbatWarning = true;
  }
}

void sendAll() {
  readTEMPOBC();
  readTEMPEPS();
  readVBAT();
  printStatus();
}

void checkWarnings() {
  float obcReading = analogRead(PIN_OBCTEMP);
  float obcTemp = (obcReading * (5.0 / 1024.0)) * 100.0; // Convert to Celsius

  float epsReading = analogRead(PIN_EPSTEMP);
  float epsTemp = (epsReading * (5.0 / 1024.0)) * 100.0; // Convert to Celsius

  float vbat = analogRead(PIN_VBAT) * (5.0 / 1023.0);

  obcTempWarning = obcTemp > TEMPERATURE_THRESHOLD;
  epsTempWarning = epsTemp > TEMPERATURE_THRESHOLD;
  vbatWarning = vbat < VOLTAGE_THRESHOLD;
}

void printStatus() {
  Serial.print("[STATUS] ");

  if (!obcTempWarning && !epsTempWarning && !vbatWarning) {
    Serial.println("NOMINAL");
  } else {
    if (obcTempWarning) {
      Serial.print("WARNING! TEMPOBC is too high: ");
      Serial.print(analogRead(PIN_OBCTEMP) * (5.0 / 1024.0) * 100.0);
      Serial.println(" deg!");
    }
    if (epsTempWarning) {
      Serial.print("WARNING! TEMPEPS is too high: ");
      Serial.print(analogRead(PIN_EPSTEMP) * (5.0 / 1024.0) * 100.0);
      Serial.println(" deg!");
    }
    if (vbatWarning) {
      Serial.print("WARNING! VBAT is too low: ");
      Serial.print(analogRead(PIN_VBAT) * (5.0 / 1023.0));
      Serial.println(" V!");
    }
  }
}

void sendWarning() {
  LoRa.beginPacket();
  LoRa.println("WARNING! Satellite detected a problem.");
  LoRa.endPacket();
}
