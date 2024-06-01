#include <Adafruit_VC0706.h>
#include <SoftwareSerial.h>
#include <SPI.h>

#define ENABLE_PAYLOAD_SERIAL
#define ENABLE_TELEMETRY_SERIAL


SoftwareSerial cameraconnection(5,6);

Adafruit_VC0706 cam = Adafruit_VC0706(&cameraconnection);

String inputmsg;

uint8_t OBCconnected = 0;

// User Flags

#define SERIALMODE      // If this line is "UNCOMMENTED", the OBC will function with Serial over USB and LoRa radio will be disabled.
                        // "COMMENT" this line to use the LoRa radio.

void setup()
{
  

  Serial.begin(9600);
  Serial.print("On Board Computer ");
  Serial.println(" initialized");

  cameraSetup();

  Serial.println("------------------");
  Serial.println("Waiting for commands: ");
}

void loop()
{
  #ifdef SERIALMODE
    if(Serial.available())
    {
      inputmsg = Serial.readStringUntil('\n');
      inputmsg.trim();
      
      if(inputmsg.equals("CAM"))
      {
        takePictureSendSerial();
      }
    }
#endif
}


// OTHER FUNCTIONS

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
    // sendPhoto();
    // delay(50);
    // Serial.println(bytesToRead);
    Serial.write(buffer, bytesToRead);

    jpglen -= bytesToRead;
  }

  cam.resumeVideo();
}


