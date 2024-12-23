#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>

#include "board.h"
#include "misc.h"




#define BOARD_SIZE 64


/* Defnition of the boar type and its relating funcions */

int main() {
    
    board_t * the_board = init_board();
    precomputePawnMove(12,-1);
    char fen[] = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    precomputePawnMoves(the_board);
    printf("\n%d",the_board->zorbist_hash);
    //init_from_FEN(fen);
    return 0;
}
