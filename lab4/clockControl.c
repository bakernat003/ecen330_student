#include "clockControl.h"
#include "clockDisplay.h"
#include "display.h"
#include "intervalTimer.h"
#include <stdbool.h>
#include <stdio.h>

#define INIT_ST_MSG "init state\n"
#define NEVER_TOUCHED_ST_MSG "never_touched_st\n"
#define WAITING_FOR_TOUCH_ST_MSG "waiting for touch_st\n"
#define ADC_TIMER_RUNNING_ST_MSG "adc_timer_running_st\n"
#define AUTO_TIMER_RUNNING_ST_MSG "auto_timer_running_st\n"
#define RATE_TIMER_RUNNING_ST_MSG "rate_timer_running_st\n"
#define RATE_TIMER_EXPIRED_ST_MSG "rate_timer_expired_st\n"

#define ADC_COUNTER_MAX_VALUE 1
#define AUTO_COUNTER_MAX_VALUE 10
#define RATE_COUNTER_MAX_VALUE 2
#define MS_COUNTER_MAX_VALUE 20
#define UPDATE_ALL 1
#define TIMER_ZERO 0

static int16_t adcCounter;
static int16_t autoCounter;
static int16_t rateCounter;
static int16_t msCounter;

// States for the controller state machine.
enum clockControl_st_t {
  init_st, // Start here, transition out of this state on the first tick.
  never_touched_st, // Wait here until the first touch - clock is disabled until
                    // set.
  waiting_for_touch_st,  // waiting for touch, clock is enabled and running.
  adc_timer_running_st,  // waiting for the touch-controller ADC to settle.
  auto_timer_running_st, // waiting for the auto-update delay to expire
                         // (user is holding down button for auto-inc/dec)
  rate_timer_running_st, // waiting for the rate-timer to expire to know when to
                         // perform the auto inc/dec.
  rate_timer_expired_st, // when the rate-timer expires, perform the inc/dec
                         // function.
  add_second_to_clock_st // add a second to the clock time and reset the ms
                         // counter.
} currentState;

// Initializes the State Machine
void clockControl_init() {
  intervalTimer_init(TIMER_ZERO);
  clockDisplay_init();
  intervalTimer_start(TIMER_ZERO);
  clockDisplay_updateTimeDisplay(UPDATE_ALL);
  intervalTimer_stop(TIMER_ZERO);
  double time = intervalTimer_getTotalDurationInSeconds(TIMER_ZERO);
  printf("%f\n", time);
  adcCounter = 0;
  autoCounter = 0;
  rateCounter = 0;
  msCounter = 0;
  currentState = init_st;
}

void debugStatePrint() {
  static enum clockControl_st_t previousState;
  static bool firstPass = true;
  // Only print the message if:
  // 1. This the first pass and the value for previousState is unknown.
  // 2. previousState != currentState - this prevents reprinting the same state
  // name over and over.
  if (previousState != currentState || firstPass) {
    firstPass = false; // previousState will be defined, firstPass is false.
    previousState =
        currentState;       // keep track of the last state that you were in.
    switch (currentState) { // This prints messages based upon the state that
                            // you were in.
    case init_st:
      printf(INIT_ST_MSG);
      printf("%d", msCounter);
      printf("\n");
      break;
    case never_touched_st:
      printf(NEVER_TOUCHED_ST_MSG);
      break;
    case waiting_for_touch_st:
      printf(WAITING_FOR_TOUCH_ST_MSG);
      break;
    case adc_timer_running_st:
      printf(ADC_TIMER_RUNNING_ST_MSG);
      break;
    case auto_timer_running_st:
      printf(AUTO_TIMER_RUNNING_ST_MSG);
      break;
    case rate_timer_running_st:
      printf(RATE_TIMER_RUNNING_ST_MSG);
      break;
    case rate_timer_expired_st:
      printf(RATE_TIMER_EXPIRED_ST_MSG);
      break;
    }
  }
}

void clockControl_tick() {
  // Perform state transition
  switch (currentState) {
  case init_st:
    currentState = never_touched_st;
    break;

  case never_touched_st:
    // If the display hasn't been touched, stay in the never_touched_st
    if (!display_isTouched()) {
      currentState = never_touched_st;
    }
    // If the display is touched, change state to the adc_timer_running_st and
    // clear the old touch data
    if (display_isTouched()) {
      currentState = adc_timer_running_st;
      display_clearOldTouchData();
    }
    break;

  case waiting_for_touch_st:
    // If the display has been touched, clear the old touch data and move to the
    // adc?timer_running_st
    if (display_isTouched()) {
      display_clearOldTouchData();
      currentState = adc_timer_running_st;
    }
    adcCounter = 0;
    autoCounter = 0;
    rateCounter = 0;
    break;

  case adc_timer_running_st:
    // If the display is not touched and the counter reaches its max, perform an
    // increment or decrement and change the state to waiting_for_touch_st. If
    // the display has been touched and the counter reaches its max, clear the
    // old touch data and swtich to auto_timer_running_st Otherwise, switch to
    // the adc_timer_running_st
    if (!display_isTouched() && (adcCounter == ADC_COUNTER_MAX_VALUE)) {
      clockDisplay_performIncDec();
      currentState = waiting_for_touch_st;
    } else if (display_isTouched() && (adcCounter == ADC_COUNTER_MAX_VALUE)) {
      display_clearOldTouchData();
      currentState = auto_timer_running_st;
    } else {
      currentState = adc_timer_running_st;
    }
    break;

  case auto_timer_running_st:
    // If the touch lasts 500ms, move to the rate_timer_runnning_st. Otherwise,
    // increment or decrement.
    if (!display_isTouched()) {
      clockDisplay_performIncDec();
      currentState = waiting_for_touch_st;
    } else if (display_isTouched() && (autoCounter == AUTO_COUNTER_MAX_VALUE)) {
      display_clearOldTouchData();
      clockDisplay_performIncDec();
      currentState = rate_timer_running_st;
    } else {
      currentState = auto_timer_running_st;
    }
    break;

  case rate_timer_running_st:
    // Changes the digit every 10ms while the rate timer is running
    if (!display_isTouched()) {
      currentState = waiting_for_touch_st;
    } else if (display_isTouched() && (rateCounter == RATE_COUNTER_MAX_VALUE)) {
      display_clearOldTouchData();
      currentState = rate_timer_expired_st;
    } else {
      currentState = rate_timer_running_st;
    }
    break;

  case rate_timer_expired_st:
    // If the display is touched, keep incrementing or decrementing. If the
    // display is no longer touched, go to the waiting_for_touch_st
    if (display_isTouched()) {
      clockDisplay_performIncDec();
      currentState = rate_timer_running_st;
    }
    if (!display_isTouched()) {
      currentState = waiting_for_touch_st;
    }
    break;

  case add_second_to_clock_st:

    break;
  default:
    // print an error message here.
    break;
  }

  // Perform state action next.
  switch (currentState) {
  case init_st:
    break;
  case never_touched_st:
    break;
  case waiting_for_touch_st:
    adcCounter = 0;
    autoCounter = 0;
    rateCounter = 0;
    msCounter++;
    if ((msCounter % MS_COUNTER_MAX_VALUE) == 0) {
      clockDisplay_advanceTimeOneSecond();
    }
    break;
  case adc_timer_running_st:
    adcCounter++;
    msCounter++;
    if ((msCounter % MS_COUNTER_MAX_VALUE) == 0) {
      clockDisplay_advanceTimeOneSecond();
    }
    break;
  case auto_timer_running_st:
    autoCounter++;
    msCounter++;
    if ((msCounter % MS_COUNTER_MAX_VALUE) == 0) {
      clockDisplay_advanceTimeOneSecond();
    }
    break;
  case rate_timer_running_st:
    rateCounter++;
    msCounter++;
    if ((msCounter % MS_COUNTER_MAX_VALUE) == 0) {
      clockDisplay_advanceTimeOneSecond();
    }
    break;
  case rate_timer_expired_st:
    rateCounter = 0;
    msCounter++;
    if ((msCounter % MS_COUNTER_MAX_VALUE) == 0) {
      clockDisplay_advanceTimeOneSecond();
    }
    break;
  default:
    // print an error message here.
    break;
  }
  debugStatePrint();
}
