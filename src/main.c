#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>

#include "board.h"
#include "misc.h"
#include "perft.h"

#define BOARD_SIZE 64
#define START "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 "

int main() {
    
    char fen[] = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
    
    board_t * board = init_from_FEN(fen);
    display_board(board);

    //play();
    //move_test();
    board_stack_t * s = c_stack();
    
    printf("%d,",perft(board,4,s));
    free(s);
    free(board);
    
    



    
    
    return 0;
}
