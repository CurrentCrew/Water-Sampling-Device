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

const int stepPerFullRev = 800;

const int secsBetweenSamples = 3;
const int numTubes = 32;

const int fillTubeSteps = 200;
const int flushSteps = 200;

const int purgeTime = 8000;

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
Sensor sensor(sensorEnablePin, sensorPin);
Switch microSwitch(switchPin);


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

void fillTube() {
  //digitalWrite(pumpEnPin, HIGH);
  // interrupt from water sensor triggering --> waterFlowing = true
  while (!waterFlowing) {
    pump.start();
  }
  pump.stop();

  waterFlowing = false;
}

void release() {
  rotary.on();
  
  Serial.println("release 1");
  verticalActuator.extend();
  delay(10000);
  Serial.println("release 2");
  horizontalActuator.retract();
  delay(3000);

  waterFlowing = false;
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
  Serial.println("release");
  // // move to home
  // rotary.dirCW();
  // while (!microSwitch.isSwitchPressed()) {
  //   rotary.step();
  // }
  //   rotary.dirCCW();

  // Interrupts
  attachInterrupt(digitalPinToInterrupt(sensorPin), stopPump, RISING);
  attachInterrupt(digitalPinToInterrupt(clockPin), onAlarm, FALLING);

  // move to home, then purge 
  rotary.dirCW();
  rotary.on();
  while (!microSwitch.isSwitchPressed()) {
    rotary.step();
  }
  rotary.dirCCW();

  lockTube();
}

void loop() {
  // takeSample();
  // delay(secsBetweenSamples*1000);

  if(alarm_setoff) {
      alarm.stop();
      // //selects next alarm: see RTClib for other approaches
      // future = rtc.now() + TimeSpan(120);
      // if (!rtc.setAlarm1(future, DS3231_A1_Minute)) {
      //   Serial.println("Failed to set Alarm1");
      // } 
    
    //runs until 31 samples have been taken
    if(sampleCounter < numTubes) {
      alarm_setoff = false;
      lockTube();  
      insertNeedle();
      purge();
      release();

      takeSample();

      //engage horizontal LA for overnight by moving to home
      rotary.dirCW();
      rotary.on();
      while (!microSwitch.isSwitchPressed()) {
        rotary.step();
      }
      rotary.dirCCW(); 
      lockTube();

      sampleCounter ++;
    }
  }

}

void stopPump() {
  waterFlowing = true;
}

void onAlarm() {
  alarm_setoff = true;
  // Clear the alarm flag to allow the next day’s event
  Serial.println("Daily alarm triggered!");
}

