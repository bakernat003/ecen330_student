#include "simonControl.h"
#include "buttonHandler.h"
#include "display.h"
#include "flashSequence.h"
#include "globals.h"
#include "simonDisplay.h"
#include "verifySequence.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define INTRO_SIMON_X 100
#define INTRO_SIMON_Y 80
#define INTRO_SIMON_TEXT_SIZE 4
#define INTRO_TOUCH_TO_PLAY_X 85
#define INTRO_TOUCH_TO_PLAY_Y 125
#define INTRO_TOUCH_TO_PLAY_TEXT_SIZE 2
#define INTRO_LENGTH_X 100
#define INTRO_LENGTH_Y 155
#define GAME_OVER_X 50
#define GAME_OVER_Y 80
#define CONGRATULATE_X 100
#define CONGRATULATE_Y 80
#define CONGRATULATE_TEXT_SIZE 5
#define TOUCH_TO_TRY_AGAIN_X 50
#define TOUCH_TO_TRY_AGAIN_Y 100
#define STRING_LENGTH 80
#define GAME_OVER_COUNTER_MAX 30
#define CONGRATULATE_COUNTER_MAX 10
#define CONGRATULATE_WAIT_FOR_TOUCH_COUNTER_MAX 50
#define BUTTON_0 0
#define BUTTON_1 1
#define BUTTON_2 2
#define BUTTON_3 3
#define RANDOM_INTEGER_MODULUS 4
#define INIT_ST_MSG "sc init state\n"
#define INTRO_MESSAGE_ST_MSG "sc intro message state\n"
#define INTRO_WAIT_FOR_TOUCH_ST_MSG "sc intro wait for touch state\n"
#define FLASH_SEQUENCE_ST_MSG "sc flash sequence state\n"
#define VERIFY_SEQUENCE_ST_MSG "sc verify sequence state\n"
#define GAME_OVER_MESSAGE_ST_MSG "sc game over message state\n"
#define GAME_OVER_ST_MSG "sc game over state\n"
#define CONGRATULATE_ST_MSG "sc congratulate state\n"
#define CONGRATULATE_DELAY_ST_MSG "sc congratulate delay state\n"
#define CONGRATULATE_WAIT_FOR_TOUCH_ST_MSG                                     \
  "sc congratulate wait for touch state\n"
#define MAX_ROUND_SEQUENCE_LENGTH 20
#define INITIAL_ROUND_SEQUENCE_LENGTH 4

static uint16_t gameOverCounter = 0;
static uint16_t congratulateCounter = 0;
static uint16_t congratulateWaitForTouchCounter = 0;
static uint16_t seedCounter = 0;
static bool isEnabled = false;
static bool isGameOver = false;
static uint16_t iterationSequenceLength;
static uint16_t roundSequenceLength;
static uint8_t sequence[MAX_ROUND_SEQUENCE_LENGTH];

void scdebugStatePrint();
void scDrawIntroMessage(bool isDraw);

// Creates a random sequence of integers between 0 and 3
void createRandomSequence() {
  srand(seedCounter);
  // Creates a random sequence of integers between 0 and 3
  for (uint8_t i = 0; i < MAX_ROUND_SEQUENCE_LENGTH; i++) {
    sequence[i] = rand() % RANDOM_INTEGER_MODULUS;
  }
}

// States for the controller state machine.
static enum simonControl_st_t {
  init_st,
  intro_message_st,
  intro_wait_for_touch_st,
  flash_sequence_st,
  verify_sequence_st,
  game_over_message_st,
  game_over_st,
  congratulate_st,
  congratulate_delay_st,
  congratulate_wait_for_touch_st
} currentState;

// Standard init function.
void simonControl_init() {
  roundSequenceLength = INITIAL_ROUND_SEQUENCE_LENGTH;
  currentState = init_st;
}

// Standard tick function.
void simonControl_tick() {
  // Perform state transition
  switch (currentState) {

  case init_st:
    seedCounter++;
    currentState = intro_message_st;
    break;

  case intro_message_st:
    seedCounter++;
    scDrawIntroMessage(true);
    currentState = intro_wait_for_touch_st;
    break;

  case intro_wait_for_touch_st:
    // If the display is touched, clear the old touch data, create a new random
    // sequence, and transition to flash_sequence_st
    if (display_isTouched()) {
      display_clearOldTouchData();
      scDrawIntroMessage(false);
      createRandomSequence();
      globals_setSequence(sequence, MAX_ROUND_SEQUENCE_LENGTH);
      iterationSequenceLength = 1;
      globals_setSequenceIterationLength(iterationSequenceLength);
      seedCounter++;
      currentState = flash_sequence_st;
    } else if (!display_isTouched()) {
      seedCounter++;
      currentState = intro_wait_for_touch_st;
    }
    break;

  case flash_sequence_st:
    // If the flash sequence is complete, disable the flash sequence state
    // machine, increase the length of the next iteration, and move to verify
    // sequence.
    if (flashSequence_isComplete()) {
      flashSequence_disable();
      iterationSequenceLength++;
      seedCounter++;
      currentState = verify_sequence_st;
    } else {
      seedCounter++;
      globals_setSequenceIterationLength(iterationSequenceLength);
    }
    break;

  case verify_sequence_st:
    // If the verifySequence SM is complete, disable the SM.
    if (verifySequence_isComplete()) {
      seedCounter++;
      verifySequence_disable();
      // If the game is over, go to the game_over_message_st. Otherwise, if the
      // end of the sequence has been reached, go to the congratulate state.
      // Otherwise, go back to flash sequence SM
      if (simonControl_isGameOver()) {
        currentState = game_over_message_st;
      } else if (iterationSequenceLength == (roundSequenceLength + 1)) {
        simonDisplay_eraseAllButtons();
        currentState = congratulate_st;
      } else {
        simonDisplay_eraseAllButtons();
        currentState = flash_sequence_st;
      }
    }
    seedCounter++;
    break;

  case game_over_message_st:
    createRandomSequence();
    globals_setSequence(sequence, MAX_ROUND_SEQUENCE_LENGTH);
    seedCounter++;
    currentState = game_over_st;
    break;

  case game_over_st:
    gameOverCounter++;
    // If the player doesn't touch the screen, display the iteration achieved in
    // the last sequence
    if (gameOverCounter >= GAME_OVER_COUNTER_MAX) {
      display_setCursor(GAME_OVER_X, GAME_OVER_Y);
      display_setTextColor(DISPLAY_BLACK);
      display_setTextSize(INTRO_TOUCH_TO_PLAY_TEXT_SIZE);
      char string[STRING_LENGTH];
      sprintf(string, "Last Sequence: %d / %d", iterationSequenceLength - 1,
              roundSequenceLength);
      display_print(string);
      iterationSequenceLength = 1;
      globals_setSequenceIterationLength(iterationSequenceLength);
      gameOverCounter = 0;
      seedCounter++;
      currentState = intro_message_st;
    }
    break;

  case congratulate_st:
    display_setCursor(CONGRATULATE_X, CONGRATULATE_Y);
    display_setTextColor(DISPLAY_WHITE);
    display_setTextSize(CONGRATULATE_TEXT_SIZE);
    display_print("Yay!");
    currentState = congratulate_delay_st;
    break;

  case congratulate_delay_st:
    // If the counter hasn't expired, stay in the congratulate_delay_st.
    // Otherwise, remove the congratulation message and show the Touch to Try
    // Again message
    if (congratulateCounter < CONGRATULATE_COUNTER_MAX) {
      congratulateCounter++;
      currentState = congratulate_delay_st;
    } else {
      display_setCursor(CONGRATULATE_X, CONGRATULATE_Y);
      display_setTextColor(DISPLAY_BLACK);
      display_setTextSize(CONGRATULATE_TEXT_SIZE);
      display_print("Yay!");
      display_setTextColor(DISPLAY_WHITE);
      display_setTextSize(INTRO_TOUCH_TO_PLAY_TEXT_SIZE);
      display_setCursor(TOUCH_TO_TRY_AGAIN_X, TOUCH_TO_TRY_AGAIN_Y);
      display_print("Touch to Try Again");
      congratulateCounter = 0;
      currentState = congratulate_wait_for_touch_st;
    }
    break;

  case congratulate_wait_for_touch_st:
    // If the timer expires, remove the message, create a new random sequence,
    // and move to the game_over_message_st
    if (congratulateWaitForTouchCounter >
        CONGRATULATE_WAIT_FOR_TOUCH_COUNTER_MAX) {
      display_setTextColor(DISPLAY_BLACK);
      display_setTextSize(INTRO_TOUCH_TO_PLAY_TEXT_SIZE);
      display_setCursor(TOUCH_TO_TRY_AGAIN_X, TOUCH_TO_TRY_AGAIN_Y);
      display_print("Touch to Try Again");
      createRandomSequence();
      globals_setSequence(sequence, MAX_ROUND_SEQUENCE_LENGTH);
      congratulateWaitForTouchCounter = 0;
      currentState = game_over_message_st;
    } else if (display_isTouched()) {
      display_clearOldTouchData();
      congratulateWaitForTouchCounter = 0;
      roundSequenceLength++;
      display_setTextColor(DISPLAY_BLACK);
      display_setTextSize(INTRO_TOUCH_TO_PLAY_TEXT_SIZE);
      display_setCursor(TOUCH_TO_TRY_AGAIN_X, TOUCH_TO_TRY_AGAIN_Y);
      display_print("Touch to Try Again");
      iterationSequenceLength = 1;
      globals_setSequenceIterationLength(iterationSequenceLength);
      createRandomSequence();
      globals_setSequence(sequence, MAX_ROUND_SEQUENCE_LENGTH);
      currentState = flash_sequence_st;
    } else {
      congratulateWaitForTouchCounter++;
    }
    break;

  default:
    printf("This is an error message from simonControl_tick() transition\n");
    break;
  }

  // Perform state action next.
  switch (currentState) {

  case init_st:
    createRandomSequence();
    // Set the provided test sequence.
    globals_setSequence(sequence, MAX_ROUND_SEQUENCE_LENGTH);
    // Start with the first element of the sequence.
    iterationSequenceLength = 1;
    globals_setSequenceIterationLength(iterationSequenceLength);
    break;

  case intro_message_st:
    break;

  case intro_wait_for_touch_st:
    break;

  case flash_sequence_st:
    flashSequence_enable();
    break;

  case verify_sequence_st:
    verifySequence_enable();
    simonDisplay_drawAllButtons();
    break;

  case game_over_message_st:
    simonDisplay_eraseAllButtons();
    display_setCursor(GAME_OVER_X, GAME_OVER_Y);
    display_setTextColor(DISPLAY_WHITE);
    display_setTextSize(INTRO_TOUCH_TO_PLAY_TEXT_SIZE);
    char string[STRING_LENGTH];
    sprintf(string, "Last Sequence: %d / %d", iterationSequenceLength - 1,
            roundSequenceLength);
    display_print(string);
    createRandomSequence();
    break;

  case game_over_st:
    break;

  case congratulate_st:
    break;

  case congratulate_delay_st:
    break;

  case congratulate_wait_for_touch_st:

    break;

  default:
    printf("This is an error message from simonControl_tick() action\n");
    break;
  }
  scdebugStatePrint();
}

// Used to Debug State machine
void scdebugStatePrint() {
  static enum simonControl_st_t previousState;
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

    case intro_message_st:
      printf(INTRO_MESSAGE_ST_MSG);
      break;

    case intro_wait_for_touch_st:
      printf(INTRO_WAIT_FOR_TOUCH_ST_MSG);
      break;

    case flash_sequence_st:
      printf(FLASH_SEQUENCE_ST_MSG);
      break;

    case verify_sequence_st:
      printf(VERIFY_SEQUENCE_ST_MSG);
      break;

    case game_over_message_st:
      printf(GAME_OVER_MESSAGE_ST_MSG);
      break;

    case game_over_st:
      printf(GAME_OVER_ST_MSG);
      break;

    case congratulate_st:
      printf(CONGRATULATE_ST_MSG);
      break;

    case congratulate_delay_st:
      printf(CONGRATULATE_DELAY_ST_MSG);
      break;

    case congratulate_wait_for_touch_st:
      printf(CONGRATULATE_WAIT_FOR_TOUCH_ST_MSG);
      break;
    }
  }
}

// Enables the control state machine.
void simonControl_enable() { isEnabled = true; }

// Disables the control state machine.
void simonControl_disable() { isEnabled = false; }

// If you want to stop the game after a mistake, check this function after each
// tick.
bool simonControl_isGameOver() {
  // If there is a timeout error or the user clicked the wrong button, then the
  // game is over
  if (verifySequence_isTimeOutError() || verifySequence_isUserInputError()) {
    isGameOver = true;
  }
}

// Use this to set the sequence length. This the sequence set by this function
// will only be recognized when the controller passes through its init state.
void simonControl_setSequenceLength(uint16_t length) {}

// Draws the intro message
void scDrawIntroMessage(bool isDraw) {
  // Either draws the message or erases it.
  if (isDraw) {
    display_setTextColor(DISPLAY_WHITE);
  } else {
    display_setTextColor(DISPLAY_BLACK);
  }
  display_setCursor(INTRO_SIMON_X, INTRO_SIMON_Y);
  display_setTextSize(INTRO_SIMON_TEXT_SIZE);
  display_print("SIMON");
  display_setCursor(INTRO_TOUCH_TO_PLAY_X, INTRO_TOUCH_TO_PLAY_Y);
  display_setTextSize(INTRO_TOUCH_TO_PLAY_TEXT_SIZE);
  display_print("Touch to play");
  display_setCursor(INTRO_LENGTH_X, INTRO_LENGTH_Y);
  display_setTextSize(INTRO_TOUCH_TO_PLAY_TEXT_SIZE);
  roundSequenceLength = INITIAL_ROUND_SEQUENCE_LENGTH;
  char str[STRING_LENGTH];
  sprintf(str, "Length: %d", roundSequenceLength);
  display_print(str);
}