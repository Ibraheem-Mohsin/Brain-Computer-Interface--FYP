#include <Servo.h>

// Pin Definitions
const int Echo = A9;     // Ultrasonic Echo
const int Trig = A8;     // Ultrasonic Trigger
const int servoPin = 3;  // Servo motor pin

// Motor Driver IBT-2 Pins
const int R_RPWM = 5;    // Right Wheel RPWM
const int R_LPWM = 6;    // Right Wheel LPWM
const int L_RPWM = 7;    // Left Wheel RPWM
const int L_LPWM = 8;    // Left Wheel LPWM

// Constants
const int Speed = 80;   // Speed (0-255 PWM)
const int spoint = 103;  // Servo center position

// Variables
char value = 'S';        // Default state
int distance;
Servo servo;

void setup() {
  Serial.begin(9600);         // For USB debug
  Serial1.begin(9600);         // For Bluetooth communication

  // Motor control pins
  pinMode(R_RPWM, OUTPUT);
  pinMode(R_LPWM, OUTPUT);
  pinMode(L_RPWM, OUTPUT);
  pinMode(L_LPWM, OUTPUT);

  // Ultrasonic sensor pins
  pinMode(Trig, OUTPUT);
  pinMode(Echo, INPUT);

  // Servo initialization
  servo.attach(servoPin);
  servo.write(spoint);

  // Initial stop
  Stop();
}

void loop() {
  if (Serial1.available() > 0) {
    value = Serial1.read();
    Serial.println(value); // Debug on USB Serial
  }

  if (value == 'G' || value == '1') {
    moveContinuouslyForward();
  } else if (value == '-' || value == '2') {
    backward();
  } else if (value == '<' || value == '3') {
    checkAndTurnLeft();
  } else if (value == '>' || value == '4') {
    checkAndTurnRight();
  } else if (value == 'S' || value == '0') {
    Stop();
  } else {
    Bluetoothcontrol();
  }
}

// ---------------- Motor Control ----------------

void moveForward() {
  analogWrite(R_RPWM, Speed);
  analogWrite(R_LPWM, 0);
  analogWrite(L_RPWM, Speed);
  analogWrite(L_LPWM, 0);
}

void moveBackward() {
  analogWrite(R_RPWM, 0);
  analogWrite(R_LPWM, Speed);
  analogWrite(L_RPWM, 0);
  analogWrite(L_LPWM, Speed);
}

void turnLeft() {
  analogWrite(R_RPWM, Speed);
  analogWrite(R_LPWM, 0);
  analogWrite(L_RPWM, 0);
  analogWrite(L_LPWM, Speed);
}

void turnRight() {
  analogWrite(R_RPWM, 0);
  analogWrite(R_LPWM, Speed);
  analogWrite(L_RPWM, Speed);
  analogWrite(L_LPWM, 0);
}

void Stop() {
  analogWrite(R_RPWM, 0);
  analogWrite(R_LPWM, 0);
  analogWrite(L_RPWM, 0);
  analogWrite(L_LPWM, 0);
}

// ---------------- Bluetooth Control ----------------

void Bluetoothcontrol() {
  if (value == 'F') {
    moveContinuouslyForward();
  } else if (value == 'B') {
    backward();
  } else if (value == 'L') {
    checkAndTurnLeft();
  } else if (value == 'R') {
    checkAndTurnRight();
  } else if (value == 'S') {
    Stop();
  }
}

// ---------------- Movement Functions ----------------

void moveContinuouslyForward() {
  while (true) {
    if (Serial1.available() > 0) {
      char newCommand = Serial1.read();
      Serial.println(newCommand);
      if (newCommand == 'S' || newCommand == '0') {
        Stop();
        value = 'S';
        return;
      }
    }

    distance = ultrasonic();
    if (distance < 30) {
      Stop();
      value = 'S';
      return;
    }

    moveForward();
    delay(100);
  }
}

void backward() {
  moveBackward();
  delay(500);
  Stop();
}

void checkAndTurnLeft() {
  servo.write(spoint + 45);
  delay(500);
  int leftDistance = ultrasonic();
  if (leftDistance > 10) {
    turnLeft();
    delay(500);
    Stop();
  }
  servo.write(spoint);
}

void checkAndTurnRight() {
  servo.write(spoint - 45);
  delay(500);
  int rightDistance = ultrasonic();
  if (rightDistance > 10) {
    turnRight();
    delay(500);
    Stop();
  }
  servo.write(spoint);
}

int ultrasonic() {
  digitalWrite(Trig, LOW);
  delayMicroseconds(4);
  digitalWrite(Trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(Trig, LOW);
  long t = pulseIn(Echo, HIGH);
  int cm = t / 29 / 2;
  return cm;
}
