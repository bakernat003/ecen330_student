#include "globals.h"

#define GLOBAL_SEQUENCE_LENGTH 100

static const uint16_t globalSequenceLength = GLOBAL_SEQUENCE_LENGTH;

static uint8_t globalSequence[GLOBAL_SEQUENCE_LENGTH];

static uint16_t currentSequenceLength;

// This is the length of the complete sequence at maximum length.
// You must copy the contents of the sequence[] array into the global variable
// that you maintain. Do not just grab the pointer as this will fail.
void globals_setSequence(const uint8_t sequence[], uint16_t length) {
  for (uint8_t i = 0; i < GLOBAL_SEQUENCE_LENGTH; i++) {
    globalSequence[i] = sequence[i];
  }
}

// This returns the value of the sequence at the index.
uint8_t globals_getSequenceValue(uint16_t index) {
  return globalSequence[index];
}

// Retrieve the sequence length.
uint16_t globals_getSequenceLength() { return globalSequenceLength; }

// This is the length of the sequence that you are currently working on.
void globals_setSequenceIterationLength(uint16_t length) {
  currentSequenceLength = length;
}

// This is the length of the sequence that you are currently working on,
// not the maximum length but the interim length as
// the person playing the game works through the pattern one color at a time.
uint16_t globals_getSequenceIterationLength() { return currentSequenceLength; }
