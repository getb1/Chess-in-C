#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>

#include "board.h"
#include "misc.h"
#include "perft.h"

#define BOARD_SIZE 64


int main() {
    
    char fen[] = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    
    board_t * board = init_from_FEN(fen);

    //play();

    board_stack_t * s = c_stack();

    play();
    
    



    
    
    return 0;
}
