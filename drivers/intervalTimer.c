#include "intervalTimer.h"
#include "stdio.h"
#include "xil_io.h"
#include "xparameters.h"
#include <stdbool.h>

#define TCSR0_OFFSET 0x00
#define TCSR1_OFFSET 0x10
#define TLR0_OFFSET 0x04
#define TCR0_OFFSET 0x08

#define TLR1_OFFSET 0x14
#define TCR1_OFFSET 0x18

#define CLEAR_REGISTER 0x0
#define CLEAR 0xFFFFF000
#define CASC 0x800
#define LOAD 0x20
#define START 0x80
#define SHIFT 32

// Takes the offset, reads timer 0, and returns the value
int32_t timer_readRegister0(int32_t offset) {
  return Xil_In32(XPAR_AXI_TIMER_0_BASEADDR + offset);
}
// Takes the offset and the value and writes value to timer 0
void timer_writeRegister0(int32_t offset, int32_t value) {
  Xil_Out32(XPAR_AXI_TIMER_0_BASEADDR + offset, value);
}
// Takes the offset, reads timer 1, and returns the value
int32_t timer_readRegister1(int32_t offset) {
  return Xil_In32(XPAR_AXI_TIMER_1_BASEADDR + offset);
}
// Takes the offset and the value and writes value to timer 1
void timer_writeRegister1(int32_t offset, int32_t value) {
  Xil_Out32(XPAR_AXI_TIMER_1_BASEADDR + offset, value);
}
/// Takes the offset, reads timer 2, and returns the value
int32_t timer_readRegister2(int32_t offset) {
  return Xil_In32(XPAR_AXI_TIMER_2_BASEADDR + offset);
}
// Takes the offset and the value and writes value to timer 2
void timer_writeRegister2(int32_t offset, int32_t value) {
  Xil_Out32(XPAR_AXI_TIMER_2_BASEADDR + offset, value);
}

// You must initialize the timers before you use them the first time.
// It is generally only called once but should not cause an error if it
// is called multiple times.
// timerNumber indicates which timer should be initialized.
// returns INTERVAL_TIMER_STATUS_OK if successful, some other value otherwise.
intervalTimer_status_t intervalTimer_init(uint32_t timerNumber) {
  // For the given timer, clears the TCSR1 and sets the counters as cascading
  // Timer0
  if (timerNumber == INTERVAL_TIMER_TIMER_0) {
    timer_writeRegister0(TCSR1_OFFSET, CLEAR);
    timer_writeRegister0(TCSR0_OFFSET, CASC);
    return INTERVAL_TIMER_STATUS_OK;
    // Timer1
  } else if (timerNumber == INTERVAL_TIMER_TIMER_1) {
    timer_writeRegister1(TCSR1_OFFSET, CLEAR);
    timer_writeRegister1(TCSR0_OFFSET, CASC);
    return INTERVAL_TIMER_STATUS_OK;
    // Timer2
  } else if (timerNumber == INTERVAL_TIMER_TIMER_2) {
    timer_writeRegister2(TCSR1_OFFSET, CLEAR);
    timer_writeRegister2(TCSR0_OFFSET, CASC);
    return INTERVAL_TIMER_STATUS_OK;
  }
  return INTERVAL_TIMER_STATUS_FAIL;
}

// This is a convenience function that initializes all interval timers.
// Simply calls intervalTimer_init() on all timers.
// returns INTERVAL_TIMER_STATUS_OK if successful, some other value otherwise.
intervalTimer_status_t intervalTimer_initAll() {
  intervalTimer_init(INTERVAL_TIMER_TIMER_0);
  intervalTimer_init(INTERVAL_TIMER_TIMER_1);
  intervalTimer_init(INTERVAL_TIMER_TIMER_2);
  return INTERVAL_TIMER_STATUS_OK;
}

// This function starts the interval timer running.
// If the interval timer is already running, this function does nothing.
// timerNumber indicates which timer should start running.
void intervalTimer_start(uint32_t timerNumber) {
  // Start the interval timer running
  if (timerNumber == INTERVAL_TIMER_TIMER_0) {
    timer_writeRegister0(TCSR0_OFFSET,
                         timer_readRegister0(TCSR0_OFFSET) | START);
  } else if (timerNumber == INTERVAL_TIMER_TIMER_1) {
    timer_writeRegister1(TCSR0_OFFSET,
                         timer_readRegister1(TCSR0_OFFSET) | START);
  } else if (timerNumber == INTERVAL_TIMER_TIMER_2) {
    timer_writeRegister2(TCSR0_OFFSET,
                         timer_readRegister2(TCSR0_OFFSET) | START);
  }
}

// This function stops a running interval timer.
// If the interval time is currently stopped, this function does nothing.
// timerNumber indicates which timer should stop running.
void intervalTimer_stop(uint32_t timerNumber) {
  // Stop the interval timer
  if (timerNumber == INTERVAL_TIMER_TIMER_0) {
    timer_writeRegister0(TCSR0_OFFSET, CASC);
  } else if (timerNumber == INTERVAL_TIMER_TIMER_1) {
    timer_writeRegister1(TCSR0_OFFSET, CASC);
  } else if (timerNumber == INTERVAL_TIMER_TIMER_2) {
    timer_writeRegister2(TCSR0_OFFSET, CASC);
  }
}

// This function is called whenever you want to reuse an interval timer.
// For example, say the interval timer has been used in the past, the user
// will call intervalTimer_reset() prior to calling intervalTimer_start().
// timerNumber indicates which timer should reset.
void intervalTimer_reset(uint32_t timerNumber) {
  // Reset the interval timer by loeading the TLRs and then loading the TCSRs.
  if (timerNumber == INTERVAL_TIMER_TIMER_0) {

    // Load TLR0 and TLR1 with 0s
    timer_writeRegister0(TLR0_OFFSET, CLEAR_REGISTER);
    timer_writeRegister0(TLR1_OFFSET, CLEAR_REGISTER);

    // Load TCSR0 and TCSR1 with TLR0 and TLR1
    timer_writeRegister0(TCSR0_OFFSET,
                         timer_readRegister0(TCSR0_OFFSET) | LOAD);
    timer_writeRegister0(TCSR1_OFFSET,
                         timer_readRegister0(TCSR1_OFFSET) | LOAD);
  } else if (timerNumber == INTERVAL_TIMER_TIMER_1) {

    // Load TLR0 and TLR1 with 0s
    timer_writeRegister1(TLR0_OFFSET, CLEAR_REGISTER);
    timer_writeRegister1(TLR1_OFFSET, CLEAR_REGISTER);

    // Load TCSR0 and TCSR1 with TLR0 and TLR1
    timer_writeRegister1(TCSR0_OFFSET,
                         timer_readRegister1(TCSR0_OFFSET) | LOAD);
    timer_writeRegister1(TCSR1_OFFSET,
                         timer_readRegister1(TCSR1_OFFSET) | LOAD);
  } else if (timerNumber == INTERVAL_TIMER_TIMER_2) {

    // Load TLR0 and TLR1 with 0s
    timer_writeRegister2(TLR0_OFFSET, CLEAR_REGISTER);
    timer_writeRegister2(TLR1_OFFSET, CLEAR_REGISTER);

    // Load TCSR0 and TCSR1 with TLR0 and TLR1
    timer_writeRegister2(TCSR0_OFFSET,
                         timer_readRegister2(TCSR0_OFFSET) | LOAD);
    timer_writeRegister2(TCSR1_OFFSET,
                         timer_readRegister2(TCSR1_OFFSET) | LOAD);
  }

  // Reinitialize the timer
  intervalTimer_init(timerNumber);
}

// Convenience function for intervalTimer_reset().
// Simply calls intervalTimer_reset() on all timers.
void intervalTimer_resetAll() {
  intervalTimer_reset(INTERVAL_TIMER_TIMER_0);
  intervalTimer_reset(INTERVAL_TIMER_TIMER_1);
  intervalTimer_reset(INTERVAL_TIMER_TIMER_2);
}

// Use this function to ascertain how long a given timer has been running.
// Note that it should not be an error to call this function on a running timer
// though it usually makes more sense to call this after intervalTimer_stop()
// has been called. The timerNumber argument determines which timer is read.
double intervalTimer_getTotalDurationInSeconds(uint32_t timerNumber) {
  uint32_t timer0;
  uint64_t timer1;
  double total;
  double time;

  // Read in the couhnter bits and save them as timer0 and timer1. Shift the
  // bits from timer1 and then OR them with timer0. Find the total timer by
  // dividing by the frequency
  if (timerNumber == INTERVAL_TIMER_TIMER_0) {
    timer0 = timer_readRegister0(TCR0_OFFSET);
    timer1 = timer_readRegister0(TCR1_OFFSET);
    total = (timer1 << SHIFT) | timer0;
    time = total / XPAR_AXI_TIMER_0_CLOCK_FREQ_HZ;
    return time;
  } else if (timerNumber == INTERVAL_TIMER_TIMER_1) {

    timer0 = timer_readRegister1(TCR0_OFFSET);
    timer1 = timer_readRegister1(TCR1_OFFSET);
    total = (timer1 << SHIFT) | timer0;
    time = total / XPAR_AXI_TIMER_1_CLOCK_FREQ_HZ;
    return time;
  } else if (timerNumber == INTERVAL_TIMER_TIMER_2) {

    timer0 = timer_readRegister2(TCR0_OFFSET);
    timer1 = timer_readRegister2(TCR1_OFFSET);
    total = (timer1 << SHIFT) | timer0;
    time = total / XPAR_AXI_TIMER_2_CLOCK_FREQ_HZ;
    return time;
  }
}
