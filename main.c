#include <stdio.h>
#include <stdlib.h>

typedef unsigned long long int U64;

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
    new->QUEENS =0x1000000000000010;
    new->KINGS = 0x0800000000000008;

    return new;
}

int main() {

    board_t * the_board = init_board();

    printf("\n%d",get_bit(0,5));

    return 0;
}