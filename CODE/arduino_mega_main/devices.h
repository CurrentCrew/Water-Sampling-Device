#ifndef DEVICES_H
#define DEVICES_H

#include "pins.h"

#include "rotary.h"
#include "alarm.h"
#include "actuator.h"
#include "sensor.h"
#include "pump.h"
#include "switch.h"
#include "interface.h"
#include <Servo.h>

// Constructor initializations
Rotary rotary(wheelStepPin, wheelDirPin, wheelEnPin);
Actuator verticalActuator(needleActuatorpin1, needleActuatorpin2);
Actuator horizontalActuator(lockingActuatorpin1, lockingActuatorpin2);
Pump pump(pumpEnPin);
Alarm alarm(clockPin);
Sensor sensor(sensorEnablePin, sensorPin);
Switch microSwitch(switchPin);
Servo adjustmentServo;

const int SERVO_IDLE = 100;
const int SERVO_PUSH = 58;


void init_devices()
{
  // Initialize Objects
  rotary.init();
  rotary.off();
  verticalActuator.init();
  horizontalActuator.init();
  pump.init();
  alarm.init();
  sensor.init();
  microSwitch.init();
  adjustmentServo.attach(SERVO_PIN);
  adjustmentServo.write(SERVO_IDLE); 
}

#endif