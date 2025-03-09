#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "board.h"
#include "misc.h"

// Function prototypes
typedef struct {
    board_t board;
    int depth;
} stack_frame_t;

int perft(board_t * board, int depth) {
    if(depth==0) {
        return 1;
    }
    int nodes = 0;
    move_t * moves = get_legal_move_side(board,board->turn);
    for(int i=0; i<300; i++) {
        if(moves[i].from==0&&moves[i].to==0) {
            break;
        }
        make_move(board,&moves[i]);
        nodes += perft(board,depth-1);
        undo_move(board);
    }
    return nodes;
}

void p(board_t * board, int depth) {
    if(depth==0) {
        return;
    }
    printf("Depth: %d\n",depth);
    move_t * moves = get_legal_move_side(board,board->turn);
    make_move(board,&moves[0]);
    display_board(board);
    getchar();
    p(board,depth-1);
    undo_move(board);
    
    printf("Depth: %d\n",depth);
    
}