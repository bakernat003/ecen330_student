#include <stdint.h>
#include <stdbool.h>

#ifndef MEMORYCONTROL_H
#define MEMORYCONTROL_H

// Standard tick function.
void memoryControl_tick();

// Call this before you call memoryControl_tick().
void memoryControl_init();

void initScoreGrid();

//Used to debug the state machine
void debugStatePrint();

void drawIntroMessage(bool erase);

//Checks to see if the card has already been matched
bool alreadyMatched(uint8_t row, uint8_t column);

//Checks to see if all the matches have been made
bool isGameOver();

//Draws the game over message
void drawGameOverMessage(bool erase);

#endif /* CLOCKCONTROL_H */