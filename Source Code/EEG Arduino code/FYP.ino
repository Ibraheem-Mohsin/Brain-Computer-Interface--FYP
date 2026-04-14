/* Obstacle Avoiding, Bluetooth Control, Voice Control Robot Car */

#include <Servo.h>
#include <AFMotor.h>

#define Echo A0
#define Trig A1
#define motor 10
#define Speed 170
#define spoint 103

char value = 'S';  // Default state is stop
int distance;
Servo servo;
AF_DCMotor M1(1);
AF_DCMotor M2(2);
AF_DCMotor M3(3);
AF_DCMotor M4(4);

void setup() {
  Serial.begin(9600);
  pinMode(Trig, OUTPUT);
  pinMode(Echo, INPUT);
  servo.attach(motor);
  servo.write(spoint);  // Set servo to default straight position
  M1.setSpeed(Speed);
  M2.setSpeed(Speed);
  M3.setSpeed(Speed);
  M4.setSpeed(Speed);
}

void loop() {
  if (Serial.available() > 0) {
    value = Serial.read();
    Serial.println(value);
  }

  if (value == 'G') {
    moveContinuouslyForward();
  } else if (value == '-') {
    backward();
  } else if (value == '<') {
    checkAndTurnLeft();
  } else if (value == '>') {
    checkAndTurnRight();
  } else if (value == 'S') {
    Stop();
  } else {
    Bluetoothcontrol();
  }
}

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

void moveContinuouslyForward() {
  while (true) {
    if (Serial.available() > 0) {
      char newCommand = Serial.read();
      Serial.println(newCommand);
      if (newCommand == 'S') {
        Stop();
        value = 'S';  // Update global state to stop
        return;
      }
    }

    distance = ultrasonic();
    Serial.print("Distance: ");
    Serial.println(distance);

    if (distance < 30) {
      Stop();
      Serial.println("Obstacle detected! Stopping.");
      value = 'S';  // Update global state to stop
      return;
    }

    M1.run(FORWARD);
    M2.run(FORWARD);
    M3.run(FORWARD);
    M4.run(FORWARD);

    delay(100);
  }
}

void backward() {
  M1.run(BACKWARD);
  M2.run(BACKWARD);
  M3.run(BACKWARD);
  M4.run(BACKWARD);
  delay(500);
  Stop();
}

void checkAndTurnLeft() {
  servo.write(spoint + 45);  // Turn servo to the left
  delay(500);
  int leftDistance = ultrasonic();
  Serial.print("Left Distance: ");
  Serial.println(leftDistance);
  
  if (leftDistance > 10) {  // Only turn if no obstacle within 10 cm
    left();
    delay(500);  // Turn for 500ms
    Stop();
  } else {
    Serial.println("Obstacle detected! Cannot turn left.");
  }
  
  servo.write(spoint);  // Reset servo to center
}

void checkAndTurnRight() {
  servo.write(spoint - 45);  // Turn servo to the right
  delay(500);
  int rightDistance = ultrasonic();
  Serial.print("Right Distance: ");
  Serial.println(rightDistance);
  
  if (rightDistance > 10) {  // Only turn if no obstacle within 10 cm
    right();
    delay(500);  // Turn for 500ms
    Stop();
  } else {
    Serial.println("Obstacle detected! Cannot turn right.");
  }
  
  servo.write(spoint);  // Reset servo to center
}

void right() {
  M1.run(BACKWARD);  // Reverse left-side motors
  M2.run(BACKWARD);
  M3.run(FORWARD);   // Forward right-side motors
  M4.run(FORWARD);
  delay(500);  // Turn for 500ms
  Stop();
}

void left() {
  M1.run(FORWARD);   // Forward left-side motors
  M2.run(FORWARD);
  M3.run(BACKWARD);  // Reverse right-side motors
  M4.run(BACKWARD);
  delay(500);  // Turn for 500ms
  Stop();
}

void Stop() {
  M1.run(RELEASE);
  M2.run(RELEASE);
  M3.run(RELEASE);
  M4.run(RELEASE);
}
