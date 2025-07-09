#ifndef ACTIONS_H
#define ACTIONS_H

#include "settings.h"
#include "devices.h"

void lockTube() {
  rotary.off(); // so that LA can move tube into correct position

  horizontalActuator.extend();
  delay(3000);

  rotary.on();
}

void unlockTube() {
  horizontalActuator.retract();
  delay(3000);
}

void insertNeedle() {
  verticalActuator.retract();
  delay(12000); // needle needs time to puncture tube, so have shortest length of LA be filling height 
}

bool isWaterFlowing()
{
  return digitalRead(sensorPin);
}

void fillTube() {
  while (!isWaterFlowing()) {
    pump.start();
  }
  pump.stop();
}

void release() {
  rotary.on();
  
  Serial.println("release 1");
  verticalActuator.extend();
  delay(10000);
  Serial.println("release 2");
  horizontalActuator.retract();
  delay(3000);
}

// handles conversion between sample number and step count
void stepWheel(int n, int direction) {
  rotary.on();
  delayMicroseconds(100);

  if (direction == 1) {
    rotary.dirCCW();
  } else {
    rotary.dirCW();
  }
  
  for (int x = 0; x < n; x++) {
    rotary.step();
  }

  rotary.dirCW();
  rotary.off();
}

//Runs pump for set amount of time
void purge() {
  //digitalWrite(pumpEnPin, HIGH);
  Serial.println("begin pump");
  pump.start();
  delay(purgeTime);
  Serial.println("end pump");
  pump.stop();
}

void goToLimitSwitch()
{
  rotary.dirCW();
  rotary.on();
  while (!microSwitch.isSwitchPressed()) {
    rotary.step();
  }
}

void takeSample() {
  // Rotates to correct position from home
  for(int i = 0; i < sampleCounter; i++) {
      stepWheel(int(20358/numTubes), 1);
      delay(500);
      // Error prevention: Lock at each sample to ensure accuracy of position 
      //  because microstepper has inaccuracies with our high load weight
      lockTube();
      unlockTube();
  }
  
  lockTube();
  insertNeedle();
  fillTube();
  delay(5000);
  release();
}

void onAlarm() {
  alarm_setoff = true;
  // Clear the alarm flag to allow the next day’s event
  Serial.println("Daily alarm triggered!");
}

void attachInterrupts()
{
  // Interrupts
  attachInterrupt(digitalPinToInterrupt(clockPin), onAlarm, FALLING);
}

#endif