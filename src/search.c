#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <limits.h>
#include <pthread.h>

#include "board.h"
#include "misc.h"
#include "perft.h"
#include "search.h"
#include "magic_numbers.h"

#define max(a, b) ((a) > (b) ? (a) : (b))

#define MAX_DEPTH 20
#define MATE_SCORE INT_MAX

#define PAWN_VALUE 100
#define KNIGHT_VALUE 320
#define BISHOP_VALUE 330
#define ROOK_VALUE 500
#define QUEEN_VALUE 1000
#define KING_VALUE 10000 

#define ISOLATED_PAWN_PENALTY -20
#define DOUBLED_PAWN_PENALTY -15
#define BACKWARD_PAWN_PENALTY -10
#define PASSED_PAWN_BONUS(rank) (10 * (rank - 1)) // Increases with rank
#define PAWN_ISLAND_PENALTY -10
#define CENTRAL_PAWN_BONUS 10
#define KING_SHIELD_BONUS 5
#define KING_SHIELD_PENALTY -10

int pawn_square_table[64] = {
     0,  0,  0,  0,  0,  0,  0,  0,
    50, 50, 50, 50, 50, 50, 50, 50,
    10, 10, 20, 30, 30, 20, 10, 10,
     5,  5, 10, 25, 25, 10,  5,  5,
     0,  0,  0, 20, 20,  0,  0,  0,
     5, -5,-10,  0,  0,-10, -5,  5,
     5, 10, 10,-20,-20, 10, 10,  5,
     0,  0,  0,  0,  0,  0,  0,  0
};


int knight_square_table[64] = {
    -50,-40,-30,-30,-30,-30,-40,-50,
    -40,-20,  0,  0,  0,  0,-20,-40,
    -30,  0, 10, 15, 15, 10,  0,-30,
    -30,  5, 15, 20, 20, 15,  5,-30,
    -30,  0, 15, 20, 20, 15,  0,-30,
    -30,  5, 10, 15, 15, 10,  5,-30,
    -40,-20,  0,  5,  5,  0,-20,-40,
    -50,-40,-30,-30,-30,-30,-40,-50
};


int bishop_square_table[64] = {
    -20,-10,-10,-10,-10,-10,-10,-20,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5, 10, 10,  5,  0,-10,
    -10,  5,  5, 10, 10,  5,  5,-10,
    -10,  0, 10, 10, 10, 10,  0,-10,
    -10, 10, 10, 10, 10, 10, 10,-10,
    -10,  5,  0,  0,  0,  0,  5,-10,
    -20,-10,-10,-10,-10,-10,-10,-20
};


int rook_square_table[64] = {
      0,  0,  0,  0,  0,  0,  0,  0,
      5, 10, 10, 10, 10, 10, 10,  5,
     -5,  0,  0,  0,  0,  0,  0, -5,
     -5,  0,  0,  0,  0,  0,  0, -5,
     -5,  0,  0,  0,  0,  0,  0, -5,
     -5,  0,  0,  0,  0,  0,  0, -5,
     -5,  0,  0,  0,  0,  0,  0, -5,
      0,  0,  0,  5,  5,  0,  0,  0
};


int queen_square_table[64] = {
    -20,-10,-10, -5, -5,-10,-10,-20,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5,  5,  5,  5,  0,-10,
     -5,  0,  5,  5,  5,  5,  0, -5,
      0,  0,  5,  5,  5,  5,  0, -5,
    -10,  5,  5,  5,  5,  5,  0,-10,
    -10,  0,  5,  0,  0,  0,  0,-10,
    -20,-10,-10, -5, -5,-10,-10,-20
};


int king_square_table_mg[64] = {
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -20,-30,-30,-40,-40,-30,-30,-20,
    -10,-20,-20,-20,-20,-20,-20,-10,
     20, 20,  0,  0,  0,  0, 20, 20,
     20, 30, 10,  0,  0, 10, 30, 20
};


int king_square_table_eg[64] = {
    -50,-40,-30,-20,-20,-30,-40,-50,
    -30,-20,-10,  0,  0,-10,-20,-30,
    -30,-10, 20, 30, 30, 20,-10,-30,
    -30,-10, 30, 40, 40, 30,-10,-30,
    -30,-10, 30, 40, 40, 30,-10,-30,
    -30,-10, 20, 30, 30, 20,-10,-30,
    -30,-30,  0,  0,  0,  0,-30,-30,
    -50,-30,-30,-30,-30,-30,-30,-50
};

int find_game_phase(board_t * board) {
    int score =0;
    score += __builtin_popcountll(board->KNIGHTS)*1;
    score += __builtin_popcountll(board->BISHOPS)*1;
    score += __builtin_popcountll(board->ROOKS)*2;
    score += __builtin_popcountll(board->QUEENS)*4;

    return score;
}

int get_value_of_piece_on_square(int square,char piece, int colour, board_t * board) {

    square = colour==1 ? square : 63-square;

    int score=0;
    int phase = find_game_phase(board);

    switch (piece) {
        case 'P': score+=pawn_square_table[square]; break;
        case 'R': score+=rook_square_table[square]; break;
        case 'N': score+=knight_square_table[square]; break;
        case 'B': score+=bishop_square_table[square]; break;
        case 'Q': score+=queen_square_table[square]; break;
        case 'r': score-=rook_square_table[square]; break;
        case 'p': score-=pawn_square_table[square]; break;
        case 'n': score-=knight_square_table[square]; break;
        case 'b': score-=bishop_square_table[square]; break;
        case 'q': score-=queen_square_table[square]; break;
        
        case 'K': if(phase>=16) {
            score += king_square_table_mg[square]; break;
        } else {
            score += king_square_table_eg[square]; break;
        }

        case 'k': if(phase>=16) {
            score -= knight_square_table[square]; break;
        } else {
            score -= king_square_table_eg[square]; break;
        }
    }
    return score;

}

int evaluate_pawn_structure(board_t * board) {
    int score=0;
    U64 white_pawns = board->WHITE&board->PAWNS;
    U64 black_pawns = board->BLACK&board->PAWNS;

    U64 file_mask = 0x0101010101010101ULL;
    U64 files[] = {file_mask, file_mask<<1, file_mask<<2, file_mask<<3, file_mask<<4, file_mask<<5, file_mask<<6, file_mask<<7};

    int pawns_per_file_white[8] = {0,0,0,0,0,0,0,0};
    int pawns_per_file_black[8] = {0,0,0,0,0,0,0,0};

    for(int i=0;i<8;++i) {
        pawns_per_file_black[i]=__builtin_popcountll(black_pawns&files[i]);
        pawns_per_file_white[i]=__builtin_popcountll(white_pawns&files[i]);
    }

    for (int file = 0; file < 8; file++) {
        if (pawns_per_file_white[file] > 0) {
            
            int isolated = (file == 0 || pawns_per_file_white[file - 1] == 0) &&
                   (file == 7 || pawns_per_file_white[file + 1] == 0);
            if (isolated) {
            score += ISOLATED_PAWN_PENALTY * pawns_per_file_white[file];
            }
            
            if (pawns_per_file_white[file] > 1) {
            score += DOUBLED_PAWN_PENALTY * (pawns_per_file_white[file] - 1);
            }
        }

        if (pawns_per_file_black[file] > 0) {
            int isolated = (file == 0 || pawns_per_file_black[file - 1] == 0) &&
                   (file == 7 || pawns_per_file_black[file + 1] == 0);
            if (isolated) {
            score -= ISOLATED_PAWN_PENALTY * pawns_per_file_black[file];
            }

            if (pawns_per_file_black[file] > 1) {
            score -= DOUBLED_PAWN_PENALTY * (pawns_per_file_black[file] - 1);
            }
        }
    }

    

    return score;
}

int evaluate_position(board_t * board) {

    int score=0;

    int white_material=0;
    int black_material=0;

    white_material += PAWN_VALUE * __builtin_popcountll(board->WHITE&board->PAWNS);
    white_material += KNIGHT_VALUE * __builtin_popcountll(board->WHITE & board->KNIGHTS);
    white_material += BISHOP_VALUE * __builtin_popcountll(board->WHITE & board->BISHOPS);
    white_material += ROOK_VALUE * __builtin_popcountll(board->WHITE & board->ROOKS);
    white_material += QUEEN_VALUE * __builtin_popcountll(board->WHITE & board->QUEENS);
    white_material += KING_VALUE * __builtin_popcountll(board->WHITE & board->KINGS);

    black_material += PAWN_VALUE * __builtin_popcountll(board->BLACK & board->PAWNS);
    black_material += KNIGHT_VALUE * __builtin_popcountll(board->BLACK & board->KNIGHTS);
    black_material += BISHOP_VALUE * __builtin_popcountll(board->BLACK & board->BISHOPS);
    black_material += ROOK_VALUE * __builtin_popcountll(board->BLACK & board->ROOKS);
    black_material += QUEEN_VALUE * __builtin_popcountll(board->BLACK & board->QUEENS);
    black_material += KING_VALUE * __builtin_popcountll(board->BLACK & board->KINGS);

    score = white_material - black_material;
    score+= evaluate_pawn_structure(board);

    for (int square = 0; square < 64; square++) {
        char piece = get_piece_at_square(board, square);
        if (piece != '.') {
            score += get_value_of_piece_on_square(square, piece, board->turn, board);
        }
    }
    U64 center_squares = 0x0000001818000000ULL; 
    U64 white_center = generate_attack_maps(board, 1) & center_squares;
    U64 black_center = generate_attack_maps(board, 0) & center_squares;
    score += (__builtin_popcountll(white_center) - __builtin_popcountll(black_center)) * 10;
    
    
    if (in_check(board, 1)) score -= 50;  
    if (in_check(board, 0)) score += 50;  

    if (board->turn == 0) score = -score;
    return score;


}



int negamax(board_t *board, int depth, board_stack_t *stack, int alpha, int beta) {
    // Terminal cases
    if (depth == 0 || is_terminal(board)) {
        
        if (checkmate(board)) {
            
            
            return -MATE_SCORE + (MAX_DEPTH - depth); 
        }
        if (stalemate(board)) {
            return 0;
        }
        return evaluate_position(board); 
    }
    
    int max_score = INT_MIN;
    move_t moves[300];
    get_legal_move_side(board, board->turn, moves);
    order_moves(moves);
    
    for (int i = 0; i < 300 && moves[i].from != 0; i++) {
        make_move(board, &moves[i], stack);
        
        
        int score = -negamax(board, depth - 1, stack, -beta, -alpha);

        
        undo_move(stack, board);
        
        if (score > max_score) {
            max_score = score;
        }
        alpha = max(score,alpha);
        if(alpha>=beta) {
            break;
        }
    }
    
    return max_score;
}

move_t * order_moves(move_t * moves) {
    // Order the moves based on most valuble captures
    int scores[300]; 
    for(int i=0;i<300;++i) {

        scores[i]=INT_MIN;
    }
    int takeV,capV;
    for(int i=0;i<300;++i) {
        

        if(moves[i].from==0&&moves[i].to==0) {
            break;
        }
        switch(toupper(moves[i].piece)) {
            case 'P': takeV = PAWN_VALUE; break;
            case 'R': takeV = ROOK_VALUE; break;
            case 'N': takeV = KNIGHT_VALUE; break;
            case 'B': takeV = BISHOP_VALUE; break;
            case 'Q': takeV = QUEEN_VALUE; break;
            case 'K': takeV = KING_VALUE; break;
            default: takeV = 0; break;
        }

        switch(toupper(moves[i].capturedPiece)) {
            case 'P': capV = PAWN_VALUE; break;
            case 'R': capV = ROOK_VALUE; break;
            case 'N': capV = KNIGHT_VALUE; break;
            case 'B': capV = BISHOP_VALUE; break;
            case 'Q': capV = QUEEN_VALUE; break;
            case 'K': capV = KING_VALUE; break;
            default: capV = 0; break;
        }

        scores[i] = capV - takeV;
    }
    
    for (int i = 1; i < 300 && moves[i].from != 0; ++i) {
        move_t key = moves[i];
        int key_score = scores[i];
        int j = i - 1;
        while (j >= 0 && scores[j] < key_score) {
            moves[j + 1] = moves[j];
            scores[j + 1] = scores[j];
            j--;
        }
        moves[j + 1] = key;
        scores[j + 1] = key_score;
    }

    


    return moves;

}


move_t find_best_move(board_t * board, int depth) {

    move_t moves[300];
    get_legal_move_side(board, board->turn, moves);
    board_stack_t * stack = c_stack();
    order_moves(moves);
    int best_score= INT_MIN;
    int best_move_pos = -1;
    int score;
    

    for(int i=0;i<300;++i) {
        
        if(moves[i].from==0&&moves[i].to==0) {
            break;
        }
        make_move(board,&moves[i],stack);
        score = -negamax(board,depth-1, stack, INT_MIN, INT_MAX);
        if(score>best_score) {
            best_score = score;
            best_move_pos = i;
        }
        
        undo_move(stack, board);

    }

    if (best_move_pos != -1) {
        printf("Best move: from %d to %d\n", moves[best_move_pos].from, moves[best_move_pos].to);
        return moves[best_move_pos];
    } else {
        printf("No legal moves found.\n");
        return moves[299];
    }
}

void *move_multi_thread(void * arg) {
    search_args_t *args = (search_args_t *)arg;
    board_t * board=args->board;
    move_t moves[300];
    get_legal_move_side(args->board, board->turn, moves);
    board_stack_t * stack = c_stack();
    order_moves(moves);
    int best_score= INT_MIN;
    int best_move_pos = -1;
    int score;
    for(int i=args->start;i<args->end;++i) {
        if(moves[i].from==0&&moves[i].to==0) {
            break;
        }
        make_move(board,&moves[i],stack);
        score = -negamax(board,args->depth-1, stack, INT_MIN, INT_MAX);
        if(score>best_score) {
            best_score = score;
            best_move_pos = i;
        }
        
        undo_move(stack, board);
    }

    args->best_move=moves[best_move_pos];
    args->best_score=best_score;

}

move_t find_best_move_multi_thread(board_t * board, int depth, int threads) {
    pthread_t threads_array[threads];
    search_args_t args[threads];

    move_t moves[300];
    get_legal_move_side(board, board->turn, moves);
    int total = 0;
    int start = 0;
    int end = 0;
    for(int i=0;i<300;++i) {
        if(moves[i].from==0 && moves[i].to==0) {
            break;
        }
        end++;
    }

    int moves_per_thread = (end-start)/threads;
    for(int i=0;i<threads;++i) {
        //printf("Starting thread %d\n",i);
        args[i].board = full_copy_board(board);
        args[i].depth = depth;
        args[i].start= start;
        args[i].end = start + moves_per_thread;

        start = start + moves_per_thread;
        if(i==threads-1) {
            args[i].end = end;
        }
        args[i].best_score = 0;
        pthread_create(&threads_array[i], NULL, move_multi_thread, (void*)&args[i]);

    }

    //Find best_move_from args
    for (int i = 0; i < threads; ++i) {
        pthread_join(threads_array[i], NULL);
    }

    int best_score = INT_MIN;
    int best_move_idx = -1;
    for (int i = 0; i < threads; ++i) {
        if (args[i].best_score > best_score) {
            best_score = args[i].best_score;
            best_move_idx = i;
        }
    }

    if (best_move_idx != -1) {
        return args[best_move_idx].best_move;
    } else {
        move_t dummy = {0};
        return dummy;
    }

}