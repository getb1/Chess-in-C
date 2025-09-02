#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

#include "board.h"
#include "misc.h"
#include "perft.h"
#include "search.h"
#include "magic_numbers.h"


// Moves Gen Speed Tracker 
/*  - 27/8/25 - 480,000 moves per second 
    - 28/8/25 - 1,674,860 moves per second - added multi threading
    - 28/8/25 - 2,000,000 - added some if statements
    - 28/8/25 - 2,129,303 - removed some for loops
    - 29/8/25 - 2,692,067 - removed more for loops
    - 30/8/25 - 3,925,379 - removed more for loops
*/
// Isues to fix
// - Legal move gen with check info


#define BOARD_SIZE 64
#define START "4k2r/rpp2ppp/1b2n1bN/nPPpP3/BB6/q4N2/Pp1P2PP/R2Q1RK1 w k d6 0 10"

int main() {
    printf("Starting Chess Engine...\n");
    char fen[] = START;
    //Magic bitboard test
    board_t * board = init_from_FEN(fen);
    board_stack_t * stack = c_stack();
    move_t moves[300];
    display_board(board);
    check_info_t info = generate_check_info(board);
    //generate_rook_blocker_bitboards(board);
    display_bitBoard(get_legal_moves_for_pawn_at_sqaure(board,35,1,info));

    //test(board);
    
    return 0;
}
