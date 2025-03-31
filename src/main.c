#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>

#include "board.h"
#include "misc.h"
#include "perft.h"
#include "magic_numbers.h"

#define BOARD_SIZE 64
#define START "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 "

int main() {
    
    //Magic bitboard test
    board_t * board = init_board();
    generate_blocker_boards_rooks(board);
    
    find_magic_number_for_rooks(10,board);
    
    
    
    return 0;
}
