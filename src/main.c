#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>

#include "board.h"
#include "misc.h"

#define BOARD_SIZE 64

/*TODO
    Work out where piece can move on given square - Castling
    checkmates
    stalemates
    get legal moves for a side in position*/

int main() {
    
    char fen[] = "r1bqkbnr/pppp1ppp/2n5/4p3/4P3/5N2/PPPP1PPP/RNBQKB1R w KQkq - 2 4";
    
    board_t * board = init_board();

    play();

    
    return 0;
}
