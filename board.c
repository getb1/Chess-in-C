#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>
#include "board.h"
#include "misc.h"

int on_board(int pos) {
    if(pos>=0&&pos<64) {
        return 1;
    }
    return 0;
}

int on_board_rank_file(int rank, int file) {
    if(rank>=0&&rank<8&&file>=0&&file<8) {
        return 1;
    }
    return 0;
}

int get_rank(int sq) {
    return (int) sq /8;
}
int get_file(int sq) {
    return sq%8;
}

void cool() {
    for(int i=7;i>=0;--i) {
        for(int j=7;j>=0;--j) {
            printf("%d ",coordinates_to_number(i,j));
        }
        printf("\n");
    }
}

void display_bitBoard(U64 bitboard) {
    int sq;
    printf("\n");
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
    
    for(int i=7;i>=0;--i) {
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

void precomputeKnightMoves(board_t * board) {
    U64 move;
    int rank,file,pos;
    const int directions[8][2] = {{-2,-1},{-2,1},{-1,2},{1,2},{2,1},{2,-1},{-1,-2},{1,-2}};
    for(int i=0;i<64;++i) {
        
        move = 0ULL;
        
        
        for(int j=0;j<8;++j) {
            rank = get_rank(i);
            file = get_file(i);
            rank += directions[j][0];
            file += directions[j][1];

            if(on_board_rank_file(rank,file)) {
                pos = coordinates_to_number(rank,file);
                move= set_bit(pos,move,1);
            }
        }
        board->KNIGHT_MOVES[i]=move;
    }

}

void precompute_rook_moves(board_t * board) {
    U64 move;
    int rank,file;
    for(int i=0;i<64;++i) {
        move = 0ULL;
        rank = get_rank(i);
        file = get_file(i);
        for(int j=0;j<8;++j) {
            if(coordinates_to_number(rank,j)!=i) {
                move |= set_bit(coordinates_to_number(rank,j),move,1);
            } 
            if(coordinates_to_number(j,file)!=i) {
                move|=set_bit(coordinates_to_number(j,file),move,1);
            }
        }
        board->ROOK_MOVES[i]=move;
    }
}

void precompute_bishop_moves(board_t * board) {
    const int directions[4][2] = {{1,1},{-1,-1},{-1,1},{1,-1}};
    U64 move;
    int rank,file;

    for(int i=0;i<64;++i) {
        rank = get_rank(i);
        file = get_file(i);
        move = 0ULL;
        for(int j=1;j<9;++j) {
            for(int k=0;k<4;++k) {
                if (on_board_rank_file(rank+(directions[k][0]*j),file+(directions[k][1]*j)))
                {
                    
                    move |= set_bit(coordinates_to_number(rank+(directions[k][0]*j),file+(directions[k][1]*j)),move,1);
                }
                
            }
        }
        
        board->BISHOP_MOVES[i]=move;
    }


}

void precompute_queen_moves(board_t * board) {
    for(int i=0;i<64;i++) {
        board->QUEEN_MOVES[i] = board->ROOK_MOVES[i]|board->BISHOP_MOVES[i];
    }
}

void precompute_king_moves(board_t * board) {
    int directions[8][2] = {{-1,-1},{0,-1},{1,-1},{1,0},{1,1},{0,1},{-1,1},{-1,0}};
    int rank,file,d_file,d_rank,new_file,new_rank;
    for(int i=0;i<64;++i) {
        board->KING_MOVES[i]=0ULL;
        rank = get_rank(i);
        file = get_file(i);
        for(int j=0;j<8;++j) {
            d_file = directions[j][0];
            d_rank = directions[j][1];

            new_rank = rank+d_rank;
            new_file = file+d_file;

            if(on_board_rank_file(new_rank,new_file)) {
                board->KING_MOVES[i]|=set_bit(coordinates_to_number(new_rank,new_file),board->KING_MOVES[i],1);
            }
        }
    }
}

U64 get_attacks_for_knight_at_square(board_t * board,int pos,int colour) {
    U64 colour_board = colour ? board->WHITE : board->BLACK;
    //colour_board = ~colour_board;
    
    return (~(colour_board)&board->KNIGHT_MOVES[pos]);
}

U64 get_attacks_for_bishop_at_square(board_t * board,int pos, int colour) {
    const int directions[4][2] = {{1,1},{-1,-1},{-1,1},{1,-1}};
    int allowed[4] = {1, 1, 1, 1};
    U64 move = board->BISHOP_MOVES[pos];

    int rank = get_rank(pos);
    int file = get_file(pos);

    int d_rank,d_file,new_rank,new_file;
    const U64 colour_board = colour ? board->WHITE : board->BLACK;
    const U64 opponet_board= colour ? board->BLACK : board->WHITE;

    for(int i=0;i<8;++i) {
        for(int j=0;j<4;++j) {
            d_rank = directions[j][0]*(i+1);
            d_file = directions[j][1]*(i+1);

            new_rank = rank+d_rank;
            new_file = file+d_file;
            
            if(on_board_rank_file(new_rank,new_file)) {
            
            if(allowed[j]==1) {
                
                if(get_bit(coordinates_to_number(new_rank,new_file),colour_board)) {
                    move &= clear_bit(coordinates_to_number(new_rank,new_file),move);
                    
                    allowed[j]=0;
                } else if(get_bit(coordinates_to_number(new_rank,new_file),opponet_board)) {
                    allowed[j]=0;
                }
            } else {
                move &= clear_bit(coordinates_to_number(new_rank,new_file),move);
            }

            }
        }
    }
    return move;

}

U64 get_attacks_for_rook_at_square(board_t * board,int pos, int colour) {
    const int directions[4][2] = {{0,1},{0,-1},{1,0},{-1,0}};
    int allowed[4] = {1, 1, 1, 1};
    U64 move = board->ROOK_MOVES[pos];

    int rank = get_rank(pos);
    int file = get_file(pos);

    int d_rank,d_file,new_rank,new_file;
    const U64 colour_board = colour ? board->WHITE : board->BLACK;
    const U64 opponet_board= colour ? board->BLACK : board->WHITE;

    for(int i=0;i<8;++i) {
        for(int j=0;j<4;++j) {
            d_rank = directions[j][0]*(i+1);
            d_file = directions[j][1]*(i+1);

            new_rank = rank+d_rank;
            new_file = file+d_file;
            
            if(on_board_rank_file(new_rank,new_file)) {
            
            if(allowed[j]==1) {
                
                if(get_bit(coordinates_to_number(new_rank,new_file),colour_board)) {
                    move &= clear_bit(coordinates_to_number(new_rank,new_file),move);
                    
                    allowed[j]=0;
                } else if(get_bit(coordinates_to_number(new_rank,new_file),opponet_board)) {
                    allowed[j]=0;
                }
            } else {
                move &= clear_bit(coordinates_to_number(new_rank,new_file),move);
            }

            }
        }
    }
    return move;

}

U64 get_attacks_for_queen_at_square(board_t * board,int pos, int colour) {
    return get_attacks_for_rook_at_square(board,pos,colour)|get_attacks_for_bishop_at_square(board,pos,colour);
}

U64 get_attacks_for_king_at_square(board_t * board, int pos, int colour) {
    U64 colour_board = colour ? board->WHITE : board->BLACK;
    //colour_board = ~colour_board;
    
    return (~(colour_board)&board->KING_MOVES[pos]);
}

U64 get_attacks_for_pawn_at_square(board_t * board,int pos, int colour) {
    U64 *colour_moves = colour ? board->WHITE_PAWN_MOVES: board->BLACK_PAWN_MOVES;
    int direction = colour ? 1:-1;
    U64 colour_board = colour ? board->WHITE : board->BLACK;
    if(colour_moves[pos]==0){
        return 0ULL;
    }

    U64 move = 0ULL;

    int rank = get_rank(pos);
    int file = get_file(pos);

    if(on_board_rank_file(rank+direction,file+1)&&get_bit(coordinates_to_number(rank+direction,file+1),colour_board)!=1) {
        move |= set_bit(coordinates_to_number(rank+direction,file+1),move,1);
    }
    if(on_board_rank_file(rank+direction,file-1)&&get_bit(coordinates_to_number(rank+direction,file-1),colour_board)!=1) {
        move |= set_bit(coordinates_to_number(rank+direction,file-1),move,1);
    }

    return move;
}

U64 generate_attack_maps(board_t * board,int colour) {

    U64 boards[6] = {board->ROOKS,board->KNIGHTS,board->BISHOPS,board->KINGS,board->QUEENS,board->PAWNS};

    U64 colour_board = colour ? board->WHITE : board->BLACK;
    U64 map = 0ULL;
    for(int i=0;i<64;++i) {
        if(get_bit(i,colour_board)) {
            if(get_bit(i,board->ROOKS)) {
                map |= get_attacks_for_rook_at_square(board,i,colour);
            } else if(get_bit(i,board->KNIGHTS)) {
                map |= get_attacks_for_knight_at_square(board,i,colour);
            } else if(get_bit(i,board->BISHOPS)) {
                map |= get_attacks_for_bishop_at_square(board,i,colour);
            } else if(get_bit(i,board->KINGS)) {
                map |= get_attacks_for_king_at_square(board,i,colour);
            } else if(get_bit(i,board->QUEENS)) {
                map |= get_attacks_for_queen_at_square(board,i,colour);
            } else if(get_bit(i,board->PAWNS)) {
                map |= get_attacks_for_pawn_at_square(board,i,colour);
            }
        }
    }
    return map;

}

int in_check(board_t *board, int colour) {

    U64 KING_POS = colour ? board->WHITE & board->KINGS : board->BLACK & board->KINGS;
    U64 attack_map = generate_attack_maps(board, 1 - colour);

    if (KING_POS & attack_map) {
        return 1;
    }
    return 0;
}

int piece_on_square(board_t * board, int pos,int colour) {
    U64 colour_board = ~(colour) ? board->WHITE : board->BLACK;

    if(get_bit(pos,colour_board)) {
        return 1;
    }
    return 0;
}

U64 get_legal_moves_for_knight_at_square(board_t *board, int pos, int colour) {
    U64 possible_moves = get_attacks_for_knight_at_square(board, pos, colour);
    U64 legal_moves = 0ULL;

    for (int i = 0; i < 64; ++i) {
        if (get_bit(i, possible_moves)) {
            U64 original_knights = board->KNIGHTS;
            U64 original_white = board->WHITE;
            U64 original_black = board->BLACK;

            board->KNIGHTS = clear_bit(pos, board->KNIGHTS);
            board->KNIGHTS = set_bit(i, board->KNIGHTS, 1);

            if (colour) {
                board->WHITE = clear_bit(pos, board->WHITE);
                board->WHITE = set_bit(i, board->WHITE, 1);
                if (piece_on_square(board, i, ~(colour))) {
                    board->BLACK = clear_bit(i, board->BLACK);
                }
            } else {
                board->BLACK = clear_bit(pos, board->BLACK);
                board->BLACK = set_bit(i, board->BLACK, 1);
                if (piece_on_square(board, i, ~(colour))) {
                    board->WHITE = clear_bit(i, board->WHITE);
                }
            }

            if (in_check(board, colour) != 1) {
                legal_moves = set_bit(i, legal_moves, 1);
            }
            
            

            // Restore original board state
            board->KNIGHTS = original_knights;
            board->WHITE = original_white;
            board->BLACK = original_black;
        }
    }

    return legal_moves;
}


hash_t get_hash_for_piece_at_square(board_t* board, int pos) {
    switch (get_piece_at_square(board,pos)) {
            case 'P' : return board->zorbist_table[0][pos]; break;
            case 'R' : return board->zorbist_table[1][pos]; break;
            case 'N' : return board->zorbist_table[2][pos]; break;
            case 'B' : return board->zorbist_table[3][pos]; break;
            case 'Q' : return board->zorbist_table[4][pos]; break;
            case 'K' : return board->zorbist_table[5][pos]; break;
            case 'p' : return board->zorbist_table[6][pos]; break;
            case 'r' : return board->zorbist_table[7][pos]; break;
            case 'n' : return board->zorbist_table[8][pos]; break;
            case 'b' : return board->zorbist_table[9][pos]; break;
            case 'q' : return board->zorbist_table[10][pos]; break;
            case 'k' : return board->zorbist_table[11][pos]; break;
            default : ; break;
        }
}

hash_t update_hash(board_t * board, move_t * move) {
    int from = move->from;
    int to = move->to;
    int start = 0;
    if(~(move->colour)) {
        start=6;
    }

    board->zorbist_hash^=board->zorbist_table[start+move->piece][from];
    board->zorbist_hash^=get_hash_for_piece_at_square(board,to);
    return board->zorbist_hash;
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
        hash ^= get_hash_for_piece_at_square(board,i);
    }
    return hash;
}

board_t * init_from_FEN(char fen[]) {
    
    board_t * board = NULL;
    board = (board_t *) malloc(sizeof(board_t));
    board = init_board(board);

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
    init_zorbisttable(board);

    return board;
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
    precomputeKnightMoves(new);
    precompute_rook_moves(new);
    precompute_bishop_moves(new);
    precompute_queen_moves(new);
    precompute_king_moves(new);
    return new;
}
// Board Functions End Here