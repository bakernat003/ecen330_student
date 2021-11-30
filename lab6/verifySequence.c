#include "verifySequence.h"
#include "buttonHandler.h"
#include "display.h"
#include "globals.h"
#include "simonDisplay.h"
#include <stdbool.h>
#include <stdio.h>

#define INIT_ST_MSG "vs init state\n"
#define WAIT_FOR_TOUCH_ST_MSG "vs run state\n"
#define FINAL_ST_MSG "vs final state\n"

#define TIMEOUT_MAX 60

void vsdebugStatePrint();

static uint16_t timeoutTimer = 0;

static bool isEnabled = false;
static bool isComplete = false;
static bool isUserInputError = false;
static bool isUserTimeOutError = false;

static uint16_t regionNumber = 0;
static uint8_t index = 0;

// States for the controller state machine.
static enum verifySequence_st_t {
  init_st, // Start here, transition out of this state on the first tick.
  vs_wait_for_touch_st,
  final_st
} currentState;

// Standard init function.
void verifySequence_init() { currentState = init_st; }

// Standard tick function.
void verifySequence_tick() {
  // Perform state transition
  switch (currentState) {

  case init_st:
    // If the SM is enabled, initialize things. Otherwise, stay in the init_st
    if (isEnabled) {
      // When the timoutTimer expires, disable the buttonhandler SM and go the
      // the final_st
      if (timeoutTimer > TIMEOUT_MAX) {
        buttonHandler_disable();
        timeoutTimer = 0;
        isUserInputError = true;
        currentState = final_st;
      } else {
        timeoutTimer++;
        isUserTimeOutError = false;
        isUserInputError = false;
        isComplete = false;
        buttonHandler_enable();
        currentState = vs_wait_for_touch_st;
      }
    } else {
      timeoutTimer = 0;
      currentState = init_st;
    }
    break;

  case vs_wait_for_touch_st:
    // If the SM isn't enabled, go to the init_st
    if (!isEnabled) {
      timeoutTimer = 0;
      currentState = init_st;
    } else {
      // When the timoutTimer expires, disable the buttonhandler SM and go the
      // the final_st
      if (timeoutTimer > TIMEOUT_MAX) {
        buttonHandler_disable();
        timeoutTimer = 0;
        isUserTimeOutError = true;
        currentState = final_st;
      } else {
        timeoutTimer++;
        // If a release is detected, disable the buttonhandler
        if (buttonHandler_releaseDetected()) {
          buttonHandler_disable();
          timeoutTimer = 0;
          regionNumber = buttonHandler_getRegionNumber();
          // Determines whether to go to the final_st or the init_st
          if (regionNumber == globals_getSequenceValue(index)) {
            // Determines whether to go to the final_st or the init_st
            if (index == globals_getSequenceIterationLength() - 1) {
              index = 0;
              currentState = final_st;
            } else {
              index++;
              currentState = init_st;
            }
          } else if (regionNumber != globals_getSequenceValue(index)) {
            isUserInputError = true;
            index = 0;
            currentState = final_st;
          }
        }
      }
    }
    break;

  case final_st:
    isComplete = true;
    // If the SM is enabled, stay in the final_st, otherwise go to the init_st
    if (isEnabled) {
      currentState = final_st;
    } else if (!isEnabled) {
      timeoutTimer = 0;
      currentState = init_st;
    }
    break;

  default:
    printf("This is an error message from verifySequence_tick() transition\n");
    break;
  }

  // Perform state action next.
  switch (currentState) {

  case init_st:
    break;

  case vs_wait_for_touch_st:
    break;

  case final_st:
    break;

  default:
    printf("This is an error message from verifySequence_tick() action\n");
    break;
  }
  vsdebugStatePrint();
}

// Used to Debug State machine
void vsdebugStatePrint() {
  static enum verifySequence_st_t previousState;
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

    case vs_wait_for_touch_st:
      printf(WAIT_FOR_TOUCH_ST_MSG);
      break;

    case final_st:
      printf(FINAL_ST_MSG);
      break;
    }
  }
}

// State machine will run when enabled.
void verifySequence_enable() { isEnabled = true; }

// This is part of the interlock. You disable the state-machine and then enable
// it again.
void verifySequence_disable() { isEnabled = false; }

// Used to detect if there has been a time-out error.
bool verifySequence_isTimeOutError() { return isUserTimeOutError; }

// Used to detect if the user tapped the incorrect sequence.
bool verifySequence_isUserInputError() { return isUserInputError; }

// Used to detect if the verifySequence state machine has finished verifying.
bool verifySequence_isComplete() { return isComplete; }
