#ifndef __PERFT_H__
#define __PERFT_H__

#include "board.h"
#include "misc.h"

int perft(board_t * board, int depth, board_stack_t * stack);
void p(board_t * board, int depth);
void perft_divide(board_t *board, int depth);
void compare_boards(board_t *board1, board_t *board2);
#endif