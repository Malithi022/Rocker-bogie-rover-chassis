#include <Servo.h>

// ==========================================
// PIN DEFINITIONS
// ==========================================
// Left side drive motors
#define motorW1_IN1 10  // Front Left
#define motorW1_IN2 11
#define motorW2_IN1 7   // Middle Left
#define motorW2_IN2 6
#define motorW3_IN1 9   // Rear Left
#define motorW3_IN2 8

// Right side drive motors
#define motorW4_IN1 13  // Front Right
#define motorW4_IN2 12
#define motorW5_IN1 4   // Middle Right
#define motorW5_IN2 5
#define motorW6_IN1 2   // Rear Right
#define motorW6_IN2 3

// Steering Servos (Arduino Mega Digital PWM Pins)
#define frontLeftServoPin  28
#define frontRightServoPin 29
#define rearLeftServoPin   30
#define rearRightServoPin  31

// ==========================================
// ACKERMANN STEERING CONSTANTS (from Report)
// Calculations for r_path = 1.0 m:
// alpha_inner = 17 deg, alpha_outer = 13 deg
// Center neutral position = 90 deg
// ==========================================
const int SERVO_CENTER = 90;
const int ANGLE_INNER  = 17;
const int ANGLE_OUTER  = 13;

Servo frontLeftServo;
Servo frontRightServo;
Servo rearLeftServo;
Servo rearRightServo;

void setup() {
  // USB Serial for diagnostic monitor
  Serial.begin(9600);
  
  // Hardware Serial3 for Bluetooth / Wireless transceiver (Pins 18 TX, 19 RX)
  Serial3.begin(9600);
  Serial.println(F("6WD Rover Initialized. Ready for commands over USB or Bluetooth Serial3."));

  // Set motor pins as outputs
  const uint8_t motorPins[] = {
    motorW1_IN1, motorW1_IN2, motorW2_IN1, motorW2_IN2, motorW3_IN1, motorW3_IN2,
    motorW4_IN1, motorW4_IN2, motorW5_IN1, motorW5_IN2, motorW6_IN1, motorW6_IN2
  };
  for (uint8_t pin : motorPins) {
    pinMode(pin, OUTPUT);
  }

  // Attach and calibrate servos to neutral center
  frontLeftServo.attach(frontLeftServoPin);
  frontRightServo.attach(frontRightServoPin);
  rearLeftServo.attach(rearLeftServoPin);
  rearRightServo.attach(rearRightServoPin);
  
  centerServos();
  allStop();
}

void loop() {
  char cmd = 0;

  // Read command from Bluetooth (Priority) or USB Serial Monitor
  if (Serial3.available() > 0) {
    cmd = Serial3.read();
  } else if (Serial.available() > 0) {
    cmd = Serial.read();
  }

  if (cmd != 0) {
    processCommand(cmd);
  }
}

// Command dispatcher
void processCommand(char cmd) {
  switch (cmd) {
    case 'F':
      centerServos();
      allForward();
      Serial.println(F("STATUS: Drive Forward | Servos Centered"));
      break;

    case 'B':
      centerServos();
      allBackward();
      Serial.println(F("STATUS: Drive Backward | Servos Centered"));
      break;

    case 'L':
      steerLeftAckermann();
      allForward();
      Serial.println(F("STATUS: Turn Left (Ackermann Geometry Engaged)"));
      break;

    case 'R':
      steerRightAckermann();
      allForward();
      Serial.println(F("STATUS: Turn Right (Ackermann Geometry Engaged)"));
      break;

    case 'S':
      allStop();
      centerServos();
      Serial.println(F("STATUS: Motors Stopped | Servos Centered"));
      break;

    default:
      Serial.print(F("WARNING: Unrecognized Command: "));
      Serial.println(cmd);
      break;
  }
}

// ==========================================
// MOTOR CONTROL PRIMITIVES
// ==========================================
void allForward() {
  // Left side forward
  digitalWrite(motorW1_IN1, HIGH); digitalWrite(motorW1_IN2, LOW);
  digitalWrite(motorW2_IN1, HIGH); digitalWrite(motorW2_IN2, LOW);
  digitalWrite(motorW3_IN1, HIGH); digitalWrite(motorW3_IN2, LOW);
  // Right side forward
  digitalWrite(motorW4_IN1, HIGH); digitalWrite(motorW4_IN2, LOW);
  digitalWrite(motorW5_IN1, HIGH); digitalWrite(motorW5_IN2, LOW);
  digitalWrite(motorW6_IN1, HIGH); digitalWrite(motorW6_IN2, LOW);
}

void allBackward() {
  // Left side backward
  digitalWrite(motorW1_IN1, LOW); digitalWrite(motorW1_IN2, HIGH);
  digitalWrite(motorW2_IN1, LOW); digitalWrite(motorW2_IN2, HIGH);
  digitalWrite(motorW3_IN1, LOW); digitalWrite(motorW3_IN2, HIGH);
  // Right side backward
  digitalWrite(motorW4_IN1, LOW); digitalWrite(motorW4_IN2, HIGH);
  digitalWrite(motorW5_IN1, LOW); digitalWrite(motorW5_IN2, HIGH);
  digitalWrite(motorW6_IN1, LOW); digitalWrite(motorW6_IN2, HIGH);
}

void allStop() {
  digitalWrite(motorW1_IN1, LOW); digitalWrite(motorW1_IN2, LOW);
  digitalWrite(motorW2_IN1, LOW); digitalWrite(motorW2_IN2, LOW);
  digitalWrite(motorW3_IN1, LOW); digitalWrite(motorW3_IN2, LOW);
  digitalWrite(motorW4_IN1, LOW); digitalWrite(motorW4_IN2, LOW);
  digitalWrite(motorW5_IN1, LOW); digitalWrite(motorW5_IN2, LOW);
  digitalWrite(motorW6_IN1, LOW); digitalWrite(motorW6_IN2, LOW);
}

// ==========================================
// STEERING KINEMATICS
// ==========================================
void centerServos() {
  frontLeftServo.write(SERVO_CENTER);
  frontRightServo.write(SERVO_CENTER);
  rearLeftServo.write(SERVO_CENTER);
  rearRightServo.write(SERVO_CENTER);
}

// Left turn: Left wheels are inner, Right wheels are outer.
// Rear wheels steer inversely to front wheels to generate turning moment.
void steerLeftAckermann() {
  frontLeftServo.write(SERVO_CENTER - ANGLE_INNER);   // 73 deg
  frontRightServo.write(SERVO_CENTER - ANGLE_OUTER);  // 77 deg
  rearLeftServo.write(SERVO_CENTER + ANGLE_INNER);    // 107 deg
  rearRightServo.write(SERVO_CENTER + ANGLE_OUTER);   // 103 deg
}

// Right turn: Right wheels are inner, Left wheels are outer.
void steerRightAckermann() {
  frontLeftServo.write(SERVO_CENTER + ANGLE_OUTER);   // 103 deg
  frontRightServo.write(SERVO_CENTER + ANGLE_INNER);  // 107 deg
  rearLeftServo.write(SERVO_CENTER - ANGLE_OUTER);    // 77 deg
  rearRightServo.write(SERVO_CENTER - ANGLE_INNER);   // 73 deg
}
