#include "memoryDisplay.h"
#include "display.h"
#include <utils.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define SPACE_BETWEEN_CARDS_SHIFT 10

static int16_t x, y;
static uint8_t z;

//The grid
static uint16_t grid[GRID_SIZE][GRID_SIZE];
static uint16_t gridCount[(GRID_SIZE*GRID_SIZE)/2];

// Called only once - performs any necessary inits.
void memoryDisplay_init() {
  display_init();
  display_fillScreen(DISPLAY_BLACK);
  initGridCount();
}

//Initializes the grid that keeps track of how many of each card is in the grid to zero
void initGridCount() {
  //Sets each value in the gridCount array to zero
  for (uint16_t i = 0; i < (GRID_SIZE*GRID_SIZE)/2; i++) {
      gridCount[i] = 0;
  }
}

//Finds and returns a card that hasn't yet been used twice
uint16_t findAvailableCard() {
  uint16_t card = 0; //Uses this to store the card that will be returned
  //Loop through the gridCount array
  for (uint16_t i = 0; i < (GRID_SIZE*GRID_SIZE)/2; i++) {
    //If there is an index value that is less than two, set card equal to that index
    if (gridCount[i] < 2) {
      card = i;
      return card;
    }
  }
}

//Initializes the grid array with random cards. There can only be two of each card.
void initGrid() {
  uint16_t value = 0;
  //Loops through the grid
  for (uint16_t i = 0; i < GRID_SIZE; i++) {
    //Loops through the grid
    for (uint16_t j = 0; j < GRID_SIZE; j++) {
      value = getRandomCard();
      //If there aren't already two cards of the random card, use that card
      if(gridCount[value] < 2) {
          grid[i][j] = value;
          gridCount[value]++;
      } else {
          //Otherwise just find an available card and use that
          value = findAvailableCard();
          grid[i][j] = value;
          gridCount[value]++;
      }
    }
  }
}

//Returns a random card
uint16_t getRandomCard() {
  return rand() % ((GRID_SIZE*GRID_SIZE)/2);
}

//Draws a card face down at the given row-column coordinates. Can also erase the card.
void drawDownCard(uint16_t row, uint16_t col, bool erase) {
  uint16_t xShift = 0; // The amount to shift the x coordinate, which depends on the
  //GRID_SIZE
  uint16_t yShift = 0; // The amount to shift the y coordinate, which depends on the
  //GRID_SIZE
  //Loops through each card
  for(uint8_t i = 0; i < (GRID_SIZE); i++) {
    //Loops through each card
    for(uint8_t j = 0; j < (GRID_SIZE); j++) {
      //If it found the correct row and collumn
      if ((i == row) && (j == col)) {
        //Either draw or erase the card
        if (erase == false) {
          display_fillRect(xShift, yShift, (DISPLAY_WIDTH/GRID_SIZE)-SPACE_BETWEEN_CARDS_SHIFT, (DISPLAY_HEIGHT/GRID_SIZE)-SPACE_BETWEEN_CARDS_SHIFT, DISPLAY_BLUE);
        } else {
          display_fillRect(xShift, yShift, (DISPLAY_WIDTH/GRID_SIZE)-SPACE_BETWEEN_CARDS_SHIFT, (DISPLAY_HEIGHT/GRID_SIZE)-SPACE_BETWEEN_CARDS_SHIFT, DISPLAY_BLACK);
        }
      }
      xShift = xShift + DISPLAY_WIDTH/GRID_SIZE; //Increases the xShift
    }
    xShift = 0; // Set the xShift back to zero to start a new row
    yShift = yShift + DISPLAY_HEIGHT/GRID_SIZE; //Increase the yShift
  }
}

//Draws a grid of cards face down
void drawDownGrid() {
  //Loops through the grid
  for(uint8_t i = 0; i < (GRID_SIZE); i++) {
    //Loops through the grid
    for(uint8_t j = 0; j < (GRID_SIZE); j++) {
      drawDownCard(i, j, false);
    }
  }
}

//Draws the face of the card at the given row-column coordinates. Can also erase the card. Is similar to drawDownCard.
void drawUpCard(uint16_t row, uint16_t col, bool erase) {
uint16_t xShift = 0; // The amount to shift the x coordinate, which depends on the
  //GRID_SIZE
  uint16_t yShift = 0; // The amount to shift the y coordinate, which depends on the
  //GRID_SIZE
  //Loops through each card
  for(uint8_t i = 0; i < (GRID_SIZE); i++) {
    //Loops through each card
    for(uint8_t j = 0; j < (GRID_SIZE); j++) {
      //If it found the correct row and collumn
      if ((i == row) && (j == col)) {
        //Either draw or erase the card
        if (erase == false) {
          //If the card is a rectangle, draw the rectangle
          if ((grid[row][col] >= 0) && (grid[row][col] <= 3)) {
            drawRectCard(xShift, yShift, row, col, false);
          } else {
            drawCircleCard(xShift, yShift, row, col, false);
          }
        //Erases the card
        } else {
          //If the card is a rectangle, erase the rectangle
          if ((grid[row][col] >= 0) && (grid[row][col] <= 3)) {
            drawRectCard(xShift, yShift, row, col, true);
          } else {
            drawCircleCard(xShift, yShift, row, col, true);
          }
        }
      }
      xShift = xShift + DISPLAY_WIDTH/GRID_SIZE; //Increases the xShift
    }
    xShift = 0; // Set the xShift back to zero to start a new row
    yShift = yShift + DISPLAY_HEIGHT/GRID_SIZE; //Increase the yShift
  }
}

//Used by drawUpCard to draw rectangle card at the given row and column
void drawRectCard(uint16_t xShift, uint16_t yShift, uint16_t row, uint16_t col, bool erase) {
  if (erase == true) {
    display_drawRect(xShift+5, yShift+5, (DISPLAY_WIDTH/GRID_SIZE)-SPACE_BETWEEN_CARDS_SHIFT-10, (DISPLAY_HEIGHT/GRID_SIZE)-SPACE_BETWEEN_CARDS_SHIFT-10, DISPLAY_BLACK);
  } else if (grid[row][col] == 0) {
    display_drawRect(xShift+5, yShift+5, (DISPLAY_WIDTH/GRID_SIZE)-SPACE_BETWEEN_CARDS_SHIFT-10, (DISPLAY_HEIGHT/GRID_SIZE)-SPACE_BETWEEN_CARDS_SHIFT-10, DISPLAY_WHITE);
  } else if (grid[row][col] == 1) {
    display_drawRect(xShift+5, yShift+5, (DISPLAY_WIDTH/GRID_SIZE)-SPACE_BETWEEN_CARDS_SHIFT-10, (DISPLAY_HEIGHT/GRID_SIZE)-SPACE_BETWEEN_CARDS_SHIFT-10, DISPLAY_RED);
  } else if (grid[row][col] == 2) {
    display_drawRect(xShift+5, yShift+5, (DISPLAY_WIDTH/GRID_SIZE)-SPACE_BETWEEN_CARDS_SHIFT-10, (DISPLAY_HEIGHT/GRID_SIZE)-SPACE_BETWEEN_CARDS_SHIFT-10, DISPLAY_YELLOW);
  } else if (grid[row][col] == 3) {
    display_drawRect(xShift+5, yShift+5, (DISPLAY_WIDTH/GRID_SIZE)-SPACE_BETWEEN_CARDS_SHIFT-10, (DISPLAY_HEIGHT/GRID_SIZE)-SPACE_BETWEEN_CARDS_SHIFT-10, DISPLAY_BLUE);
  } else {
    printf("Not a rectangle\n");
  }
}

//Used by drawUpCard to draw circle card at the given row and column
void drawCircleCard(uint16_t xShift, uint16_t yShift, uint16_t row, uint16_t col, bool erase) {
  if (erase == true) {
    display_drawCircle(xShift + (((DISPLAY_WIDTH/GRID_SIZE))/2)-4, yShift + (((DISPLAY_HEIGHT/GRID_SIZE))/2)-4, (((DISPLAY_HEIGHT/GRID_SIZE)-SPACE_BETWEEN_CARDS_SHIFT-10)/2), DISPLAY_BLACK);
  } else if (grid[row][col] == 4) {
    display_drawCircle(xShift + (((DISPLAY_WIDTH/GRID_SIZE))/2)-4, yShift + (((DISPLAY_HEIGHT/GRID_SIZE))/2)-4, (((DISPLAY_HEIGHT/GRID_SIZE)-SPACE_BETWEEN_CARDS_SHIFT-10)/2), DISPLAY_WHITE);
  } else if (grid[row][col] == 5) {
    display_drawCircle(xShift + (((DISPLAY_WIDTH/GRID_SIZE))/2)-4, yShift + (((DISPLAY_HEIGHT/GRID_SIZE))/2)-4, (((DISPLAY_HEIGHT/GRID_SIZE)-SPACE_BETWEEN_CARDS_SHIFT-10)/2), DISPLAY_RED);
  } else if (grid[row][col] == 6) {
    display_drawCircle(xShift + (((DISPLAY_WIDTH/GRID_SIZE))/2)-4, yShift + (((DISPLAY_HEIGHT/GRID_SIZE))/2)-4, (((DISPLAY_HEIGHT/GRID_SIZE)-SPACE_BETWEEN_CARDS_SHIFT-10)/2), DISPLAY_YELLOW);
  } else if (grid[row][col] == 7) {
    display_drawCircle(xShift + (((DISPLAY_WIDTH/GRID_SIZE))/2)-4, yShift + (((DISPLAY_HEIGHT/GRID_SIZE))/2)-4, (((DISPLAY_HEIGHT/GRID_SIZE)-SPACE_BETWEEN_CARDS_SHIFT-10)/2), DISPLAY_BLUE);
  }
  
}

// After a touch has been detected and after the proper delay, this sets the row
// and column arguments according to where the user touched the board.
void touchScreenComputeBoardRowColumn(uint8_t *row, uint8_t *column) {
  display_getTouchedPoint(&x, &y, &z);
  // Sets the column to the appropriate column given the x coordinate of the
  // touched point
  if ((x < DISPLAY_WIDTH / GRID_SIZE)) {
    *column = 0;
  } else if ((x > DISPLAY_WIDTH / GRID_SIZE) && (x < (DISPLAY_WIDTH / 2))) {
    *column = 1;
  } else if ((x > (DISPLAY_WIDTH / 2)) && (x < 3 * (DISPLAY_WIDTH / GRID_SIZE))) {
    *column = 2;
  } else {
    *column = 3;
  }
  // Sets the row to the appropriate row given the y coordinate of the touched
  // point
  if ((y < DISPLAY_HEIGHT / GRID_SIZE)) {
    *row = 0;
  } else if ((y > DISPLAY_HEIGHT / GRID_SIZE) && (y < 2 * (DISPLAY_HEIGHT / GRID_SIZE))) {
    *row = 1;
  } else if ((y > (2 * DISPLAY_HEIGHT / GRID_SIZE)) && (y < 3 * (DISPLAY_HEIGHT / GRID_SIZE))) {
    *row = 2;
  } else {
    *row = 3;
  }
}

void choiceFeedback(uint16_t row, uint16_t col, bool correct, bool erase) {
uint16_t xShift = 0; // The amount to shift the x coordinate, which depends on the
  //GRID_SIZE
  uint16_t yShift = 0; // The amount to shift the y coordinate, which depends on the
  //GRID_SIZE
  //Loops through each card
  for(uint8_t i = 0; i < (GRID_SIZE); i++) {
    //Loops through each card
    for(uint8_t j = 0; j < (GRID_SIZE); j++) {
      //If it found the correct row and column
      if ((i == row) && (j == col)) {
        //Either draw or erase the card
        if (erase == false) {
          //Draw the rectangle
            drawRectFeedback(xShift, yShift, row, col, correct, false);
        //Erases the rectangle
        } else {
          //Erase the rectangle
            drawRectFeedback(xShift, yShift, row, col, correct, true);
        }
      }
      xShift = xShift + DISPLAY_WIDTH/GRID_SIZE; //Increases the xShift
    }
    xShift = 0; // Set the xShift back to zero to start a new row
    yShift = yShift + DISPLAY_HEIGHT/GRID_SIZE; //Increase the yShift
  }
}

//Used by drawUpCard to draw rectangle card at the given row and column
void drawRectFeedback(uint16_t xShift, uint16_t yShift, uint16_t row, uint16_t col, bool correct, bool erase) {
  if (erase == true) {
    display_drawRect(xShift, yShift, (DISPLAY_WIDTH/GRID_SIZE)-SPACE_BETWEEN_CARDS_SHIFT, (DISPLAY_HEIGHT/GRID_SIZE)-SPACE_BETWEEN_CARDS_SHIFT, DISPLAY_BLACK);
  } else if (correct == true) {
    display_drawRect(xShift, yShift, (DISPLAY_WIDTH/GRID_SIZE)-SPACE_BETWEEN_CARDS_SHIFT, (DISPLAY_HEIGHT/GRID_SIZE)-SPACE_BETWEEN_CARDS_SHIFT, DISPLAY_GREEN);
  } else if (correct == false) {
    display_drawRect(xShift, yShift, (DISPLAY_WIDTH/GRID_SIZE)-SPACE_BETWEEN_CARDS_SHIFT, (DISPLAY_HEIGHT/GRID_SIZE)-SPACE_BETWEEN_CARDS_SHIFT, DISPLAY_RED);
  }
}

uint8_t readCard(uint8_t row, uint8_t column) {
  return grid[row][column];
}

// Run a test of memory-display functions.
void memoryDisplay_runTest() {
  memoryDisplay_init();
}


