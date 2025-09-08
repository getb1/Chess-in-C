#ifndef SEARCH_H
#define SEARCH_H
#include "board.h"
#include "perft.h"
#include "misc.h"

typedef struct {
    board_t * board;
    int depth;
    int start;
    int end;
    int best_score;
    move_t best_move;
} search_args_t;

move_t find_best_move(board_t * board, int depth);
move_t * order_moves(move_t * moves);
int evaluate_position(board_t * board);
move_t find_best_move_multi_thread(board_t * board, int depth, int threads);

#endif