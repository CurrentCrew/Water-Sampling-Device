/**
 * New rotary test using ~50:1 gear ratio
**/

#include <AccelStepper.h>

const int wheelStepPin = 42;
const int wheelDirPin = 41;
const int wheelEnPin = 40;
const int stepPerFullRev = 400;

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
    delayMicroseconds(1000);
    digitalWrite(stp_pin, LOW);
    delayMicroseconds(1000);
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
    stepper.setMaxSpeed(800);      // Max speed in steps per second
    stepper.setAcceleration(100);
    delay(1000);
    // tests one full revolution
    // gear ratio: 50 4397/4913 * 200 steps per revolution 10179, 20358
    for(int i=0; i<32; i++) {
      stepWheel(20358, 1);
      delay(500);
    }
    
}

void loop() {

    // if (stepper.distanceToGo() == 0) {
    //   stepper.moveTo(stepper.currentPosition()+636);
    // }
    // stepper.run();
  // delay(1000);
  // stepWheel(stepPerFullRev/32, 1);
  // Serial.println("move stepper");
}