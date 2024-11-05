#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

typedef unsigned long long int U64;

#define BOARD_SIZE 64

int get_bit(int pos, int number) {
    return 1 & (number>>pos);
}

int set_bit(int pos, int number, int new_bit) {
    if(new_bit) {
    return (1<<pos) | number;
    } else {
        return (number & (~(1 << (pos))));
    }
}

int toggle_bit(int pos, int number) {
    return (number ^ (1 << (pos)));
}

/* Defnition of the boar type and its relating funcions */
typedef struct BOARD {
    U64 WHITE;
    U64 BLACK;
    U64 PAWNS;
    U64 ROOKS;
    U64 KNIGHTS;
    U64 BISHOPS;
    U64 QUEENS;
    U64 KINGS;
} board_t;

board_t * init_board() {
    board_t * new = NULL;
    new = (board_t *) malloc(sizeof(board_t));

    new->WHITE = 0xffff000000000000;
    new->BLACK = 0x000000000000ffff;
    new->PAWNS = 0x00ff00000000ff00;
    new->ROOKS = 0x8100000000000081;
    new->KNIGHTS=0x4200000000000042;
    new->BISHOPS=0x2400000000000024;
    new->QUEENS =0x1000000000000010;
    new->KINGS = 0x0800000000000008;

    return new;
}

int coordinates_to_number(int row, int col) {
    return (8*row)+col;
}

char get_piece_at_square(board_t * board, int square) {

    int boards[6] = {board->PAWNS, board->ROOKS, board->KNIGHTS, board->BISHOPS, board->QUEENS, board->KINGS};
    
    char characters[6] = "prnbqk";
    char to_return;

    for(int i=0;i<BOARD_SIZE;++i) {
        
        if(get_bit(square,boards[i])) {
            to_return = characters[i];
            
            break;
        }
    }
    
    to_return = get_bit(square, board->WHITE) ? toupper(to_return) : to_return;
    return to_return;

}

void display_board(board_t * board) {
    for(int i=0;i<17;++i) {
        printf("-");
    }
    
    for(int i=0;i<8;++i) {
        printf("\n|");

        for(int j=0;j<8;++j) {
            char piece = get_piece_at_square(board,coordinates_to_number(i,j));
            
            printf("%c|",piece);
        }
    }
    printf("\n");
    for(int i=0;i<17;++i) {
        printf("-");
    }

}

int main() {

    board_t * the_board = init_board();

    //printf("%c",get_piece_at_square(the_board, 40));
    printf("%d",get_bit(the_board->ROOKS, 56));
    //display_board(the_board);
    
    return 0;
}