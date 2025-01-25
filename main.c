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

    char fen[] = "rnb1r3/p1ppnppp/1pkb1q2/4p3/1P1P1PPP/2Q1BN1R/PPP1P3/RN3B1K w - - 0 1";
    board_t * b = init_from_FEN(fen);
    display_board(b);
    display_bitBoard(generate_attack_maps(b,1));
    printf("%d",in_check(b,1));
    return 0;
}
