#include "ticTacToeControl.h"
#include "buttons.h"
#include "display.h"
#include "intervalTimer.h"
#include "minimax.h"
#include "ticTacToeDisplay.h"
#include <stdbool.h>
#include <stdio.h>

#define INIT_ST_MSG "init state\n"
#define DRAW_INSTRUCTIONS_ST_MSG "draw instructions state\n"
#define INSTRUCTIONS_ST_MSG "instructions state \n"
#define START_ST_MSG "start state\n"
#define ADC_ST_MSG "adc state\n"
#define DRAW_HUMAN_MOVE_ST_MSG "draw human move state\n"
#define DRAW_COM_MOVE_ST_MSG "draw com move state\n"
#define WAIT_HUMAN_MOVE_ST_MSG "wait human move state\n"
#define CLEAR_SCREEN_ST_MSG "clear screen state\n"
#define CLEAR_INTRO_ST_MSG "clear intro state\n"
#define IC_MAX 50
#define SC_MAX 60
#define RESET_MASK 1
#define INTRO_X1 25
#define INTRO_Y1 80
#define INTRO_X2 100
#define INTRO_Y2 100
#define INTRO_X3 25
#define INTRO_Y3 120
#define INTRO_X4 70
#define INTRO_Y4 140
#define TEXT_SIZE 2

static minimax_board_t board;
static bool player_is_x;
static uint8_t row, column;
static int16_t instructCounter;
static int16_t startCounter;

void debugStatePrint();

// States for the controller state machine.
enum clockControl_st_t {
  init_st,
  draw_instructions_st,
  instructions_st,
  clear_intro_st,
  start_st,
  adc_st,
  draw_human_move_st,
  draw_com_move_st,
  wait_human_move_st,
  clear_screen_st
} currentState;

// Tick the tic-tac-toe conroller state machine
void ticTacToeControl_tick() {
  // Perform state transition
  switch (currentState) {
  case init_st:
    currentState = draw_instructions_st;
    break;

  case draw_instructions_st:
    currentState = instructions_st;
    break;

  case instructions_st:
    // If the counter is less than the max, stay in the instructions state.
    // Otherwise, initialize the display and the board and move to the start
    // state
    if (instructCounter < IC_MAX) {
      currentState = instructions_st;
    } else {
      minimax_initBoard(&board);
      currentState = clear_intro_st;
    }
    break;

  case clear_intro_st:
    ticTacToeDisplay_drawBoardLines();
    currentState = start_st;
    break;

  case start_st:
    // If the counter is less than the max, stay in the start state. If the
    // display is touched, set player_is_x equal to true and transition to the
    // adc state. If the counter reaches max, then the computer goes first as X
    if ((startCounter < SC_MAX) && !display_isTouched()) {
      currentState = start_st;
    } else if (display_isTouched()) {
      display_clearOldTouchData();
      player_is_x = false;
      currentState = adc_st;
    } else if (startCounter >= SC_MAX) {
      player_is_x = true;
      currentState = draw_com_move_st;
    }
    break;

  case adc_st:
    currentState = draw_human_move_st;
    break;

  case draw_human_move_st:
    currentState = draw_com_move_st;
    break;

  case draw_com_move_st:
    currentState = wait_human_move_st;
    break;

  case wait_human_move_st:
    // If BTN0 is pressed, clear the X and Os
    if (((buttons_read() & RESET_MASK) == RESET_MASK) && !display_isTouched()) {
      currentState = clear_screen_st;
    }
    // If the display isn't touched, stay in the wait human move state. If the
    // display is touched, move to the adc state.
    if (!display_isTouched() && (buttons_read() & RESET_MASK) != RESET_MASK) {
      currentState = wait_human_move_st;
    } else if (display_isTouched() &&
               (buttons_read() & RESET_MASK) != RESET_MASK) {
      display_clearOldTouchData();
      currentState = adc_st;
    }
    break;

  case clear_screen_st:
    minimax_initBoard(&board);
    startCounter = 0;
    currentState = start_st;
    break;

  default:
    printf("This is an error message from tick() state transition\n");
    break;
  }

  // Perform state action next.
  switch (currentState) {
  case init_st:
    break;

  case draw_instructions_st:
    display_setCursor(INTRO_X1, INTRO_Y1);
    display_setTextSize(TEXT_SIZE);
    display_println("Touch board to play X\n");
    display_setCursor(INTRO_X2, INTRO_Y2);
    display_println("--or--\n");
    display_setCursor(INTRO_X3, INTRO_Y3);
    display_println("wait for the computer\n");
    display_setCursor(INTRO_X4, INTRO_Y4);
    display_println("and play O\n");
    break;

  case instructions_st:
    instructCounter++;
    break;

  case clear_intro_st:
    display_setTextColor(DISPLAY_BLACK);
    display_setCursor(INTRO_X1, INTRO_Y1);
    display_setTextSize(TEXT_SIZE);
    display_println("Touch board to play X\n");
    display_setCursor(INTRO_X2, INTRO_Y2);
    display_println("--or--\n");
    display_setCursor(INTRO_X3, INTRO_Y3);
    display_println("wait for the computer\n");
    display_setCursor(INTRO_X4, INTRO_Y4);
    display_println("and play O\n");
    break;

  case start_st:
    startCounter++;
    break;

  case adc_st:
    break;

  case draw_human_move_st:
    ticTacToeDisplay_touchScreenComputeBoardRowColumn(&row, &column);
    // If the human is an X, draw an X. If the human is an O, draw an O
    if (!player_is_x) {
      ticTacToeDisplay_drawX(row, column, false);
      board.squares[row][column] = MINIMAX_X_SQUARE;
    } else {
      ticTacToeDisplay_drawO(row, column, false);
      board.squares[row][column] = MINIMAX_O_SQUARE;
    }
    break;

  case draw_com_move_st:
    minimax_computeNextMove(&board, player_is_x, &row, &column);
    // If the computer is an X, draw an X. If the computer is an O, draw an O
    if (player_is_x) {
      ticTacToeDisplay_drawX(row, column, false);
      board.squares[row][column] = MINIMAX_X_SQUARE;
    } else {
      ticTacToeDisplay_drawO(row, column, false);
      board.squares[row][column] = MINIMAX_O_SQUARE;
    }
    break;

  case wait_human_move_st:
    break;

  case clear_screen_st:
    // Clears the screen. If there is an X, redraw an X in black. If there is an
    // O, redraw an O in black
    for (int16_t i = 0; i < MINIMAX_BOARD_ROWS; i++) {
      // Clears the screen. If there is an X, redraw an X in black. If there is
      // an O, redraw an O in black
      for (int16_t j = 0; j < MINIMAX_BOARD_COLUMNS; j++) {
        // Clears the screen. If there is an X, redraw an X in black. If there
        // is an O, redraw an O in black
        if (board.squares[i][j] == MINIMAX_X_SQUARE) {
          ticTacToeDisplay_drawX(i, j, true);
        } else if (board.squares[i][j] == MINIMAX_O_SQUARE) {
          ticTacToeDisplay_drawO(i, j, true);
        }
      }
    }
    break;

  default:
    printf("This is an error message from tick() state action\n");
    break;
  }
  debugStatePrint();
}

// Initialize the tic-tac-toe conroller state machine
void ticTacToeControl_init() {
  startCounter = 0;
  instructCounter = 0;
  display_init();
  buttons_init();
  currentState = init_st;
}

// Used to debug the state machine
void debugStatePrint() {
  static enum clockControl_st_t previousState;
  static bool firstPass = true;
  // Only print the message if:
  // 1. This the first pass and the value for previousState is unknown.
  // 2. previousState != currentState - this prevents reprinting the same state
  // name over and over.
  if (previousState != currentState || firstPass) {
    firstPass = false; // previousState will be defined, firstPass is false.
    previousState = currentState;
    // keep track of the last state that you were in.
    // This prints messages based upon the state that
    // you were in.
    switch (currentState) {
    case init_st:
      printf(INIT_ST_MSG);
      break;

    case draw_instructions_st:
      printf(DRAW_INSTRUCTIONS_ST_MSG);
      break;

    case instructions_st:
      printf(INSTRUCTIONS_ST_MSG);
      break;

    case clear_intro_st:
      printf(CLEAR_INTRO_ST_MSG);
      break;

    case start_st:
      printf(START_ST_MSG);
      break;

    case adc_st:
      printf(ADC_ST_MSG);
      break;

    case draw_human_move_st:
      printf(DRAW_HUMAN_MOVE_ST_MSG);
      break;

    case draw_com_move_st:
      printf(DRAW_COM_MOVE_ST_MSG);
      break;

    case wait_human_move_st:
      printf(WAIT_HUMAN_MOVE_ST_MSG);
      break;

    case clear_screen_st:
      printf(CLEAR_SCREEN_ST_MSG);
      break;
    }
  }
}