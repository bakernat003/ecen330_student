/*
This software is provided for student assignment use in the Department of
Electrical and Computer Engineering, Brigham Young University, Utah, USA.
Users agree to not re-host, or redistribute the software, in source or binary
form, to other persons or other institutions. Users may modify and use the
source code for personal or educational use.
For questions, contact Brad Hutchings or Jeff Goeders, https://ece.byu.edu/
*/

#ifndef BUTTONS_H
#define BUTTONS_H

#include <stdint.h>
#define BUTTONS_INIT_STATUS_OK 1
#define BUTTONS_INIT_STATUS_FAIL 0
#define BUTTONS_BTN0_MASK 0x1
#define BUTTONS_BTN1_MASK 0x2
#define BUTTONS_BTN2_MASK 0x4
#define BUTTONS_BTN3_MASK 0x8

// Initializes the button driver software and hardware. Returns one of the
// defined status values (above).
int32_t buttons_init();

// Returns the current value of all 4 buttons as the lower 4 bits of the
// returned value. bit3 = BTN3, bit2 = BTN2, bit1 = BTN1, bit0 = BTN0.
int32_t buttons_read();

// Runs a test of the buttons. As you push the buttons, graphics and messages
// will be written to the LCD panel. The test will until all 4 pushbuttons are
// simultaneously pressed.
void buttons_runTest();

#endif