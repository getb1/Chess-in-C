#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>
#include "board.h"
#include "misc.h"
#include "magic_numbers.h"

#define lsb(x) __builtin_ctzll(x)

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
    copy->zorbist_hash = original->zorbist_hash;
    copy->zorbist_to_move = original->zorbist_to_move;
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
        printf(" %d",i+1);
    }
    printf("\n");
    for(int i=0;i<17;++i) {
        printf("-");
    }
    printf("\n a b c d e f g h");
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
    U64 magic_number = board->BISHOP_MAGICS[pos];
    U64 mask = board->BISHOP_MASKS[pos];
    U64 blockers = board->BLACK | board->WHITE;
    U64 colour_board = colour ? board->WHITE : board->BLACK;

    blockers &= mask;
    U64 possible_moves = board->BISHOP_ATTACK_TABLE[pos][transform(blockers, magic_number, BBits[pos])];

    possible_moves &= ~(colour_board);

    return possible_moves;

}

U64 get_attacks_for_rook_at_square(board_t * board,int pos, int colour) {
    
    U64 magic_number = board->ROOK_MAGICS[pos];
    U64 mask = board->ROOK_MASKS[pos];
    U64 blockers = board->BLACK | board->WHITE;
    U64 colour_board = colour ? board->WHITE : board->BLACK;



    blockers &= mask;
    U64 possible_moves = board->ROOK_ATTACK_TABLE[pos][transform(blockers, magic_number, RBits[pos])];

    possible_moves &= ~(colour_board);

    return possible_moves;

    



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

U64 get_attacks_for_knight_at_square_unfiltered(board_t * board, int pos, int colour) {
    return board->KNIGHT_MOVES[pos];
}

U64 get_attacks_for_bishop_at_square_unfiltered(board_t * board, int pos, int colour) {
    U64 magic_number = board->BISHOP_MAGICS[pos];
    U64 mask = board->BISHOP_MASKS[pos];
    U64 blockers = board->BLACK | board->WHITE;

    blockers &= mask;
    U64 possible_moves = board->BISHOP_ATTACK_TABLE[pos][transform(blockers, magic_number, BBits[pos])];

    return possible_moves;

}

U64 get_attacks_for_rook_at_square_unfiltered(board_t * board, int pos, int colour) {

    
    U64 magic_number = board->ROOK_MAGICS[pos];
    U64 mask = board->ROOK_MASKS[pos];
    U64 blockers = board->BLACK | board->WHITE;

    blockers &= mask;
    U64 possible_moves = board->ROOK_ATTACK_TABLE[pos][transform(blockers, magic_number, RBits[pos])];

    return possible_moves;
}

U64 get_attacks_for_pawn_at_square_unfiltered(board_t * board, int pos, int colour) {
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

    if(on_board_rank_file(rank+direction,file+1)) {
        move |= set_bit(coordinates_to_number(rank+direction,file+1),move,1);
    }
    if(on_board_rank_file(rank+direction,file-1)) {
        move |= set_bit(coordinates_to_number(rank+direction,file-1),move,1);
    }

    return move;
}

U64 get_attacks_for_king_at_square_unfiltered(board_t * board, int pos, int colour) {
    return board->KING_MOVES[pos];
}
U64 get_attacks_for_queen_at_square_unfiltered(board_t * board, int pos, int colour) {
    return get_attacks_for_rook_at_square_unfiltered(board,pos,colour)|get_attacks_for_bishop_at_square_unfiltered(board,pos,colour);
}


U64 generate_attack_maps(board_t * board,int colour) {

    U64 boards[6] = {board->ROOKS,board->KNIGHTS,board->BISHOPS,board->KINGS,board->QUEENS,board->PAWNS};

    U64 colour_board = colour ? board->WHITE : board->BLACK;
    U64 map = 0ULL;
    int i = __builtin_ctzll(colour_board);

    while(colour_board) {
        
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
        colour_board = clear_bit(i,colour_board);
        i=__builtin_ctzll(colour_board);
    }
    return map;

}

U64 generate_attack_maps_unfiltered(board_t * board,int colour) {

    U64 boards[6] = {board->ROOKS,board->KNIGHTS,board->BISHOPS,board->KINGS,board->QUEENS,board->PAWNS};

    U64 colour_board = colour ? board->WHITE : board->BLACK;
    U64 map = 0ULL;

    int i = __builtin_ctzll(colour_board);

    while (colour_board)
     {
        if(get_bit(i,colour_board)) {
            if(get_bit(i,board->ROOKS)) {
                map |= get_attacks_for_rook_at_square_unfiltered(board,i,colour);
            } else if(get_bit(i,board->KNIGHTS)) {
                map |= get_attacks_for_knight_at_square_unfiltered(board,i,colour);
            } else if(get_bit(i,board->BISHOPS)) {
                map |= get_attacks_for_bishop_at_square_unfiltered(board,i,colour);
            } else if(get_bit(i,board->KINGS)) {
                map |= get_attacks_for_king_at_square_unfiltered(board,i,colour);
            } else if(get_bit(i,board->QUEENS)) {
                map |= get_attacks_for_queen_at_square_unfiltered(board,i,colour);
            } else if(get_bit(i,board->PAWNS)) {
                map |= get_attacks_for_pawn_at_square_unfiltered(board,i,colour);
            }
        }
        colour_board = clear_bit(i,colour_board);
        i=__builtin_ctzll(colour_board);
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

U64 generate_checkers(board_t * board, int colour) {
    U64 king = board->turn ? (board->WHITE & board->KINGS) : (board->BLACK & board->KINGS);
    U64 opponent_board = board->turn ? board->BLACK : board->WHITE;
    U64 player_board = board->turn ? board->WHITE : board->BLACK;
    int king_pos = find_msb(king);
    int king_rank = get_rank(king_pos);
    int king_file = get_file(king_pos);
    int direction = board->turn ? 1:-1;
    U64 checkers = 0ULL;
    U64 checkers_2=0ULL;
    U64 opponent_knights = opponent_board & board->KNIGHTS;
    U64 knight_attacks = get_attacks_for_knight_at_square(board,king_pos,board->turn);
    checkers_2 = (knight_attacks & opponent_knights);

    U64 opponent_bishops = opponent_board & board->BISHOPS;
    U64 bishop_attacks = get_attacks_for_bishop_at_square(board,king_pos,board->turn);
    
    checkers_2 |= (bishop_attacks & opponent_bishops);

    U64 opponent_rooks = opponent_board & board->ROOKS;
    U64 rook_attacks = get_attacks_for_rook_at_square(board,king_pos,board->turn);
    checkers_2 |= (rook_attacks & opponent_rooks);

    U64 opponent_queens = opponent_board & board->QUEENS;
    U64 queen_attacks = get_attacks_for_queen_at_square(board,king_pos,board->turn);
    checkers_2 |= (queen_attacks & opponent_queens);

    // Check for pawns
    U64 opponent_pawns = opponent_board & board->PAWNS;

    if(on_board_rank_file(king_rank+direction,king_file+1)) {
        int pawn_square = coordinates_to_number(king_rank+direction,king_file+1);
        if(get_bit(pawn_square,opponent_pawns)) {
            checkers_2 = set_bit(pawn_square,checkers_2,1);
        }
    }
    if(on_board_rank_file(king_rank+direction,king_file-1)) {
        int pawn_square = coordinates_to_number(king_rank+direction,king_file-1);
        if(get_bit(pawn_square,opponent_pawns)) {
            checkers_2 = set_bit(pawn_square,checkers_2,1);
        }
    }

    
    
    return checkers_2;
    }

int count_checkers(board_t * board, int colour) {
    U64 king = board->turn ? (board->WHITE & board->KINGS) : (board->BLACK & board->KINGS);
    U64 opponent_board = board->turn ? board->BLACK : board->WHITE;
    U64 player_board = board->turn ? board->WHITE : board->BLACK;
    int king_pos = find_msb(king);
    int king_rank = get_rank(king_pos);
    int king_file = get_file(king_pos);
    int direction = board->turn ? 1:-1;
    U64 checkers = 0ULL;
    U64 checkers_2=0ULL;
    U64 opponent_knights = opponent_board & board->KNIGHTS;
    U64 knight_attacks = get_attacks_for_knight_at_square(board,king_pos,board->turn);
    checkers_2 = (knight_attacks & opponent_knights);

    U64 opponent_bishops = opponent_board & board->BISHOPS;
    U64 bishop_attacks = get_attacks_for_bishop_at_square(board,king_pos,board->turn);
    
    checkers_2 |= (bishop_attacks & opponent_bishops);

    U64 opponent_rooks = opponent_board & board->ROOKS;
    U64 rook_attacks = get_attacks_for_rook_at_square(board,king_pos,board->turn);
    checkers_2 |= (rook_attacks & opponent_rooks);

    U64 opponent_queens = opponent_board & board->QUEENS;
    U64 queen_attacks = get_attacks_for_queen_at_square(board,king_pos,board->turn);
    checkers_2 |= (queen_attacks & opponent_queens);

    // Check for pawns
    U64 opponent_pawns = opponent_board & board->PAWNS;

    if(on_board_rank_file(king_rank+direction,king_file+1)) {
        int pawn_square = coordinates_to_number(king_rank+direction,king_file+1);
        if(get_bit(pawn_square,opponent_pawns)) {
            checkers_2 = set_bit(pawn_square,checkers_2,1);
        }
    }
    if(on_board_rank_file(king_rank+direction,king_file-1)) {
        int pawn_square = coordinates_to_number(king_rank+direction,king_file-1);
        if(get_bit(pawn_square,opponent_pawns)) {
            checkers_2 = set_bit(pawn_square,checkers_2,1);
        }
    }

    
    
    return __builtin_popcountll(checkers_2);
}

int en_passant_valid(board_t * board, int position) {
    if(board->enPassantsq==-1) {
        return -1;
    }
    

    if(board->turn) {
        

        int rank = get_rank(position);
        int file = get_file(position);
        int enp_rank = get_rank(board->enPassantsq);
        int enp_file = get_file(board->enPassantsq);
        int capture_pos;

        

        if(rank+1==enp_rank) {
            
            if(file==enp_file+1) {
                capture_pos = coordinates_to_number(rank,enp_file);
                
            
            } else if (file==enp_file-1) {
                capture_pos = coordinates_to_number(rank,enp_file);
                
                
            } else {
                return -1;
            }
            U64 original_white = board->WHITE;
            U64 original_black = board->BLACK;
            U64 original_pawns = board->PAWNS;
            

            
            board->WHITE = clear_bit(position,board->WHITE);
            board->PAWNS = clear_bit(position, board->PAWNS);
            board->BLACK = clear_bit(capture_pos,board->BLACK);
            board->PAWNS = clear_bit(capture_pos,board->PAWNS);
            
            board->WHITE = set_bit(board->enPassantsq,board->WHITE,1);
            board->PAWNS = set_bit(board->enPassantsq,board->PAWNS,1);
            int invalid=0;

            
            if(in_check(board,1)) {
                
                invalid =1;
            }

            board->WHITE = original_white;
            board->BLACK = original_black;
            board->PAWNS = original_pawns;
            return invalid;

            
        }


    } else {
        int rank = get_rank(position);
        int file = get_file(position);
        int enp_rank = get_rank(board->enPassantsq);
        int enp_file = get_file(board->enPassantsq);
        int capture_pos;
        if(rank-1==enp_rank) {
            if(file==enp_file+1) {
                capture_pos = coordinates_to_number(rank,enp_file);
            
            } else if (file==enp_file-1) {
                capture_pos = coordinates_to_number(rank,enp_file);
                
            } else {
                return -1;
            }
            U64 original_white = board->WHITE;
            U64 original_black = board->BLACK;
            U64 original_pawns = board->PAWNS;

            board->BLACK = clear_bit(position,board->BLACK);
            board->PAWNS = clear_bit(position, board->PAWNS);
            board->WHITE = clear_bit(capture_pos,board->WHITE);
            board->PAWNS = clear_bit(capture_pos,board->PAWNS);

            board->BLACK = set_bit(board->enPassantsq,board->BLACK,1);
            board->PAWNS = set_bit(board->enPassantsq,board->PAWNS,1);
            int invalid=0;
            
            if(in_check(board,0)) {
                invalid =1;
            }

            board->WHITE = original_white;
            board->BLACK = original_black;
            board->PAWNS = original_pawns;
            return invalid;

            
        }
    }
    return -1;
}

check_info_t generate_check_info(board_t * board) {

    check_info_t check_info = {0, 0ULL, 0ULL, 0ULL, 0, 0, 0ULL};

    U64 king = board->turn ? (board->WHITE & board->KINGS) : (board->BLACK & board->KINGS);
    U64 opponent_board = board->turn ? board->BLACK : board->WHITE;
    U64 player_board = board->turn ? board->WHITE : board->BLACK;
    int king_pos = find_msb(king);
    int king_rank = get_rank(king_pos);
    int king_file = get_file(king_pos);
    check_info.in_check = in_check(board, board->turn);
    check_info.pinned_rays = 0ULL;
    check_info.pinned_pieces = 0ULL;
    

    if(check_info.in_check) {
        U64 check_attacks = 0ULL;
        U64 checkers = 0ULL;
        // Find the number of checkers
        int i = __builtin_ctzll(opponent_board);
        while(opponent_board) {
            if(get_bit(i,opponent_board)) {
                if(get_bit(i,board->ROOKS)) {
                    U64 attacks = get_attacks_for_rook_at_square(board,i,1-board->turn);
                    if(attacks & king) {
                        checkers = set_bit(i,checkers,1);
                        check_attacks |= generate_path_between_two_points(i,king_pos,1);
                    }
                } else if(get_bit(i,board->BISHOPS)) {
                    U64 attacks = get_attacks_for_bishop_at_square(board,i,1-board->turn);
                    if(attacks & king) {
                        checkers = set_bit(i,checkers,1);
                        check_attacks |= generate_path_between_two_points(i,king_pos,1);
                    }
                } else if(get_bit(i,board->QUEENS)) {
                    U64 attacks = get_attacks_for_queen_at_square(board,i,1-board->turn);
                    if(attacks & king) {
                        checkers = set_bit(i,checkers,1);
                        check_attacks |= generate_path_between_two_points(i,king_pos,1);
                        
                    }
                
                } else if(get_bit(i,board->KNIGHTS)) {
                    U64 attacks = get_attacks_for_knight_at_square(board,i,1-board->turn);
                    if(attacks & king) {
                        checkers = set_bit(i,checkers,1);
                        check_attacks |= set_bit(i,check_attacks,1);
                        
                    }
                } else if(get_bit(i,board->PAWNS)) {
                    U64 attacks = get_attacks_for_pawn_at_square(board,i,1-board->turn);
                    if(attacks & king) {
                        checkers = set_bit(i,checkers,1);
                        check_attacks |= set_bit(i,check_attacks,1);
                        if(board->enPassantsq!=-1) {
                            
                            int direction = board->turn ? 1:-1;
                            int ep_rank = get_rank(board->enPassantsq);
                            int ep_file = get_file(board->enPassantsq);
                            int file = get_file(i);
                            int rank = get_rank(i);
                            
                            if(ep_rank==rank+direction&&file==ep_file) {
                                check_attacks |= set_bit(board->enPassantsq,check_attacks,1);}

                        }
                    }
                }  }  
            opponent_board = clear_bit(i,opponent_board);
            i=__builtin_ctzll(opponent_board);
            }
        check_info.checkers = checkers;
        check_info.block_or_captures = check_attacks;
        check_info.num_checkers = __builtin_popcountll(checkers);
        }

        // Generate pinned pieces
        U64 pinned_pieces = 0ULL;


        // Remove Pieces and see if the king is put in check. If yes then the piece is pinned
        // If the king is in check remove the checking piece to check for pins

        //Store checking pieces
        
        
        
        U64 b_magic = board->BISHOP_MAGICS[king_pos];
        U64 b_mask = board->BISHOP_MASKS[king_pos];
        U64 r_magic = board->ROOK_MAGICS[king_pos];
        U64 r_mask = board->ROOK_MASKS[king_pos];
        U64 blockers = 0ULL;
        
        player_board&=board->BISHOP_ATTACK_TABLE[king_pos][transform(blockers, b_magic, BBits[king_pos])]|board->ROOK_ATTACK_TABLE[king_pos][transform(blockers, r_magic, RBits[king_pos])];
        
        player_board &= ~(board->KINGS);
        
        int i = __builtin_ctzll(player_board);

        check_info.en_passant_pinned[0]=0;
        check_info.en_passant_pinned[1]=0;
        check_info.en_passant_pinned_squares[0]=-1;
        check_info.en_passant_pinned_squares[1]=-1;
        
        int en_passant_checked=0;
        while(player_board) {
                
                if(get_bit(i,board->ROOKS)) {
                    U64 original_rooks = board->ROOKS;
                    U64 original_white = board->WHITE;
                    U64 original_black = board->BLACK;
                    
                    board->ROOKS = clear_bit(i, board->ROOKS);
                    if (board->turn) {
                        board->WHITE = clear_bit(i, board->WHITE);
                    } else {
                        board->BLACK = clear_bit(i, board->BLACK);
                    }

                    if (check_info.num_checkers!= count_checkers(board, board->turn)) {

                        pinned_pieces = set_bit(i, pinned_pieces, 1);
                        U64 pinner = generate_checkers(board, board->turn)^check_info.checkers;
                        int pinner_pos = find_msb(pinner);
                        
                        check_info.pinned_rays |= generate_path_between_two_points(pinner_pos, king_pos, 1);
                    }

                    // Restore the original state
                    board->ROOKS = original_rooks;
                    board->WHITE = original_white;
                    board->BLACK = original_black;
                    
                }
                else if(get_bit(i,board->KNIGHTS)) {
                    U64 original_knights = board->KNIGHTS;
                    U64 original_white = board->WHITE;
                    U64 original_black = board->BLACK;
                    
                    board->KNIGHTS = clear_bit(i, board->KNIGHTS);
                    if (board->turn) {
                        board->WHITE = clear_bit(i, board->WHITE);
                    } else {
                        board->BLACK = clear_bit(i, board->BLACK);
                    }
                    if (check_info.num_checkers!= count_checkers(board, board->turn)) {
                        pinned_pieces = set_bit(i, pinned_pieces, 1);
                        check_info.pinned_rays |= generate_path_between_two_points(i, king_pos, 1);
                    }
                    // Restore the original state
                    board->KNIGHTS = original_knights;
                    board->WHITE = original_white;
                    board->BLACK = original_black;
                }
                else  if(get_bit(i,board->BISHOPS)) {
                    U64 original_bishops = board->BISHOPS;
                    U64 original_white = board->WHITE;
                    U64 original_black = board->BLACK;

                    board->BISHOPS = clear_bit(i, board->BISHOPS);
                    if (board->turn) {
                        board->WHITE = clear_bit(i, board->WHITE);
                    } else {
                        board->BLACK = clear_bit(i, board->BLACK);
                    }

                    if (check_info.num_checkers!= count_checkers(board, board->turn)) {
                        pinned_pieces = set_bit(i, pinned_pieces, 1);
                        U64 pinner = generate_checkers(board, board->turn)^check_info.checkers;
                        int pinner_pos = find_msb(pinner);
                        check_info.pinned_rays |= generate_path_between_two_points(pinner_pos, king_pos, 1);
                    }

                    // Restore the original state
                    board->BISHOPS = original_bishops;
                    board->WHITE = original_white;
                    board->BLACK = original_black;
                }
                else if(get_bit(i,board->QUEENS)) {
                    U64 original_queens = board->QUEENS;
                    U64 original_white = board->WHITE;
                    U64 original_black = board->BLACK;

                    board->QUEENS = clear_bit(i, board->QUEENS);
                    if (board->turn) {
                        board->WHITE = clear_bit(i, board->WHITE);
                    } else {
                        board->BLACK = clear_bit(i, board->BLACK);
                    }

                    if (check_info.num_checkers!= count_checkers(board, board->turn)) {
                        pinned_pieces = set_bit(i, pinned_pieces, 1);
                        U64 checkers = generate_checkers(board, board->turn);
                        

                        U64 pinner = checkers ^ (check_info.checkers);
                        
                        int pinner_pos = find_msb(pinner);
                        
                        check_info.pinned_rays |= generate_path_between_two_points(pinner_pos, king_pos, 1);
                        
                    }

                    // Restore the original state
                    board->QUEENS = original_queens;
                    board->WHITE = original_white;
                    board->BLACK = original_black;
                }   
                else if(get_bit(i,board->PAWNS)) {
                    
                    U64 original_pawns = board->PAWNS;
                    U64 original_white = board->WHITE;
                    U64 original_black = board->BLACK;

                    board->PAWNS = clear_bit(i, board->PAWNS);
                    if (board->turn) {
                        board->WHITE = clear_bit(i, board->WHITE);
                    } else {
                        board->BLACK = clear_bit(i, board->BLACK);
                    }

                    if (check_info.num_checkers!= count_checkers(board, board->turn)) {
                        pinned_pieces = set_bit(i, pinned_pieces, 1);
                        U64 pinner = generate_checkers(board, board->turn)^check_info.checkers;
                        int pinner_pos = __builtin_ctzll(pinner);
                        check_info.pinned_rays |= generate_path_between_two_points(pinner_pos, king_pos, 1);
                        // Check to see if a piece can become pinned by enpassant

                    
                     
                    }
                    board->PAWNS = original_pawns;
                    board->WHITE = original_white;
                    board->BLACK = original_black;
                    
                    if(board->enPassantsq!=-1) {
                        
                        int result = en_passant_valid(board,i);    
                        if(result!=-1) {
                            
                            check_info.en_passant_pinned[en_passant_checked]=result;
                            check_info.en_passant_pinned_squares[en_passant_checked]=i;
                            en_passant_checked++;
                            
                        }
                            
                        }
                    board->PAWNS = original_pawns;
                    board->WHITE = original_white;
                    board->BLACK = original_black;

                    // Restore the original state
                    
                }
                player_board = clear_bit(i,player_board);
                
                i=__builtin_ctzll(player_board);
                
            }
        
        
        check_info.pinned_pieces = pinned_pieces;
        
        return check_info;
}

void display_check_info(check_info_t info) {
    printf("In Check: %d\n",info.in_check);
    printf("Number of Checkers: %d\n",info.num_checkers);
    printf("Checkers: \n");
    display_bitBoard(info.checkers);
    printf("Block or Capture Squares: \n");
    display_bitBoard(info.block_or_captures);
    printf("Pinned Pieces: \n");
    display_bitBoard(info.pinned_pieces);
    printf("Pinned Rays: \n");
    display_bitBoard(info.pinned_rays);

}

int piece_on_square(board_t * board, int pos,int colour) {
    U64 colour_board = (colour) ? board->WHITE : board->BLACK;

    if(get_bit(pos,colour_board)) {
        return 1;
    }
    return 0;
}

int get_direction(int from, int to) {
    int from_rank = get_rank(from);
    int from_file = get_file(from);
    int to_rank = get_rank(to);
    int to_file = get_file(to);

    if(from_rank == to_rank) {
        return (to_file > from_file) ? 1 : -1; // Horizontal
    } else if(from_file == to_file) {
        return (to_rank > from_rank) ? 2 : -2; // Vertical
    } else if(abs(from_rank - to_rank) == abs(from_file - to_file)) {
        if(to_rank > from_rank && to_file > from_file) {
            return 3; // Diagonal up-right
        } else if(to_rank > from_rank && to_file < from_file) {
            return 3; // Diagonal up-left
        } else if(to_rank < from_rank && to_file > from_file) {
            return -3; // Diagonal down-right
        } else {
            return -3; // Diagonal down-left
        }
    }
    return 0; // Not aligned
}

int diagonal_direction(int from, int to) {
    int from_rank = get_rank(from);
    int from_file = get_file(from);
    int to_rank = get_rank(to);
    int to_file = get_file(to);

    if(abs(from_rank - to_rank) == abs(from_file - to_file)) {
        if(to_rank > from_rank && to_file > from_file) {
            return 1; // Diagonal up-right
        } else if(to_rank > from_rank && to_file < from_file) {
            return 2; // Diagonal up-left
        } else if(to_rank < from_rank && to_file > from_file) {
            return 3; // Diagonal down-right
        } else {
            return 4; // Diagonal down-left
        }
    }
    return 0; // Not aligned
}

U64 get_diagonal_mask(int square, int is_a1h8) {
    if (is_a1h8) {
        // Use the fact that A1-H8 diagonals have constant (rank - file)
        static const U64 diagonals[] = {
            0x0000000000000080ULL, 0x0000000000008040ULL, 0x0000000000804020ULL,
            0x0000000080402010ULL, 0x0000008040201008ULL, 0x0000804020100804ULL,
            0x0080402010080402ULL, 0x8040201008040201ULL, 0x4020100804020100ULL,
            0x2010080402010000ULL, 0x1008040201000000ULL, 0x0804020100000000ULL,
            0x0402010000000000ULL, 0x0201000000000000ULL, 0x0100000000000000ULL
        };
        int diagonal_index = (square / 8) - (square % 8) + 7;
        return diagonals[diagonal_index];
    } else {
        // A8-H1 diagonals have constant (rank + file)
        static const U64 diagonals[] = {
            0x0000000000000001ULL, 0x0000000000000102ULL, 0x0000000000010204ULL,
            0x0000000001020408ULL, 0x0000000102040810ULL, 0x0000010204081020ULL,
            0x0001020408102040ULL, 0x0102040810204080ULL, 0x0204081020408000ULL,
            0x0408102040800000ULL, 0x0810204080000000ULL, 0x1020408000000000ULL,
            0x2040800000000000ULL, 0x4080000000000000ULL, 0x8000000000000000ULL
        };
        int diagonal_index = (square / 8) + (square % 8);
        return diagonals[diagonal_index];
    }
}

U64 get_legal_moves_for_knight_at_square(board_t *board, int pos, int colour, check_info_t info) {
    //display_check_info(info);
    if(get_bit(pos,info.pinned_rays)) {
        return 0ULL; // Knight is pinned, no legal moves
    }
    if(info.num_checkers>1) {
        return 0ULL; // Double check, knight cannot block or capture both
    }

    U64 possible_moves = get_attacks_for_knight_at_square(board, pos, colour);
    U64 legal_moves = 0ULL;
    if(info.in_check==1) {
        possible_moves &= info.block_or_captures;
        
        return possible_moves;
    }
    else {
        legal_moves = possible_moves;
        return legal_moves;
    }
}


U64 get_legal_moves_for_rook_at_sqaure(board_t *board, int pos, int colour,check_info_t info) {
    U64 possible_moves = get_attacks_for_rook_at_square(board, pos, colour);
    
    if(possible_moves == 0ULL) {
        return 0ULL; // No possible moves
    }


     possible_moves = get_attacks_for_rook_at_square(board, pos, colour);
    
    if(possible_moves == 0ULL || info.num_checkers>1) {
        return 0ULL; // No possible moves
    }

    U64 legal_moves_2 = 0ULL;
    if(get_bit(pos,info.pinned_pieces)) {
        // prevent moves along adacent pin rays
        possible_moves &= info.pinned_rays;
        int king_pos = find_msb(board->turn ? (board->WHITE & board->KINGS) : (board->BLACK & board->KINGS));
        int direction = get_direction(king_pos,pos);
        if(direction==0||direction==3||direction==-3) {
            return 0ULL; // Not aligned with king, no legal moves
        } else if(direction==1||direction==-1) {
            possible_moves &= info.pinned_rays;
            int rank=get_rank(pos);
            // Shift a horizontal mask onto the pin rays
            U64 horizontal_mask = 0xFFULL << (rank * 8);
            possible_moves &= horizontal_mask;
        } else if(direction==2||direction==-2) {
            possible_moves &= info.pinned_rays;
            int file=get_file(pos);
            // Shift a vertical mask onto the pin rays
            U64 vertical_mask = 0x0101010101010101ULL << file;
            possible_moves &= vertical_mask;
        }

    }
    if(info.in_check==1) {
        possible_moves &= info.block_or_captures;
        
        legal_moves_2 = possible_moves;
    }
    else {
        legal_moves_2 = possible_moves;
        }

    if(possible_moves == 0ULL) {
        return 0ULL; // No possible moves
    }
    
    return legal_moves_2;
}

U64 get_legal_moves_for_bishop_at_sqaure(board_t *board, int pos, int colour, check_info_t info) {
    
    U64 possible_moves = get_attacks_for_bishop_at_square(board, pos, colour);
    if(possible_moves == 0ULL) {
        return 0ULL; // No possible moves
    }
    U64 legal_moves = 0ULL;
    U64 legal_moves_2 = 0ULL;

    if(info.num_checkers>1) {
        possible_moves= 0ULL; // Double check, bishop cannot block or capture both
    }

    if(get_bit(pos,info.pinned_pieces)) {
        int direction = get_direction(find_msb(board->turn ? (board->WHITE & board->KINGS) : (board->BLACK & board->KINGS)),pos);

        if(direction==0||direction==1||direction==-1||direction==2||direction==-2) {
            possible_moves = 0ULL; // Not aligned with king, no legal moves
        } else {
            
            possible_moves &= info.pinned_rays;
            
            // prevent moves along adacent pin rays need to do
            
            U64 new = info.pinned_rays ^ board->turn ? board->BLACK : board->WHITE;
            int king_pos = find_msb(board->turn ? (board->WHITE & board->KINGS) : (board->BLACK & board->KINGS));
            int queen=0;
            if(get_bit(pos,board->QUEENS)) {
                board->QUEENS = clear_bit(pos, board->QUEENS);
                queen=1;
            }
            else {board->BISHOPS = clear_bit(pos, board->BISHOPS);}
            
            if (colour) {
                board->WHITE = clear_bit(pos, board->WHITE);
            } else {
                board->BLACK = clear_bit(pos, board->BLACK);
            
            }
            U64 checkers = generate_checkers(board, board->turn);
            checkers ^= info.checkers;
            int pinner_pos = find_msb(checkers);
            possible_moves &= generate_path_between_two_points(pinner_pos, king_pos, 1);
            if(queen) {
                board->QUEENS = set_bit(pos, board->QUEENS, 1);
            }
            else {
            board->BISHOPS = set_bit(pos, board->BISHOPS, 1);}
            if (colour) {
                board->WHITE = set_bit(pos, board->WHITE, 1);
            } else {
                board->BLACK = set_bit(pos, board->BLACK, 1);
            }       
        }

    }

    if(info.in_check==1) {
        possible_moves &= info.block_or_captures;
        
        legal_moves_2 = possible_moves;
    }
    else {
        legal_moves_2 = possible_moves;
    }

    

    return legal_moves_2;
}

U64 get_legal_moves_for_queen_at_square(board_t *board, int pos, int colour, check_info_t info) {
    return get_legal_moves_for_bishop_at_sqaure(board,pos,colour, info)|get_legal_moves_for_rook_at_sqaure(board,pos,colour,info);
}

U64 get_legal_moves_for_king_at_sqaure(board_t *board, int pos, int colour) {
    // Get the attacks for the king
    U64 possible_moves = get_attacks_for_king_at_square(board, pos, colour);
    U64 legal_moves=0ULL;

    int king_pos = find_msb(board->KINGS & (colour ? board->WHITE : board->BLACK));
    board->KINGS = clear_bit(king_pos, board->KINGS);

    // Remove the king from the board temporarily to stop it blocking opponents attacks
    if(colour) {
        board->WHITE = clear_bit(king_pos, board->WHITE);
    } else {
        board->BLACK = clear_bit(king_pos, board->BLACK);
    }
    
    // Generate this attack map
    U64 real_attack_map_unfiltered = generate_attack_maps_unfiltered(board, !(colour));
    U64 real_attack_map = real_attack_map_unfiltered & ~(colour ? board->WHITE : board->BLACK);
    // Put the king back on the board

    board->KINGS = set_bit(king_pos, board->KINGS, 1);
    
    if(colour) {
        board->WHITE = set_bit(king_pos, board->WHITE, 1);
    } else {
        board->BLACK = set_bit(king_pos, board->BLACK, 1);
    }

    // The issue is that the king doesnt check if by taking a piece it will move into check
    // To fix this without a for loop 
    int queen_side_bit,rook_side_bit;
    
    rook_side_bit = colour ? 1 : 3;
    queen_side_bit= colour ? 0:2;
    U64 attack_map = generate_attack_maps(board, !(colour));
    int king_rank = colour?0:7;
    
    possible_moves &= ~real_attack_map;
    legal_moves = possible_moves;
    

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
    
    return legal_moves& ~(generate_attack_maps_unfiltered(board,!(colour))) & ~(real_attack_map_unfiltered);
}

U64 handle_pinned_pawn_moves(board_t * board,int pos, U64 possible_moves, int colour, check_info_t info) {
    int pin_direction = get_direction(find_msb(board->turn ? (board->WHITE & board->KINGS) : (board->BLACK & board->KINGS)),pos);

    if(abs(pin_direction)==1) {
        return 0ULL;
    } else if(abs(pin_direction)==2) {
        possible_moves &= (info.pinned_rays&~(board->turn ? board->WHITE : board->BLACK));
        return possible_moves;
    } else if(abs(pin_direction)==3) {
        int diagonal_dir = diagonal_direction(find_msb(board->turn ? (board->WHITE & board->KINGS) : (board->BLACK & board->KINGS)),pos);
        U64 diagonal_mask = get_diagonal_mask(pos,diagonal_dir==1||diagonal_dir==4);
        possible_moves &= (diagonal_mask & info.pinned_rays);
        
        return possible_moves;
    }

    return possible_moves;
}

U64 get_legal_moves_for_pawn_at_sqaure(board_t * board,int pos, int colour, check_info_t info) {
    // get the possible moves for the pawn
    
    int direction = colour ? 1 : -1;
    
    U64 colour_moves = colour ? board->WHITE_PAWN_MOVES[pos] : board->BLACK_PAWN_MOVES[pos];

    U64 possible_moves = get_attacks_for_pawn_at_square(board,pos,colour)|colour_moves;
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
    if(info.num_checkers>1) {

        return 0ULL; // Double check, pawn cannot block or capture both
    }

    U64 possible_moves_2 = possible_moves;
    U64 legal_moves_2 = 0ULL;
    
    // Cehck if position is in en passant pinned squares
    
    for(int i=0;i<2;++i) {
        if(pos==info.en_passant_pinned_squares[i]) {
            if(info.en_passant_pinned[i]==1) {
                possible_moves_2=clear_bit(board->enPassantsq,possible_moves_2);
            }
            break;
        }
    }

    int direction_to_king = get_direction(find_msb(board->turn ? (board->WHITE & board->KINGS) : (board->BLACK & board->KINGS)),pos);
    if(get_bit(pos,info.pinned_pieces)) {
        
        possible_moves_2 &= handle_pinned_pawn_moves(board,pos,possible_moves,colour, info);
        
    }
    if(info.in_check==1) {
        possible_moves_2 &= info.block_or_captures;
        
        legal_moves_2 = possible_moves_2;
    }
    else {
        legal_moves_2 = possible_moves_2;
    }
    
    
    
    return legal_moves_2;

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
    
    populate_magic_numbers(new);
    // Precomputed otherwise it would take 30s to load on a raspberry pi
    



    return new;
    
}

//Putting it all together

U64 get_legal_moves_for_side_bitboards(board_t * board,int colour) {
    U64 legal_moves = 0ULL;
    U64 colour_board = colour ? board->WHITE : board->BLACK;
    U64 opponet_board = colour ? board->BLACK : board->WHITE;
    U64 attack_map = generate_attack_maps(board,colour);
    U64 possible_moves = 0ULL;
    check_info_t info = generate_check_info(board);
    
    for(int i=0;i<64;++i) {
        if(colour) {
        switch(get_piece_at_square(board,i)) {
            case 'P' : possible_moves = get_legal_moves_for_pawn_at_sqaure(board,i,colour,info); break;
            case 'R' : possible_moves = get_legal_moves_for_rook_at_sqaure(board,i,colour,info); break;
            case 'N' : possible_moves = get_legal_moves_for_knight_at_square(board,i,colour,info); break;   
            case 'B' : possible_moves = get_legal_moves_for_bishop_at_sqaure(board,i,colour,info); break;
            case 'Q' : possible_moves = get_legal_moves_for_queen_at_square(board,i,colour,info); break;
            case 'K' : possible_moves = get_legal_moves_for_king_at_sqaure(board,i,colour); break;
            
            default : continue; break;
        }
        legal_moves |= possible_moves;

        } else {
            switch(get_piece_at_square(board,i)) {
                case 'p' : possible_moves = get_legal_moves_for_pawn_at_sqaure(board,i,colour,info); break;
                case 'r' : possible_moves = get_legal_moves_for_rook_at_sqaure(board,i,colour,info); break;
                case 'n' : possible_moves = get_legal_moves_for_knight_at_square(board,i,colour,info); break;   
                case 'b' : possible_moves = get_legal_moves_for_bishop_at_sqaure(board,i,colour,info); break;
                case 'q' : possible_moves = get_legal_moves_for_queen_at_square(board,i,colour,info); break;
                case 'k' : possible_moves = get_legal_moves_for_king_at_sqaure(board,i,colour); break;
                
                default : continue; break;
            }
            legal_moves |= possible_moves;
        }
    
    }
    

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
    check_info_t info = generate_check_info(board);

    

    for(int i=0;i<pop_count;++i) {
        U64 possible_moves = 0ULL;
        char piece = get_piece_at_square(board,msb_pos);
        if(colour) {
            
            switch(piece) {
                case 'P' : possible_moves = get_legal_moves_for_pawn_at_sqaure(board,msb_pos,colour,info); break;
                case 'R' : possible_moves = get_legal_moves_for_rook_at_sqaure(board,msb_pos,colour,info);break;
                case 'N' : possible_moves = get_legal_moves_for_knight_at_square(board,msb_pos,colour,info); break;
                case 'B' : possible_moves = get_legal_moves_for_bishop_at_sqaure(board,msb_pos,colour,info); break;
                case 'Q' : possible_moves = get_legal_moves_for_queen_at_square(board,msb_pos,colour,info); break;
                case 'K' : possible_moves = get_legal_moves_for_king_at_sqaure(board,msb_pos,colour); break;
                default : break;
            }
        } else {
            
            switch(piece) {
                case 'p' : possible_moves = get_legal_moves_for_pawn_at_sqaure(board,msb_pos,colour,info); break;
                case 'r' : possible_moves = get_legal_moves_for_rook_at_sqaure(board,msb_pos,colour, info); break;
                case 'n' : possible_moves = get_legal_moves_for_knight_at_square(board,msb_pos,colour,info); break;
                case 'b' : possible_moves = get_legal_moves_for_bishop_at_sqaure(board,msb_pos,colour,info); break;
                case 'q' : possible_moves = get_legal_moves_for_queen_at_square(board,msb_pos,colour,info); break;
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


