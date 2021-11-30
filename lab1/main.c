/*
This software is provided for student assignment use in the Department of
Electrical and Computer Engineering, Brigham Young University, Utah, USA.

Users agree to not re-host, or redistribute the software, in source or binary
form, to other persons or other institutions. Users may modify and use the
source code for personal or educational use.

For questions, contact Brad Hutchings or Jeff Goeders, https://ece.byu.edu/
*/

// Print out "hello world" on both the console and the LCD screen.

#include "display.h"

#define TEXT_SIZE 2
#define CURSOR_X 0
#define CURSOR_Y 0

#include <stdio.h>
int main() {
  display_init(); // Must init all of the software and underlying hardware for
                  // LCD.
  display_fillScreen(DISPLAY_BLACK);     // Blank the screen.
  display_setCursor(CURSOR_X, CURSOR_Y); // The upper left of the LCD screen.
  display_setTextColor(DISPLAY_RED);     // Make the text red.
  display_setTextSize(TEXT_SIZE);        // Make the text a little larger.
  display_drawLine(0, 0, 700, 500, DISPLAY_GREEN);
  display_drawCircle(70, 120, 25, DISPLAY_RED);
  display_fillCircle(250, 120, 25, DISPLAY_RED);
  display_setCursor(display_height(), 0);
  display_drawLine(0, display_height(), display_width(), 0, DISPLAY_GREEN);
  display_fillTriangle(165, 100, 135, 40, 195, 40, DISPLAY_YELLOW);
  display_drawTriangle(165, 130, 135, 190, 195, 190, DISPLAY_YELLOW);
  return 0;
}

// This function must be defined but can be left empty for now.
// You will use this function in a later lab.
// It is called in the timer interrupt service routine (see interrupts.c in
// supportFiles).
void isr_function() {
  // Empty for now.
}