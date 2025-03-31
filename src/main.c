#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>

#include "board.h"
#include "misc.h"
#include "perft.h"
#include "search.h"


#define BOARD_SIZE 64
#define START "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 "

int main() {
    
    //Magic bitboard test
    board_t * board = init_board();
   
    move_t * move = move_find_best_move(board, 4);
    printf("Best move: %s\n",move_to_string(move));
    
    
    return 0;
}
