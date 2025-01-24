#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>

#include "board.h"
#include "misc.h"

#define BOARD_SIZE 64

/*TODO
    Work out where piece can move on given square
    attack maps
    checks
    checkmates
    stalemates
    get lebgal moves for a side in position*/

int main() {
    
    board_t * the_board = init_board();

    char fen[] = "rnbqkbnr/pp1ppppp/8/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq - 1 2";
    board_t * b = init_from_FEN(fen);
    display_board(b);
    
    return 0;
}
