#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

#define MOTOR_IN1 9
#define MOTOR_IN2 10 // Pin number for the LED
const int shakeThreshold = 10;  // Adjust the threshold as needed

#define BNO055_SAMPLERATE_DELAY_MS (100)
Adafruit_BNO055 bno = Adafruit_BNO055(-1, 0x28, &Wire);


void setup(void)
{
  pinMode(MOTOR_IN1, OUTPUT);
  pinMode(MOTOR_IN2, OUTPUT);
  Serial.begin(115200);

  while (!Serial)
    delay(10); // wait for serial port to open!

  Serial.println("Orientation Sensor Test");
  Serial.println("");

  /* Initialise the sensor */
  if (!bno.begin())
  {
    /* There was a problem detecting the BNO055 ... check your connections */
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while (1);
  }

    analogWrite(MOTOR_IN1, 0); //M1 ON Clockwise, M2 Counterclockwise
    analogWrite(MOTOR_IN2, 0); // Initially turn off the LED

  delay(1000);
}

void loop(void)
{
  // could add VECTOR_ACCELEROMETER, VECTOR_MAGNETOMETER,VECTOR_GRAVITY...
  sensors_event_t accelerometerData;
  bno.getEvent(&accelerometerData, Adafruit_BNO055::VECTOR_ACCELEROMETER);

  printEvent(&accelerometerData);

  // Check if the accelerometer is shaken
  if (abs(accelerometerData.acceleration.x) > shakeThreshold ||
      abs(accelerometerData.acceleration.y) > shakeThreshold ||
      abs(accelerometerData.acceleration.z) > shakeThreshold)
  {
    analogWrite(MOTOR_IN1, 127); //M1 ON Clockwise, M2 Counterclockwise
    analogWrite(MOTOR_IN2, 0);
    delay(500);  // Turn on the LED
  }
  else
  {
    analogWrite(MOTOR_IN1, 0); //M1 ON Clockwise, M2 Counterclockwise
    analogWrite(MOTOR_IN2, 0);
  }

  Serial.println("--");
  delay(BNO055_SAMPLERATE_DELAY_MS);
}

void printEvent(sensors_event_t* event) {
  double x = -1000000, y = -1000000 , z = -1000000; //dumb values, easy to spot problem
  if (event->type == SENSOR_TYPE_ACCELEROMETER) {
    Serial.print("Accl:");
    x = event->acceleration.x;
    y = event->acceleration.y;
    z = event->acceleration.z;
  }
}