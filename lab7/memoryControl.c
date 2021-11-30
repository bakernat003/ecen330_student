#include "memoryControl.h"
#include "memoryDisplay.h"
#include "display.h"
#include "intervalTimer.h"
#include <stdbool.h>
#include <stdio.h>

#define INIT_ST_MSG "init state\n"
#define DISPLAY_INTRO_MESSAGE_ST_MSG "display intro message state\n"
#define DRAW_GRID_ST_MSG "draw grid state\n"
#define WAIT_FOR_FIRST_TOUCH_ST_MSG "wait for first touch state\n"
#define READ_FIRST_TOUCH_ST_MSG "read first touch state\n"
#define WAIT_FOR_SECOND_TOUCH_ST_MSG "wait for second touch state\n"
#define READ_SECOND_TOUCH_ST_MSG "read second touch state\n"
#define VERIFY_TOUCHES_ST_MSG "verify touches state\n"
#define MATCH_MADE_ST_MSG "match made state\n"
#define NOT_A_MATCH_ST_MSG "not a match state\n"
#define CONGRATS_ST_MSG "congrats state\n"

#define FEEDBACK_DELAY_MAX 30
#define GAMEOVER_DELAY_MAX 60

static uint16_t feedbackDelayCounter = 0;
static uint16_t gameOverCounter = 0;

static uint8_t firstChoiceRow;
static uint8_t firstChoiceCol;

static uint8_t secondChoiceRow;
static uint8_t secondChoiceCol;

static uint8_t firstChoiceVal;
static uint8_t secondChoiceVal;

static uint8_t scoreGrid[GRID_SIZE][GRID_SIZE];

static uint8_t matchesAttempted;

static uint16_t seedCounter = 0;

// States for the controller state machine.
enum memoryControl_st_t {
    init_st, // Start here, transition out of this state on the first tick.
    display_intro_message_st,
    drawGrid,
    wait_for_first_touch_st,
    read_first_touch_st,
    wait_for_second_touch_st,
    read_second_touch_st,
    verify_touches_st,
    match_made_st,
    not_a_match_st,
    congrats_st
} currentState;

// Initializes the State Machine
void memoryControl_init() {
  //memoryDisplay_init();
  seedCounter++;
  currentState = init_st;
}


void memoryControl_tick() {
  // Perform state transition
  switch (currentState) {

  case init_st:
    seedCounter++;
    drawIntroMessage(false);
    matchesAttempted = 0;
    initScoreGrid();
    initGridCount();
    currentState = display_intro_message_st;
  break;

  case display_intro_message_st:
    seedCounter++;
    if(!display_isTouched()) {
        currentState = display_intro_message_st;
    } else {
        display_clearOldTouchData();
        drawIntroMessage(true);
        srand(seedCounter);
        initGrid();
        currentState = drawGrid;
    }
  break;

    case drawGrid:
        seedCounter++;
        drawDownGrid();
        currentState = wait_for_first_touch_st;
    break;

    case wait_for_first_touch_st:
        seedCounter++;
        if(!display_isTouched()) {
            currentState = wait_for_first_touch_st;
        } else {
            display_clearOldTouchData();
            currentState = read_first_touch_st;
        }
    break;

    case read_first_touch_st:
        seedCounter++;
        touchScreenComputeBoardRowColumn(&firstChoiceRow, &firstChoiceCol);
        if (alreadyMatched(firstChoiceRow, firstChoiceCol)) {
            currentState = wait_for_first_touch_st;
        } else {
            drawDownCard(firstChoiceRow, firstChoiceCol, true);
            drawUpCard(firstChoiceRow, firstChoiceCol, false);
            firstChoiceVal = readCard(firstChoiceRow, firstChoiceCol);
            currentState = wait_for_second_touch_st;
        }
    break;

    case wait_for_second_touch_st:
        seedCounter++;
        if(!display_isTouched()) {
            currentState = wait_for_second_touch_st;
        } else {
            display_clearOldTouchData();
            currentState = read_second_touch_st;
        }
    break;

    case read_second_touch_st:
        seedCounter++;
        touchScreenComputeBoardRowColumn(&secondChoiceRow, &secondChoiceCol);
        if (alreadyMatched(secondChoiceRow, secondChoiceCol)) {
            currentState = wait_for_second_touch_st;
        } else {
            //Check to see if they touched the same card
            if ((firstChoiceRow == secondChoiceRow) && (firstChoiceCol == secondChoiceCol)) {
                currentState = wait_for_second_touch_st;
            } else {
                drawDownCard(secondChoiceRow, secondChoiceCol, true);
                drawUpCard(secondChoiceRow, secondChoiceCol, false);
                secondChoiceVal = readCard(secondChoiceRow, secondChoiceCol);
                currentState = verify_touches_st;
            }
        }
    break;

    case verify_touches_st:
        seedCounter++;
        //Checks to see if a correct match was made
        if (firstChoiceVal == secondChoiceVal) {
            choiceFeedback(firstChoiceRow, firstChoiceCol, true, false); // Draws the first choice feedback rectangle
            choiceFeedback(secondChoiceRow, secondChoiceCol, true, false); // Draws the second choice feedback rectangle
            scoreGrid[firstChoiceRow][firstChoiceCol] = 1;
            scoreGrid[secondChoiceRow][secondChoiceCol] = 1;
            matchesAttempted++;
            currentState = match_made_st;
        } else {
            choiceFeedback(firstChoiceRow, firstChoiceCol, false, false); // Draws the first choice feedback rectangle
            choiceFeedback(secondChoiceRow, secondChoiceCol, false, false); // Draws the second choice feedback rectangle
            matchesAttempted++;
            currentState = not_a_match_st;
        }
    break;

    case match_made_st:
        seedCounter++;
        if (feedbackDelayCounter < FEEDBACK_DELAY_MAX) {
            feedbackDelayCounter++;
            currentState = match_made_st;
        } else {
            feedbackDelayCounter = 0;
            choiceFeedback(firstChoiceRow, firstChoiceCol, true, true); // Erases the first choice feedback rectangle
            choiceFeedback(secondChoiceRow, secondChoiceCol, true, true); // Erases the second choice feedback rectangle
            drawUpCard(firstChoiceRow, firstChoiceCol, true);
            drawUpCard(secondChoiceRow, secondChoiceCol, true);
            if (isGameOver()) {
                drawGameOverMessage(false);
                currentState = congrats_st;
            } else {
                currentState = wait_for_first_touch_st;
            }
        }
    break;

    case not_a_match_st:
        seedCounter++;
        if (feedbackDelayCounter < FEEDBACK_DELAY_MAX) {
            feedbackDelayCounter++;
            currentState = not_a_match_st;
        } else {
            feedbackDelayCounter = 0;
            choiceFeedback(firstChoiceRow, firstChoiceCol, false, true); // Erases the first choice feedback rectangle
            choiceFeedback(secondChoiceRow, secondChoiceCol, false, true); // Erases the second choice feedback rectangle
            drawDownCard(firstChoiceRow, firstChoiceCol, false);
            drawDownCard(secondChoiceRow, secondChoiceCol, false);
            currentState = wait_for_first_touch_st;
        }
    break;

    case congrats_st:
        seedCounter++;
        if (gameOverCounter < GAMEOVER_DELAY_MAX) {
            gameOverCounter++;
            currentState = congrats_st;
        } else {
            gameOverCounter = 0;
            drawGameOverMessage(true);
            currentState = init_st;
        }
    break;

  default:
    printf("This is an error message from memoryControl transition\n");
    break;
  }

  // Perform state action next.
  switch (currentState) {
  case init_st:
    break;

  case display_intro_message_st:
  break;

  case drawGrid:
  break;

  case wait_for_first_touch_st:
  break;

  case read_first_touch_st:
  break;

  case wait_for_second_touch_st:
  break;

  case read_second_touch_st:
  break;

  case verify_touches_st:
  break;

  case match_made_st:
  break;

  case not_a_match_st:
  break;
  
  case congrats_st:
  break;

  default:
    printf("This is an error message from memoryControl action\n");
    break;
  }
  debugStatePrint();
}

//Used to debug the state machine
void debugStatePrint() {
  static enum memoryControl_st_t previousState;
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

    case display_intro_message_st:
        printf(DISPLAY_INTRO_MESSAGE_ST_MSG);
    break;

    case drawGrid:
        printf(DRAW_GRID_ST_MSG);
    break;

    case wait_for_first_touch_st:
        printf(WAIT_FOR_FIRST_TOUCH_ST_MSG);
    break;

    case read_first_touch_st:
        printf(READ_FIRST_TOUCH_ST_MSG);
    break;

    case wait_for_second_touch_st:
        printf(WAIT_FOR_SECOND_TOUCH_ST_MSG);
    break;

    case read_second_touch_st:
        printf(READ_SECOND_TOUCH_ST_MSG);
    break;

    case verify_touches_st:
        printf(VERIFY_TOUCHES_ST_MSG);
    break;

    case match_made_st:
        printf(MATCH_MADE_ST_MSG);
    break;

    case not_a_match_st:
        printf(NOT_A_MATCH_ST_MSG);
    break;

    case congrats_st:
        printf(CONGRATS_ST_MSG);
    break;
    }
  }
}

void drawIntroMessage(bool erase) {
    if (erase == false) {
        display_setTextColor(DISPLAY_WHITE);
    } else {
        display_setTextColor(DISPLAY_BLACK);
    }
    display_setCursor(60,80);
    display_setTextSize(3);
    display_println("MEMORY GAME\n");
    display_setCursor(10,130);
    display_setTextSize(2);
    display_println("Touch the screen to begin\n");
}

void drawGameOverMessage(bool erase) {
    if (erase == false) {
        display_setTextColor(DISPLAY_WHITE);
    } else {
        display_setTextColor(DISPLAY_BLACK);
    }
    display_setCursor(120,80);
    display_setTextSize(3);
    display_println("YAY!\n");
    display_setCursor(30,130);
    display_setTextSize(2);
    char str[100];
    sprintf(str, "Matches Attempted: %d \n", matchesAttempted);
    display_print(str);
}

void initScoreGrid() {
    for (uint8_t i = 0; i < GRID_SIZE; i++) {
        for (uint8_t j = 0; j < GRID_SIZE; j++) {
            scoreGrid[i][j] = 0;
        }
    }
}

bool alreadyMatched(uint8_t row, uint8_t column) {
    if(scoreGrid[row][column] == 1) {
        return true;
    } else {
        return false;
    }
}

bool isGameOver() {
    for (uint8_t i = 0; i < GRID_SIZE; i++) {
        for (uint8_t j = 0; j < GRID_SIZE; j++) {
            if (scoreGrid[i][j] == 0) {
                return false;
            }
        }
    }
    return true;
}

