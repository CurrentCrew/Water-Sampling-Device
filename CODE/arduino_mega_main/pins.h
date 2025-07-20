#ifndef PINS_H
#define PINS_H

const int sensorEnablePin = 4;
const int sensorPin = 7;

const int clockPin = 18;

const int SERVO_PIN = 12;

#ifdef SETUP_1

const int needleActuatorpin1 = 27;
const int needleActuatorpin2 = 26;

const int wheelStepPin = 42;
const int wheelDirPin = 41;
const int wheelEnPin = 40;

const int UP_BUTTON_PIN = 67;
const int DOWN_BUTTON_PIN = 68;
const int SELECT_BUTTON_PIN = 3;

const int pumpEnPin = 30;

const int switchPin = 6;

#endif

#ifdef SETUP_2

const int needleActuatorpin1 = 26;
const int needleActuatorpin2 = 27;

const int wheelStepPin = 43;
const int wheelDirPin = 40;
const int wheelEnPin = 42;

const int UP_BUTTON_PIN = 48;
const int DOWN_BUTTON_PIN = 50;
const int SELECT_BUTTON_PIN = 46;

const int pumpEnPin = 36;

const int switchPin = 2;

#endif

#endif