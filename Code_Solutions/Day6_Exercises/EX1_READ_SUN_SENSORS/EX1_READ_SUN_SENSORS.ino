//PROGRAM TO READ SUN SENSORS WITH A FUNCTION
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
}
  
    
void readSunSensors()
{
  ss1 = analogRead(SUN_SENSOR_1);
  ss2 = analogRead(SUN_SENSOR_2);
  ss3 = analogRead(SUN_SENSOR_3);
  ss4 = analogRead(SUN_SENSOR_4);
  Serial.print("Sun sensor 1: "); Serial.println(ss1);
  delay(500);
  Serial.print("Sun sensor 2: "); Serial.println(ss2);
  delay(500); 
  Serial.print("Sun sensor 3: "); Serial.println(ss3);
  delay(500);
  Serial.print("Sun sensor 4: "); Serial.println(ss4);
}
