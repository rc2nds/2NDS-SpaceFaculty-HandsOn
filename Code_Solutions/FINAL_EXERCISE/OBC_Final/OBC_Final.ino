#include <Adafruit_VC0706.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include <LoRa.h>


#define PIN_VBAT   A0
#define PIN_OBCTEMP A1

#define RFM95_CS 10
#define RFM95_RST 9
#define RFM95_INT 2

#define ADCS_RESET 7      // Active Low
#define ADCS_ENABLE 8     // Active Low


// Change to 434.0 or other frequency, must match RX's freq!
// #define RF95_FREQ 434.0 //915.0

#define ENABLE_PAYLOAD_SERIAL
#define ENABLE_TELEMETRY_SERIAL

SoftwareSerial ADCSSerial (3,4);

SoftwareSerial cameraconnection(5,6);

Adafruit_VC0706 cam = Adafruit_VC0706(&cameraconnection);


String secretKey = "<PASSWORD>";
String inputmsg;

uint8_t OBCconnected = 0;

// User Flags

#define SERIALMODE      // If this line is "UNCOMMENTED", the OBC will function with Serial over USB and LoRa radio will be disabled.
                        // "COMMENT" this line to use the LoRa radio.

void setup()
{
  pinMode(ADCS_ENABLE, OUTPUT);
  digitalWrite(ADCS_ENABLE, LOW);
  pinMode(ADCS_RESET, OUTPUT);
  digitalWrite(ADCS_RESET, HIGH);
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  Serial.begin(9600);
  Serial.print("On Board Computer ");
  Serial.print(secretKey);
  Serial.println(" initialized");

  #ifndef SERIALMODE
    setupLora();
  #endif
  cameraSetup();

  Serial.println("------------------");
  Serial.println("Waiting for commands: ");

  ADCSSerial.begin(9600);
}

void loop()
{
  #ifdef SERIALMODE
    if(Serial.available())
    {
      inputmsg = Serial.readStringUntil('\n');
      inputmsg.trim();
      // if(inputmsg.equals("ENADCS"))
      // {
      //   digitalWrite(ADCS_ENABLE, HIGH);
      //   Serial.println("ADCS Enabled");
      // }
      if(inputmsg.equals("TEMPOBC"))
      {
        readOBCTemp();
      }
      if(inputmsg.equals("TEMPEPS"))
      {
        readEPSTemp();
      }
      if(inputmsg.equals("VBAT"))
      {
        readVbat();
      }
      if(inputmsg.equals("ENADCS"))
      {
        enableADCS();
      }
      if(inputmsg.equals("TIME"))
      {
        sendTime();
      }
      if(inputmsg.equals("CAM"))
      {
        ADCSSerial.end();
        takePictureSendSerial();
        ADCSSerial.begin(9600);
      }
      else
      {
        ADCSSerial.println(inputmsg);
      }
    }

  #else

    receiveLora();
  #endif

  if(ADCSSerial.available())
  {
    String content = ADCSSerial.readStringUntil('\n');
    content.trim();

    #ifdef SERIALMODE
      Serial.println(content);
    #else
      // send packet
      LoRa.beginPacket();
      LoRa.print(content);
      LoRa.endPacket();
    #endif
  }
}


// OTHER FUNCTIONS

// LoRa INITIAL SETUP
void setupLora(void)
{

  if (!LoRa.begin(434E6)) {
    Serial.println("Starting LoRa failed!");
  }
  else
  {
    Serial.println("LoRa radio init OK!");
  }
  // LoRa.setSyncWord(1);
}

// LoRa RECEPTION
void receiveLora(void)
{
  // try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // received a packet
    String LoRaData ="";

    // read packet
    while (LoRa.available()) {
      LoRaData = LoRaData+(char)LoRa.read();
    }

    if(LoRaData.substring(0, secretKey.length()) == secretKey)
    {
      LoRaData.remove(0, secretKey.length());
      Serial.println(LoRaData);
    }

    if(LoRaData != "CAM")
    {
      Serial.print("Received packet '");
      Serial.print(LoRaData);
      Serial.println("'");
    }

    if(LoRaData == "VBAT")
    {
      readVbat();
    }
    if(LoRaData == "ENADCS")
    {
      enableADCS();
    }
    if(LoRaData == "TEMPOBC")
    {
      readOBCTemp();
    }
    if(LoRaData == "TEMPEPS")
    {
      readEPSTemp();
    }
    if(LoRaData == "CAM")
    {
      ADCSSerial.end();
      takePictureSendSerial();
      ADCSSerial.begin(9600);
    }
    if(LoRaData == "TIME")
    {
      sendTime();
    }
    if(LoRaData == "ALL")
    {
      sendAll();
      ADCSSerial.println(LoRaData);
    }
    else
    {
      ADCSSerial.println(LoRaData);
    }
  }
}

// LoRa TRANSMISSION
void transmitLora()
{
  // send packet
  LoRa.beginPacket();
  LoRa.print(inputmsg);
  LoRa.endPacket();
}

void sendPhoto()
{
    // send packet
  LoRa.beginPacket();
  LoRa.print(inputmsg);
  // LoRa.print(counter);
  LoRa.endPacket();
}

// THIS FUNCTION WILL SETUP THE CAMERA
void cameraSetup(void)
{
  Serial.println("VC0706 Camera snapshot test");
            
  // Try to locate the camera
  if (cam.begin()) {
    Serial.println("Camera Found!");
  }
  else
  {
    Serial.println("No camera found!");
  }

  char *reply = cam.getVersion();
  if (reply == 0) {
    Serial.println("Failed to get version");
  } else {
    Serial.println("Camera Version:");
    Serial.print(reply);
  }

  cam.setImageSize(VC0706_640x480);        // biggest
  //cam.setImageSize(VC0706_320x240);        // medium
  // cam.setImageSize(VC0706_160x120);          // small

  uint8_t imgsize = cam.getImageSize();
  Serial.print("Image size: ");
  if (imgsize == VC0706_640x480) Serial.println("640x480");
  if (imgsize == VC0706_320x240) Serial.println("320x240");
  if (imgsize == VC0706_160x120) Serial.println("160x120");
  delay(500);
}

// THIS FUNCTION WILL TAKE A PICTURE AND SEND IT BY SERIAL USB 
void takePictureSendSerial()
{
  // inputmsg.concat("Image Incoming");
  // transmitLora();
  cameraconnection.listen();
  // cam.begin();
  //cam.reset();
  cam.takePicture();

  // Get the size of the image (frame) taken  
  uint32_t jpglen = cam.frameLength();
  //Serial.println(sizeof(jpglen));

  // Read all the data up to # bytes!
  byte wCount = 0; // For counting # of writes
  while (jpglen > 0) 
  {
    // read 32 bytes at a time;
    uint8_t *buffer;
    uint8_t bytesToRead = min((uint32_t)32, jpglen); // change 32 to 64 for a speedup but may not work with all setups!
    buffer = cam.readPicture(bytesToRead);
    inputmsg = buffer;
    sendPhoto();
    delay(50);
    Serial.println(bytesToRead);
    Serial.write(buffer, bytesToRead);

    jpglen -= bytesToRead;
  }

  cam.resumeVideo();
}


void readOBCTemp(void)
{
  int reading = analogRead(A1);
  float temp = (reading * (5100.00 / 1024.00)) / 10.00;
  #ifdef SERIALMODE
    Serial.print("OBC Temperature: ");
    Serial.print(temp);
    Serial.println("°C");
  #else
    inputmsg = secretKey;
    inputmsg.concat("OBC Temperature: ");
    inputmsg.concat(temp);
    inputmsg.concat("°C");
    transmitLora();
    inputmsg = "";
  #endif
}

void readEPSTemp(void)
{
  int reading = analogRead(A2);
  float temp = (reading * (5100.00 / 1024.00)) / 10.00;
  #ifdef SERIALMODE
    Serial.print("EPS Temperature: ");
    Serial.print(temp);
    Serial.println("°C");
  #else
    inputmsg = secretKey;
    inputmsg.concat("EPS Temp is: ");
    inputmsg.concat(temp);
    inputmsg.concat("°C");
    transmitLora();
    inputmsg = "";
  #endif
}

void readVbat(void)
{
  float vbat = analogRead(A0);
  vbat = vbat * (5.10 / 1023.00);
  #ifdef SERIALMODE
    Serial.print("Battery voltage: ");
    Serial.print(vbat);
    Serial.println("V");
  #else
    inputmsg = secretKey;
    inputmsg.concat("Battery voltage: ");
    inputmsg.concat(vbat);
    inputmsg.concat("V");
    transmitLora();
    inputmsg = "";
  #endif
}

void enableADCS()
{
  digitalWrite(ADCS_ENABLE, HIGH);
  #ifdef SERIALMODE
    Serial.println("ADCS Enabled");
  #else
    inputmsg = secretKey;
    inputmsg.concat("ADCS Enabled");
    transmitLora();
    inputmsg = "";
  #endif
}

void sendTime()
{
  unsigned long timenow = millis() / 1000;
  #ifdef SERIALMODE
    Serial.print("Time since boot: ");
    Serial.print(timenow);
    Serial.println("s");
  #else
    inputmsg = secretKey;
    inputmsg.concat("Time since boot: ");
    inputmsg.concat(timenow);
    inputmsg.concat("s");
    transmitLora();
    inputmsg = "";
  #endif
}

void sendAll()
{
  readEPSTemp();
  delay(50);
  readVbat();
  delay(50);
  readOBCTemp();
  delay(50);
  sendTime();
  delay(50);
}