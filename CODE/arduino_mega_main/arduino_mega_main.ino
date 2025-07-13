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

struct Vector2 med_button_half_size = {64, 16};
struct Vector2 med_button_size = {128, 16};
struct Vector2 big_button_size = {128, 32};

struct Vector2 big_button_center_pos = {64-big_button_size.x/2, 31-big_button_size.y/2};

// Main Screen
UIScreen home_screen;

struct Vector2 prime_pump_button_pos = {0, 0};
UIButton prime_pump_button(1, {0, 0}, med_button_size);
UIButton start_button(2, {0, 16}, med_button_size);
UIButton manual_button(3, {0, 32}, med_button_size);

// Manual Control Screen
UIScreen manual_control_screen;
UIButton inject_needle_button(1, {0, 0}, med_button_half_size);
UIButton release_needle_button(2, {64, 0}, med_button_half_size);
UIButton extend_lock_button(3, {0, 16}, med_button_half_size);
UIButton release_lock_button(4, {64, 16}, med_button_half_size);
UIButton manual_control_screen_back(5, {0, 36}, med_button_size);

// Prime Screen
UIScreen prime_screen;
UIButton prime_release_button(1, big_button_center_pos, big_button_size);

// Waiting Screen
UIScreen waiting_screen;
UIText waiting_text(1, big_button_center_pos, big_button_size);

// Main Loop Screen
UIScreen main_loop_screen;
Vector2 main_loop_text_pos = {0, 10};
// UIText main_loop_text(1, med_button_size);
UIButton main_loop_stop_button(1, big_button_center_pos, big_button_size);

void updateDisplay()
{
  current_screen->render(main_display);
}

void setStepWheel(int steps, void (*_stepWheelEnd)())
{
  state = STEP_WHEEL_STATE;
  stepWheelEnd = _stepWheelEnd;
  wheel_steps = steps;
  setWaitingText("Stepping", 8);
  rotary.on();
  rotary.dirCCW();
  Serial.println("Stepping");
}

struct WaitState release_needle_state {
  10 * SECONDS,
  []() {
    setWaitingText("Releasing Needle", 16);
    verticalActuator.extend();
  }
};

struct WaitState release_lock_state {
  4 * SECONDS,
  []() {
    setWaitingText("Releasing Lock", 14);
    horizontalActuator.retract();
  }
};

struct WaitState insert_needle_state {
  10 * SECONDS,
  []() {
    setWaitingText("Inserting Needle", 16);
    verticalActuator.retract();
  }
};

struct WaitState extend_lock_state {
  4 * SECONDS,
  []() {
    setWaitingText("Extending Lock", 16);
    horizontalActuator.extend();
  }
};

struct WaitState purge_state {
  4 * SECONDS,
  []() {
    setWaitingText("PurgingWater", 16);
    pump.start();
  }
};

struct WaitState pump_off {
  1,
  []() {
    pump.stop();
  }
};

// Release Stack
struct WaitState release_stack_objs[] = {
  release_needle_state,
  release_lock_state
};

WaitStack release_stack = {
  release_stack_objs,
  2
};

// Main Loop Start STack
struct WaitState main_loop_start_stack_objs[] = {
  insert_needle_state, purge_state, pump_off, release_needle_state, release_lock_state
};

WaitStack main_loop_stack = {
  main_loop_start_stack_objs,
  5
};

// pre_inject_stack
struct WaitState pre_inject_stack_objs[] = {
  extend_lock_state, insert_needle_state
};

WaitStack pre_inject_stack = {
  main_loop_start_stack_objs,
  2
};

void setWaitingText(char* text, int text_length)
{
  waiting_text.setText(text, text_length, 1);
  updateDisplay();
}

void goToLimit(void (*_goToLimitEnd)())
{
  setWaitingText("Going to limit switch", 21);
  state = GO_TO_LIMIT_STATE;
  rotary.dirCW();
  rotary.on();
  goToLimitEnd = _goToLimitEnd;
}

void setMainLoopIdle()
{
  state = MAIN_LOOP_IDLE;
  setWaitingText("Waiting for alarm", 17);
}

UIScreen *state_screens[] = {
  &home_screen,      // IDLE_STATE
  &waiting_screen,   // WAIT_STACK_STATE
  &waiting_screen,   // WAIT_STATE
  &prime_screen,     // PRIMING_STATE
  &waiting_screen,   // GO_TO_LIMIT_STATE
  &waiting_screen,   // MAIN_LOOP_IDLE
  &waiting_screen,   // STEP_WHEEL_STATE
  &waiting_screen,   // FILL_TILL_WATER
  &manual_control_screen    // MANUAL_CONTROL
};

void setManualState()
{
  state = MANUAL_CONTROL;
}

void init_ui()
{
  // Home screen
  prime_pump_button.setText("Prime Pump", 10, 1);
  start_button.setText("Start", 5, 1);
  manual_button.setText("Manual Control", 14, 1);

  prime_pump_button.setOnPress([]() {
    state = PRIMING_STATE;
    pump.start();
  });

  manual_button.setOnPress(setManualState);

  start_button.setOnPress([]() {
    setWaitStack(&release_stack, []() {
      goToLimit([]() {
        setWaitState(&extend_lock_state, &setMainLoopIdle);
      });
    });
  });


  home_screen.addElement(&prime_pump_button);
  home_screen.addElement(&start_button);
  home_screen.addElement(&manual_button);

  // Manual Controls Screen

  inject_needle_button.setText("Inject", 6, 1);
  release_needle_button.setText("Release", 7, 1);
  extend_lock_button.setText("Lock", 4, 1);
  release_lock_button.setText("Unlock", 6, 1);
  manual_control_screen_back.setText("Back", 4, 1);

  inject_needle_button.setOnPress([]() {
    setWaitState(&insert_needle_state, setManualState);
  });

  release_needle_button.setOnPress([]() {
    setWaitState(&release_needle_state, setManualState);
  });

  extend_lock_button.setOnPress([]() {
    setWaitState(&extend_lock_state, setManualState);
  });

  release_lock_button.setOnPress([]() {
    setWaitState(&release_lock_state, setManualState);
  });

  manual_control_screen_back.setOnPress([]() {
    state = IDLE_STATE;
  });

  manual_control_screen.addElement(&inject_needle_button);
  manual_control_screen.addElement(&release_needle_button);
  manual_control_screen.addElement(&extend_lock_button);
  manual_control_screen.addElement(&release_lock_button);
  manual_control_screen.addElement(&manual_control_screen_back);

  // Prime Screen
  prime_release_button.setText("Stop Prime", 10, 1);

  prime_release_button.setOnPress([]() {
    state = IDLE_STATE;
    pump.stop();
  });

  prime_screen.addElement(&prime_release_button);

  // Waiting Screen
  waiting_screen.addElement(&waiting_text);
}

void setup() {
  Serial.begin(9600);
  
  // Object initialization
  init_devices();
  attachInterrupts();

  initializeInterface();
  init_ui();

  current_screen = &home_screen;

  updateDisplay();

  // adjustmentServo.write(100); 
  // delay(1000);
  // adjustmentServo.write(58); 
}

void loop() {
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
    case WAIT_STACK_STATE:
      if (time_since(millis(), last_wait_state) > current_wait_stack->stack[wait_stack_index].lifetime)
      {
        wait_stack_index++;
        if (wait_stack_index == current_wait_stack->stack_length)
          waitStateEnd();
        else {
          current_wait_stack->stack[wait_stack_index].action();
          last_wait_state = millis();
          delay(10); // otherwise it will double count
        }
      }
      break;

    case WAIT_STATE:
      if (time_since(millis(), last_wait_state) > current_wait_state->lifetime)
        waitStateEnd();
      break;

    case GO_TO_LIMIT_STATE:
      if (microSwitch.isSwitchPressed()) {
        state = IDLE_STATE; // to help prevent locks
        goToLimitEnd();
        return;
      }
      rotary.step();
      break;
    case MAIN_LOOP_IDLE:
      if(alarm_setoff) {
        alarm.stop();
        setWaitStack(&main_loop_stack, []() {
          setStepWheel(int(20358/numTubes) * sampleCounter, []() {
            sampleCounter += 5;
            adjustmentServo.write(SERVO_PUSH); 
            setWaitStack(&pre_inject_stack, []() {
              adjustmentServo.write(SERVO_IDLE); 
              state = FILL_TILL_WATER;
              pump.start();
              setWaitingText("Filling", 7);
            });
          });
        });
      }
      break;

    case STEP_WHEEL_STATE:
      rotary.step();
      wheel_steps--;

      if (wheel_steps<=0)
        stepWheelEnd(); 
      break;

    case FILL_TILL_WATER:
      if (isWaterFlowing())
      {
        pump.stop();
        setWaitStack(&release_stack, []() {
          goToLimit([]() {
            setWaitState(&extend_lock_state, []() {
              if (sampleCounter >= numTubes)
                state = IDLE_STATE;
              else
                state = MAIN_LOOP_IDLE;
            });
          });
        });
      }
      break;
    case MANUAL_CONTROL:
      break;
  }
  

}

