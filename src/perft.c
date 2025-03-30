#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "board.h"
#include "misc.h"

int perft(board_t * board, int depth, board_stack_t * stack) {
    if (depth == 0) {
        return 1;
    }
    move_t legal_moves[300];
    get_legal_move_side(board, board->turn, legal_moves);
    int nodes = 0;
    for (int i = 0; i < 300; i++) {

        if (legal_moves[i].from == 0 && legal_moves[i].to == 0) {
            break;
        }

        if(depth==8) {
            printf("%d",i);
        }
        make_move(board, &legal_moves[i], stack);
        nodes += perft(board, depth - 1, stack);
        board = undo_move(stack, board);
    }
    return nodes;
}


void perft_divide(board_t *board, int depth) {
    move_t moves[300];
    get_legal_move_side(board, board->turn, moves);
    int total = 0;
    
    for (int i = 0; i < 300; i++) {
        if (moves[i].from == 0 && moves[i].to == 0) {
            break;
        }
        board_stack_t * stack = c_stack();
        make_move(board, &moves[i], stack);

        int count = perft(board, depth - 1,stack);
        char * from = int_to_char_sq(moves[i].from);
        char * to = int_to_char_sq(moves[i].to);
        printf("%d:%s%s: %d\n",i, from, to, count);
        free(from);
        free(to);
        
        total += count;
        undo_move(stack, board);
    }
    printf("Total: %d\n", total);
}



void compare_boards(board_t *board1, board_t *board2) {
    if (board1->WHITE != board2->WHITE) {
        printf("White pieces differ\n");
        display_bitBoard(board1->WHITE);
        display_bitBoard(board2->WHITE);
        printf("\n");
    }
    if (board1->BLACK != board2->BLACK) {
        printf("Black pieces differ\n");
    }
    if (board1->PAWNS != board2->PAWNS) {
        printf("Pawns differ\n");
    }
    if (board1->ROOKS != board2->ROOKS) {
        printf("Rooks differ\n");
    }
    if (board1->KNIGHTS != board2->KNIGHTS) {
        printf("Knights differ\n");
    }
    if (board1->BISHOPS != board2->BISHOPS) {
        printf("Bishops differ\n");
    }
    if (board1->QUEENS != board2->QUEENS) {
        printf("Queens differ\n");
    }
    if (board1->KINGS != board2->KINGS) {
        printf("Kings differ\n");
    }

    if (board1->castleFlags != board2->castleFlags) {
        printf("Castle flags differ\n");
    }
    if (board1->enPassantsq != board2->enPassantsq) {
        printf("En passant squares differ\n");
    }
    if (board1->turn != board2->turn) {
        printf("Turn differs\n");
    }
    if (board1->halfMoveCLock != board2->halfMoveCLock) {
        printf("Half move clock differs\n");
    }
    
}