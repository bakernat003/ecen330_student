#include "minimax.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define TOP 0
#define MID 1
#define BOT 2
#define LFT 0

#define RGT 2
#define MINIMAX_NO_SCORE 3
#define HIGH_SCORE_INIT -100
#define LOW_SCORE_INIT 100

bool minimax_xWins(minimax_board_t *board);
bool minimax_oWins(minimax_board_t *board);
bool minimax_Tie(minimax_board_t *board);
void minimax_initScoreTable(
    minimax_score_t scoreTable[MINIMAX_BOARD_ROWS][MINIMAX_BOARD_COLUMNS]);
minimax_score_t minimax_HighScore(
    minimax_score_t scoreTable[MINIMAX_BOARD_ROWS][MINIMAX_BOARD_COLUMNS]);
minimax_move_t minimax_HighMove(
    minimax_score_t scoreTable[MINIMAX_BOARD_ROWS][MINIMAX_BOARD_COLUMNS]);
minimax_score_t minimax_LowScore(
    minimax_score_t scoreTable[MINIMAX_BOARD_ROWS][MINIMAX_BOARD_COLUMNS]);
minimax_move_t minimax_LowMove(
    minimax_score_t scoreTable[MINIMAX_BOARD_ROWS][MINIMAX_BOARD_COLUMNS]);
minimax_score_t minimax_algorithm(minimax_board_t *board, bool player_is_x);
bool minimax_IsEmptyBoard(minimax_board_t *board);

minimax_move_t choice;

// Init the board to all empty squares.
void minimax_initBoard(minimax_board_t *board) {
  // Goes through the board and empties each square
  for (uint16_t i = 0; i < MINIMAX_BOARD_ROWS; i++) {
    // Goes through the board and empties each square
    for (uint16_t j = 0; j < MINIMAX_BOARD_COLUMNS; j++) {
      board->squares[i][j] = MINIMAX_EMPTY_SQUARE;
    }
  }
}

// Prints the board that is passed in to the terminal
void minimax_printBoard(minimax_board_t *board) {
  // Goes through the board and prints the value of each square
  for (uint16_t i = 0; i < MINIMAX_BOARD_ROWS; i++) {
    // Goes through the board and prints the value of each square
    for (uint16_t j = 0; j < MINIMAX_BOARD_COLUMNS; j++) {
      printf("%4d", board->squares[i][j]);
    }
    printf("\n");
  }
}

// Returns the score of the board, based upon the player and the board.
// This returns one of 4 values: MINIMAX_X_WINNING_SCORE,
// MINIMAX_O_WINNING_SCORE, MINIMAX_DRAW_SCORE, MINIMAX_NOT_ENDGAME
// Note: the player argument makes it possible to speed up this function.
// Assumptions:
// (1) if current_player_is_x == true, the last thing played was an 'X'.
// (2) if current_player_is_x == false, the last thing played was an 'O'.
// Hint: If you know the game was not over when an 'X' was played,
// you don't need to look for 'O's, and vice-versa.
minimax_score_t minimax_computeBoardScore(minimax_board_t *board,
                                          bool player_is_x) {
  // Returns the score based upon whether X or O won, if it's a tie, or if the
  // game isn't over yet
  if (minimax_xWins(board)) {
    return MINIMAX_X_WINNING_SCORE;
  } else if (minimax_oWins(board)) {
    return MINIMAX_O_WINNING_SCORE;
  } else if (minimax_Tie(board)) {
    return MINIMAX_DRAW_SCORE;
  } else {
    return MINIMAX_NOT_ENDGAME;
  }
}

// Takes a board and returns true if the board is a tie, and false if the board
// isn't a tie
bool minimax_Tie(minimax_board_t *board) {
  // Iterates through each square and checks whether or not the square is empty.
  // If it's empty, we know it's not a tie
  for (uint16_t i = 0; i < MINIMAX_BOARD_ROWS; i++) {
    // Iterates through each square and checks whether or not the square is
    // empty. If it's empty, we know it's not a tie
    for (uint16_t j = 0; j < MINIMAX_BOARD_COLUMNS; j++) {
      // Checks whether or not the square is empty. If it's empty, we know it's
      // not a tie
      if (board->squares[i][j] == MINIMAX_EMPTY_SQUARE) {
        return false;
      }
    }
  }
  return true;
}

// Takes a board and checks it to see if X has won
bool minimax_xWins(minimax_board_t *board) {
  // Check to see if X has won
  if ((board->squares[TOP][LFT] == MINIMAX_X_SQUARE) &&
      (board->squares[TOP][MID] == MINIMAX_X_SQUARE) &&
      (board->squares[TOP][RGT] == MINIMAX_X_SQUARE)) {
    return true;
  } else if ((board->squares[MID][LFT] == MINIMAX_X_SQUARE) &&
             (board->squares[MID][MID] == MINIMAX_X_SQUARE) &&
             (board->squares[MID][RGT] == MINIMAX_X_SQUARE)) {
    return true;
  } else if ((board->squares[BOT][LFT] == MINIMAX_X_SQUARE) &&
             (board->squares[BOT][MID] == MINIMAX_X_SQUARE) &&
             (board->squares[BOT][RGT] == MINIMAX_X_SQUARE)) {
    return true;
  } else if ((board->squares[TOP][LFT] == MINIMAX_X_SQUARE) &&
             (board->squares[MID][LFT] == MINIMAX_X_SQUARE) &&
             (board->squares[BOT][LFT] == MINIMAX_X_SQUARE)) {
    return true;
  } else if ((board->squares[TOP][MID] == MINIMAX_X_SQUARE) &&
             (board->squares[MID][MID] == MINIMAX_X_SQUARE) &&
             (board->squares[BOT][MID] == MINIMAX_X_SQUARE)) {
    return true;
  } else if ((board->squares[TOP][RGT] == MINIMAX_X_SQUARE) &&
             (board->squares[MID][RGT] == MINIMAX_X_SQUARE) &&
             (board->squares[BOT][RGT] == MINIMAX_X_SQUARE)) {
    return true;
  } else if ((board->squares[TOP][LFT] == MINIMAX_X_SQUARE) &&
             (board->squares[MID][MID] == MINIMAX_X_SQUARE) &&
             (board->squares[BOT][RGT] == MINIMAX_X_SQUARE)) {
    return true;
  } else if ((board->squares[TOP][RGT] == MINIMAX_X_SQUARE) &&
             (board->squares[MID][MID] == MINIMAX_X_SQUARE) &&
             (board->squares[BOT][LFT] == MINIMAX_X_SQUARE)) {
    return true;
  } else {
    return false;
  }
}

// Takes a board and checks it to see if O has won
bool minimax_oWins(minimax_board_t *board) {
  // Check to see if O has won
  if ((board->squares[TOP][LFT] == MINIMAX_O_SQUARE) &&
      (board->squares[TOP][MID] == MINIMAX_O_SQUARE) &&
      (board->squares[TOP][RGT] == MINIMAX_O_SQUARE)) {
    return true;
  } else if ((board->squares[MID][LFT] == MINIMAX_O_SQUARE) &&
             (board->squares[MID][MID] == MINIMAX_O_SQUARE) &&
             (board->squares[MID][RGT] == MINIMAX_O_SQUARE)) {
    return true;
  } else if ((board->squares[BOT][LFT] == MINIMAX_O_SQUARE) &&
             (board->squares[BOT][MID] == MINIMAX_O_SQUARE) &&
             (board->squares[BOT][RGT] == MINIMAX_O_SQUARE)) {
    return true;
  } else if ((board->squares[TOP][LFT] == MINIMAX_O_SQUARE) &&
             (board->squares[MID][LFT] == MINIMAX_O_SQUARE) &&
             (board->squares[BOT][LFT] == MINIMAX_O_SQUARE)) {
    return true;
  } else if ((board->squares[TOP][MID] == MINIMAX_O_SQUARE) &&
             (board->squares[MID][MID] == MINIMAX_O_SQUARE) &&
             (board->squares[BOT][MID] == MINIMAX_O_SQUARE)) {
    return true;
  } else if ((board->squares[TOP][RGT] == MINIMAX_O_SQUARE) &&
             (board->squares[MID][RGT] == MINIMAX_O_SQUARE) &&
             (board->squares[BOT][RGT] == MINIMAX_O_SQUARE)) {
    return true;
  } else if ((board->squares[TOP][LFT] == MINIMAX_O_SQUARE) &&
             (board->squares[MID][MID] == MINIMAX_O_SQUARE) &&
             (board->squares[BOT][RGT] == MINIMAX_O_SQUARE)) {
    return true;
  } else if ((board->squares[TOP][RGT] == MINIMAX_O_SQUARE) &&
             (board->squares[MID][MID] == MINIMAX_O_SQUARE) &&
             (board->squares[BOT][LFT] == MINIMAX_O_SQUARE)) {
    return true;
  } else {
    return false;
  }
}

// Determine that the game is over by looking at the score.
bool minimax_isGameOver(minimax_score_t score) {
  // Check to see if the game is over. The game is over if X has won, if O has
  // won, or if it's a draw
  if ((score == MINIMAX_X_WINNING_SCORE) ||
      (score == MINIMAX_O_WINNING_SCORE) || (score == MINIMAX_DRAW_SCORE)) {
    return true;
  } else {
    return false;
  }
}

// Initializes the score table to the MINIMAX_NO_SCORE value
void minimax_initScoreTable(
    minimax_score_t scoreTable[MINIMAX_BOARD_ROWS][MINIMAX_BOARD_COLUMNS]) {
  // Sets each square in the score table equal to the MINIMAX_NO_SCORE value
  for (uint16_t i = 0; i < MINIMAX_BOARD_ROWS; i++) {
    // Sets each square in the score table equal to the MINIMAX_NO_SCORE value
    for (uint16_t j = 0; j < MINIMAX_BOARD_COLUMNS; j++) {
      scoreTable[i][j] = MINIMAX_NO_SCORE;
    }
  }
}

// Checks to see whether the board is empty
bool minimax_IsEmptyBoard(minimax_board_t *board) {
  bool isEmpty = true;
  // Iterate through the board
  for (uint16_t i = 0; i < MINIMAX_BOARD_ROWS; i++) {
    // Iterate through the board
    for (uint16_t j = 0; j < MINIMAX_BOARD_COLUMNS; j++) {
      // If a square isn't empty, return false
      if ((board->squares[i][j] == MINIMAX_X_SQUARE) ||
          board->squares[i][j] == MINIMAX_O_SQUARE) {
        return false;
      }
    }
  }
  return isEmpty;
}

// A recursive algorithm that changes the variable choice to the best move. It
// takes a board and whether the computer is playing as X or O, and returns the
// score of the board.
minimax_score_t minimax_algorithm(minimax_board_t *board, bool player_is_x) {

  minimax_score_t score = minimax_computeBoardScore(board, !player_is_x);
  // If the game is over, score the board and return the score
  if (minimax_isGameOver(score)) {
    return minimax_computeBoardScore(board, !player_is_x);
  } else if (minimax_IsEmptyBoard(board)) {
    choice.row = 0;
    choice.column = 0;
    return minimax_computeBoardScore(board, !player_is_x);
  } else {
    minimax_score_t scoreTable[MINIMAX_BOARD_ROWS][MINIMAX_BOARD_COLUMNS];
    minimax_initScoreTable(scoreTable);
    // Creates the board's score table
    for (uint16_t i = 0; i < MINIMAX_BOARD_ROWS; i++) {
      // Creates the board's score table
      for (uint16_t j = 0; j < MINIMAX_BOARD_COLUMNS; j++) {
        // If the square is empty, put the desired marker in the empty square
        if (board->squares[i][j] == MINIMAX_EMPTY_SQUARE) {
          // If player_is_x, put an X on the empty square. Otherwise, put an O
          if (player_is_x) {
            board->squares[i][j] = MINIMAX_X_SQUARE;
          } else {
            board->squares[i][j] = MINIMAX_O_SQUARE;
          }
          score = minimax_algorithm(board, !player_is_x);
          scoreTable[i][j] = score;
          board->squares[i][j] = MINIMAX_EMPTY_SQUARE;
        }
      }
    }
    // If player_is_x == true, set the choice row and column to the highest move
    // in the score table. If player_is_x == false, set the choice row and
    // column to the lowest move in the score table.
    if (player_is_x) {
      choice.row = minimax_HighMove(scoreTable).row;
      choice.column = minimax_HighMove(scoreTable).column;
      score = minimax_HighScore(scoreTable);
    } else {
      choice.row = minimax_LowMove(scoreTable).row;
      choice.column = minimax_LowMove(scoreTable).column;
      score = minimax_LowScore(scoreTable);
    }
    return score;
  }
}

// Takes a board's score table and returns the highest score in the table
minimax_score_t minimax_HighScore(
    minimax_score_t scoreTable[MINIMAX_BOARD_ROWS][MINIMAX_BOARD_COLUMNS]) {
  minimax_score_t highScore = HIGH_SCORE_INIT;
  // Iterate through each spot in the table, and if the table value exceeeds the
  // highScore it's seen so far, set highScore equal to that table value
  for (uint16_t i = 0; i < MINIMAX_BOARD_ROWS; i++) {
    // Iterate through each spot in the table, and if the table value exceeeds
    // the highScore it's seen so far, set highScore equal to that table value
    for (uint16_t j = 0; j < MINIMAX_BOARD_COLUMNS; j++) {
      // If the scoretable isn't empty
      if (scoreTable[i][j] != MINIMAX_NO_SCORE) {
        // Set highScore equal to the table value
        if (scoreTable[i][j] > highScore) {
          highScore = scoreTable[i][j];
        }
      }
    }
  }
  return highScore;
}

// Takes a board's score table and returns the lowest score in the table
minimax_score_t minimax_LowScore(
    minimax_score_t scoreTable[MINIMAX_BOARD_ROWS][MINIMAX_BOARD_COLUMNS]) {
  minimax_score_t lowScore = LOW_SCORE_INIT;
  // Iterate through each spot in the table, and if the table value is lower
  // than the lowcore it's seen so far, set lowScore equal to that table value
  for (uint16_t i = 0; i < MINIMAX_BOARD_ROWS; i++) {
    // Iterate through each spot in the table, and if the table value is lower
    // than the lowcore it's seen so far, set lowScore equal to that table value
    for (uint16_t j = 0; j < MINIMAX_BOARD_COLUMNS; j++) {
      // If the scoretable isn't empty
      if (scoreTable[i][j] != MINIMAX_NO_SCORE) {
        // Set lowScore equal to the table value
        if (scoreTable[i][j] < lowScore) {
          lowScore = scoreTable[i][j];
        }
      }
    }
  }
  return lowScore;
}

// Takes a board's score table and returns the move with the highest score in
// the table
minimax_move_t minimax_HighMove(
    minimax_score_t scoreTable[MINIMAX_BOARD_ROWS][MINIMAX_BOARD_COLUMNS]) {
  minimax_score_t highScore = HIGH_SCORE_INIT;
  minimax_move_t move;
  // Iterate through each table entry, find the highest score, and return the
  // move equal to that high score
  for (uint16_t i = 0; i < MINIMAX_BOARD_ROWS; i++) {
    // Iterate through each table entry, find the highest score, and return the
    // move equal to that high score
    for (uint16_t j = 0; j < MINIMAX_BOARD_COLUMNS; j++) {
      // If the scoretable isn't empty
      if (scoreTable[i][j] != MINIMAX_NO_SCORE) {
        // Set move equal to the move with the highest score
        if (scoreTable[i][j] > highScore) {
          highScore = scoreTable[i][j];
          move.row = i;
          move.column = j;
        }
      }
    }
  }
  return move;
}

// Takes a board's score table and returns the move with the lowest score in the
// table
minimax_move_t minimax_LowMove(
    minimax_score_t scoreTable[MINIMAX_BOARD_ROWS][MINIMAX_BOARD_COLUMNS]) {
  minimax_score_t lowScore = LOW_SCORE_INIT;
  minimax_move_t move;
  // Iterate through each table entry, find the lowest score, and return the
  // move equal to that low score
  for (uint16_t i = 0; i < MINIMAX_BOARD_ROWS; i++) {
    // Iterate through each table entry, find the lowest score, and return the
    // move equal to that low score
    for (uint16_t j = 0; j < MINIMAX_BOARD_COLUMNS; j++) {
      // If the scoretable isn't empty
      if (scoreTable[i][j] != MINIMAX_NO_SCORE) {
        // Set move equal to the move with the lowest score
        if (scoreTable[i][j] < lowScore) {
          lowScore = scoreTable[i][j];
          move.row = i;
          move.column = j;
        }
      }
    }
  }
  return move;
}

// This routine is not recursive but will invoke the recursive minimax function.
// You will call this function from the controlling state machine that you will
// implement in a later milestone. It computes the row and column of the next
// move based upon: the current board, the player. true means the computer is X.
// false means the computer is O. When called from the controlling state
// machine, you will call this function as follows:
// 1. If the computer is playing as X, you will call this function with
// current_player_is_x = true.
// 2. If the computer is playing as O, you will call this function with
// current_player_is_x = false. minimax_computeNextMove directly passes the
// current_player_is_x argument into the minimax() (helper) function. To assign
// values to the row and column arguments, you must use the following syntax in
// the body of the function: *row = move_row; *column = move_column; (for
// example).
void minimax_computeNextMove(minimax_board_t *board, bool current_player_is_x,
                             uint8_t *row, uint8_t *column) {
  minimax_algorithm(board, current_player_is_x);
  *row = choice.row;
  *column = choice.column;
}