//Create a function that compares the three sun sensors so that the serial monitor output looks like this:
//THE CAMERA IS OBSERVING EARTH       or
//THE SATELLITE POSITION NEEDS TO BE ADJUSTED


#define SUN_SENSOR_1 A0
#define SUN_SENSOR_2 A1
#define SUN_SENSOR_3 A2
#define SUN_SENSOR_4 A3

// Sun sensor variables
uint8_t ss1 = 0;
uint8_t ss2 = 0;
uint8_t ss3 = 0;
uint8_t ss4 = 0;

void setup()
{
  Serial.begin(9600);
}

void loop()
{
  readSunSensors();
  checkCameraOrientation();
}
  
    
void readSunSensors()
{
  ss1 = analogRead(SUN_SENSOR_1);
  ss2 = analogRead(SUN_SENSOR_2);
  ss3 = analogRead(SUN_SENSOR_3);
  ss4 = analogRead(SUN_SENSOR_4);
  //Serial.print("Sun sensor 1: "); Serial.println(ss1);
  //delay(500);
  //Serial.print("Sun sensor 2: "); Serial.println(ss2);
  //delay(500); 
  //Serial.print("Sun sensor 3: "); Serial.println(ss3);
  //delay(500);
  //Serial.print("Sun sensor 4: "); Serial.println(ss4);
}

void checkCameraOrientation()
{
  if (ss4 > ss3 && ss4 > ss2 && ss4 > ss1)
  {
    Serial.println("THE CAMERA IS OBSERVING EARTH");
  }
  else
  {
    Serial.println("THE SATELLITE POSITION NEEDS TO BE ADJUSTED");
  }
  delay(200);
}
