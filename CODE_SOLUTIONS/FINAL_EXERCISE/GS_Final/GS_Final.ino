#include <SPI.h>
#include <LoRa.h>

#define RFM95_INT 2
#define RFM95_RST 9
#define RFM95_CS 10
#define LED_B1 4
#define LED_B2 6
#define LED_R 8

String secretKey = "<PASSWORD>";
String inputmsg;

uint8_t GSconnected = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(LED_B1, OUTPUT);
  pinMode(LED_B2, OUTPUT);
  pinMode(LED_R, OUTPUT);
  digitalWrite(LED_B1, HIGH);
  delay(100);
  digitalWrite(LED_B1, LOW);
  digitalWrite(LED_B2, HIGH);
  delay(100);
  digitalWrite(LED_B2, LOW);
  digitalWrite(LED_R, HIGH);
  delay(100);
  digitalWrite(LED_R, LOW);
  setupLora();
  Serial.print("Ground ");
  Serial.print(secretKey);
  Serial.println(" Initialized...");
}

void loop() {
  // put your main code here, to run repeatedly:

  // Keep transmitting a connection request
  // If a connection is received, enable user input, else say "Waiting for connection"

  // while(GSconnected == 0)
  // {
  //   connRequest();
  //   delay(100);
  //   receiveLora();
  // }

  if (Serial.available()) {
    // if(GSconnected == 0)
    // {
    //   Serial.println("Not connected to a receiver");
    // }
    // else
    // {
    inputmsg = secretKey;
    inputmsg += Serial.readStringUntil('\n');
    inputmsg.trim();
    inputmsg.toUpperCase();
    transmitLora();
    inputmsg = "";
    // }
  }

  receiveLora();
}


// OTHER FUNCTIONS

// LoRa INITIAL SETUP
void setupLora(void) {

  if (!LoRa.begin(434E6)) {
    Serial.println("Starting LoRa failed!");
  } else {
    Serial.println("LoRa radio init OK!");
  }

  // LoRa.setSyncWord(1);
}

// LoRa TRANSMISSION
void transmitLora(void) {
  if (inputmsg != "<STATION70>CONN_REQUEST") {
    Serial.print("Sending packet: ");
    Serial.println(inputmsg);
  }

  // send packet
  LoRa.beginPacket();
  LoRa.print(inputmsg);
  // LoRa.print(counter);
  LoRa.endPacket();
}

void receiveLora() {
  // try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // received a packet
    String LoRaData = "";

    // read packet
    while (LoRa.available()) {
      LoRaData = LoRaData + (char)LoRa.read();
      // Serial.println(LoRaData);
    }

    // Serial.println(LoRaData);
    // if(LoRaData == "Image Incoming")
    // {
    //   getPhoto();
    // }
    // else
    // {
    if (LoRaData.substring(0, secretKey.length()) == secretKey) {
      LoRaData.remove(0, secretKey.length());
      // Serial.println(secretKey.length());
      // if(LoRaData == "HANDSHAKE...")
      // {
      //   msgConnected();
      // }
      // else if(GSconnected == 0)
      // {
      //   Serial.println("Searching for receiver...");
      // }
      Serial.print("Received packet '");
      Serial.print(LoRaData);
      Serial.print("'");
      // print RSSI of packet
      Serial.print(" with RSSI ");
      Serial.println(LoRa.packetRssi());
    } else {
      Serial.println("No reply, is there a listener around?\r\nVerify secretKey pairing");
    }


    // }
  }
}

void getPhoto() {
  // try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // received a packet
    String LoRaData = "";

    // read packet
    while (LoRa.available()) {
      LoRaData = LoRaData + (char)LoRa.read();
      // Serial.println(LoRaData);
    }
    Serial.print(LoRaData);
  }
}

void msgConnected(void) {
  Serial.println("Receiver <STATION70> connected");
  GSconnected == 1;
}

void connRequest(void) {
  inputmsg = secretKey;
  inputmsg += "CONN_REQUEST";
  inputmsg.trim();
  transmitLora();
  inputmsg = "";
}