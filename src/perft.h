#ifndef __PERFT_H__
#define __PERFT_H__

#include "board.h"
#include "misc.h"

typedef struct {
    board_t *board;
    int depth;
    int start;
    int end;
    int result;
} perft_args_t;

int perft(board_t * board, int depth, board_stack_t * stack);
void p(board_t * board, int depth);
void perft_divide(board_t *board, int depth);
void compare_boards(board_t *board1, board_t *board2);
int perft_multi_threaded(board_t * board, int depth, int threads, int verbose);
board_t * full_copy_board(board_t * original);
#endif