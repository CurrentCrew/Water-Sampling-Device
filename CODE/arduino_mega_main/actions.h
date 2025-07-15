#ifndef ACTIONS_H
#define ACTIONS_H

#include "settings.h"
#include "devices.h"

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

bool isWaterFlowing()
{
  return digitalRead(sensorPin);
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

  attachInterrupt(digitalPinToInterrupt(clockPin), onAlarm, FALLING);

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