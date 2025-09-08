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
    - 7/9/25  - 8,845,444 - optimised pawn move gen
*/
// Isues to fix
// - Legal move gen with check info


#define BOARD_SIZE 64
#define START "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"




int main() {
    printf("Starting Chess Engine...\n");
    char fen[] = START;
    //Magic bitboard test
    board_t * board = init_from_FEN(fen);
    board_stack_t * stack = c_stack();

    // Add this line at the placeholder:
   play();
    
    return 0;
}
