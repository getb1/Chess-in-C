#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "board.h"
#include "misc.h"

int perft(board_t * board,int depth,board_stack_t * stack) {

    if(depth==0) {
        return 1;
    }

    int nodes = 0;

    move_t * moves = get_legal_move_side(board,board->turn); 
    
    for(int i=0;i<300;i++) {
        if(moves[i].from==0&&moves[i].to==0) {
            break;
        }

        make_move(board,&moves[i],stack);
        display_board(board);
        nodes = nodes+perft(board,depth-1,stack);
        board = undo_move(stack,board);

    }
    return nodes;

}

