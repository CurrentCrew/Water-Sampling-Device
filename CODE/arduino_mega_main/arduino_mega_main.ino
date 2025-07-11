// Debug Serials
#define ALARM_SERIAL Serial
#define ACTUATOR_SERIAL Serial
#define SENSOR_SERIAL Serial
#define PUMP_SERIAL Serial
#define SWITCH_SERIAL Serial
#define ROTARY_SERIAL Serial

#define TEST_MODE

#include "states.h"
#include "actions.h"
#include "devices.h"

struct Vector2 med_button_size = {128, 16};
struct Vector2 big_button_size = {128, 32};

struct Vector2 big_button_center_pos = {64-big_button_size.x/2, 31-big_button_size.y/2};

// Main Screen
UIScreen home_screen;

struct Vector2 prime_pump_button_pos = {0, 0};
UIButton prime_pump_button(1, prime_pump_button_pos, med_button_size);

struct Vector2 start_button_pos = {0, 17};
UIButton start_button(2, start_button_pos, med_button_size);

struct Vector2 release_button_pos = {0, 33};
UIButton release_button(3, release_button_pos, med_button_size);

// Prime Screen
UIScreen prime_screen;
UIButton prime_release_button(1, big_button_center_pos, big_button_size);

// Releasing Screen
UIScreen releasing_screen;
UIText release_text(1, big_button_center_pos, big_button_size);

// Main Loop Screen
UIScreen main_loop_screen;
UIButton main_loop_stop_button(1, big_button_center_pos, big_button_size);

const unsigned long ULONG_MAX = (unsigned long)-1;



UIScreen *state_screens[] = {
  &home_screen,      // IDLE_STATE
  &prime_screen,     // PRIMING_STATE
  &releasing_screen, // RELEASE_STATE_1
  &releasing_screen, // RELEASE_STATE_2
};

// To avoid millis() wrapping messing things up
unsigned long  time_since(unsigned long current, unsigned long last)
{
  return current>last? (current - last):(ULONG_MAX - last + current);
}

void updateDisplay()
{
  current_screen->render(main_display);
}

void primePumpButtonPressed()
{
  pump.start();
  state = PRIMING_STATE;
}

void switchToRelease1()
{
  release_text.setText(F("Releasing Needle"), 16, 1);
  release_state_start = millis();
  verticalActuator.extend();
  state = RELEASE_STATE_1;
}

void switchToRelease2()
{
  release_text.setText(F("Releasing Lock"), 14, 1);
  release_state_start = millis();
  horizontalActuator.retract();
  state = RELEASE_STATE_2;
  updateDisplay();
}

void primeReleaseButtonPressed()
{
  state = IDLE_STATE;
  pump.stop();
}

void start()
{
  release();

  goToLimitSwitch();

  rotary.dirCCW();

  lockTube();

  while (true)
  {
    delay(1000);
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
}

void init_ui()
{
  // Home screen
  prime_pump_button.setText(F("Prime Pump"), 10, 1);
  start_button.setText(F("Start"), 5, 1);
  release_button.setText(F("Release"), 7, 1);

  prime_pump_button.setOnPress(&primePumpButtonPressed);
  release_button.setOnPress(&switchToRelease1);
  start_button.setOnPress(&start);


  home_screen.addElement(&prime_pump_button);
  home_screen.addElement(&start_button);
  home_screen.addElement(&release_button);

  // Prime Screen
  prime_release_button.setText(F("Stop Prime"), 10, 1);

  prime_release_button.setOnPress(&primeReleaseButtonPressed);

  prime_screen.addElement(&prime_release_button);

  // Releasing Screen
  releasing_screen.addElement(&release_text);
}

void setup() {
  Serial.begin(9600);
  
  // Object initialization
  init_devices();
  attachInterrupts();

  initializeInterface();
  init_ui();

  current_screen = &home_screen;

  current_screen->render(main_display);
}

void loop() {
  // Serial.println(digitalRead(UP_BUTTON_PIN));
  delay(1);

  if (up_pressed)
    onUpButton();

  if (down_pressed)
    onDownButton();

  if (select_pressed)
    onSelectButton();

  if (current_screen != state_screens[state])
    switchScreen(state_screens[state]);

  switch (state) {
    case IDLE_STATE:
      break;
    case PRIMING_STATE:
      break;
    case PURGE_STATE:
      break;
    case RELEASE_STATE_1:
      // if it has been 10 seconds
      if (time_since(millis(), release_state_start) > 10*1000)
        switchToRelease2();
      break;
    case RELEASE_STATE_2:
      // if it has been 4 seconds
      if (time_since(millis(), release_state_start) > 4*1000)
        state = IDLE_STATE;
      break;

  }
  

}

