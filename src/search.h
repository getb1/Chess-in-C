#ifndef SEARCH_H
#define SEARCH_H
#include "board.h"
#include "perft.h"
#include "misc.h"



move_t * move_find_best_move(board_t * board, int depth);
int evaluate_board(board_t * board);
int minimax(board_t * board, int depth, int alpha, int beta);
#endif