#include "clockDisplay.h"
#include "display.h"
#include "utils.h"
#include <stdio.h>

#define NO_UPDATE 0
#define TEST_DELAY 100
#define TEST_DURATION 100
#define HALF 2
#define DOUBLE 2
#define INIT_HEIGHT 2.5
#define INIT_HOURS 12
#define INIT_MINS 59
#define INIT_SECS 59
#define CHAR_LEN 9
#define QUARTER 4
#define THIRD 3
#define FIFTH 5
#define SIXTH 6
#define LEFT_WIDTH 50
#define SIZE_6 20
#define SIZE_5 9
#define TOP_LEFT_SHIFT 25
#define TOP_RIGHT_SHIFT 1.5
#define BOTTOM_LEFT_SHIFT 75
#define BOTTOM_RIGHT_SHIFT 50
#define HEIGHT_MULT 4.0 / 5.0
#define MED_TOP_LEFT_SHIFT 40
#define MED_TOP_RIGHT_SHIFT 10
#define BOTTOM_MIDDLE_SHIFT 35
#define MED_SHIFT 15
#define LARGE_LEFT_SHIFT 60
#define TOP_MID_SHIFT 30
#define MID_SHIFT 20

int8_t hours, mins, secs;
char timeOld[CHAR_LEN];
char timeNew[CHAR_LEN];

void clockDisplay_drawTriangles();
void clockDisplay_advanceTimeOneMinute();
void clockDisplay_advanceTimeOneHour();
void clockDisplay_decrementTimeOneHour();
void clockDisplay_decrementTimeOneMinute();
void clockDisplay_decrementTimeOneSecond();

// Called only once - performs any necessary inits.
// Draws the triangles, initializes the display, and initializes timeNew and
// timeOld
void clockDisplay_init() {
  hours = INIT_HOURS;
  mins = INIT_MINS;
  secs = INIT_SECS;
  sprintf(timeNew, "%2hd:%02hd:%02hd", hours, mins, secs);
  sprintf(timeOld, "%2hd:%02hd:%02hd", hours, mins, secs);
  display_init();
  display_setTextColor(DISPLAY_GREEN);
  display_fillScreen(DISPLAY_BLACK);
  display_setTextSize(CLOCKDISPLAY_TEXT_SIZE);
  clockDisplay_drawTriangles();
}

// Updates the time display with latest time, making sure to update only those
// digits that have changed since the last update. if forceUpdateAll is true,
// update all digits.
void clockDisplay_updateTimeDisplay(bool forceUpdateAll) {
  // Don't update all the digits if forceUpdateAll is 0
  if (forceUpdateAll == 0) {
    display_setTextSize(CLOCKDISPLAY_TEXT_SIZE);
    char timeMod[CHAR_LEN];
    // Fills the timeMod array with blank spaces
    for (int i = 0; i < CHAR_LEN - 1; i++) {
      timeMod[i] = ' ';
    }
    timeMod[CHAR_LEN - 1] = 0;
    // Compares timeNew with timeOld. Redraw the digits that differ.
    for (int i = 0; i < CHAR_LEN - 1; ++i) {
      // If there is a digit that differs, copy the changed digit into the
      // correct location in the timeMod array
      if (timeNew[i] != timeOld[i]) {
        timeMod[i] = timeOld[i];
        display_setTextColor(DISPLAY_BLACK);
        // Sets the cursor at the appropriate position
        if (CLOCKDISPLAY_TEXT_SIZE == SIXTH) {
          display_setCursor(DISPLAY_WIDTH / SIZE_6,
                            DISPLAY_HEIGHT / INIT_HEIGHT);
        } else if (CLOCKDISPLAY_TEXT_SIZE == FIFTH) {
          display_setCursor(DISPLAY_WIDTH / SIZE_5,
                            DISPLAY_HEIGHT / INIT_HEIGHT);
        } else if (CLOCKDISPLAY_TEXT_SIZE == QUARTER) {
          display_setCursor(DISPLAY_WIDTH / FIFTH,
                            DISPLAY_HEIGHT / INIT_HEIGHT);
        } else if (CLOCKDISPLAY_TEXT_SIZE == THIRD) {
          display_setCursor(DISPLAY_WIDTH / QUARTER,
                            DISPLAY_HEIGHT / INIT_HEIGHT);
        }
        display_println(timeMod);
        timeMod[i] = timeNew[i];
        display_setTextColor(DISPLAY_GREEN);
        // Sets the cursor at the appropriate position
        if (CLOCKDISPLAY_TEXT_SIZE == SIXTH) {
          display_setCursor(DISPLAY_WIDTH / SIZE_6,
                            DISPLAY_HEIGHT / INIT_HEIGHT);
        } else if (CLOCKDISPLAY_TEXT_SIZE == FIFTH) {
          display_setCursor(DISPLAY_WIDTH / SIZE_5,
                            DISPLAY_HEIGHT / INIT_HEIGHT);
        } else if (CLOCKDISPLAY_TEXT_SIZE == QUARTER) {
          display_setCursor(DISPLAY_WIDTH / FIFTH,
                            DISPLAY_HEIGHT / INIT_HEIGHT);
        } else if (CLOCKDISPLAY_TEXT_SIZE == THIRD) {
          display_setCursor(DISPLAY_WIDTH / QUARTER,
                            DISPLAY_HEIGHT / INIT_HEIGHT);
        }
        display_println(timeMod);
        timeMod[i] = ' ';
        timeOld[i] = timeNew[i];
      }
    }
  } else {
    display_setTextColor(DISPLAY_BLACK);
    // Sets the cursor at the appropriate position
    if (CLOCKDISPLAY_TEXT_SIZE == SIXTH) {
      display_setCursor(DISPLAY_WIDTH / SIZE_6, DISPLAY_HEIGHT / INIT_HEIGHT);
    } else if (CLOCKDISPLAY_TEXT_SIZE == FIFTH) {
      display_setCursor(DISPLAY_WIDTH / SIZE_5, DISPLAY_HEIGHT / INIT_HEIGHT);
    } else if (CLOCKDISPLAY_TEXT_SIZE == QUARTER) {
      display_setCursor(DISPLAY_WIDTH / FIFTH, DISPLAY_HEIGHT / INIT_HEIGHT);
    } else if (CLOCKDISPLAY_TEXT_SIZE == THIRD) {
      display_setCursor(DISPLAY_WIDTH / QUARTER, DISPLAY_HEIGHT / INIT_HEIGHT);
    }
    display_println(timeOld);
    display_setTextColor(DISPLAY_GREEN);
    // Sets the cursor at the appropriate position
    if (CLOCKDISPLAY_TEXT_SIZE == SIXTH) {
      display_setCursor(DISPLAY_WIDTH / SIZE_6, DISPLAY_HEIGHT / INIT_HEIGHT);
    } else if (CLOCKDISPLAY_TEXT_SIZE == FIFTH) {
      display_setCursor(DISPLAY_WIDTH / SIZE_5, DISPLAY_HEIGHT / INIT_HEIGHT);
    } else if (CLOCKDISPLAY_TEXT_SIZE == QUARTER) {
      display_setCursor(DISPLAY_WIDTH / FIFTH, DISPLAY_HEIGHT / INIT_HEIGHT);
    } else if (CLOCKDISPLAY_TEXT_SIZE == THIRD) {
      display_setCursor(DISPLAY_WIDTH / QUARTER, DISPLAY_HEIGHT / INIT_HEIGHT);
    }
    display_println(timeNew);
    // Sets each value in timeOld equal to the corresponding value in timeNew
    for (int i = 0; i < CHAR_LEN - 1; i++) {
      timeOld[i] = timeNew[i];
    }
  }
}

// Reads the touched coordinates and performs the increment or decrement,
// depending upon the touched region.
void clockDisplay_performIncDec() {
  int16_t x, y;
  uint8_t z;
  display_getTouchedPoint(&x, &y, &z);
  // If the top left corner was touched, increment the hours.
  // If the top middle was touched, increment the minutes.
  // If the top right corner was touched, increment the seconds
  if ((x < DISPLAY_WIDTH / THIRD) && (y < DISPLAY_HEIGHT / HALF)) {
    clockDisplay_advanceTimeOneHour();
  } else if (((x > DISPLAY_WIDTH / THIRD) &&
              (x < DOUBLE * DISPLAY_WIDTH / THIRD)) &&
             (y < DISPLAY_HEIGHT / HALF)) {
    clockDisplay_advanceTimeOneMinute();
  } else if ((x > (DOUBLE * DISPLAY_WIDTH / THIRD)) &&
             (y < DISPLAY_HEIGHT / HALF)) {
    clockDisplay_advanceTimeOneSecond();
  }
  // If the bottom left corner was touched, decrement the hours.
  // If the bottom middle was touched, decrement the minutes.
  // If the bottom right corner was touched, decrement the seconds
  if ((x < DISPLAY_WIDTH / THIRD) && (y > DISPLAY_HEIGHT / HALF)) {
    clockDisplay_decrementTimeOneHour();
  } else if (((x > DISPLAY_WIDTH / THIRD) && (x < 2 * DISPLAY_WIDTH / THIRD)) &&
             (y > DISPLAY_HEIGHT / HALF)) {
    clockDisplay_decrementTimeOneMinute();
  } else if ((x > (2 * DISPLAY_WIDTH / THIRD)) && (y > DISPLAY_HEIGHT / HALF)) {
    clockDisplay_decrementTimeOneSecond();
  }
}

// Decrements the seconds variable and then updates the display
void clockDisplay_decrementTimeOneSecond() {
  secs--;
  // Rolls over the seconds
  if (secs == 0) {
    secs = INIT_SECS;
  }
  sprintf(timeNew, "%2hd:%02hd:%02hd", hours, mins, secs);
  clockDisplay_updateTimeDisplay(0);
}

// Decrements the minutes variable and then updates the display
void clockDisplay_decrementTimeOneMinute() {
  mins--;
  // Rolls over the minutes
  if (mins == 0) {
    mins = INIT_MINS;
  }
  sprintf(timeNew, "%2hd:%02hd:%02hd", hours, mins, secs);
  clockDisplay_updateTimeDisplay(NO_UPDATE);
}

// Decrements the hours variable and then updates the display
void clockDisplay_decrementTimeOneHour() {
  hours--;
  // Rolls over the hours
  if (hours == 0) {
    hours = INIT_HOURS;
  }
  sprintf(timeNew, "%2hd:%02hd:%02hd", hours, mins, secs);
  clockDisplay_updateTimeDisplay(NO_UPDATE);
}

// Advances the time forward by 1 hour and update the display
void clockDisplay_advanceTimeOneHour() {
  hours++;
  // Rolls over the hours
  if (hours == INIT_HOURS + 1) {
    hours = 1;
  }
  sprintf(timeNew, "%2hd:%02hd:%02hd", hours, mins, secs);
  clockDisplay_updateTimeDisplay(NO_UPDATE);
}

// Advances the time forward by 1 minute and update the display
void clockDisplay_advanceTimeOneMinute() {
  mins++;
  // Rolls over the minutes
  if (mins == INIT_MINS + 1) {
    mins = 0;
    hours++;
  }
  // Rolls over the hours
  if (hours == INIT_HOURS + 1) {
    hours = 1;
  }
  sprintf(timeNew, "%2hd:%02hd:%02hd", hours, mins, secs);
  clockDisplay_updateTimeDisplay(NO_UPDATE);
}

// Advances the time forward by 1 second and update the display.
void clockDisplay_advanceTimeOneSecond() {
  secs++;
  // Increases the minutes variable if the seconds variable rolls over
  if (secs == INIT_SECS + 1) {
    secs = 0;
    mins++;
    sprintf(timeNew, "%2hd:%02hd:%02hd", hours, mins, secs);
    clockDisplay_updateTimeDisplay(NO_UPDATE);
  }
  // Increases the hours variable if the minutes variable rolls over
  if (mins == INIT_MINS + 1) {
    mins = 0;
    hours++;
  }
  // Rolls over the hours variable
  if (hours == INIT_HOURS + 1) {
    hours = 1;
  }
  sprintf(timeNew, "%2hd:%02hd:%02hd", hours, mins, secs);
  clockDisplay_updateTimeDisplay(NO_UPDATE);
}

// Run a test of clock-display functions.
void clockDisplay_runTest() {
  clockDisplay_init();
  // Increases hours
  for (hours = 0; hours <= INIT_HOURS; hours++) {
    sprintf(timeNew, "%2hd:%02hd:%02hd", hours, mins, secs);
    clockDisplay_updateTimeDisplay(NO_UPDATE);
    utils_msDelay(TEST_DELAY);
  }
  // Decreases hours
  for (hours = INIT_HOURS; hours > 0; hours--) {
    sprintf(timeNew, "%2hd:%02hd:%02hd", hours, mins, secs);
    clockDisplay_updateTimeDisplay(NO_UPDATE);
    utils_msDelay(TEST_DELAY);
  }
  hours = INIT_HOURS;
  sprintf(timeNew, "%2hd:%02hd:%02hd", hours, mins, secs);
  clockDisplay_updateTimeDisplay(1);
  // Increases minutes
  for (mins = 0; mins <= INIT_MINS; mins++) {
    sprintf(timeNew, "%2hd:%02hd:%02hd", hours, mins, secs);
    clockDisplay_updateTimeDisplay(NO_UPDATE);
    utils_msDelay(TEST_DELAY);
  }
  // Decreases minutes
  for (mins = INIT_MINS; mins > 0; mins--) {
    sprintf(timeNew, "%2hd:%02hd:%02hd", hours, mins, secs);
    clockDisplay_updateTimeDisplay(NO_UPDATE);
    utils_msDelay(TEST_DELAY);
  }
  mins = INIT_MINS;
  sprintf(timeNew, "%2hd:%02hd:%02hd", hours, mins, secs);
  clockDisplay_updateTimeDisplay(1);
  // Increases seconds
  for (secs = 0; secs <= INIT_SECS; secs++) {
    sprintf(timeNew, "%2hd:%02hd:%02hd", hours, mins, secs);
    clockDisplay_updateTimeDisplay(NO_UPDATE);
    utils_msDelay(TEST_DELAY);
  }
  // Decreases seconds
  for (secs = INIT_SECS; secs > 0; secs--) {
    sprintf(timeNew, "%2hd:%02hd:%02hd", hours, mins, secs);
    clockDisplay_updateTimeDisplay(NO_UPDATE);
    utils_msDelay(TEST_DELAY);
  }
  secs = INIT_SECS;
  sprintf(timeNew, "%2hd:%02hd:%02hd", hours, mins, secs);
  clockDisplay_updateTimeDisplay(NO_UPDATE);
  // Run the test for 10 seconds
  for (int i = 0; i < TEST_DURATION; i++) {
    clockDisplay_advanceTimeOneSecond();
    utils_msDelay(TEST_DELAY);
  }
}

// Draws the triangles, taking the text size into account
void clockDisplay_drawTriangles() {
  // Draws the triagnles if CLOCKDISPLAY_TEXT_SIZE is 6
  if (CLOCKDISPLAY_TEXT_SIZE == SIXTH) {
    display_setCursor(DISPLAY_WIDTH / SIZE_6, DISPLAY_HEIGHT / INIT_HEIGHT);
    display_println(timeNew);
    display_fillTriangle((DISPLAY_WIDTH / QUARTER) - LEFT_WIDTH,
                         DISPLAY_HEIGHT / THIRD, (DISPLAY_WIDTH / QUARTER),
                         DISPLAY_HEIGHT / THIRD,
                         (DISPLAY_WIDTH / QUARTER) - TOP_LEFT_SHIFT,
                         DISPLAY_HEIGHT / FIFTH, DISPLAY_GREEN);
    display_fillTriangle(
        (DISPLAY_WIDTH / HALF) - TOP_LEFT_SHIFT, DISPLAY_HEIGHT / THIRD,
        (DISPLAY_WIDTH / HALF) + TOP_LEFT_SHIFT, DISPLAY_HEIGHT / THIRD,
        (DISPLAY_WIDTH / HALF), DISPLAY_HEIGHT / FIFTH, DISPLAY_GREEN);
    display_fillTriangle((DISPLAY_WIDTH / TOP_RIGHT_SHIFT) + TOP_LEFT_SHIFT,
                         DISPLAY_HEIGHT / THIRD,
                         (DISPLAY_WIDTH / TOP_RIGHT_SHIFT) + BOTTOM_LEFT_SHIFT,
                         DISPLAY_HEIGHT / THIRD,
                         (DISPLAY_WIDTH / TOP_RIGHT_SHIFT) + BOTTOM_RIGHT_SHIFT,
                         DISPLAY_HEIGHT / FIFTH, DISPLAY_GREEN);
    display_fillTriangle(
        (DISPLAY_WIDTH / QUARTER) - LEFT_WIDTH, (HALF * DISPLAY_HEIGHT) / THIRD,
        (DISPLAY_WIDTH / QUARTER), (HALF * DISPLAY_HEIGHT) / THIRD,
        (DISPLAY_WIDTH / QUARTER) - TOP_LEFT_SHIFT,
        (DISPLAY_HEIGHT) * (HEIGHT_MULT), DISPLAY_GREEN);
    display_fillTriangle((DISPLAY_WIDTH / HALF) - TOP_LEFT_SHIFT,
                         (HALF * DISPLAY_HEIGHT) / THIRD,
                         (DISPLAY_WIDTH / HALF) + TOP_LEFT_SHIFT,
                         (HALF * DISPLAY_HEIGHT) / THIRD,
                         (DISPLAY_WIDTH / HALF),
                         (DISPLAY_HEIGHT) * (HEIGHT_MULT), DISPLAY_GREEN);
    display_fillTriangle((DISPLAY_WIDTH / TOP_RIGHT_SHIFT) + TOP_LEFT_SHIFT,
                         (HALF * DISPLAY_HEIGHT) / THIRD,
                         (DISPLAY_WIDTH / TOP_RIGHT_SHIFT) + BOTTOM_LEFT_SHIFT,
                         (HALF * DISPLAY_HEIGHT) / THIRD,
                         (DISPLAY_WIDTH / TOP_RIGHT_SHIFT) + BOTTOM_RIGHT_SHIFT,
                         (DISPLAY_HEIGHT) * (HEIGHT_MULT), DISPLAY_GREEN);
  }
  // Draws the triagnles if CLOCKDISPLAY_TEXT_SIZE is 5
  else if (CLOCKDISPLAY_TEXT_SIZE == FIFTH) {
    display_setCursor(DISPLAY_WIDTH / SIZE_5, DISPLAY_HEIGHT / INIT_HEIGHT);
    display_println(timeNew);
    display_fillTriangle(
        (DISPLAY_WIDTH / QUARTER) - MED_TOP_LEFT_SHIFT, DISPLAY_HEIGHT / THIRD,
        (DISPLAY_WIDTH / QUARTER) + MED_TOP_RIGHT_SHIFT, DISPLAY_HEIGHT / THIRD,
        (DISPLAY_WIDTH / QUARTER) - MED_SHIFT, DISPLAY_HEIGHT / FIFTH,
        DISPLAY_GREEN);
    display_fillTriangle(
        (DISPLAY_WIDTH / HALF) - BOTTOM_MIDDLE_SHIFT, DISPLAY_HEIGHT / THIRD,
        (DISPLAY_WIDTH / HALF) + MED_SHIFT, DISPLAY_HEIGHT / THIRD,
        (DISPLAY_WIDTH / HALF) - MED_TOP_RIGHT_SHIFT, DISPLAY_HEIGHT / FIFTH,
        DISPLAY_GREEN);
    display_fillTriangle(
        (DISPLAY_WIDTH / TOP_RIGHT_SHIFT) + MED_SHIFT, DISPLAY_HEIGHT / THIRD,
        (DISPLAY_WIDTH / TOP_RIGHT_SHIFT) + LARGE_LEFT_SHIFT,
        DISPLAY_HEIGHT / THIRD,
        (DISPLAY_WIDTH / TOP_RIGHT_SHIFT) + BOTTOM_MIDDLE_SHIFT,
        DISPLAY_HEIGHT / FIFTH, DISPLAY_GREEN);
    display_fillTriangle(
        (DISPLAY_WIDTH / QUARTER) - MED_TOP_LEFT_SHIFT,
        ((HALF * DISPLAY_HEIGHT) / THIRD) - MED_TOP_RIGHT_SHIFT,
        (DISPLAY_WIDTH / QUARTER) + MED_TOP_RIGHT_SHIFT,
        ((HALF * DISPLAY_HEIGHT) / THIRD) - MED_TOP_RIGHT_SHIFT,
        (DISPLAY_WIDTH / QUARTER) - MED_SHIFT,
        (DISPLAY_HEIGHT) * (HEIGHT_MULT)-MED_TOP_RIGHT_SHIFT, DISPLAY_GREEN);
    display_fillTriangle(
        (DISPLAY_WIDTH / HALF) - BOTTOM_MIDDLE_SHIFT,
        ((HALF * DISPLAY_HEIGHT) / THIRD) - MED_TOP_RIGHT_SHIFT,
        (DISPLAY_WIDTH / HALF) + MED_SHIFT,
        ((HALF * DISPLAY_HEIGHT) / THIRD) - MED_TOP_RIGHT_SHIFT,
        (DISPLAY_WIDTH / HALF) - MED_TOP_RIGHT_SHIFT,
        (DISPLAY_HEIGHT) * (HEIGHT_MULT)-MED_TOP_RIGHT_SHIFT, DISPLAY_GREEN);
    display_fillTriangle(
        (DISPLAY_WIDTH / TOP_RIGHT_SHIFT) + MED_SHIFT,
        ((HALF * DISPLAY_HEIGHT) / THIRD) - MED_TOP_RIGHT_SHIFT,
        (DISPLAY_WIDTH / TOP_RIGHT_SHIFT) + LARGE_LEFT_SHIFT,
        ((HALF * DISPLAY_HEIGHT) / THIRD) - MED_TOP_RIGHT_SHIFT,
        (DISPLAY_WIDTH / TOP_RIGHT_SHIFT) + BOTTOM_MIDDLE_SHIFT,
        (DISPLAY_HEIGHT) * (HEIGHT_MULT)-MED_TOP_RIGHT_SHIFT, DISPLAY_GREEN);
  }
  // Draws the triagnles if CLOCKDISPLAY_TEXT_SIZE is 4
  else if (CLOCKDISPLAY_TEXT_SIZE == QUARTER) {
    display_setCursor(DISPLAY_WIDTH / FIFTH, DISPLAY_HEIGHT / INIT_HEIGHT);
    display_println(timeNew);
    display_fillTriangle(
        (DISPLAY_WIDTH / QUARTER) - MED_SHIFT, DISPLAY_HEIGHT / THIRD,
        (DISPLAY_WIDTH / QUARTER) + BOTTOM_MIDDLE_SHIFT, DISPLAY_HEIGHT / THIRD,
        (DISPLAY_WIDTH / QUARTER) + MED_TOP_RIGHT_SHIFT, DISPLAY_HEIGHT / FIFTH,
        DISPLAY_GREEN);
    display_fillTriangle(
        (DISPLAY_WIDTH / HALF) - TOP_MID_SHIFT, DISPLAY_HEIGHT / THIRD,
        (DISPLAY_WIDTH / HALF) + MID_SHIFT, DISPLAY_HEIGHT / THIRD,
        (DISPLAY_WIDTH / HALF) - FIFTH, DISPLAY_HEIGHT / FIFTH, DISPLAY_GREEN);
    display_fillTriangle(
        (DISPLAY_WIDTH / TOP_RIGHT_SHIFT) - FIFTH, DISPLAY_HEIGHT / THIRD,
        (DISPLAY_WIDTH / TOP_RIGHT_SHIFT) + MED_TOP_LEFT_SHIFT,
        DISPLAY_HEIGHT / THIRD, (DISPLAY_WIDTH / TOP_RIGHT_SHIFT) + MID_SHIFT,
        DISPLAY_HEIGHT / FIFTH, DISPLAY_GREEN);
    display_fillTriangle((DISPLAY_WIDTH / QUARTER) - MED_SHIFT,
                         ((HALF * DISPLAY_HEIGHT) / THIRD) - MID_SHIFT,
                         (DISPLAY_WIDTH / QUARTER) + BOTTOM_MIDDLE_SHIFT,
                         ((HALF * DISPLAY_HEIGHT) / THIRD) - MID_SHIFT,
                         (DISPLAY_WIDTH / QUARTER) + MED_TOP_RIGHT_SHIFT,
                         (DISPLAY_HEIGHT) * (HEIGHT_MULT)-MID_SHIFT,
                         DISPLAY_GREEN);
    display_fillTriangle((DISPLAY_WIDTH / HALF) - TOP_MID_SHIFT,
                         ((HALF * DISPLAY_HEIGHT) / THIRD) - MID_SHIFT,
                         (DISPLAY_WIDTH / HALF) + MID_SHIFT,
                         ((HALF * DISPLAY_HEIGHT) / THIRD) - MID_SHIFT,
                         (DISPLAY_WIDTH / HALF) - FIFTH,
                         (DISPLAY_HEIGHT) * (HEIGHT_MULT)-MID_SHIFT,
                         DISPLAY_GREEN);
    display_fillTriangle((DISPLAY_WIDTH / TOP_RIGHT_SHIFT) - FIFTH,
                         ((HALF * DISPLAY_HEIGHT) / THIRD) - MID_SHIFT,
                         (DISPLAY_WIDTH / TOP_RIGHT_SHIFT) + MED_TOP_LEFT_SHIFT,
                         ((HALF * DISPLAY_HEIGHT) / THIRD) - MID_SHIFT,
                         (DISPLAY_WIDTH / TOP_RIGHT_SHIFT) + MID_SHIFT,
                         (DISPLAY_HEIGHT) * (HEIGHT_MULT)-MID_SHIFT,
                         DISPLAY_GREEN);
  }
  // Draws the triagnles if CLOCKDISPLAY_TEXT_SIZE is 3
  else if (CLOCKDISPLAY_TEXT_SIZE == THIRD) {
    display_setCursor(DISPLAY_WIDTH / QUARTER, DISPLAY_HEIGHT / INIT_HEIGHT);
    display_println(timeNew);
    display_fillTriangle(
        (DISPLAY_WIDTH / QUARTER) - FIFTH, DISPLAY_HEIGHT / THIRD,
        (DISPLAY_WIDTH / QUARTER) + BOTTOM_MIDDLE_SHIFT, DISPLAY_HEIGHT / THIRD,
        (DISPLAY_WIDTH / QUARTER) + MED_SHIFT, DISPLAY_HEIGHT / FIFTH,
        DISPLAY_GREEN);
    display_fillTriangle(
        (DISPLAY_WIDTH / HALF) - TOP_MID_SHIFT, DISPLAY_HEIGHT / THIRD,
        (DISPLAY_WIDTH / HALF) + MED_SHIFT, DISPLAY_HEIGHT / THIRD,
        (DISPLAY_WIDTH / HALF) - MED_TOP_RIGHT_SHIFT, DISPLAY_HEIGHT / FIFTH,
        DISPLAY_GREEN);
    display_fillTriangle(
        (DISPLAY_WIDTH / TOP_RIGHT_SHIFT) - TOP_LEFT_SHIFT,
        DISPLAY_HEIGHT / THIRD,
        (DISPLAY_WIDTH / TOP_RIGHT_SHIFT) + MED_TOP_RIGHT_SHIFT,
        DISPLAY_HEIGHT / THIRD, (DISPLAY_WIDTH / TOP_RIGHT_SHIFT) - FIFTH,
        DISPLAY_HEIGHT / FIFTH, DISPLAY_GREEN);
    display_fillTriangle((DISPLAY_WIDTH / QUARTER) - FIFTH,
                         ((HALF * DISPLAY_HEIGHT) / THIRD) - TOP_MID_SHIFT,
                         (DISPLAY_WIDTH / QUARTER) + BOTTOM_MIDDLE_SHIFT,
                         ((HALF * DISPLAY_HEIGHT) / THIRD) - TOP_MID_SHIFT,
                         (DISPLAY_WIDTH / QUARTER) + MED_SHIFT,
                         (DISPLAY_HEIGHT) * (HEIGHT_MULT)-TOP_MID_SHIFT,
                         DISPLAY_GREEN);
    display_fillTriangle((DISPLAY_WIDTH / HALF) - TOP_MID_SHIFT,
                         ((HALF * DISPLAY_HEIGHT) / THIRD) - TOP_MID_SHIFT,
                         (DISPLAY_WIDTH / HALF) + MED_SHIFT,
                         ((HALF * DISPLAY_HEIGHT) / THIRD) - TOP_MID_SHIFT,
                         (DISPLAY_WIDTH / HALF) - MED_TOP_RIGHT_SHIFT,
                         (DISPLAY_HEIGHT) * (HEIGHT_MULT)-TOP_MID_SHIFT,
                         DISPLAY_GREEN);
    display_fillTriangle(
        (DISPLAY_WIDTH / TOP_RIGHT_SHIFT) - TOP_LEFT_SHIFT,
        ((HALF * DISPLAY_HEIGHT) / THIRD) - TOP_MID_SHIFT,
        (DISPLAY_WIDTH / TOP_RIGHT_SHIFT) + MED_TOP_RIGHT_SHIFT,
        ((HALF * DISPLAY_HEIGHT) / THIRD) - TOP_MID_SHIFT,
        (DISPLAY_WIDTH / TOP_RIGHT_SHIFT) - FIFTH,
        (DISPLAY_HEIGHT) * (HEIGHT_MULT)-TOP_MID_SHIFT, DISPLAY_GREEN);
  }
}
