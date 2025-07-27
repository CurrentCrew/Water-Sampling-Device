#ifndef UI_H
#define UI_H

#include "interface.h"

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
UIButton settings_button(3, {0, 48}, med_button_size);

// Settings Screen
UIScreen settings_screen;
UIButton purge_time_button(1, {0, 0}, med_button_size);
UIButton account_steps_button(2, {0, 16}, med_button_size);
UIButton samples_taken_button(2, {0, 16}, med_button_size);

// Manual Control Screen
UIScreen manual_control_screen;
UIButton inject_needle_button(1, {0, 0}, med_button_half_size);
UIButton release_needle_button(2, {64, 0}, med_button_half_size);
UIButton step_forward_button(5, {0, 16}, med_button_half_size);
UIButton step_backwards_button(6, {64, 16}, med_button_half_size);
UIButton go_to_limit_button(7, {0, 32}, med_button_half_size);
UIButton manual_control_screen_back(8, {64, 32}, med_button_half_size);

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

void setWaitingText(char* text, int text_length)
{
  waiting_text.setText(text, text_length, 1);
  updateDisplay();
}

void initUI()
{
  // Home screen
  prime_pump_button.setText("Prime Pump", 10, 1);
  start_button.setText("Start", 5, 1);
  manual_button.setText("Manual Control", 14, 1);

  home_screen.addElement(&prime_pump_button);
  home_screen.addElement(&start_button);
  home_screen.addElement(&manual_button);

  // Manual Controls Screen

  inject_needle_button.setText("Inject", 6, 1);
  release_needle_button.setText("Release", 7, 1);
  step_forward_button.setText("Forward", 7, 1);
  step_backwards_button.setText("Backwards", 9, 1);
  go_to_limit_button.setText("To Limit", 8, 1);
  manual_control_screen_back.setText("Back", 4, 1);

  manual_control_screen.addElement(&inject_needle_button);
  manual_control_screen.addElement(&release_needle_button);
  manual_control_screen.addElement(&step_forward_button);
  manual_control_screen.addElement(&step_backwards_button);
  manual_control_screen.addElement(&go_to_limit_button);
  manual_control_screen.addElement(&manual_control_screen_back);

  // Prime Screen
  prime_release_button.setText("Stop Prime", 10, 1);

  prime_screen.addElement(&prime_release_button);

  // Waiting Screen
  waiting_screen.addElement(&waiting_text);
}

#endif