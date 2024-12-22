#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>


typedef unsigned long long int U64;
typedef __uint64_t hash_t;


#define BOARD_SIZE 64

int get_bit(int pos, U64 number) {
    return 1 & (number>>pos);
}

hash_t rand64(void) {
  uint64_t r = 0;
  for (int i=0; i<64; i += 15 /*30*/) {
    r = r*((uint64_t)RAND_MAX + 1) + rand();
  }
  return r;
}

U64 set_bit(int pos, U64 number, int new_bit) {
    return ((U64)1<<pos) | number;
    
}

U64 toggle_bit(int pos, U64 number) {
    return (number ^ (1 << (pos)));
}

int coordinates_to_number(int rank, int file) {
    return (8*rank)+file;
}

/* Defnition of the boar type and its relating funcions */
typedef struct Move {
    int from;
    int to;
    int piece;
}move_t;

typedef struct BOARD {
    //bitboards
    U64 WHITE;
    U64 BLACK;
    U64 PAWNS;
    U64 ROOKS;
    U64 KNIGHTS;
    U64 BISHOPS;
    U64 QUEENS;
    U64 KINGS;
    // other data
    int enPassantsq;
    int moves;
    int turn;
    int halfMoveCLock;
    int castleFlags; // 4 bit number 1111 where bits 0 and 1 are black QK and same for white

    hash_t zorbist_table[12][64];
    hash_t zorbist_castle_table[4];
    hash_t zorbist_en_passant[8];
    hash_t zorbist_to_move;
    hash_t zorbist_hash;
    // Moves
    U64 WHITE_PAWN_MOVES[64];
    U64 BLACK_PAWN_MOVES[64];
} board_t;

hash_t update_board_hash(board_t* board, int update, move_t* move) {
    hash_t hash=board->zorbist_hash;



}

void display_bitBoard(U64 bitboard) {
    int sq;
    printf("\n");
    printf("%u \n",bitboard);
    for(int i=7;i>=0;--i) {
        for(int j=7;j>=0;j--) {
            sq = coordinates_to_number(i,j); 
            if(get_bit(sq,bitboard)){
                
                printf("* ");
            } else {
                printf("· ");
            }
        }
        printf("\n");
    }
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
    
    for(int i=8;i>=0;--i) {
        printf("\n|");

        for(int j=7;j>=0;--j) {
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

    if(strlen(sq)!=2) {return -1;}

    int file=sq[0]-'a';
    int rank=8-(sq[1]-'0');

    if(file<0||file>7 || rank<0||rank>7) {return -1;}

    return rank*8 + file;
}

board_t * init_from_FEN(char fen[]) {
    
    board_t * board = NULL;
    board = (board_t *) malloc(sizeof(board_t));


    //board->zorbist_table = init_zorbisttable();
    board->WHITE = 0ULL;
    board->BLACK = 0ULL;
    board->BISHOPS = 0ULL;
    board->PAWNS = 0ULL;
    board->ROOKS = 0ULL;
    board->KNIGHTS = 0ULL;
    board->QUEENS = 0ULL;
    board->KINGS = 0ULL;
    board->enPassantsq=-1;

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
    
    // loop through the pieces in the fen and add them to the board
    int rank=7, file=0;

    for(int i=0; pieces[i]!='\0' && pieces[i]!= ' '; ++i) {
        if(pieces[i]=='/') {
            rank--;
            file=0; // if the piece is a slash then we can move down to the next rank in the baord
                    // and return to the first square in it
        } else if(isdigit(pieces[i])) {
            file+= pieces[i]-'0';
        } else {
            int sqNum = (7-file)+8*rank; // locate where the piece should go on the board
            U64 mask = 1ULL << sqNum;

            // add the piece to the correct colour board depending on its capitalisation 
            if(isupper(pieces[i])) {
                board->WHITE|=mask;
            } else {
                board->BLACK|=mask;
            }
            // place the piece onto the correct board
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
    // choose the correct turn depending on the charachter and add correct castle flags
    int bits[4] = {0,1,2,3};
    char symbols[] = "qkQK";

    for(int i=0;i<4;i++) {
        if(strchr(castle, symbols[i])) {
        board->castleFlags = set_bit(i, board->castleFlags, 1);}
    }
    // add the rst of the data to the board structure
    board->enPassantsq=char_to_intsq(enpass);
    board->halfMoveCLock = *halfmove_clock-'0';
    board->moves = *moves-'0';

    return board;
}

U64 precomputePawnMove(int square, int direction) {
    int rank = square/8;
    int file = square%8;
    
    U64 move=0ULL;
    if (rank!=7 && rank!=0) {
        if(rank==6&&direction==-1) {
            move = set_bit(coordinates_to_number(4,file),move,1);
            return set_bit(coordinates_to_number(5,file),move,1);
        } else if(rank==1&&direction==1) {
            
            move = set_bit(coordinates_to_number(3,file),move,1);
            return set_bit(coordinates_to_number(2,file),move,1);
        }
        else {
            return set_bit(coordinates_to_number(rank+direction,file),move,1);
        }
    }
    
    return 0ULL;
}

void precomputePawnMoves(board_t * board) {

    for(int i=0;i<64;++i) {
        board->WHITE_PAWN_MOVES[i] = precomputePawnMove(i,1);
        board->BLACK_PAWN_MOVES[i] = precomputePawnMove(i,-1);
    }
}

hash_t init_zorbisttable(board_t * board) {
    for(int i=0;i<12;++i) {
        for(int j=0;j<64;++j) {
            board->zorbist_table[i][j]=rand64();
        }
    }
    for(int i=0;i<4;++i) {
        board->zorbist_castle_table[i]=rand64();
    }
    for(int i=0;i<8;++i) {
        board->zorbist_en_passant[i]=rand64();
    }
    
    hash_t hash = 0;
    for(int i=0;i<2;++i){
        
        switch (get_piece_at_square(board,i)) {
            case 'P' : hash ^= board->zorbist_table[0][i]; break;
            case 'R' : hash ^= board->zorbist_table[1][i]; break;
            case 'N' : hash ^= board->zorbist_table[2][i]; break;
            case 'B' : hash ^= board->zorbist_table[3][i]; break;
            case 'Q' : hash ^= board->zorbist_table[4][i]; break;
            case 'K' : hash ^= board->zorbist_table[5][i]; break;
            case 'p' : hash ^= board->zorbist_table[6][i]; break;
            case 'r' : hash ^= board->zorbist_table[7][i]; break;
            case 'n' : hash ^= board->zorbist_table[8][i]; break;
            case 'b' : hash ^= board->zorbist_table[9][i]; break;
            case 'q' : hash ^= board->zorbist_table[10][i]; break;
            case 'k' : hash ^= board->zorbist_table[11][i]; break;
            default : printf("HELLO"); break;
        }
            
        
    }
    return hash;
}

board_t * init_board() {
    board_t * new = NULL;
    new = (board_t *) malloc(sizeof(board_t));

    new->WHITE = 0x000000000000ffff;
    new->BLACK = 0xffff000000000000;
    new->PAWNS = 0x00ff00000000ff00;
    new->ROOKS = 0x8100000000000081;
    new->KNIGHTS=0x4200000000000042;
    new->BISHOPS=0x2400000000000024;
    new->QUEENS =0x1000000000000010;
    new->KINGS = 0x0800000000000008;
    new->zorbist_hash = init_zorbisttable(new);
    precomputePawnMoves(new);
    return new;
}
// Board Functions End Here
int main() {
    
    board_t * the_board = init_board();
    precomputePawnMove(12,-1);
    char fen[] = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    precomputePawnMoves(the_board);
    printf("\n%d",the_board->zorbist_hash);
    //init_from_FEN(fen);
    return 0;
}
