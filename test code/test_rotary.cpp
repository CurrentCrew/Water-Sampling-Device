/**
 * Turns rotary to each vial 
**/

#include <AccelStepper.h>

const int wheelStepPin = 42;
const int wheelDirPin = 41;
const int wheelEnPin = 40;
const int stepPerFullRev = 3200;

int step_position = 0;


AccelStepper stepper(AccelStepper::DRIVER, wheelStepPin, wheelDirPin);
void lockWheel(const int ena_pin) {
    digitalWrite(ena_pin, HIGH);
}
void unlockWheel(const int ena_pin) {
    digitalWrite(ena_pin, LOW);
}
void dirCCW(const int dir_pin) {
    digitalWrite(dir_pin, HIGH);
}
void dirCW(const int dir_pin) {
    digitalWrite(dir_pin, LOW);
}

// 250 steps/sec
void step(const int stp_pin) {
    digitalWrite(stp_pin, HIGH);
    delayMicroseconds(2000);
    digitalWrite(stp_pin, LOW);
    delayMicroseconds(2000);
}

// handles conversion between sample number and step count
void stepWheel(int n, int direction) {
    unlockWheel(wheelEnPin);
    delayMicroseconds(100);

    if (direction == 1) {
    dirCCW(wheelDirPin);
    } else {
    dirCW(wheelDirPin);
    }

    for (int x = 0; x < n; x++) {
    step(wheelStepPin);
    }

    dirCW(wheelDirPin);
}

void setup() {
    Serial.begin(9600);
    pinMode(wheelStepPin, OUTPUT);
    pinMode(wheelDirPin, OUTPUT);
    pinMode(wheelEnPin, OUTPUT);
    digitalWrite(wheelEnPin, LOW);
    delay(10000);
    // tests one full revolution
    for(int i=0; i<32; i++) {
      stepWheel(stepPerFullRev/32, 1);
      Serial.print("move stepper:");
      Serial.println(i);
      delay(1000);
    }
    
}

void loop() {
  // delay(1000);
  // stepWheel(stepPerFullRev/32, 1);
  // Serial.println("move stepper");
}