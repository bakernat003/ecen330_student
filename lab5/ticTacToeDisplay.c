#include "ticTacToeDisplay.h"
#include "buttons.h"
#include "display.h"
#include "minimax.h"
#include "stdio.h"
#include "switches.h"
#include "utils.h"

#define CONTINUE_LOOP 1
#define ACD_DELAY 50
#define THIRD 3
#define DOUBLE 2
#define HALF 2
#define SIXTH 6.0
#define RIGHTMOST 5.0 / 6.0
#define BOTTOMMOST 5.0 / 6.0
#define TOP 0
#define MID 1
#define BOT 2
#define LFT 0
#define RGT 2
#define CIRCLE_RADIUS 30
#define LFT_TOP_LEFT_X_POS 30
#define MID_TOP_LEFT_X_POS 140
#define RGT_TOP_LEFT_X_POS 250
#define TOP_TOP_LEFT_Y_POS 20
#define MID_TOP_LEFT_Y_POS 100
#define BOT_TOP_LEFT_Y_POS 180
#define X_SIZE 40
#define SWITCHES_MASK 1
#define BUTTONS_MASK 2

static int16_t x, y;
static uint8_t z;
static uint8_t row;
static uint8_t col;

// Inits the tic-tac-toe display, draws the lines that form the board.
void ticTacToeDisplay_init() {
  display_init();
  switches_init();
  buttons_init();
  ticTacToeDisplay_drawBoardLines();
}

// Draws an X at the specified row and column.
// erase == true means to erase the X by redrawing it as background. erase ==
// false, draw the X as foreground.
void ticTacToeDisplay_drawX(uint8_t row, uint8_t column, bool erase) {
  uint16_t topLeftXPos;
  uint16_t topLeftYPos;
  uint16_t bottomRightXPos;
  uint16_t bottomRightYPos;
  uint16_t topRightXPos;
  uint16_t topRightYPos;
  uint16_t bottomLeftXPos;
  uint16_t bottomLeftYPos;
  // Determines line coordinates to draw an X in the correct column
  if (column == LFT) {
    topLeftXPos = LFT_TOP_LEFT_X_POS;
    bottomLeftXPos = topLeftXPos;
    topRightXPos = topLeftXPos + X_SIZE;
    bottomRightXPos = topRightXPos;
  } else if (column == MID) {
    topLeftXPos = MID_TOP_LEFT_X_POS;
    bottomLeftXPos = topLeftXPos;
    topRightXPos = topLeftXPos + X_SIZE;
    bottomRightXPos = topRightXPos;
  } else if (column == RGT) {
    topLeftXPos = RGT_TOP_LEFT_X_POS;
    bottomLeftXPos = topLeftXPos;
    topRightXPos = topLeftXPos + X_SIZE;
    bottomRightXPos = topRightXPos;
  }
  // Determines line coordinates to draw an X in the correct row
  if (row == TOP) {
    topLeftYPos = TOP_TOP_LEFT_Y_POS;
    topRightYPos = topLeftYPos;
    bottomLeftYPos = topLeftYPos + X_SIZE;
    bottomRightYPos = bottomLeftYPos;
  } else if (row == MID) {
    topLeftYPos = MID_TOP_LEFT_Y_POS;
    topRightYPos = topLeftYPos;
    bottomLeftYPos = topLeftYPos + X_SIZE;
    bottomRightYPos = bottomLeftYPos;
  } else if (row == BOT) {
    topLeftYPos = BOT_TOP_LEFT_Y_POS;
    topRightYPos = topLeftYPos;
    bottomLeftYPos = topLeftYPos + X_SIZE;
    bottomRightYPos = bottomLeftYPos;
  }
  // Draws the X given the line coordinates. Draws it in black if erase == true.
  // Draws it in white if erase == false
  if (erase == true) {
    display_drawLine(topLeftXPos, topLeftYPos, bottomRightXPos, bottomRightYPos,
                     DISPLAY_BLACK);
    display_drawLine(topRightXPos, topRightYPos, bottomLeftXPos, bottomLeftYPos,
                     DISPLAY_BLACK);
  } else {
    display_drawLine(topLeftXPos, topLeftYPos, bottomRightXPos, bottomRightYPos,
                     DISPLAY_WHITE);
    display_drawLine(topRightXPos, topRightYPos, bottomLeftXPos, bottomLeftYPos,
                     DISPLAY_WHITE);
  }
}

// Draws an O at the specified row and column.
// erase == true means to erase the X by redrawing it as background. erase ==
// false, draw the X as foreground.
void ticTacToeDisplay_drawO(uint8_t row, uint8_t column, bool erase) {
  uint16_t xPos;
  uint16_t yPos;
  // Determines line coordinates to draw an O in the correct column
  if (column == LFT) {
    xPos = DISPLAY_WIDTH / SIXTH;
  } else if (column == MID) {
    xPos = DISPLAY_WIDTH / HALF;
  } else if (column == RGT) {
    xPos = (RIGHTMOST)*DISPLAY_WIDTH;
  }
  // Determines line coordinates to draw an O in the correct row
  if (row == TOP) {
    yPos = DISPLAY_HEIGHT / SIXTH;
  } else if (row == MID) {
    yPos = DISPLAY_HEIGHT / HALF;
  } else if (row == BOT) {
    yPos = (BOTTOMMOST)*DISPLAY_HEIGHT;
  }
  // Draws the X given the line coordinates. Draws it in black if erase == true.
  // Draws it in white if erase == false
  if (erase == true) {
    display_drawCircle(xPos, yPos, CIRCLE_RADIUS, DISPLAY_BLACK);
  } else {
    display_drawCircle(xPos, yPos, CIRCLE_RADIUS, DISPLAY_WHITE);
  }
}

// After a touch has been detected and after the proper delay, this sets the row
// and column arguments according to where the user touched the board.
void ticTacToeDisplay_touchScreenComputeBoardRowColumn(uint8_t *row,
                                                       uint8_t *column) {
  display_getTouchedPoint(&x, &y, &z);
  // Sets the column to the appropriate column given the x coordinate of the
  // touched point
  if ((x < DISPLAY_WIDTH / THIRD)) {
    *column = LFT;
  } else if ((x > DISPLAY_WIDTH / THIRD) &&
             (x < DOUBLE * (DISPLAY_WIDTH / THIRD))) {
    *column = MID;
  } else {
    *column = RGT;
  }
  // Sets the row to the appropriate row given the y coordinate of the touched
  // point
  if ((y < DISPLAY_HEIGHT / THIRD)) {
    *row = TOP;
  } else if ((y > DISPLAY_HEIGHT / THIRD) &&
             (y < DOUBLE * (DISPLAY_HEIGHT / THIRD))) {
    *row = MID;
  } else {
    *row = BOT;
  }
}

// This will draw the four board lines.
void ticTacToeDisplay_drawBoardLines() {
  display_drawLine(DISPLAY_WIDTH / THIRD, TOP, DISPLAY_WIDTH / THIRD,
                   DISPLAY_HEIGHT, DISPLAY_WHITE);
  display_drawLine(DOUBLE * DISPLAY_WIDTH / THIRD, TOP,
                   DOUBLE * DISPLAY_WIDTH / THIRD, DISPLAY_HEIGHT,
                   DISPLAY_WHITE);
  display_drawLine(LFT, DISPLAY_HEIGHT / THIRD, DISPLAY_WIDTH,
                   DISPLAY_HEIGHT / THIRD, DISPLAY_WHITE);
  display_drawLine(LFT, DOUBLE * DISPLAY_HEIGHT / THIRD, DISPLAY_WIDTH,
                   DOUBLE * DISPLAY_HEIGHT / THIRD, DISPLAY_WHITE);
}

// Runs a test of the display. Does the following.
// Draws the board. Each time you touch one of the screen areas, the screen will
// paint an X or an O, depending on whether switch 0 (SW0) is slid up (O) or
// down (X). When BTN0 is pushed, the screen is cleared. The test terminates
// when BTN1 is pushed.
void ticTacToeDisplay_runTest() {
  ticTacToeDisplay_init();
  // Continuously run the loop until the BTN1 is pressed
  while (CONTINUE_LOOP) {
    // When the display is touched, clear the old data, delay while values
    // settle, get the touched point, compute the row and column, and draw an X
    // or O depending on the switch
    if (display_isTouched()) {
      display_clearOldTouchData();
      utils_msDelay(ACD_DELAY);
      display_getTouchedPoint(&x, &y, &z);
      ticTacToeDisplay_touchScreenComputeBoardRowColumn(&row, &col);
      uint16_t switches = switches_read();
      // Draw an X or O depending on the switch
      if ((switches & SWITCHES_MASK) == SWITCHES_MASK) {
        ticTacToeDisplay_drawO(row, col, false);
      } else {
        ticTacToeDisplay_drawX(row, col, false);
      }
    }
    // If BTN0 is pressed, clear the screen
    if ((buttons_read() & SWITCHES_MASK) == SWITCHES_MASK) {
      ticTacToeDisplay_init();
    }
    // If BTN1 is pressed, terminate the test
    if ((buttons_read() & BUTTONS_MASK) == BUTTONS_MASK) {
      printf("DONE\n");
      break;
    }
  }
}