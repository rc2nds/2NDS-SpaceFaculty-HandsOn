#include <LoRa.h>
#include <SoftwareSerial.h>
#define tempobcpin A1
#define tempepspin A2
#define batterypin A0

int secs = 0;
int mins = 0;
int tempobcvolt;
float tempobc;
int tempepsvolt;
float tempeps;
float battery;
SoftwareSerial ADCSSerial(3, 4);

void setup() {
  Serial.begin(9600);
  ADCSSerial.begin(9600);
  if (!LoRa.begin(430E6)) {
    Serial.println("Starting LoRa failed!");
    while (1)
      ;
  }
}

void loop() {
  delay(1000);
  secs = secs + 2;
  if (secs == 60) {
    mins++;
    secs = 0;
  }

  if (ADCSSerial.available() > 0) {
    String data = ADCSSerial.readStringUntil("\n");
    Serial.print(data);
    if (Serial.available() > 0) {
      char input = Serial.read();
      if (input = 'A') {
        LoRa.beginPacket();
        gettimebrackets();
        LoRa.print("[GROUP NAME] ");
        getbattery();
        LoRa.print(" | ");
        gettempobc();
        LoRa.print(" | ");
        gettempeps();
        LoRa.print(data);
        //LoRa.println("");
        LoRa.endPacket();
      }
    }
  }
}

void gettimebrackets() {
  LoRa.print("[");
  LoRa.print(mins);
  LoRa.print("m ");
  LoRa.print(secs);
  LoRa.print("s] ");
}

void gettempobc() {
  tempobcvolt = analogRead(tempobcpin);
  tempobc = (tempobcvolt * 0.488);
  LoRa.print("TEMPOBC = ");
  LoRa.print(tempobc);
  LoRa.print(" deg");
}

void gettempeps() {
  tempepsvolt = analogRead(tempepspin);
  tempeps = (tempepsvolt * 0.488);
  LoRa.print("TEMPEPS = ");
  LoRa.print(tempeps);
  LoRa.print(" deg");
}

void getbattery() {
  battery = analogRead(batterypin);
  battery = battery * (5.0 / 1023.0);
  LoRa.print("VBAT = ");
  LoRa.print(battery);
  LoRa.print(" V");
}