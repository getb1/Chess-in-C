#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>
#include "board.h"
#include "misc.h"

U64 get_rook_mask(int sq) {
    U64 mask = 0ULL;
    int r = sq/8, f = sq%8;
    
    for(int i=r+1; i<7; i++) mask |= 1ULL << (i*8 + f);
    for(int i=r-1; i>0; i--) mask |= 1ULL << (i*8 + f);
    for(int j=f+1; j<7; j++) mask |= 1ULL << (r*8 + j);
    for(int j=f-1; j>0; j--) mask |= 1ULL << (r*8 + j);
    display_bitBoard(mask);
    return mask;
}


void generate_rook_masks(board_t * board) {
    for(int i=0;i<64;++i) {
        board->ROOK_MASKS[i] = get_rook_mask(i);
    }
}

board_t * copy_board(board_t * original) {
    

    
    board_t* copy = (board_t*)malloc(sizeof(board_t));
    copy->WHITE = original->WHITE;
    copy->BLACK = original->BLACK;
    copy->PAWNS = original->PAWNS;
    copy->ROOKS = original->ROOKS;
    copy->KNIGHTS = original->KNIGHTS;
    copy->BISHOPS = original->BISHOPS;
    copy->QUEENS = original->QUEENS;
    copy->KINGS = original->KINGS;

    // Game state
    copy->enPassantsq = original->enPassantsq;
    copy->moves = original->moves;
    copy->turn = original->turn;
    copy->halfMoveCLock = original->halfMoveCLock;
    copy->castleFlags = original->castleFlags;
    return copy;

}
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

int find_msb(U64 x) {
    if (x == 0) return -1; // No bits are set
    return 63 - __builtin_clzll(x);
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
    printf("\n");
}

int turn_to_int(char turn) {
    return (turn =='w'|| turn=='W') ? 1:0;
}

int char_to_intsq(char sq[]) {
    
    if(strlen(sq)!=2) {return -1;}

    int file = 'h'-tolower(sq[0]);
    
    int rank = sq[1]-'1';
    

    return coordinates_to_number(rank,file);
}

int is_square_empty(board_t * board, int square) {
    return get_piece_at_square(board,square)=='.';
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

U64 remove_edge_squares(U64 bitboard) {
    U64 mask = 0x007E7E7E7E7E7E00; // Mask to remove edge squares
    
    return bitboard & mask;
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

void generate_blocker_boards_rooks(board_t *board) {
    for(int i = 0; i < 64; ++i) {
        U64 move = board->ROOK_MOVES[i];
        int num_bits = __builtin_popcountll(move);
        U64 num_patterns = 1ULL << num_bits;
        
        U64 *blockers = (U64 *)malloc(num_patterns * sizeof(U64));
        if(blockers == NULL) {
            fprintf(stderr, "Memory allocation failed for square %d\n", i);
            exit(EXIT_FAILURE);
        }

        
        for(int j = 0; j < num_patterns; ++j) {
            blockers[j] = 0ULL;
            U64 temp = move;
            int bit_index = 0;
            
            while(temp) {
                int square = __builtin_ctzll(temp);  
                if(j & (1 << bit_index)) {           
                    blockers[j] |= (1ULL << square);
                }
                temp &= temp - 1;  
                bit_index++;
            }
        }
        
        board->ROOK_BLOCKERS[i] = blockers;
    }
    
    display_bitBoard(board->ROOK_BLOCKERS[36][15]);
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
    U64 opponet_board = colour ? board->BLACK : board->WHITE;
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

U64 get_legal_moves_for_rook_at_sqaure(board_t *board, int pos, int colour) {
    U64 possible_moves = get_attacks_for_rook_at_square(board, pos, colour);
    U64 legal_moves = 0ULL;

    for (int i = 0; i < 64; ++i) {
        if (get_bit(i, possible_moves)) {
            U64 original_rooks = board->ROOKS;
            U64 original_white = board->WHITE;
            U64 original_black = board->BLACK;

            board->ROOKS = clear_bit(pos, board->ROOKS);
            board->ROOKS = set_bit(i, board->ROOKS, 1);

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
            board->ROOKS = original_rooks;
            board->WHITE = original_white;
            board->BLACK = original_black;
        }
    }
    
    return legal_moves;
}

U64 get_legal_moves_for_bishop_at_sqaure(board_t *board, int pos, int colour) {
    U64 possible_moves = get_attacks_for_bishop_at_square(board, pos, colour);
    U64 legal_moves = 0ULL;

    for (int i = 0; i < 64; ++i) {
        if (get_bit(i, possible_moves)) {
            U64 original_bishops = board->BISHOPS;
            U64 original_white = board->WHITE;
            U64 original_black = board->BLACK;

            board->BISHOPS = clear_bit(pos, board->BISHOPS);
            board->BISHOPS = set_bit(i, board->BISHOPS, 1);

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
            board->BISHOPS = original_bishops;
            board->WHITE = original_white;
            board->BLACK = original_black;
        }
    }

    return legal_moves;
}

U64 get_legal_moves_for_queen_at_square(board_t *board, int pos, int colour) {
    return get_legal_moves_for_bishop_at_sqaure(board,pos,colour)|get_legal_moves_for_rook_at_sqaure(board,pos,colour);
}

U64 get_legal_moves_for_king_at_sqaure(board_t *board, int pos, int colour) {
    U64 possible_moves = get_attacks_for_king_at_square(board, pos, colour);
    U64 legal_moves = 0ULL;

    
    
    
    for (int i = 0; i < 64; ++i) {
        if (get_bit(i, possible_moves)) {

            U64 original_kings = board->KINGS;
            U64 original_white = board->WHITE;
            U64 original_black = board->BLACK;

            board->KINGS = clear_bit(pos, board->KINGS);
            board->KINGS = set_bit(i, board->KINGS, 1);

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
            board->KINGS = original_kings;
            board->WHITE = original_white;
            board->BLACK = original_black;
        }
        }
        int queen_side_bit,rook_side_bit;
        
        rook_side_bit = colour ? 1 : 3;
        queen_side_bit= colour ? 0:2;
        U64 attack_map = generate_attack_maps(board, !(colour));
        int king_rank = colour?0:7;
        

        if(get_bit(rook_side_bit,board->castleFlags) && !(in_check(board,colour))){
            
            int rook_side_file_a = 2;
            int rook_side_file_b = 1;
            if(is_square_empty(board,coordinates_to_number(king_rank,rook_side_file_a))&&is_square_empty(board,coordinates_to_number(king_rank,rook_side_file_b))) {
                if((get_bit(coordinates_to_number(king_rank,rook_side_file_a),attack_map)==0)&&(get_bit(coordinates_to_number(king_rank,rook_side_file_b),attack_map)==0)) {
                    
                        legal_moves = set_bit(coordinates_to_number(king_rank,rook_side_file_b),legal_moves,1);
                    
                    

                }
            }
        }

        if(get_bit(queen_side_bit,board->castleFlags)&&!(in_check(board,colour))) {
            int queen_side_file_a = 5;
            int queen_side_file_b = 4;
            int queen_side_file_c = 6;
            if(is_square_empty(board,coordinates_to_number(king_rank,queen_side_file_a))&&is_square_empty(board,coordinates_to_number(king_rank,queen_side_file_b))&&is_square_empty(board,coordinates_to_number(king_rank,queen_side_file_c))) {
            if(!((get_bit(coordinates_to_number(king_rank,queen_side_file_a),attack_map)||get_bit(coordinates_to_number(king_rank,queen_side_file_b),attack_map)))) {
                legal_moves = set_bit(coordinates_to_number(king_rank,queen_side_file_a),legal_moves,1);
            }}
        
        }
    
    return legal_moves;
}

U64 get_legal_moves_for_pawn_at_sqaure(board_t * board,int pos, int colour) {
    // get the possible moves for the pawn
    
    int direction = colour ? 1 : -1;
    U64 possible_moves = get_attacks_for_pawn_at_square(board,pos,colour)|precomputePawnMove(pos,direction);
    U64 legal_moves = 0ULL;
    
    U64 colour_board = colour ? board->WHITE : board->BLACK;
    U64 opponet_board = colour ? board->BLACK : board->WHITE;

    

    int rank = get_rank(pos);
    int file = get_file(pos);

    int one_ahead = coordinates_to_number(rank+direction,file);
    int two_ahead = coordinates_to_number(rank+(2*direction),file);
    int left = coordinates_to_number(rank+direction,file-1);
    int right = coordinates_to_number(rank+direction,file+1);

    if(!is_square_empty(board,one_ahead)) {
        
        possible_moves=clear_bit(one_ahead,possible_moves);
        possible_moves=clear_bit(two_ahead,possible_moves);
    }

    if(!is_square_empty(board,two_ahead)) {
        possible_moves=clear_bit(two_ahead,possible_moves);
    }

    if(is_square_empty(board,left)&&board->enPassantsq!=left) {
        possible_moves=clear_bit(left,possible_moves);
    } 
    if(is_square_empty(board,right)&&board->enPassantsq!=right) {
        possible_moves=clear_bit(right,possible_moves);
    }


    if(possible_moves==0) {
        return 0ULL;
    }
    
    
    int msb_pos = find_msb(possible_moves);
    while(possible_moves !=0) {
    U64 original_pawns = board->PAWNS;
    U64 original_white = board->WHITE;
    U64 original_black = board->BLACK;

    board->PAWNS = clear_bit(pos, board->PAWNS);
    board->PAWNS = set_bit(msb_pos, board->PAWNS, 1);
    if (colour) {
        board->WHITE = clear_bit(pos, board->WHITE);
        board->WHITE = set_bit(msb_pos, board->WHITE, 1);
        if (msb_pos == board->enPassantsq && board->enPassantsq != -1) {
            int captured_pos = msb_pos - 8; // Black pawn one rank below
            board->BLACK = clear_bit(captured_pos, board->BLACK);
            board->PAWNS = clear_bit(captured_pos, board->PAWNS);
        } else if (piece_on_square(board, msb_pos, 0)) {
            board->BLACK = clear_bit(msb_pos, board->BLACK);
        }
    } else {
        board->BLACK = clear_bit(pos, board->BLACK);
        board->BLACK = set_bit(msb_pos, board->BLACK, 1);
        if (msb_pos == board->enPassantsq && board->enPassantsq != -1) {
            int captured_pos = msb_pos + 8; // White pawn one rank above
            board->WHITE = clear_bit(captured_pos, board->WHITE);
            board->PAWNS = clear_bit(captured_pos, board->PAWNS);
        } else if (piece_on_square(board, msb_pos, 1)) {
            board->WHITE = clear_bit(msb_pos, board->WHITE);
        }
    }

    if (in_check(board, colour) != 1) {
        legal_moves = set_bit(msb_pos, legal_moves, 1);
    }
    possible_moves = clear_bit(msb_pos, possible_moves);
    msb_pos = find_msb(possible_moves);
    board->PAWNS = original_pawns;
    board->WHITE = original_white;
    board->BLACK = original_black;
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
    
    
    board_t * board = init_board();

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
    board->castleFlags=0ULL;

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
    int bits[4] = {3,2,1,0};
    char symbols[] = "QKqk";
    
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
    
    if (new == NULL) {
        fprintf(stderr, "Failed to allocate memory for board\n");
        exit(EXIT_FAILURE);
    }
    

    new->WHITE = 0x000000000000ffff;
    new->BLACK = 0xffff000000000000;
    new->PAWNS = 0x00ff00000000ff00;
    new->ROOKS = 0x8100000000000081;
    new->KNIGHTS=0x4200000000000042;
    new->BISHOPS=0x2400000000000024;
    new->QUEENS =0x1000000000000010;
    new->KINGS = 0x0800000000000008;
    new->zorbist_hash = init_zorbisttable(new);
    new->turn=1;
    new->castleFlags = 0x0f;
    new->enPassantsq = -1;
    new->halfMoveCLock = 0;
    new->moves = 0;
    precomputePawnMoves(new);
    precomputeKnightMoves(new);
    precompute_rook_moves(new);
    precompute_bishop_moves(new);
    precompute_queen_moves(new);
    precompute_king_moves(new);
    
    return new;
    
}

//Putting it all together

U64 get_legal_moves_for_side_bitboards(board_t * board,int colour) {
    U64 legal_moves = 0ULL;
    U64 colour_board = colour ? board->WHITE : board->BLACK;
    U64 opponet_board = colour ? board->BLACK : board->WHITE;
    U64 attack_map = generate_attack_maps(board,colour);
    U64 possible_moves = 0ULL;
    
    for(int i=0;i<64;++i) {
        if(colour) {
        switch(get_piece_at_square(board,i)) {
            case 'P' : possible_moves = get_legal_moves_for_pawn_at_sqaure(board,i,colour); break;
            case 'R' : possible_moves = get_legal_moves_for_rook_at_sqaure(board,i,colour); break;
            case 'N' : possible_moves = get_legal_moves_for_knight_at_square(board,i,colour); break;   
            case 'B' : possible_moves = get_legal_moves_for_bishop_at_sqaure(board,i,colour); break;
            case 'Q' : possible_moves = get_legal_moves_for_queen_at_square(board,i,colour); break;
            case 'K' : possible_moves = get_legal_moves_for_king_at_sqaure(board,i,colour); break;
            
            default : continue; break;
        }
        legal_moves |= possible_moves;

        } else {
            switch(get_piece_at_square(board,i)) {
                case 'p' : possible_moves = get_legal_moves_for_pawn_at_sqaure(board,i,colour); break;
                case 'r' : possible_moves = get_legal_moves_for_rook_at_sqaure(board,i,colour); break;
                case 'n' : possible_moves = get_legal_moves_for_knight_at_square(board,i,colour); break;   
                case 'b' : possible_moves = get_legal_moves_for_bishop_at_sqaure(board,i,colour); break;
                case 'q' : possible_moves = get_legal_moves_for_queen_at_square(board,i,colour); break;
                case 'k' : possible_moves = get_legal_moves_for_king_at_sqaure(board,i,colour); break;
                
                default : continue; break;
            }
            legal_moves |= possible_moves;
        }
    
    }
    display_bitBoard(legal_moves);

    return legal_moves;
}

board_stack_t * c_stack() {
    board_stack_t * new = (board_stack_t*) malloc(sizeof(board_stack_t));
    new->top=0;
    return new;
}

board_stack_t * push(board_stack_t * stack, board_t * board) {
    stack->stack[stack->top] = copy_board(board);
    stack->top++;
    return stack;
}

board_t * pop(board_stack_t * stack) {
    stack->top--;
    return stack->stack[stack->top];
}


int make_move(board_t* board, move_t * move, board_stack_t * stack) {
    // Push the board onto the move stack for undoing
    push(stack,board);
    int from = move->from;
    int to = move->to;
    char piece = get_piece_at_square(board,from);
    char to_piece = get_piece_at_square(board,to);

    // copy current data into the previous store

    

    if(piece=='P'||piece=='p') {
        // If the pawn is moved the nreset the half move clock
        int prev=-1;
        board->halfMoveCLock=0;
        // If we are moving 2 squares then change the enpassant square otherwise set to -1
        if(from-to==16) {
            board->enPassantsq = from-8;
        } else if(from-to==-16) {
            board->enPassantsq = from+8;
        } else {
            prev = board->enPassantsq;
            board->enPassantsq = -1;
            
        }
        
        // handle captures
        if(get_file(from) - get_file(to)==-1 || get_file(from) - get_file(to)==1) {
            if(get_piece_at_square(board,to)=='.') {
                int capturedSQ;

                if(isupper(piece)) {
                    capturedSQ = to+8;
                } else {
                    capturedSQ=to-8;
                }

                char captured_pawn = get_piece_at_square(board,capturedSQ);
                move->capturedPiece=captured_pawn;
                move->enPassantCaptureSq=capturedSQ;
                if (isupper(captured_pawn)||(to==prev&&board->turn==0)) {
                    board->WHITE = clear_bit(capturedSQ, board->WHITE);
                    board->PAWNS = clear_bit(capturedSQ, board->PAWNS);
                    
                    if(to==prev) {
                        
                        board->WHITE = clear_bit(prev+8, board->WHITE);
                        board->PAWNS = clear_bit(prev+8, board->PAWNS);
                    }
                } else if(isupper(captured_pawn)==0||(to==prev&&board->turn==1)) {
                    board->BLACK = clear_bit(capturedSQ, board->BLACK);
                    board->PAWNS = clear_bit(capturedSQ, board->PAWNS);
                    
                    if(to==prev) {
                        board->BLACK = clear_bit(prev-8, board->BLACK);
                        board->PAWNS = clear_bit(prev-8, board->PAWNS);
                        
                    }
                }
            } else {
                move->enPassantCaptureSq = -1;
            }
        }

    } else {
        board->halfMoveCLock++;
        board->enPassantsq = -1;
    }

    if(piece=='R') {
        if(from==0) {
            board->castleFlags = clear_bit(1,board->castleFlags);
        } else if(from==7) {
            board->castleFlags = clear_bit(0,board->castleFlags);
        }
    } else if (piece=='r') {
        if(from==56) {
            board->castleFlags = clear_bit(3,board->castleFlags);
        } else if(from==63) {
            board->castleFlags = clear_bit(2,board->castleFlags);
        }
    } else if (piece=='K') {
        if(from==3) {
            board->castleFlags= clear_bit(0,board->castleFlags);
            board->castleFlags= clear_bit(1,board->castleFlags);
            if(to==1) {
                board->ROOKS = clear_bit(0,board->ROOKS);
                board->ROOKS = set_bit(2,board->ROOKS,1);
                board->WHITE = set_bit(2,board->WHITE,1);
                board->WHITE = clear_bit(0,board->WHITE);
            } else if(to==5) {
                board->ROOKS = clear_bit(7,board->ROOKS);
                board->ROOKS = set_bit(4,board->ROOKS,1);
                board->WHITE = set_bit(4,board->WHITE,1);
                board->WHITE = clear_bit(7,board->WHITE);
            }
        }
    } else if(piece=='k') {
        if(from==59) {
            board->castleFlags = clear_bit(2,board->castleFlags);
            board->castleFlags = clear_bit(3,board->castleFlags);
            if(to==57) {
                board->ROOKS = clear_bit(56,board->ROOKS);
                board->ROOKS = set_bit(58,board->ROOKS,1);
                board->BLACK = set_bit(58,board->BLACK,1);
                board->BLACK = clear_bit(56,board->BLACK);
            } else if(to==61) {
                board->ROOKS = clear_bit(63,board->ROOKS);
                board->ROOKS = set_bit(60,board->ROOKS,1);
                board->BLACK = set_bit(60,board->BLACK,1);
                board->BLACK = clear_bit(63,board->BLACK);
            }
        }
    }
    
    if(to_piece!='.') {
        if(isupper(to_piece)) {
            board->WHITE = clear_bit(to,board->WHITE);
        } else {
            board->BLACK = clear_bit(to,board->BLACK);
        }
        to_piece = toupper(to_piece);
        switch (to_piece) {
            case 'P' : board->PAWNS = clear_bit(to,board->PAWNS); break;
            case 'R' : board->ROOKS = clear_bit(to,board->ROOKS);
            if(to==0) {
                board->castleFlags = clear_bit(1,board->castleFlags);
            } else if(to==7) {
                board->castleFlags = clear_bit(0,board->castleFlags);
            } else if(to==56) {
                board->castleFlags = clear_bit(3,board->castleFlags);
            } else if(to==63) {
                board->castleFlags = clear_bit(2,board->castleFlags);
            }; break;
            case 'N' : board->KNIGHTS = clear_bit(to,board->KNIGHTS); break;
            case 'B' : board->BISHOPS = clear_bit(to,board->BISHOPS); break;
            case 'Q' : board->QUEENS = clear_bit(to,board->QUEENS); break;
            case 'K' : board->KINGS = clear_bit(to,board->KINGS); break;
            default : break;
        }
    }

    if(piece=='.') {
        return -1;
    }

    if(isupper(piece)) {
        board->WHITE = clear_bit(from,board->WHITE);
        board->WHITE = set_bit(to,board->WHITE,1);
    } else {
        board->BLACK=clear_bit(from,board->BLACK);
        board->BLACK=set_bit(to,board->BLACK,1);
    }

    piece = toupper(piece);

    switch (piece){
    case 'P' : board->PAWNS = clear_bit(from,board->PAWNS); 
    if(move->promotedPiece!='.'){
        
        switch(toupper(move->promotedPiece)) {

            case 'R' : board->ROOKS = set_bit(to,board->ROOKS,1); break;
            case 'N' : board->KNIGHTS = set_bit(to,board->KNIGHTS,1); break;
            case 'B' : board->BISHOPS = set_bit(to,board->BISHOPS,1); break;
            case 'Q' : board->QUEENS = set_bit(to,board->QUEENS,1); break;
            default : break;
        }
    } else {

    board->PAWNS = set_bit(to,board->PAWNS,1);} break;
    case 'R' : board->ROOKS = clear_bit(from,board->ROOKS); board->ROOKS = set_bit(to,board->ROOKS,1); break;
    case 'N' : board->KNIGHTS = clear_bit(from,board->KNIGHTS); board->KNIGHTS = set_bit(to,board->KNIGHTS,1);break;
    case 'B' : board->BISHOPS = clear_bit(from,board->BISHOPS); board->BISHOPS = set_bit(to,board->BISHOPS,1); break;
    case 'Q' : board->QUEENS = clear_bit(from,board->QUEENS); board->QUEENS = set_bit(to,board->QUEENS,1); break;
    case 'K' : board->KINGS = clear_bit(from,board->KINGS); board->KINGS = set_bit(to,board->KINGS,1); break;
    default : break;
    }

    
    board->turn = 1-board->turn;
    board->moves++;

    move->capturedPiece=to_piece;
    
    return 0;

}

void display_stack(board_stack_t *stack) {
    for (int i = 0; i < stack->top; i++) {
        // Print the stack position for clarity
        printf("Stack position %d:\n", i);
        
        // Display the board state using the existing function
        display_board(stack->stack[i]);
        
        // Display whose turn it is based on the turn field
        printf("Turn: %s\n", stack->stack[i]->turn ? "White" : "Black");
        
        // Add a newline for separation between boards
        printf("\n");
    }
}

board_t * undo_move(board_stack_t * stack, board_t * board) {
    board_t * board_copy = pop(stack);

    board->WHITE = board_copy->WHITE;
    board->BLACK = board_copy->BLACK;
    board->PAWNS = board_copy->PAWNS;
    board->ROOKS = board_copy->ROOKS;
    board->KNIGHTS = board_copy->KNIGHTS;
    board->BISHOPS = board_copy->BISHOPS;
    board->QUEENS = board_copy->QUEENS;
    board->KINGS = board_copy->KINGS;
    board->enPassantsq = board_copy->enPassantsq;
    board->turn = board_copy->turn;
    board->castleFlags = board_copy->castleFlags;
    board->halfMoveCLock = board_copy->halfMoveCLock;
    board->moves = board_copy->moves;
    board->zorbist_hash = board_copy->zorbist_hash;
    board->zorbist_to_move = board_copy->zorbist_to_move;
    free(board_copy);
    
    return board;

}

move_t * get_legal_move_side(board_t * board, int colour, move_t * legal_moves) {

    
    memset(legal_moves, 0, sizeof(move_t)*300);
    int move_count = 0;

    U64 colour_board = colour ? board->WHITE : board->BLACK;
    const int pop_count = __builtin_popcountll(colour_board);
    int msb_pos = find_msb(colour_board);

    for(int i=0;i<pop_count;++i) {
        U64 possible_moves = 0ULL;
        char piece = get_piece_at_square(board,msb_pos);
        if(colour) {
            switch(piece) {
                case 'P' : possible_moves = get_legal_moves_for_pawn_at_sqaure(board,msb_pos,colour); break;
                case 'R' : possible_moves = get_legal_moves_for_rook_at_sqaure(board,msb_pos,colour);break;
                case 'N' : possible_moves = get_legal_moves_for_knight_at_square(board,msb_pos,colour); break;
                case 'B' : possible_moves = get_legal_moves_for_bishop_at_sqaure(board,msb_pos,colour); break;
                case 'Q' : possible_moves = get_legal_moves_for_queen_at_square(board,msb_pos,colour); break;
                case 'K' : possible_moves = get_legal_moves_for_king_at_sqaure(board,msb_pos,colour); break;
                default : break;
            }
        } else {
            switch(piece) {
                case 'p' : possible_moves = get_legal_moves_for_pawn_at_sqaure(board,msb_pos,colour); break;
                case 'r' : possible_moves = get_legal_moves_for_rook_at_sqaure(board,msb_pos,colour); break;
                case 'n' : possible_moves = get_legal_moves_for_knight_at_square(board,msb_pos,colour); break;
                case 'b' : possible_moves = get_legal_moves_for_bishop_at_sqaure(board,msb_pos,colour); break;
                case 'q' : possible_moves = get_legal_moves_for_queen_at_square(board,msb_pos,colour); break;
                case 'k' : possible_moves = get_legal_moves_for_king_at_sqaure(board,msb_pos,colour); break;
                default : break;
            }
        }
        int possible_move_count = __builtin_popcountll(possible_moves);
        
        int from=msb_pos;
        int to = find_msb(possible_moves);
        for(int j=0;j<possible_move_count;++j) {
            legal_moves[move_count].from = from;
            legal_moves[move_count].to = to;
            legal_moves[move_count].piece = piece;
            legal_moves[move_count].colour = colour;
            
            if(piece=='P'||piece=='p') {
                if((to<8&&!(colour))||(to>55&&colour)) {
                    legal_moves[move_count].promotedPiece = colour ? 'Q' : 'q';
                    move_count++;
                    char pieces[] = "rnb";
                    for(int k=0;k<3;++k) {
                        legal_moves[move_count].promotedPiece = colour ? toupper(pieces[k]) : pieces[k];
                        legal_moves[move_count].from = from;
                        legal_moves[move_count].to = to;
                        legal_moves[move_count].piece = piece;
                        legal_moves[move_count].colour = colour;
                        move_count++;
                    }

                } else {
                    legal_moves[move_count].promotedPiece = '.';
                    move_count++;
                }
            } else{
                legal_moves[move_count].promotedPiece = '.';
                move_count++;
            }

            
            possible_moves = clear_bit(to,possible_moves);
            to = find_msb(possible_moves);
        }
        colour_board=clear_bit(msb_pos,colour_board);
        msb_pos = find_msb(colour_board);
    }
    
    return legal_moves;

}

int checkmate(board_t * board) {
    if(in_check(board, board->turn)) {
        move_t moves[300];
    get_legal_move_side(board, board->turn, moves);
    if(moves[0].from==0&&moves[0].to==0) {
        return 1;
    }
        
    }
    return 0;
}

int stalemate(board_t * board) {
    if(in_check(board,board->turn)) {
        return 0;
    }
    move_t moves[300];
    get_legal_move_side(board, board->turn, moves);
    if(moves[0].from==0&&moves[0].to==0||board->halfMoveCLock>=50) {
        return 1;
    }
    return 0;
}

int is_terminal(board_t * board) {
    if(checkmate(board)||stalemate(board)) {
        return 1;
    }
    return 0;
}
// Board Functions End Here


void play() {
    board_t * board = init_board();
    board_stack_t*  stack = c_stack();
    
    while(!(is_terminal(board))) {
        display_board(board);
       
        move_t moves[300];
        get_legal_move_side(board, board->turn, moves);

        for(int i=0;i<300;i++) {
            if(moves[i].from==0&&moves[i].to==0) {
                break;
            }

            printf("%d From: %d To: %d PIece:%c Capture %c\n", i+1,moves[i].from, moves[i].to,moves[i].piece,moves[i].capturedPiece);



        }
        int m;
        scanf("%d",&m);
        
        make_move(board,&moves[m],stack);
        memset(moves,0,sizeof(moves));
        
    }
}

void move_test() {

    char fen[] = "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8";
    
    board_t * board = init_from_FEN(fen);
    display_board(board);
    board_stack_t*  stack = c_stack();
    move_t moves[300];

    get_legal_move_side(board,board->turn, moves);
    display_board(board);
    for(int i =0;i<300;i++) {
        if(moves[i].to==0&&moves[i].from==0) {
            break;
        }
        
        make_move(board,&moves[i],stack);
        
        move_t move_a[300];
        get_legal_move_side(board,board->turn, move_a);
        for(int j=0;j<300;++j) {
            if(move_a[j].to==0&&move_a[j].from==0) {
            break;
            }
            make_move(board,&move_a[j],stack);
            move_t move_b[300];
            get_legal_move_side(board,board->turn, move_b);
            display_board(board);
            for(int k=0;k<300;++k) {
                if(move_b[k].to==0&&move_b[k].from==0) {
                    break;
                }
                printf("%d, From:%d - To:%d\n",k,move_b[k].from,move_b[k].to);
            }
            board=undo_move(stack,board);
            int m;
            scanf("%d",&m);


        }
        board=undo_move(stack,board);
        }
        
        
        
    }    


