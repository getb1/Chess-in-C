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
    int castleFlags;
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


board_t * init_from_FEN(char fen[]) {
    printf("HELLO");
    board_t * board = NULL;
    board = (board_t *) malloc(sizeof(board_t));
    board->WHITE = 0x0000000000000000;
    board->BLACK = 0x0000000000000000;
    board->BISHOPS = 0x0000000000000000;
    board->PAWNS = 0x0000000000000000;
    board->ROOKS = 0x0000000000000000;
    board->KNIGHTS = 0x0000000000000000;
    board->QUEENS = 0x0000000000000000;
    board->KINGS = 0x0000000000000000;
    
    char * token = strtok(fen, " ");
    printf("HELLO");

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
    
    
    

    
    int j=0;
    int sqNum=-1;
    for(int i=0;i<8;++i) {
        int pos =0;
        
        printf("HELLO");
        (void) getc(stdin);
        printf("%i",sqNum);

        while (pieces[j] !='/'&&pieces[j]!='\0'){
            if(isdigit(pieces[j])) {
                pos = pos + (pieces[j]-'0');
                
            } else {
                sqNum = coordinates_to_number(i,pos);
            if(isupper(pieces[j])) {
                
                
                board->WHITE = set_bit(sqNum, board->WHITE, 1);
            } else{
                board->BLACK = set_bit(sqNum, board->BLACK, 1);
            }
            printf("%c", pieces[j]);
            switch (tolower(pieces[j]))
            {
            case 'p':
                board->PAWNS = set_bit(sqNum, board->PAWNS, 1);
                break;
            case 'r':
                board->ROOKS = set_bit(sqNum, board->ROOKS, 1);
                break;
            case 'n':
                board->KNIGHTS = set_bit(sqNum, board->KNIGHTS, 1);
                printf("%lld", board->KNIGHTS);
                break;
            case 'b':
                board->BISHOPS = set_bit(sqNum, board->BISHOPS, 1);
                break;
            case 'q':
                board->QUEENS = set_bit(sqNum, board->QUEENS, 1);
                break;
            case 'k':
                board->KINGS = set_bit(sqNum, board->KINGS, 1);
                break;
            default:
                break;
            }
        
        }
        j++;
        pos++;
        }
        

    display_board(board);
    j++;
    
}return board;}





int main() {
    printf("HELLO");
    (void) getc(stdin);
    //board_t * the_board = init_board();
    printf("HELLO");
    (void) getc(stdin);
    char fen[] = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    
    init_from_FEN(fen);
    printf("HELLO");
    (void) getc(stdin);
    return 0;
}
