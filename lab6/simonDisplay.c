#include "simonDisplay.h"
#include "display.h"
#include <stdio.h>
#include <utils.h>

#define ACD_DELAY 50
#define HALF 2

#define BUTTON_LEFT_COLUMN_X 40
#define BUTTON_RIGHT_COLUMN_X 220
#define BUTTON_TOP_ROW_Y 30
#define BUTTON_BOTTOM_ROW_Y 150

#define SQUARE_LEFT_COLUMN_X 30
#define SQUARE_RIGHT_COLUMN_X 170
#define SQUARE_TOP_ROW_Y 0
#define SQUARE_BOTTOM_ROW_Y 120

#define SQUARE_SHIFT 15
#define SQUARE_RIGHT_SHIFT 25

// Given coordinates from the touch pad, computes the region number.
// The entire touch-screen is divided into 4 rectangular regions, numbered 0
// - 3. Each region will be drawn with a different color. Colored buttons remind
// the user which square is associated with each color. When you press
// a region, computeRegionNumber returns the region number that is used
// by the other routines.
/*
|----------|----------|
|          |          |
|    0     |     1    |
|  (RED)   | (YELLOW) |
-----------------------
|          |          |
|     2    |    3     |
|  (BLUE)  |  (GREEN) |
-----------------------
*/
int8_t simonDisplay_computeRegionNumber(int16_t x, int16_t y) {
  int8_t region;
  // Computes the region number
  if ((x < DISPLAY_WIDTH / HALF) && (y < DISPLAY_HEIGHT / HALF)) {
    region = SIMON_DISPLAY_REGION_0;
  } else if ((x > DISPLAY_WIDTH / HALF) && (y < DISPLAY_HEIGHT / HALF)) {
    region = SIMON_DISPLAY_REGION_1;
  } else if ((x < DISPLAY_WIDTH / HALF) && (y > DISPLAY_HEIGHT / HALF)) {
    region = SIMON_DISPLAY_REGION_2;
  } else if ((x > DISPLAY_WIDTH / HALF) && (y > DISPLAY_HEIGHT / HALF)) {
    region = SIMON_DISPLAY_REGION_3;
  }
  return region;
}

// Draws a colored "button" that the user can touch.
// The colored button is centered in the region but does not fill the region.
// If erase argument is true, draws the button as black background to erase it.
void simonDisplay_drawButton(uint8_t regionNumber, bool erase) {
  uint8_t lftColX = BUTTON_LEFT_COLUMN_X;
  uint8_t rgtColX = BUTTON_RIGHT_COLUMN_X;
  uint8_t topRowY = BUTTON_TOP_ROW_Y;
  uint8_t botRowY = BUTTON_BOTTOM_ROW_Y;
  // Either draw or erase the button
  if (erase == SIMON_DISPLAY_DRAW) {
    // Draw the button
    if (regionNumber == SIMON_DISPLAY_REGION_0) {
      display_fillRect(lftColX, topRowY, SIMON_DISPLAY_BUTTON_WIDTH,
                       SIMON_DISPLAY_BUTTON_HEIGHT, DISPLAY_RED);
    } else if (regionNumber == SIMON_DISPLAY_REGION_1) {
      display_fillRect(rgtColX, topRowY, SIMON_DISPLAY_BUTTON_WIDTH,
                       SIMON_DISPLAY_BUTTON_HEIGHT, DISPLAY_YELLOW);
    } else if (regionNumber == SIMON_DISPLAY_REGION_2) {
      display_fillRect(lftColX, botRowY, SIMON_DISPLAY_BUTTON_WIDTH,
                       SIMON_DISPLAY_BUTTON_HEIGHT, DISPLAY_BLUE);
    } else if (regionNumber == SIMON_DISPLAY_REGION_3) {
      display_fillRect(rgtColX, botRowY, SIMON_DISPLAY_BUTTON_WIDTH,
                       SIMON_DISPLAY_BUTTON_HEIGHT, DISPLAY_GREEN);
    }
  } else if (erase == SIMON_DISPLAY_ERASE) {
    // Erase the button
    if (regionNumber == SIMON_DISPLAY_REGION_0) {
      display_fillRect(lftColX, topRowY, SIMON_DISPLAY_BUTTON_WIDTH,
                       SIMON_DISPLAY_BUTTON_HEIGHT, DISPLAY_BLACK);
    } else if (regionNumber == SIMON_DISPLAY_REGION_1) {
      display_fillRect(rgtColX, topRowY, SIMON_DISPLAY_BUTTON_WIDTH,
                       SIMON_DISPLAY_BUTTON_HEIGHT, DISPLAY_BLACK);
    } else if (regionNumber == SIMON_DISPLAY_REGION_2) {
      display_fillRect(lftColX, botRowY, SIMON_DISPLAY_BUTTON_WIDTH,
                       SIMON_DISPLAY_BUTTON_HEIGHT, DISPLAY_BLACK);
    } else if (regionNumber == SIMON_DISPLAY_REGION_3) {
      display_fillRect(rgtColX, botRowY, SIMON_DISPLAY_BUTTON_WIDTH,
                       SIMON_DISPLAY_BUTTON_HEIGHT, DISPLAY_BLACK);
    }
  }
}

// Convenience function that draws all of the buttons.
void simonDisplay_drawAllButtons() {
  simonDisplay_drawButton(SIMON_DISPLAY_REGION_0, SIMON_DISPLAY_DRAW);
  simonDisplay_drawButton(SIMON_DISPLAY_REGION_1, SIMON_DISPLAY_DRAW);
  simonDisplay_drawButton(SIMON_DISPLAY_REGION_2, SIMON_DISPLAY_DRAW);
  simonDisplay_drawButton(SIMON_DISPLAY_REGION_3, SIMON_DISPLAY_DRAW);
}

// Convenience function that erases all of the buttons.
void simonDisplay_eraseAllButtons() {
  simonDisplay_drawButton(SIMON_DISPLAY_REGION_0, SIMON_DISPLAY_ERASE);
  simonDisplay_drawButton(SIMON_DISPLAY_REGION_1, SIMON_DISPLAY_ERASE);
  simonDisplay_drawButton(SIMON_DISPLAY_REGION_2, SIMON_DISPLAY_ERASE);
  simonDisplay_drawButton(SIMON_DISPLAY_REGION_3, SIMON_DISPLAY_ERASE);
}

// Draws a bigger square that completely fills the region.
// If the erase argument is true, it draws the square as black background to
// "erase" it.
void simonDisplay_drawSquare(uint8_t regionNo, bool erase) {
  uint8_t lftColX = SQUARE_LEFT_COLUMN_X;
  uint8_t rgtColX = SQUARE_RIGHT_COLUMN_X;
  uint8_t topRowY = SQUARE_TOP_ROW_Y;
  uint8_t botRowY = SQUARE_BOTTOM_ROW_Y;
  // Either draw or erase the square
  if (erase == SIMON_DISPLAY_DRAW) {
    // draw the square
    if (regionNo == SIMON_DISPLAY_REGION_0) {
      display_fillRect(lftColX, topRowY,
                       SIMON_DISPLAY_SQUARE_WIDTH - SQUARE_SHIFT,
                       SIMON_DISPLAY_SQUARE_HEIGHT - SQUARE_SHIFT, DISPLAY_RED);
    } else if (regionNo == SIMON_DISPLAY_REGION_1) {
      display_fillRect(rgtColX + SQUARE_RIGHT_SHIFT, topRowY,
                       SIMON_DISPLAY_SQUARE_WIDTH - SQUARE_SHIFT,
                       SIMON_DISPLAY_SQUARE_HEIGHT - SQUARE_SHIFT,
                       DISPLAY_YELLOW);
    } else if (regionNo == SIMON_DISPLAY_REGION_2) {
      display_fillRect(
          lftColX, botRowY, SIMON_DISPLAY_SQUARE_WIDTH - SQUARE_SHIFT,
          SIMON_DISPLAY_SQUARE_HEIGHT - SQUARE_SHIFT, DISPLAY_BLUE);
    } else if (regionNo == SIMON_DISPLAY_REGION_3) {
      display_fillRect(rgtColX + SQUARE_RIGHT_SHIFT, botRowY,
                       SIMON_DISPLAY_SQUARE_WIDTH - SQUARE_SHIFT,
                       SIMON_DISPLAY_SQUARE_HEIGHT - SQUARE_SHIFT,
                       DISPLAY_GREEN);
    }
  } else if (erase == SIMON_DISPLAY_ERASE) {
    // erase the square
    if (regionNo == SIMON_DISPLAY_REGION_0) {
      display_fillRect(
          lftColX, topRowY, SIMON_DISPLAY_SQUARE_WIDTH - SQUARE_SHIFT,
          SIMON_DISPLAY_SQUARE_HEIGHT - SQUARE_SHIFT, DISPLAY_BLACK);
    } else if (regionNo == SIMON_DISPLAY_REGION_1) {
      display_fillRect(rgtColX + SQUARE_RIGHT_SHIFT, topRowY,
                       SIMON_DISPLAY_SQUARE_WIDTH - SQUARE_SHIFT,
                       SIMON_DISPLAY_SQUARE_HEIGHT - SQUARE_SHIFT,
                       DISPLAY_BLACK);
    } else if (regionNo == SIMON_DISPLAY_REGION_2) {
      display_fillRect(
          lftColX, botRowY, SIMON_DISPLAY_SQUARE_WIDTH - SQUARE_SHIFT,
          SIMON_DISPLAY_SQUARE_HEIGHT - SQUARE_SHIFT, DISPLAY_BLACK);
    } else if (regionNo == SIMON_DISPLAY_REGION_3) {
      display_fillRect(rgtColX + SQUARE_RIGHT_SHIFT, botRowY,
                       SIMON_DISPLAY_SQUARE_WIDTH - SQUARE_SHIFT,
                       SIMON_DISPLAY_SQUARE_HEIGHT - SQUARE_SHIFT,
                       DISPLAY_BLACK);
    }
  }
}

// Runs a brief demonstration of how buttons can be pressed and squares lit up
// to implement the user interface of the Simon game. The routine will continue
// to run until the touchCount has been reached, e.g., the user has touched the
// pad touchCount times.
void simonDisplay_runTest(uint16_t touchCount) {
  int16_t x, y;
  int8_t z;
  int8_t count = 0;
  uint8_t region;

  display_init();
  display_fillScreen(DISPLAY_BLACK);
  simonDisplay_drawAllButtons();
  // Tests the buttons
  while (count <= touchCount) {
    // If the display is touched, erase draw the button
    if (display_isTouched()) {
      display_clearOldTouchData();
      utils_msDelay(ACD_DELAY);
      display_getTouchedPoint(&x, &y, &z);
      region = simonDisplay_computeRegionNumber(x, y);
      simonDisplay_drawButton(region, SIMON_DISPLAY_ERASE);
      simonDisplay_drawSquare(region, SIMON_DISPLAY_DRAW);
      count++;
    }
  }
}