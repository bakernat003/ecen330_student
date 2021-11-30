#include "flashSequence.h"
#include "globals.h"
#include "simonDisplay.h"
#include <stdbool.h>
#include <stdio.h>

#define INIT_ST_MSG "fs init state\n"
#define DRAW_SQUARE_ST_MSG "fs draw square state\n"
#define SHOW_PAUSE_ST_MSG "fs show pause state\n"
#define ERASE_SQUARE_ST_MSG "fs erase square state\n"
#define FINAL_ST_MSG "fs final state\n"
#define SHOW_PAUSE_TIMER_MAX 2

void fsdebugStatePrint();

static bool isEnabled = false;
static bool isComplete = false;

static uint16_t showPauseTimer = 0;

static uint16_t index = 0;

// States for the controller state machine.
static enum flashSequence_st_t {
  init_st, // Start here, transition out of this state on the first tick.
  draw_square_st,
  show_pause_st,
  erase_square_st,
  final_st
} currentState;

// Standard init function.
void flashSequence_init() { currentState = init_st; }

// Standard tick function.
void flashSequence_tick() {
  // Perform state transition
  switch (currentState) {

  case init_st:
    // If SM is enabled, go to draw_square_st
    if (isEnabled) {
      isComplete = false;
      currentState = draw_square_st;
    } else {
      currentState = init_st;
    }
    break;

  case draw_square_st:
    // If the SM isn't inabled, go back to the init-st. Otherwise draw the
    // square.
    if (!isEnabled) {
      currentState = init_st;
    } else {
      simonDisplay_drawSquare(globals_getSequenceValue(index), false);
      currentState = show_pause_st;
    }
    break;

  case show_pause_st:
    // If the SM isn't inabled, go back to the init-st. Provides a delay.
    if (!isEnabled) {
      currentState = init_st;
    } else if (showPauseTimer < SHOW_PAUSE_TIMER_MAX) {
      showPauseTimer++;
      currentState = show_pause_st;
    } else {
      showPauseTimer = 0;
      currentState = erase_square_st;
    }
    break;

  case erase_square_st:
    // If the SM isn't inabled, go back to the init-st.
    if (!isEnabled) {
      currentState = init_st;
    }
    simonDisplay_drawSquare(globals_getSequenceValue(index), true);
    // If the index is less than the sequence length, go the the draw_square-st
    if (index < globals_getSequenceIterationLength() - 1) {
      index++;
      currentState = draw_square_st;
    } else if (index = globals_getSequenceIterationLength()) {
      index = 0;
      currentState = final_st;
    }
    break;

  case final_st:
    isComplete = true;
    // If the SM isn't inabled, go back to the init-st. Otherwise, stay in the
    // final_st
    if (isEnabled) {
      currentState = final_st;
    } else if (!isEnabled) {
      currentState = init_st;
    }
    break;

  default:
    printf("This is an error message from flashSequence_tick() transition\n");
    break;
  }

  // Perform state action next.
  switch (currentState) {

  case init_st:
    break;

  case draw_square_st:
    break;

  case show_pause_st:
    break;

  case erase_square_st:
    break;

  case final_st:

    break;

  default:
    printf("This is an error message from flashSequence_tick() action\n");
    break;
  }
  fsdebugStatePrint();
}

// Used to Debug State machine
void fsdebugStatePrint() {
  static enum flashSequence_st_t previousState;
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

    case draw_square_st:
      printf(DRAW_SQUARE_ST_MSG);
      break;

    case show_pause_st:
      printf(SHOW_PAUSE_ST_MSG);
      break;

    case erase_square_st:
      printf(ERASE_SQUARE_ST_MSG);
      break;

    case final_st:
      printf(FINAL_ST_MSG);
      break;
    }
  }
}

// Turns on the state machine. Part of the interlock.
void flashSequence_enable() { isEnabled = true; }

// Turns off the state machine. Part of the interlock.
void flashSequence_disable() { isEnabled = false; }

// Other state machines can call this to determine if this state machine is
// finished.
bool flashSequence_isComplete() { return isComplete; }
