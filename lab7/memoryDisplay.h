#include <stdint.h>
#include <stdbool.h>
#ifndef MEMORYDISPLAY_H
#define MEMORYDISPLAY_H


#define GRID_SIZE 4 //Can be either 2 or 4

// Called only once - performs any necessary inits.
void memoryDisplay_init();

//Initializes the grid that keeps track of how many of each card is in the grid to zero
void initGridCount();

//Finds and returns a card that hasn't yet been used twice
uint16_t findAvailableCard();

//Initializes the grid array with random cards. There can only be two of each card.
void initGrid();

//Returns a random card
uint16_t getRandomCard();

//Draws a card face down at the given coordinates
void drawDownCard(uint16_t row, uint16_t col, bool erase);

//Draws a grid of cards face down
void drawDownGrid();

//Draws the face of the card at the given row-column coordinates. Can also erase the card.
//It draws one of 18 shapes, and draws the one according to the grid array
void drawUpCard(uint16_t row, uint16_t col, bool erase);

//Used by drawUpCard to draw rectangle Card at the given row and column
void drawRectCard(uint16_t xShift, uint16_t yShift, uint16_t row, uint16_t col, bool erase);

//Used by drawUpCard to draw circle card at the given row and column
void drawCircleCard(uint16_t xShift, uint16_t yShift, uint16_t row, uint16_t col, bool erase);

void touchScreenComputeBoardRowColumn(uint8_t *row, uint8_t *column);

uint8_t readCard(uint8_t row, uint8_t column);

void choiceFeedback(uint16_t row, uint16_t col, bool correct, bool erase);

void drawRectFeedback(uint16_t xShift, uint16_t yShift, uint16_t row, uint16_t col, bool correct, bool erase);

// Run a test of memory-display functions.
void memoryDisplay_runTest();

#endif /* MEMORYDISPLAY_H */
