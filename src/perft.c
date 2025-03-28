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

        if(depth==8) {
            printf("%d",i);
        }
        make_move(board, &legal_moves[i], stack);
        nodes += perft(board, depth - 1, stack);
        board = undo_move(stack, board);
    }
    return nodes;
}


void perft_divide(board_t *board, int depth) {
    move_t moves[300];
    get_legal_move_side(board, board->turn, moves);
    int total = 0;
    
    for (int i = 0; i < 300; i++) {
        if (moves[i].from == 0 && moves[i].to == 0) {
            break;
        }
        board_stack_t * stack = c_stack();
        make_move(board, &moves[i], stack);

        int count = perft(board, depth - 1,stack);
        char * from = int_to_char_sq(moves[i].from);
        char * to = int_to_char_sq(moves[i].to);
        printf("%d:%s%s: %d\n",i, from, to, count);
        free(from);
        free(to);
        
        total += count;
        undo_move(stack, board);
    }
    printf("Total: %d\n", total);
}
