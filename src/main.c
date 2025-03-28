#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>

#include "board.h"
#include "misc.h"
#include "perft.h"

#define BOARD_SIZE 64
#define START "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 "

int main() {
    
    char fen[] = "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8";
    
    board_t * board = init_from_FEN(fen);
    move_t moves[300];
    board_stack_t * stack = c_stack();
    get_legal_move_side(board,board->turn, moves);
    make_move(board, &moves[41], stack);
    display_board(board);
    

    //play();
    perft_divide(board,2);
    board_stack_t * s = c_stack();
    
    free(s);
    free(board);
    
    return 0;
}
