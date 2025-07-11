#ifndef SETTINGS_H
#define SETTINGS_H

const int stepPerFullRev = 800;

const int secsBetweenSamples = 3;
const int numTubes = 32;

const int fillTubeSteps = 200;
const int flushSteps = 200;

const int purgeTime = 8000;

int sampleCounter = 1;

int currentPosition;

// hardware interrupt flags
volatile bool alarm_setoff = true;
volatile bool up_pressed = false;
volatile bool down_pressed = false;
volatile bool select_pressed = false;

int state = IDLE_STATE;

#endif