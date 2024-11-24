#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>




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

int coordinates_to_number(int row, int col) {
    return (8*row)+col;
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

    int enPassantsq;
    int moves;
    int turn;
    int halfMoveCLock;
    int castleFlags; // 4 bit number 1111 where bits 0 and 1 are black QK and same for white
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

char get_piece_at_square(board_t * board, int square) {
    U64 boards[] = {board->PAWNS, board->ROOKS, board->KNIGHTS, board->BISHOPS, board->QUEENS, board->KINGS};
    char characters[] = "prnbqk";
    char to_return = '.';  // Default to empty square

    U64 mask = 1ULL << square;

    for(int i = 0; i < 6; ++i) {
        if(boards[i] & mask) {
            to_return = characters[i];
            break;
        }
    }

    if (to_return != '.') {
        if (board->WHITE & mask) {
            to_return = toupper(to_return);
        }
    }

    return to_return;
}

void display_board(board_t * board) {
    printf("\n");
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

int turn_to_int(char turn) {
    return (turn =='w'|| turn=='W') ? 1:0;
}

int char_to_intsq(char sq[]) {
    char file = sq[0];
    char rank = sq[1];

    char files[] = "hgfedcba";
    int iRank = rank-'1';
    int iFIle;
    for(int i=0;i<8;++i) {
        if(files[i]==files) {
            iFIle=i;
            break;
        }
    }

    return  (iRank*8)+iFIle;
}

board_t * init_from_FEN(char fen[]) {
    
    board_t * board = NULL;
    board = (board_t *) malloc(sizeof(board_t));
    board->WHITE = 0ULL;
    board->BLACK = 0ULL;
    board->BISHOPS = 0ULL;
    board->PAWNS = 0ULL;
    board->ROOKS = 0ULL;
    board->KNIGHTS = 0ULL;
    board->QUEENS = 0ULL;
    board->KINGS = 0ULL;
    //board->turn = 0;
    char * token = strtok(fen, " ");
    
    // Split the string into individual parts for parsing
    char * pieces = token;
    token = strtok(NULL, " ");
    char * turn = token;
    token = strtok(NULL, " ");
    char * castle = token;
    token = strtok(NULL, " ");
    char * enpass = token;
    token = strtok(NULL, " ");
    char * halfmove_clock = token;
    token = strtok(NULL, " ");
    char * moves = token;
    

    int rank=0, file=0;

    for(int i=0; pieces[i]!='\0' && pieces[i]!= ' '; ++i) {
        if(pieces[i]=='/') {
            rank++;
            file=0;
        } else if(isdigit(pieces[i])) {
            file+= pieces[i]-'0';
        } else {
            int sqNum = rank*8+file;
            U64 mask = 1ULL << sqNum;

            if(isupper(pieces[i])) {
                board->WHITE|=mask;
            } else {
                board->BLACK|=mask;
            }

            switch (tolower(pieces[i])) {
             case 'p': board->PAWNS |=mask; break;
             case 'r': board->ROOKS |=mask; break;
             case 'n': board->KNIGHTS |=mask; break;
             case 'b': board->BISHOPS |=mask; break;
             case 'q': board->QUEENS |=mask; break;
             case 'k': board->KINGS |=mask; break;
            }
            file++;
        }

    }

    board->turn = turn_to_int(turn[0]);

    int bits[4] = {0,1,2,3};
    char symbols[] = "qkQK";

    for(int i=0;i<4;i++) {
        if(strchr(castle, symbols[i])) {
        board->castleFlags = set_bit(i, board->castleFlags, 1);}
    }
    
    printf("%i", board->castleFlags);
    printf("%i",board->turn);
display_board(board);
return board;}





int main() {
    
    //board_t * the_board = init_board();
    
    char fen[] = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    
    init_from_FEN(fen);
    
    return 0;
}
