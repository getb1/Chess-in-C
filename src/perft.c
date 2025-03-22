#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "board.h"
#include "misc.h"

int perft(board_t * board, int depth, board_stack_t * stack) {
    if (depth == 0) {
        return 1;
    }
    move_t legal_moves[300];
    get_legal_move_side(board, board->turn, legal_moves);
    int nodes = 0;
    for (int i = 0; i < 300; i++) {
        if (legal_moves[i].from == 0 && legal_moves[i].to == 0) {
            break;
        }
        make_move(board, &legal_moves[i], stack);
        nodes += perft(board, depth - 1, stack);
        board = undo_move(stack, board);
    }
    return nodes;
}

