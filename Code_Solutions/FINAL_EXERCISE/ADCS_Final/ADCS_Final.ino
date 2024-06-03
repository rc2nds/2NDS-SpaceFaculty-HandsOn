#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <Adafruit_GPS.h>
#include <utility/imumaths.h>
#include <SoftwareSerial.h>

// Arduino Nano Every Pin definitions

#define T_ADCS_R_OBC 2
#define R_ADCS_T_OBC 3
#define ENABLE_GPS 4

// 5 NOT CONNECTED
// 6 NOT CONNECTED

#define R_GPS_T_ADCS 7
#define T_GPS_R_ADCS 8

#define MOTOR_IN1 9
#define MOTOR_IN2 10

#define SUN_SENSOR_1 A0
#define SUN_SENSOR_2 A1
#define SUN_SENSOR_3 A2
#define SUN_SENSOR_4 A3

#define ADCS_TEMP A6

// END Arduino Nano Every Pin definitions

// GPS_Tx connected to D8, GPS_Rx connected to pin D7
SoftwareSerial GPSSerial(8,7); // Rx, Tx

// ADCS_Tx connected to D2, ADCS_Rx connected to pin D3
SoftwareSerial OBCSerial (3,2);

// BNO055 Init
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28, &Wire);

// GPS Init
Adafruit_GPS GPS(&GPSSerial);

// Sun sensor variables
uint8_t ss1 = 0;
uint8_t ss2 = 0;
uint8_t ss3 = 0;
uint8_t ss4 = 0;

char c = "";

String secretKey = "<PASSWORD>";
String content;

uint8_t ADCSStatus = 0;


// User Flags

// #define GPSTEST         // If this line is "UNCOMMENTED", the ADCS will work in Serial mode with only GPS functional.
                           // "COMMENT" this line to use the ADCS normally with all other functions enabled except GPS.

// #define SERIALMODE      // If this line is "UNCOMMENTED", the ADCS will function with Serial over USB and LoRa radio will be disabled.
                          // "COMMENT" this line to use the LoRa radio.

void setup()
{
  Serial.begin(9600);
  OBCSerial.begin(9600);
  
  pinMode(MOTOR_IN2, OUTPUT);
  pinMode(MOTOR_IN1, OUTPUT);
  sensorSetup();

  Serial.print("ADCS ");
  Serial.print(secretKey);
  Serial.println(" Initialized...");
}

void loop()
{
  #ifdef GPSTEST
  if(Serial.available())
  {
    content = Serial.readStringUntil('\n');
    content.trim();
    
    if(content.equals("GPS"))
    {
      readGPS();
    }
  }

  #else

    #ifdef SERIALMODE

      if(Serial.available())
      {
        content = Serial.readStringUntil('\n');
        content.trim();
        if(content.substring(0, secretKey.length()) == secretKey)
        {
          content.remove(0, secretKey.length());
        }

        if(content.equals("ALL"))
        {
          readIMU();
          delay(100);
          readTemp();
          delay(100);
          readSunSensors();
        }
        if(content.equals("IMU"))
        {
          readIMU();
        }
        if(content.equals("TEMPADCS"))
        {
          readTemp();
        }
        if(content.equals("SUN"))
        {
          readSunSensors();
        }
        if(content.equals("NIC"))
        {
          wheelForever();
        }
        if(content.equals("GYRO"))
        {
          wheelControl();
        }
      }

    #else

      if(OBCSerial.available())
      {
        content = OBCSerial.readStringUntil('\n');
        content.trim();
        if(content.substring(0, secretKey.length()) == secretKey)
        {
          content.remove(0, secretKey.length());
        }
        if(content.equals("ENADCS"))
        {
          ADCSStatus = 1;
        }

        if(ADCSStatus == 1)
        {
          if(content.equals("ALL"))
          {
            readIMU();
            delay(100);
            readTemp();
            delay(100);
            readSunSensors();
          }
          if(content.equals("IMU"))
          {
            readIMU();
          }
          if(content.equals("TEMPADCS"))
          {
            readTemp();
          }
          if(content.equals("SUN"))
          {
            readSunSensors();
          }
          if(content.equals("NIC"))
          {
            wheelForever();
          }
          if(content.equals("GYRO"))
          {
            wheelControl();
          }
        }
        else if(((content.equals("ALL")) || (content.equals("IMU") || (content.equals("TEMPADCS") || (content.equals("SUN") || (content.equals("GYRO")) && ADCSStatus == 0)))))
        {
          OBCSerial.print(secretKey);
          OBCSerial.println("ADCS is disabled. Enable it by sending command ENADCS");
        }
      }

    #endif

#endif

}


// FUNCTION WHEEL
void wheelForever(void)
{
  analogWrite(MOTOR_IN1,255);
  analogWrite(MOTOR_IN2,0);
  delay(500);
}

void sensorSetup(void)
{
  setupIMU();

  #ifdef GPSTEST
  setupGPS();
  #endif

}

void setupIMU(void)
{
  if(!bno.begin())
  {
    /* There was a problem detecting the BNO055 ... check your connections */
    OBCSerial.println("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
  }
  else
  {
    OBCSerial.println("BNO055 detected!");
  }
}

#ifdef GPSTEST
void setupGPS()
{
  // 9600 NMEA is the default baud rate for Adafruit MTK GPS's
  if(GPS.begin(9600))
  {
    Serial.println("GPS ok");
  }
  else
  {
    Serial.println("GPS not working");
  }
  // turn on RMC (recommended minimum) and GGA (fix data) including altitude
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  // set the update rate  
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);
  // Request updates on antenna status, comment out to keep quiet
  GPS.sendCommand(PGCMD_NOANTENNA);
}

void readGPS()
{
  // GPSSerial.begin(9600);
  GPSSerial.listen();
  while (!GPS.newNMEAreceived())
    c = GPS.read();

  GPS.parse(GPS.lastNMEA());
  // Serial.listen();
  Serial.println(GPS.lastNMEA());

  // GPSSerial.end();
}
#endif

void readSunSensors()
{
  ss1 = analogRead(SUN_SENSOR_1);
  ss2 = analogRead(SUN_SENSOR_2);
  ss3 = analogRead(SUN_SENSOR_3);
  ss4 = analogRead(SUN_SENSOR_4);

  #ifdef SERIALMODE
    Serial.print(secretKey);
    Serial.print("Sun sensor 1: "); Serial.println(ss1);
    delay(50);
    Serial.print(secretKey);
    Serial.print("Sun sensor 2: "); Serial.println(ss2);
    delay(50);
    Serial.print(secretKey);
    Serial.print("Sun sensor 3: "); Serial.println(ss3);
    delay(50);
    Serial.print(secretKey);
    Serial.print("Sun sensor 4: "); Serial.println(ss4);

  #else
    OBCSerial.print(secretKey);
    OBCSerial.print("Sun sensor 1: "); OBCSerial.println(ss1);
    delay(50);
    OBCSerial.print(secretKey);
    OBCSerial.print("Sun sensor 2: "); OBCSerial.println(ss2);
    delay(50);
    OBCSerial.print(secretKey);
    OBCSerial.print("Sun sensor 3: "); OBCSerial.println(ss3);
    delay(50);
    OBCSerial.print(secretKey);
    OBCSerial.print("Sun sensor 4: "); OBCSerial.println(ss4);
  #endif
}

void readIMU(void)
{

  /* Board layout:
         +----------+
         |         *| RST   PITCH  ROLL  HEADING
     ADR |*        *| SCL
     INT |*        *| SDA     ^            /->
     PS1 |*        *| GND     |            |
     PS0 |*        *| 3VO     Y    Z-->    \-X
         |         *| VIN
         +----------+
  */

  //could add VECTOR_ACCELEROMETER, VECTOR_MAGNETOMETER,VECTOR_GRAVITY...
  sensors_event_t angVelocityData, magnetometerData, accelerometerData, gravityData;
  bno.getEvent(&angVelocityData, Adafruit_BNO055::VECTOR_GYROSCOPE);
  bno.getEvent(&magnetometerData, Adafruit_BNO055::VECTOR_MAGNETOMETER);
  bno.getEvent(&accelerometerData, Adafruit_BNO055::VECTOR_ACCELEROMETER);
  bno.getEvent(&gravityData, Adafruit_BNO055::VECTOR_GRAVITY);

  #ifdef SERIALMODE
    Serial.print(secretKey);
    Serial.println("IMU Data:");
  #else
    OBCSerial.print(secretKey);
    OBCSerial.println("IMU Data:");
    delay(100);
  #endif

  printEvent(&angVelocityData);
  delay(100);
  printEvent(&magnetometerData);
  delay(100);
  printEvent(&accelerometerData);
  delay(100);
  printEvent(&gravityData);
  delay(100);

  int8_t boardTemp = bno.getTemp();
  #ifdef SERIALMODE
    Serial.print(secretKey);
    Serial.print(F("IMU Board Temperature: "));
    Serial.print(boardTemp);
    Serial.println("°C");
    Serial.print(secretKey);
    Serial.println("--");
  #else
    OBCSerial.print(secretKey);
    OBCSerial.print(F("IMU Board Temperature: "));
    OBCSerial.print(boardTemp);
    OBCSerial.println("°C");
    delay(100);
    OBCSerial.print(secretKey);
    OBCSerial.println("--");
  #endif
}

void readTemp(void)
{
  int reading = analogRead(ADCS_TEMP);
  float temp = (reading * (5100.00 / 1024.00)) / 10.00;
  #ifdef SERIALMODE
    Serial.print(secretKey);
    Serial.print("ADCS temperature is: ");
    Serial.print(temp);
    Serial.println("°C");
  #else
    OBCSerial.print(secretKey);
    OBCSerial.print("ADCS temperature is: ");
    OBCSerial.print(temp);
    OBCSerial.println("°C");
  #endif
}

void wheelControl(void)
{
  ss1 = analogRead(SUN_SENSOR_1);
  ss4 = analogRead(SUN_SENSOR_4);
  #ifdef SERIALMODE
    if(ss1 >= ss4)
    {
      Serial.print(secretKey);
      Serial.print("SS1: ");
      Serial.print(ss1);
      Serial.print(" || SS4: ");
      Serial.println(ss4);
      analogWrite(MOTOR_IN1,1023);
      analogWrite(MOTOR_IN2,0);
      delay(500);
      analogWrite(MOTOR_IN1,0);
      analogWrite(MOTOR_IN2,1023);
      delay(500);
      analogWrite(MOTOR_IN1,0);
      analogWrite(MOTOR_IN2,0);
      // for(int i = 255; i>= 0; i--)
      // {
      //   analogWrite(MOTOR_IN2,i);
      //   analogWrite(MOTOR_IN1,0);
        // delay(10);
      // }
      // analogWrite(MOTOR_IN2,255);
      // delay(2000);
      // analogWrite(MOTOR_IN2,0);
    }
    else if(ss1 < ss4)
    {
      Serial.print(secretKey);
      Serial.print("SS1: ");
      Serial.print(ss1);
      Serial.print(" || SS4: ");
      Serial.println(ss4);
      analogWrite(MOTOR_IN2,1023);
      analogWrite(MOTOR_IN1,0);
      delay(500);
      analogWrite(MOTOR_IN2,0);
      analogWrite(MOTOR_IN1,1023);
      delay(500);
      analogWrite(MOTOR_IN2,0);
      analogWrite(MOTOR_IN1,0);
    }
  #else
    if(ss1 >= ss4)
    {
      OBCSerial.print(secretKey);
      OBCSerial.print("SS1: ");
      OBCSerial.print(ss1);
      OBCSerial.print(" || SS4: ");
      OBCSerial.println(ss4);
      analogWrite(MOTOR_IN1,1023);
      analogWrite(MOTOR_IN2,0);
      delay(500);
      analogWrite(MOTOR_IN1,0);
      analogWrite(MOTOR_IN2,1023);
      delay(500);
      analogWrite(MOTOR_IN1,0);
      analogWrite(MOTOR_IN2,0);
      // for(int i = 255; i>= 0; i--)
      // {
      //   analogWrite(MOTOR_IN2,i);
      //   analogWrite(MOTOR_IN1,0);
        // delay(10);
      // }
      // analogWrite(MOTOR_IN2,255);
      // delay(2000);
      // analogWrite(MOTOR_IN2,0);
    }
    else if(ss1 < ss4)
    {
      OBCSerial.print(secretKey);
      OBCSerial.print("SS1: ");
      OBCSerial.print(ss1);
      OBCSerial.print(" || SS4: ");
      OBCSerial.println(ss4);
      analogWrite(MOTOR_IN2,1023);
      analogWrite(MOTOR_IN1,0);
      delay(500);
      analogWrite(MOTOR_IN2,0);
      analogWrite(MOTOR_IN1,1023);
      delay(500);
      analogWrite(MOTOR_IN2,0);
      analogWrite(MOTOR_IN1,0);
    }
  #endif
}

void printEvent(sensors_event_t* event) {
  double x = -1000000, y = -1000000 , z = -1000000; //dumb values, easy to spot problem
  #ifdef SERIALMODE
    Serial.print(secretKey);
  #else
    OBCSerial.print(secretKey);
  #endif
  if (event->type == SENSOR_TYPE_ACCELEROMETER) {
    #ifdef SERIALMODE
      Serial.print("Accl:");
      x = event->acceleration.x;
      y = event->acceleration.y;
      z = event->acceleration.z;
    #else
      OBCSerial.print("Accl:");
      x = event->acceleration.x;
      y = event->acceleration.y;
      z = event->acceleration.z;
    #endif
  }
  else if (event->type == SENSOR_TYPE_ORIENTATION) {
    #ifdef SERIALMODE
      Serial.print("Orient:");
      x = event->orientation.x;
      y = event->orientation.y;
      z = event->orientation.z;
    #else
      OBCSerial.print("Orient:");
      x = event->orientation.x;
      y = event->orientation.y;
      z = event->orientation.z;
    #endif
  }
  else if (event->type == SENSOR_TYPE_MAGNETIC_FIELD) {
    #ifdef SERIALMODE
      Serial.print("Mag:");
      x = event->magnetic.x;
      y = event->magnetic.y;
      z = event->magnetic.z;
    #else
      OBCSerial.print("Mag:");
      x = event->magnetic.x;
      y = event->magnetic.y;
      z = event->magnetic.z;
    #endif
  }
  else if (event->type == SENSOR_TYPE_GYROSCOPE) {
    #ifdef SERIALMODE
      Serial.print("Gyro:");
      x = event->gyro.x;
      y = event->gyro.y;
      z = event->gyro.z;
    #else
      OBCSerial.print("Gyro:");
      x = event->gyro.x;
      y = event->gyro.y;
      z = event->gyro.z;
    #endif
  }
  else if (event->type == SENSOR_TYPE_ROTATION_VECTOR) {
    #ifdef SERIALMODE
      Serial.print("Rot:");
      x = event->gyro.x;
      y = event->gyro.y;
      z = event->gyro.z;
    #else
      OBCSerial.print("Rot:");
      x = event->gyro.x;
      y = event->gyro.y;
      z = event->gyro.z;
    #endif
  }
  else if (event->type == SENSOR_TYPE_LINEAR_ACCELERATION) {
    #ifdef SERIALMODE
      Serial.print("Linear:");
      x = event->acceleration.x;
      y = event->acceleration.y;
      z = event->acceleration.z;
    #else
      OBCSerial.print("Linear:");
      x = event->acceleration.x;
      y = event->acceleration.y;
      z = event->acceleration.z;
    #endif
  }
  else if (event->type == SENSOR_TYPE_GRAVITY) {
    #ifdef SERIALMODE
      Serial.print("Gravity:");
      x = event->acceleration.x;
      y = event->acceleration.y;
      z = event->acceleration.z;
    #else
      OBCSerial.print("Gravity:");
      x = event->acceleration.x;
      y = event->acceleration.y;
      z = event->acceleration.z;
    #endif
  }
  else {
    #ifdef SERIALMODE
      Serial.print("Unk:");
    #else
      OBCSerial.print("Unk:");
    #endif
  }

  #ifdef SERIALMODE
    Serial.print("\tx= ");
    Serial.print(x);
    Serial.print(" |\ty= ");
    Serial.print(y);
    Serial.print(" |\tz= ");
    Serial.println(z);
  #else
    OBCSerial.print("\tx= ");
    OBCSerial.print(x);
    OBCSerial.print(" |\ty= ");
    OBCSerial.print(y);
    OBCSerial.print(" |\tz= ");
    OBCSerial.println(z);
  #endif
}