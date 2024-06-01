//THIS PROGRAM contains a function that:
//If the left sensor has more light, the motor starts rotating counterclockwise for 1 second. 
//If the back sensor has more light, the motor is stopped.
//If the right sensor has more light, the motor starts rotating clockwise for 1 second.
//Include a Serial.print of the motor status (rotating ccw, rotating cw or stopped).

#define MOTOR_IN1 9
#define MOTOR_IN2 10
#define LEFT_SENSOR A0
#define BACK_SENSOR A3
#define RIGHT_SENSOR A1

void setup() {
  pinMode(MOTOR_IN1, OUTPUT);
  pinMode(MOTOR_IN2, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  int leftLight = analogRead(LEFT_SENSOR);
  int backLight = analogRead(BACK_SENSOR);
  int rightLight = analogRead(RIGHT_SENSOR);

  if (leftLight > backLight && leftLight > rightLight) {
    rotateCounterClockwise();
    Serial.println("Motor rotating counterclockwise");
  } else if (backLight > leftLight && backLight > rightLight) {
    stopMotor();
    Serial.println("Motor stopped");
  } else if (rightLight > leftLight && rightLight > backLight) {
    rotateClockwise();
    Serial.println("Motor rotating clockwise");
  }

  delay(5000); // 5 seconds sample time
}

void rotateCounterClockwise() {
  analogWrite(MOTOR_IN1, 255);
  analogWrite(MOTOR_IN2, 0);
  delay(1000);
  stopMotor();
}

void rotateClockwise() {
  analogWrite(MOTOR_IN1, 0);
  analogWrite(MOTOR_IN2, 255);
  delay(1000);
  stopMotor();
}

void stopMotor() {
  analogWrite(MOTOR_IN1, 0);
  analogWrite(MOTOR_IN2, 0);
}
