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
    
    char fen[] = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
    
    board_t * board = init_from_FEN(fen);

    char fen_2[] = "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1";

    board_t * board_2 = init_from_FEN(fen_2);
    
    board_stack_t * s = c_stack();
    move_t moves[300];
    get_legal_move_side(board_2,board_2->turn, moves);
    make_move(board_2,&moves[0],s);
    get_legal_move_side(board_2,board_2->turn, moves);
    make_move(board_2,&moves[0],s);
    
    
    //play();
    display_board(board_2);
    perft_divide(board_2,1);
    
    
    free(s);
    free(board);
    
    return 0;
}
