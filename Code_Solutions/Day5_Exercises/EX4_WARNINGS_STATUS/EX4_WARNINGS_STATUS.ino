#include <SPI.h>
#include <LoRa.h>

#define PIN_OBCTEMP A1
#define PIN_EPSTEMP A2
#define PIN_VBAT A0

#define TEMPERATURE_THRESHOLD 30
#define VOLTAGE_THRESHOLD 3.6

unsigned long previousMillis = 0;
unsigned long interval = 1000; // Check every 1 second

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

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    checkWarnings();
  }
}

void readTEMPOBC() {
  float obcReading = analogRead(PIN_OBCTEMP);
  float obcTemp = (obcReading * (5100.00 / 1024.00)) / 10.00;
  
  LoRa.beginPacket();
  LoRa.print("[Team Name] TEMPOBC = ");
  LoRa.print(obcTemp);
  LoRa.println(" deg");
  LoRa.endPacket();
}

void readTEMPEPS() {
  float epsReading = analogRead(PIN_EPSTEMP);
  float epsTemp = (epsReading * (5100.00 / 1024.00)) / 10.00;
  
  LoRa.beginPacket();
  LoRa.print("[Team Name] TEMPEPS = ");
  LoRa.print(epsTemp);
  LoRa.println(" deg");
  LoRa.endPacket();
}

void readVBAT() {
  float vbat = analogRead(PIN_VBAT) * (5.10 / 1023.00);
  
  if (vbat < VOLTAGE_THRESHOLD) {
    vbatWarning = true;
  }
}

void sendAll() {
  float obcReading = analogRead(PIN_OBCTEMP);
  float obcTemp = (obcReading * (5100.00 / 1024.00)) / 10.00;
  float epsReading = analogRead(PIN_EPSTEMP);
  float epsTemp = (epsReading * (5100.00 / 1024.00)) / 10.00;
  float vbat = analogRead(PIN_VBAT) * (5.10 / 1023.00);

  LoRa.beginPacket();
  LoRa.print("[Team Name] TEMPOBC = ");
  LoRa.print(obcTemp);
  LoRa.print(" deg | TEMPEPS = ");
  LoRa.print(epsTemp);
  LoRa.print(" deg | VBAT = ");
  LoRa.print(vbat);
  LoRa.println(" V");
  LoRa.endPacket();
}

void checkWarnings() {
  float obcReading = analogRead(PIN_OBCTEMP);
  float obcTemp = (obcReading * (5100.00 / 1024.00)) / 10.00;

  float epsReading = analogRead(PIN_EPSTEMP);
  float epsTemp = (epsReading * (5100.00 / 1024.00)) / 10.00;

  float vbat = analogRead(PIN_VBAT) * (5.10 / 1023.00);

  obcTempWarning = obcTemp > TEMPERATURE_THRESHOLD;
  epsTempWarning = epsTemp > TEMPERATURE_THRESHOLD;
  vbatWarning = vbat < VOLTAGE_THRESHOLD;
}

void printStatus() {
  Serial.print("[");
  printTime();
  Serial.print("] ");

  if (!obcTempWarning && !epsTempWarning && !vbatWarning) {
    Serial.println("NOMINAL");
  } else {
    if (obcTempWarning) {
      Serial.print("WARNING! TEMPOBC is too high: ");
      Serial.print(analogRead(PIN_OBCTEMP) * (5100.00 / 1024.00)/10.00);
      Serial.println(" deg!");
    }
    if (epsTempWarning) {
      Serial.print("WARNING! TEMPEPS is too high: ");
      Serial.print(analogRead(PIN_EPSTEMP) * (5100.00 / 1024.00)/10.00);
      Serial.println(" deg!");
    }
    if (vbatWarning) {
      Serial.print("WARNING! VBAT is too low: ");
      Serial.print(analogRead(PIN_VBAT) * (5.10 / 1023.00));
      Serial.println(" V!");
    }
  }
}

void printTime() {
  unsigned long totalSeconds = millis() / 1000;
  unsigned long minutes = totalSeconds / 60;
  unsigned long seconds = totalSeconds % 60;

  if (minutes < 10) {
    Serial.print("0");
  }
  Serial.print(minutes);
  Serial.print("m ");
  
  if (seconds < 10) {
    Serial.print("0");
  }
  Serial.print(seconds);
  Serial.print("s");
}
