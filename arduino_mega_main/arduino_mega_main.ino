#include "pins.h"

#define ROTARY_SERIAL Serial
#include "rotary.h"

#define ALARM_SERIAL Serial
#include "alarm.h"

#define ACTUATOR_SERIAL Serial
#include "actuator.h"

#define SENSOR_SERIAL Serial
#include "sensor.h"

#define PUMP_SERIAL Serial
#include "pump.h"

#define SWITCH_SERIAL Serial
#include "switch.h"

#include <Wire.h>
#include <RTClib.h>

const int stepPerFullRev = 3200;

const int secsBetweenSamples = 3;
const int numTubes = 32;

const int fillTubeSteps = 200;
const int flushSteps = 200;

const int purgeTime = 5000;

int sampleCounter = 1;

int currentPosition;

// hardware interrupt flags
volatile bool waterFlowing = false;
volatile bool alarm_setoff = false;

// Constructor initializations
Rotary rotary(wheelStepPin, wheelDirPin, wheelEnPin);
Actuator verticalActuator(needleActuatorpin1, needleActuatorpin2);
Actuator horizontalActuator(lockingActuatorpin1, lockingActuatorpin2);
Pump pump(pumpEnPin);
Alarm alarm(clockPin);
Sensor sensor(sensorPin);
Switch microSwitch(switchPin);


void takeSample() {
  // Rotates to correct position from home
  //  Replaces stepWheel(currentPosition, 1); because we will have purged at home before
  for(int i = 0; i < sampleCounter; i++) {
      stepWheel(stepPerFullRev/numTubes, 1);
      delay(500);
      lockTube();
      unlockTube();
  }
  
  lockTube();
  insertNeedle();
  fillTube();
  release();
}

//engages horizontal LA
void lockTube() {
  rotary.unlock();   

  horizontalActuator.extend();
  delay(3000);

  rotary.lock();
}

void unlockTube() {
  horizontalActuator.retract();
  delay(3000);
}

//engages vertical LA
void insertNeedle() {
  verticalActuator.retract();
  delay(12000);
}

void fillTube() {
  //digitalWrite(pumpEnPin, HIGH);
  Serial.println("begin pump");
  while (!waterFlowing) {
    pump.start();
  }

  Serial.println("end pump");
  pump.stop();

  waterFlowing = false;
}

void release() {
  rotary.lock();
  
  Serial.println("release 1");
  verticalActuator.retract();
  delay(10000);

  Serial.println("release 2");
  horizontalActuator.retract();
  delay(3000);

  waterFlowing = false;
}

// handles conversion between sample number and step count
void stepWheel(int n, int direction) {
  rotary.lock();
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
  rotary.unlock();
}

//Runs pump for set amount of time
void purge() {
  //digitalWrite(pumpEnPin, HIGH);
  Serial.println("begin pump");
  pump.start();
  delay(purgeTime);
  Serial.println("end pump");
  pump.stop();
  waterFlowing = false;
}

void setup() {
  Serial.begin(9600);
  
  // Object initialization
  rotary.init();
  verticalActuator.init();
  horizontalActuator.init();
  pump.init();
  alarm.init();
  sensor.init();
  microSwitch.init();

  release();
  lockTube();

  // Interrupts
  attachInterrupt(digitalPinToInterrupt(sensorPin), stopPump, RISING);
  attachInterrupt(digitalPinToInterrupt(clockPin), onAlarm, FALLING);
}

void loop() {
  // takeSample();
  // delay(secsBetweenSamples*1000);

  if(alarm_setoff) {
    
    //turns off alarm
    alarm.stop();

      // //selects next alarm: see RTClib for other approaches
      // future = rtc.now() + TimeSpan(120);
      // if (!rtc.setAlarm1(future, DS3231_A1_Minute)) {
      //   Serial.println("Failed to set Alarm1");
      // } 
    
    //runs until 31 samples have been taken
    if(sampleCounter < numTubes) {
      insertNeedle();
      
      // move to home, then purge 
      rotary.dirCW();
      while (!microSwitch.isSwitchPressed()) {
        rotary.step();
      }
      rotary.dirCCW();
      purge(); // goes through purge tube for water to go out through bottom
      
      release();
      takeSample();

      //engage horizontal LA for overnight
      lockTube();
      sampleCounter ++;
    }

    alarm_setoff = false;
  }

}

void stopPump() {
  waterFlowing = true;
}

void onAlarm() {
  // Clear the alarm flag to allow the next day’s event
  alarm_setoff = true;
  Serial.println("Daily alarm triggered!");
}

