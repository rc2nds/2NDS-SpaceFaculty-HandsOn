//write a new function called sendAll() that is capable of sending all the readings in one line. This function has to be activated when typing “ALL”.

#include <SPI.h>
#include <LoRa.h>

#define PIN_OBCTEMP A1
#define PIN_EPSTEMP A2
#define PIN_VBAT A0

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
    } else if (input == "TEMPCOMPARISON") {
      maxTemp();
    } else if (input == "VBAT") {
      readVBAT();
    } else if (input == "ALL") {
      sendAll();
    }
  }

  delay(100); // Delay for debounce and to allow other tasks
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

void maxTemp() {
  float obcReading = analogRead(PIN_OBCTEMP);
  float obcTemp = (obcReading * (5100.00 / 1024.00)) / 10.00;
  float epsReading = analogRead(PIN_EPSTEMP);
  float epsTemp = (epsReading * (5100.00 / 1024.00)) / 10.00;
  
  float tempDiff = abs(obcTemp - epsTemp);
  
  if (obcTemp > epsTemp) {
    Serial.print("TEMPOBC is greater than TEMPEPS - Difference = ");
    Serial.print(tempDiff);
    Serial.println("°C");
  } else if (epsTemp > obcTemp) {
    Serial.print("TEMPEPS is greater than TEMPOBC - Difference = ");
    Serial.print(tempDiff);
    Serial.println("°C");
  } else {
    Serial.println("Both temperatures are equal");
  }
}

void readVBAT() {
  float vbat = analogRead(PIN_VBAT) * (5.10 / 1023.00);
  
  LoRa.beginPacket();
  LoRa.print("[Team Name] VBAT = ");
  LoRa.print(vbat);
  LoRa.println(" V");
  LoRa.endPacket();
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
