#include "pins.h"

#define ROTARY_SERIAL Serial
#include "rotary.h"

#include <Wire.h>
#include <RTClib.h>

const int stepPerFullRev = 3200;

const int secsBetweenSamples = 3;

int sampleCounter = 1;

const int fillTubeSteps = 200;
const int flushSteps = 200;

// hardware interrupt flags
volatile bool waterFlowing = false;
volatile bool alarm = false;

int currentPosition;

DateTime now;
DateTime future;

void takeSample() {
  // Rotates to correct position from home
  //  Replaces stepWheel(currentPosition, 1); because we will have purged at home before
  for(int i = 0; i < sampleCounter; i++) {
      stepWheel(stepPerFullRev/32, 1);
      delay(500);
  }
  
  lockTube();
  insertNeedle();
  fillTube();
  release();
}

void lockTube() {
  // Secure tube with horizontal LA
  digitalWrite(wheelEnPin, HIGH);
  // extends 
  digitalWrite(lockingActuatorpin1, HIGH);
  digitalWrite(lockingActuatorpin2, LOW);
  delay(3000);
}

void insertNeedle() {
  // with vertical LA
  digitalWrite(needleActuatorpin1, LOW);
  digitalWrite(needleActuatorpin2, HIGH);

  delay(12000);

}

void fillTube() {
  //digitalWrite(pumpEnPin, HIGH);
  Serial.println("begin pump");
  while (!waterFlowing) {
    digitalWrite(pumpEnPin, HIGH);
  }
  Serial.println("end pump");
  digitalWrite(pumpEnPin, LOW);
  waterFlowing = false;
}

void release() {
  digitalWrite(wheelEnPin, LOW);
  
  Serial.println("release 1");
  digitalWrite(needleActuatorpin1, HIGH);
  digitalWrite(needleActuatorpin2, LOW);

  delay(10000);

  Serial.println("release 2");
  digitalWrite(lockingActuatorpin1, LOW);
  digitalWrite(lockingActuatorpin2, HIGH);

  delay(3000);
  waterFlowing = false;
}

void stepWheel(int n, int direction) {
  
  if (direction == 1) {
    digitalWrite(wheelDirPin, HIGH);
  } else {
    digitalWrite(wheelDirPin, LOW);
  }
  
  for (int x = 0; x < n; x++) {
    digitalWrite(wheelStepPin, HIGH);
    delayMicroseconds(500);
    digitalWrite(wheelStepPin, LOW);
    delayMicroseconds(500);
  }
  digitalWrite(wheelDirPin, LOW);
}

void setup() {
  Serial.begin(9600);

  //Set up water sensor
  digitalWrite(sensorEnablePin, HIGH);

  // Set up linearActuators
  pinMode(needleActuatorpin1, OUTPUT);
  pinMode(needleActuatorpin2, OUTPUT);
  pinMode(lockingActuatorpin1,  OUTPUT);
  pinMode(lockingActuatorpin2, OUTPUT);

  // Set up wheel stepper motor
  pinMode(wheelStepPin, OUTPUT);
  pinMode(wheelDirPin, OUTPUT);
  pinMode(wheelEnPin, OUTPUT);
  digitalWrite(wheelEnPin, LOW);

  // Set up pump
  pinMode(pumpEnPin, OUTPUT);
  digitalWrite(pumpEnPin, LOW);

  release();

  Serial.println("move stepper");
  stepWheel(stepPerFullRev/32, 1);
  currentPosition = stepPerFullRev/32;
  lockTube();

  attachInterrupt(digitalPinToInterrupt(sensorPin),stopPump,RISING);
  // pinMode(sensorPin, INPUT);

}

void loop() {
  takeSample();
  delay(secsBetweenSamples*1000);
}

void stopPump() {
  waterFlowing = true;
}
