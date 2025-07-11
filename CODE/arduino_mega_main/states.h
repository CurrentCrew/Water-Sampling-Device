#ifndef STATES_H
#define STATES_H

#define IDLE_STATE    0
#define PRIMING_STATE 1

#define RELEASE_STATE_1 2 // Vertial Actuator
#define RELEASE_STATE_2 3 // Horizontal Actuator
unsigned long release_state_start;

#define MAIN_LOOP_RELEASE_STATE_1 4 // Vertial Actuator
#define MAIN_LOOP_RELEASE_STATE_2 5 // Horizontal Actuator

#define PURGE_STATE  4

#endif