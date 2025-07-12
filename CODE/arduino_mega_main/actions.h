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

#define BUTTON_DEBOUNCE 300

unsigned long lastUpButtonPress = 0;
unsigned long lastDownButtonPress = 0;
unsigned long lastSelectButtonPress = 0;

void onUpButton()
{
  Serial.println("Up button pressed");
  up_pressed = false;
  if (time_since(millis(), lastUpButtonPress) > BUTTON_DEBOUNCE)
  {
    current_screen->move(-1);
    current_screen->render(main_display);
    lastUpButtonPress = millis();
  }
}

void onDownButton()
{
  Serial.println("Down button pressed");
  down_pressed = false;
  if (time_since(millis(), lastDownButtonPress) > BUTTON_DEBOUNCE)
  {
    current_screen->move(1);
    current_screen->render(main_display);
    lastDownButtonPress = millis();
  }
}

void onSelectButton()
{
  Serial.println("Select button pressed");
  select_pressed = false;
  if (time_since(millis(), lastSelectButtonPress) > BUTTON_DEBOUNCE)
  {
    current_screen->select();
    current_screen->render(main_display);
    lastSelectButtonPress = millis();
  }
}

void attachInterrupts()
{
  pinMode(UP_BUTTON_PIN, INPUT_PULLUP);
  pinMode(DOWN_BUTTON_PIN, INPUT_PULLUP);
  pinMode(SELECT_BUTTON_PIN, INPUT_PULLUP);

  // Interrupts
  // Enable pin change interrupt group 6 (PCIE6)
  PCICR |= (1 << PCIE2);

  // Enable PCINT53, PCINT54, PCINT55 in PCMSK6
  PCMSK2 |= (1 << PCINT21);  // Pin 67
  PCMSK2 |= (1 << PCINT22);  // Pin 68
  PCMSK2 |= (1 << PCINT23);  // Pin 69
}

ISR(PCINT2_vect) {
  // Check which pin triggered it
  if (digitalRead(UP_BUTTON_PIN) == LOW) {
    up_pressed = true;
  }
  if (digitalRead(DOWN_BUTTON_PIN) == LOW) {
    down_pressed = true;
  }
  if (digitalRead(SELECT_BUTTON_PIN) == LOW) {
    select_pressed = true;
  }
}

#endif