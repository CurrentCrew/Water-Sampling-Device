#ifndef STATES_H
#define STATES_H

#define IDLE_STATE    0
#define WAIT_STACK_STATE 1
#define WAIT_STATE 2
#define PRIMING_STATE 3
#define GO_TO_LIMIT_STATE 4
#define MAIN_LOOP_IDLE 5
#define STEP_WHEEL_STATE 6
#define FILL_TILL_WATER 7

struct WaitState {
  int lifetime;
  void (*action)();
};

struct WaitStack {
  struct WaitState *stack;
  int stack_length;
};

struct WaitState *current_wait_state;
struct WaitStack *current_wait_stack;
int wait_stack_index;
unsigned long last_wait_state;
void (*waitStateEnd)();

void (*goToLimitEnd)();

int wheel_steps;
void (*stepWheelEnd)();

int state = IDLE_STATE;

const unsigned long ULONG_MAX = (unsigned long)-1;
#define SECONDS 1000

// To avoid millis() wrapping messing things up
unsigned long  time_since(unsigned long current, unsigned long last)
{
  return current>last? (current - last):(ULONG_MAX - last + current);
}

void setWaitStack(WaitStack *stack, void (*_waitStateEnd)())
{
  state = WAIT_STACK_STATE;
  current_wait_stack = stack;
  last_wait_state = millis();
  wait_stack_index = 0;
  waitStateEnd = _waitStateEnd;
  stack->stack[0].action();
}

void setWaitState(struct WaitState *wait_state, void (*_waitStateEnd)())
{
  state = WAIT_STATE;
  current_wait_state = wait_state;
  last_wait_state = millis();
  waitStateEnd = _waitStateEnd;
  wait_state->action();
}

// #define PURGE_STATE  4

#endif