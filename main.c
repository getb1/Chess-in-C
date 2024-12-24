#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>

#include "board.h"
#include "misc.h"




#define BOARD_SIZE 64

int main() {
    
    board_t * the_board = init_board();
    precomputePawnMove(12,-1);
    char fen[] = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    
    display_bitBoard(the_board->KNIGHT_MOVES[30]);
    display_bitBoard(set_bit(30,0ULL,1));
    return 0;
}
