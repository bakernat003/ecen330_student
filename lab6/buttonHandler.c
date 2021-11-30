#include "buttonHandler.h"
#include "display.h"
#include "simonDisplay.h"
#include <stdbool.h>
#include <stdio.h>

#define INIT_ST_MSG "bh init state\n"
#define WAIT_FOR_TOUCH_ST_MSG "bh wait for touch state\n"
#define ADC_ST_MSG "bh adc state\n"
#define WAIT_RELEASE_ST_MSG "bh wait release state\n"
#define FINAL_ST_MSG "bh final state\n"

void bhdebugStatePrint();

static bool isReleased = false;
static bool isEnabled = false;
static bool isComplete = false;

// States for the controller state machine.
static enum buttonHandler_st_t {
  init_st, // Start here, transition out of this state on the first tick.
  wait_for_touch_st,
  adc_st,
  wait_release_st,
  final_st
} currentState;

// Standard init function.
void buttonHandler_init() { currentState = init_st; }

// Standard tick function.
void buttonHandler_tick() {
  // Perform state transition
  switch (currentState) {

  case init_st:
    // Only run if isEnabled
    if (isEnabled) {
      isReleased = false;
      isComplete = false;
      currentState = wait_for_touch_st;
    } else {
      currentState = init_st;
    }
    break;

  case wait_for_touch_st:
    // If isn't enabled, go to init_st
    if (!isEnabled) {
      currentState = init_st;
    } else if (display_isTouched()) {
      display_clearOldTouchData();
      currentState = adc_st;
    } else {
      currentState = wait_for_touch_st;
    }
    break;

  case adc_st:
    // If isn't enabled, go to init_st
    if (!isEnabled) {
      currentState = init_st;
    } else {
      simonDisplay_drawSquare(buttonHandler_getRegionNumber(),
                              SIMON_DISPLAY_DRAW);
      currentState = wait_release_st;
    }
    break;

  case wait_release_st:
    // If isn't enabled, go to init_st
    if (!isEnabled) {
      simonDisplay_drawSquare(buttonHandler_getRegionNumber(),
                              SIMON_DISPLAY_ERASE);
      currentState = init_st;
    } else if (display_isTouched()) {
      currentState = wait_release_st;
    } else if (!display_isTouched()) {
      isReleased = true;
      simonDisplay_drawSquare(buttonHandler_getRegionNumber(),
                              SIMON_DISPLAY_ERASE);
      printf("Should redraw button\n");
      simonDisplay_drawButton(buttonHandler_getRegionNumber(),
                              SIMON_DISPLAY_DRAW);
      currentState = final_st;
    }
    break;

  case final_st:
    isComplete = true;
    // If isn't enabled, go to init_st. Otherwise, stay in final_st
    if (isEnabled) {
      currentState = final_st;
    } else if (!isEnabled) {
      currentState = init_st;
    }
    break;

  default:
    printf("This is an error message from buttonHandler_tick() transition\n");
    break;
  }

  // Perform state action next.
  switch (currentState) {
  case init_st:
    break;

  case wait_for_touch_st:
    break;

  case adc_st:
    break;

  case wait_release_st:
    break;

  case final_st:
    break;

  default:
    printf("This is an error message from buttonHandler_tick() action\n");
    break;
  }
  bhdebugStatePrint();
}

// Used to Debug State machine
void bhdebugStatePrint() {
  static enum buttonHandler_st_t previousState;
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
      break;
    case wait_for_touch_st:
      printf(WAIT_FOR_TOUCH_ST_MSG);
      break;
    case adc_st:
      printf(ADC_ST_MSG);
      break;
    case wait_release_st:
      printf(WAIT_RELEASE_ST_MSG);
      break;
    case final_st:
      printf(FINAL_ST_MSG);
      break;
    }
  }
}

// Get the simon region numbers
uint8_t buttonHandler_getRegionNumber() {
  uint8_t region;
  int16_t x, y;
  int8_t z;
  display_getTouchedPoint(&x, &y, &z);
  region = simonDisplay_computeRegionNumber(x, y);
  return region;
}

// Turn on the state machine. Part of the interlock.
void buttonHandler_enable() { isEnabled = true; }

// Turn off the state machine. Part of the interlock.
void buttonHandler_disable() { isEnabled = false; }

// The only thing this function does is return a boolean flag set by the
// buttonHandler state machine. To wit: Once enabled, the buttonHandler
// state-machine first waits for a touch. Once a touch is detected, the
// buttonHandler state-machine computes the region-number for the touched area.
// Next, the buttonHandler state-machine waits until the player removes their
// finger. At this point, the state-machine should set a bool flag that
// indicates the the player has removed their finger. Once the buttonHandler()
// state-machine is disabled, it should clear this flag.
// All buttonHandler_releasedDetected() does is return the value of this flag.
// As such, the body of this function should only contain a single line of code.
// If this function does more than return a boolean set by the buttonHandler
// state machine, you are going about this incorrectly.
bool buttonHandler_releaseDetected() { return isReleased; }

// Let's you know that the buttonHander is waiting in the interlock state.
bool buttonHandler_isComplete() { return isComplete; }

// Allows an external controller to notify the buttonHandler that a time-out has
// occurred.
void buttonHandler_timeOutOccurred() {}
