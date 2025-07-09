// Debug Serials
#define ALARM_SERIAL Serial
#define ACTUATOR_SERIAL Serial
#define SENSOR_SERIAL Serial
#define PUMP_SERIAL Serial
#define SWITCH_SERIAL Serial
#define ROTARY_SERIAL Serial

#include "actions.h"
#include "devices.h"

UIScreen main_screen;

struct Vector2 prime_pump_button_pos = {0, 0};
struct Vector2 prime_pump_button_size = {128, 16};
UIButton prime_pump_button(1, prime_pump_button_pos, prime_pump_button_size);

struct Vector2 start_button_pos = {0, 17};
struct Vector2 start_button_size = {128, 16};
UIButton start_button(2, start_button_pos, start_button_size);

struct Vector2 release_button_pos = {0, 33};
struct Vector2 release_button_size = {128, 16};
UIButton release_button(3, release_button_pos, release_button_size);

void init_ui()
{
  prime_pump_button.setText(F("Prime Pump"), 10, 1);
  start_button.setText(F("Start"), 5, 1);
  release_button.setText(F("Release"), 7, 1);

  main_screen.addElement(&prime_pump_button);
  main_screen.addElement(&start_button);
  main_screen.addElement(&release_button);
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

void setup() {
  Serial.begin(9600);
  
  // Object initialization
  init_devices();
  attachInterrupts();

  initializeInterface();
  init_ui();

  while (true) {
    main_screen.move(1);
    main_screen.render(main_display);
    delay(500);

    main_screen.move(1);
    main_screen.render(main_display);
    delay(500);

    main_screen.move(-1);
    main_screen.render(main_display);
    delay(500);

    main_screen.move(-1);
    main_screen.render(main_display);
    delay(500);
  }

  
}

void loop() {
  

  

}

