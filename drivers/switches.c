#include "switches.h"
#include "leds.h"
#include "stdio.h"
#include "xil_io.h"
#include "xparameters.h"

#define GPIO_DATA_OFFSET 0x00
#define GPIO_TRI_OFFSET 0x04
#define INIT_TRI 0x0000000F
#define LEDS_OFF 0x00000000

int32_t switches_readGpioRegister(int32_t offset) {
  return Xil_In32(XPAR_SLIDE_SWITCHES_BASEADDR + offset);
}

void switches_writeGpioRegister(int32_t offset, int32_t value) {
  Xil_Out32(XPAR_SLIDE_SWITCHES_BASEADDR + offset, value);
}

// Initializes the SWITCHES driver software and hardware. Returns one of the
// STATUS values
int32_t switches_init() {
  leds_init(true);
  switches_writeGpioRegister(GPIO_TRI_OFFSET, INIT_TRI);
  int32_t gpioTri = switches_readGpioRegister(GPIO_TRI_OFFSET);
  if (gpioTri == INIT_TRI) {
    printf("%d OK\n", gpioTri);
    return SWITCHES_INIT_STATUS_OK;
  } else {
    return SWITCHES_INIT_STATUS_FAIL;
  }
}

// Returns the current value of all 4 switches as the lower 4 bits of the
// returned value. bit3 = SW3, bit2 = SW2, bit1 = SW1, bit0 = SW0.
int32_t switches_read() { return switches_readGpioRegister(GPIO_DATA_OFFSET); }

// Runs a test of the switches. As you slide the switches, LEDs directly above
// the switches will illuminate. The test will run until all switches are slid
// upwards. When all 4 slide switches are slid upward, this function will
// return.
void switches_runTest() {
  switches_init();
  while ((switches_readGpioRegister(GPIO_DATA_OFFSET) & INIT_TRI) != INIT_TRI) {
    leds_write(switches_readGpioRegister(GPIO_DATA_OFFSET));
  }
  leds_write(LEDS_OFF);
}
