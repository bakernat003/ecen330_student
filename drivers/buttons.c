#include "buttons.h"
#include "display.h"
#include "stdio.h"
#include "xil_io.h"
#include "xparameters.h"
#include <stdbool.h>

#define GPIO_DATA_OFFSET 0x00
#define GPIO_TRI_OFFSET 0x04
#define INIT_TRI 0x0000000F
#define LEDS_OFF 0x00000000

#define TEXT_SIZE 2
#define CURSOR_X 0
#define CURSOR_Y 0
#define TEXT_Y 50
#define TEXT_OFFSET 15
#define SCREEN_PERCENT 3.0 / 4.0
#define QUARTER 4
#define HALF 2

int32_t buttons_readGpioRegister(int32_t offset) {
  return Xil_In32(XPAR_PUSH_BUTTONS_BASEADDR + offset);
}

void buttons_writeGpioRegister(int32_t offset, int32_t value) {
  Xil_Out32(XPAR_PUSH_BUTTONS_BASEADDR + offset, value);
}

// Initializes the button driver software and hardware. Returns one of the
// defined status values (above).
int32_t buttons_init() {
  display_init();
  display_fillScreen(DISPLAY_BLACK);     // Blank the screen.
  display_setCursor(CURSOR_X, CURSOR_Y); // The upper left of the LCD screen.
  display_setTextColor(DISPLAY_WHITE);   // Make the text white
  display_setTextSize(TEXT_SIZE);        // Make the text a little larger.

  buttons_writeGpioRegister(GPIO_TRI_OFFSET, INIT_TRI);
  int32_t gpioTri = buttons_readGpioRegister(GPIO_TRI_OFFSET);
  if (gpioTri == INIT_TRI) {
    return BUTTONS_INIT_STATUS_OK;
  } else {
    return BUTTONS_INIT_STATUS_FAIL;
  }
}

// Returns the current value of all 4 buttons as the lower 4 bits of the
// returned value. bit3 = BTN3, bit2 = BTN2, bit1 = BTN1, bit0 = BTN0.
int32_t buttons_read() { return buttons_readGpioRegister(GPIO_DATA_OFFSET); }

// Runs a test of the buttons. As you push the buttons, graphics and messages
// will be written to the LCD panel. The test will until all 4 pushbuttons are
// simultaneously pressed.
void buttons_runTest() {
  buttons_init();
  bool btn0drawn = 0;
  bool btn1drawn = 0;
  bool btn2drawn = 0;
  bool btn3drawn = 0;

  // The program runs until all four buttons are pressed ad the same time

  while ((buttons_read() & INIT_TRI) != INIT_TRI) {
    // Checks to see if button 0 is pressed. If it is, draw the rectangle. If
    // not, remove the rectangle.
    if (((buttons_read() & BUTTONS_BTN0_MASK) == BUTTONS_BTN0_MASK) &&
        (btn0drawn == 0)) {
      display_fillRect(SCREEN_PERCENT * DISPLAY_WIDTH, 0, DISPLAY_WIDTH / 4,
                       DISPLAY_HEIGHT / HALF, DISPLAY_WHITE);
      display_setCursor((SCREEN_PERCENT * DISPLAY_WIDTH) + TEXT_OFFSET,
                        TEXT_Y);           // The upper left of the LCD screen.
      display_setTextColor(DISPLAY_BLACK); // Make the text white
      display_print("BTN0");
      btn0drawn = 1;
    } else if (((buttons_read() & BUTTONS_BTN0_MASK) != BUTTONS_BTN0_MASK) &&
               (btn0drawn == 1)) {
      display_fillRect(SCREEN_PERCENT * DISPLAY_WIDTH, 0,
                       DISPLAY_WIDTH / QUARTER, DISPLAY_HEIGHT / HALF,
                       DISPLAY_BLACK);
      btn0drawn = 0;
    }
    // Checks to see if button 1 is pressed. If it is, draw the rectangle. If
    // not, remove the rectangle.
    if (((buttons_read() & BUTTONS_BTN1_MASK) == BUTTONS_BTN1_MASK) &&
        (btn1drawn == 0)) {
      display_fillRect(DISPLAY_WIDTH / HALF, 0, DISPLAY_WIDTH / QUARTER,
                       DISPLAY_HEIGHT / HALF, DISPLAY_GREEN);
      display_setCursor((DISPLAY_WIDTH / HALF) + TEXT_OFFSET,
                        TEXT_Y);           // The upper left of the LCD screen.
      display_setTextColor(DISPLAY_BLACK); // Make the text white
      display_print("BTN1");
      btn1drawn = 1;
    } else if (((buttons_read() & BUTTONS_BTN1_MASK) != BUTTONS_BTN1_MASK) &&
               (btn1drawn == 1)) {
      display_fillRect(DISPLAY_WIDTH / HALF, 0, DISPLAY_WIDTH / QUARTER,
                       DISPLAY_HEIGHT / HALF, DISPLAY_BLACK);
      btn1drawn = 0;
    }
    // Checks to see if button 2 is pressed. If it is, draw the rectangle. If
    // not, remove the rectangle.
    if (((buttons_read() & BUTTONS_BTN2_MASK) == BUTTONS_BTN2_MASK) &&
        (btn2drawn == 0)) {
      display_fillRect(DISPLAY_WIDTH / QUARTER, 0, DISPLAY_WIDTH / QUARTER,
                       DISPLAY_HEIGHT / HALF, DISPLAY_RED);
      display_setCursor((DISPLAY_WIDTH / QUARTER) + TEXT_OFFSET,
                        TEXT_Y);           // The upper left of the LCD screen.
      display_setTextColor(DISPLAY_WHITE); // Make the text white
      display_print("BTN2");
      btn2drawn = 1;
    } else if (((buttons_read() & BUTTONS_BTN2_MASK) != BUTTONS_BTN2_MASK) &&
               (btn2drawn == 1)) {
      display_fillRect(DISPLAY_WIDTH / QUARTER, 0, DISPLAY_WIDTH / QUARTER,
                       DISPLAY_HEIGHT / HALF, DISPLAY_BLACK);
      btn2drawn = 0;
    }
    // Checks to see if button 3 is pressed. If it is, draw the rectangle. If
    // not, remove the rectangle.
    if (((buttons_read() & BUTTONS_BTN3_MASK) == BUTTONS_BTN3_MASK) &&
        (btn3drawn == 0)) {
      display_fillRect(0, 0, DISPLAY_WIDTH / QUARTER, DISPLAY_HEIGHT / HALF,
                       DISPLAY_BLUE);

      display_setCursor(CURSOR_X + TEXT_OFFSET,
                        TEXT_Y);           // The upper left of the LCD screen.
      display_setTextColor(DISPLAY_WHITE); // Make the text white
      display_print("BTN3");
      btn3drawn = 1;
    } else if (((buttons_read() & BUTTONS_BTN3_MASK) != BUTTONS_BTN3_MASK) &&
               (btn3drawn == 1)) {
      display_fillRect(0, 0, DISPLAY_WIDTH / QUARTER, DISPLAY_HEIGHT / HALF,
                       DISPLAY_BLACK);
      btn3drawn = 0;
    }
  }
}
