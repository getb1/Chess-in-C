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
    
    char fen[] = "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8";
    
    board_t * board = init_from_FEN(fen);

    char fen_2[] = "rnbq1k1r/pp1Pbppp/2p5/8/2B5/7n/PPP1N1PP/RNBQ1RK1 w - - 3 9";

    board_t * board_2 = init_from_FEN(fen_2);

    

    //play();
    perft_divide(board,5);
    board_stack_t * s = c_stack();
    
    free(s);
    free(board);
    
    return 0;
}
