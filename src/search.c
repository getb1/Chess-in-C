#include "board.h"
#include "misc.h"
#include "perft.h"
#include <stdio.h>
int max(int a, int b) {
    return (a > b) ? a : b;
}
int min(int a, int b) {
    return (a < b) ? a : b;
}
int find_lsb(U64 x) {
    if (x == 0) return -1; // No bits are set
    return __builtin_ctzll(x);
}

//Bonus for being near the centre
const int knight_bonus[64] = {
    -50, -40, -30, -30, -30, -30, -40, -50,
    -40, -20, 0, 0, 0, 0, -20, -40,
    -30, 0, 10, 15, 15, 10, 0, -30,
    -30, 5, 15, 20, 20, 15, 5, -30,
    -30, 5, 15, 20, 20, 15, 5, -30,
    -30, 0, 10, 15, 15, 10, 0, -30,
    -40,-20 ,0 ,0 ,0 ,0 ,-20,-40,
    -50,-40,-30,-30,-30,-30,-40,-50
};

const int pawn_bonus[64] = {
    0,0,0,0,0,0,0,0,
    5,5,10,5,5,10,5,5,
    10,10,10,20,20,10,10,10,
    30,30,30,40,40,30,30,30,
    50,50,50,60,60,50,50,50,
    70,70,70,80,80,70,70,70,
    90,90,90,100,100,90,90,90,
    100,100,100,100,100,100,100,100
};


const int king_bonus[64] = {
    20,50,20,20,20,20,50,20,
    20,20,0,0,0,0,20,20,
    0,0,-20,-20,-20,-20,0,0,
    -20,-20,-30,-40,-40,-30,-20,-20,
    -20,-20,-30,-40,-40,-30,-20,-20,
    0,0,-20,-20,-20,-20,0,0,
    -20,-30,-20,0,0,-20,-30,-20,
    -50,-50,-50,-50,-50,-50,-50,-50
};

int evaluate(board_t * board) {

    if(checkmate(board)) {
        if(board->turn == 0) {
            return -99999;
        } else {
            return 99999;
        }
    }
    if(stalemate(board)) {
        return 0;
    }

    const int king_weight = 1000;
    const int queen_weight = 900;
    const int rook_weight = 500;
    const int bishop_weight = 300;
    const int knight_weight = 300;
    const int pawn_weight = 100;

    int white_pawns = board->WHITE&board->PAWNS;
    int black_pawns = board->BLACK&board->PAWNS;
    int white_knights = board->WHITE&board->KNIGHTS;
    int black_knights = board->BLACK&board->KNIGHTS;
    int white_bishops = board->WHITE&board->BISHOPS;
    int black_bishops = board->BLACK&board->BISHOPS;
    int white_rooks = board->WHITE&board->ROOKS;
    int black_rooks = board->BLACK&board->ROOKS;
    int white_queens = board->WHITE&board->QUEENS;
    int black_queens = board->BLACK&board->QUEENS;
    int white_king = board->WHITE&board->KINGS;
    int black_king = board->BLACK&board->KINGS;

    int material_score = king_weight * (__builtin_popcountll(white_king) - __builtin_popcountll(black_king)) +
                         queen_weight * (__builtin_popcountll(white_queens) - __builtin_popcountll(black_queens)) +
                         rook_weight * (__builtin_popcountll(white_rooks) - __builtin_popcountll(black_rooks)) +
                         bishop_weight * (__builtin_popcountll(white_bishops) - __builtin_popcountll(black_bishops)) +
                         knight_weight * (__builtin_popcountll(white_knights) - __builtin_popcountll(black_knights)) +
                         pawn_weight * (__builtin_popcountll(white_pawns) - __builtin_popcountll(black_pawns));

    int mobility_score = 0;

    int knight_mobility_score = 0;
    int pawn_mobility_score = 0;

    for(int i=0;i<64;i++) {
        if(get_bit(i,white_knights)) {
            knight_mobility_score += knight_bonus[i];
        }
        if(get_bit(i,black_knights)) {
            knight_mobility_score -= knight_bonus[i];
        }
        if(get_bit(i,white_pawns)) {
            pawn_mobility_score += pawn_bonus[i];
        }
        if(get_bit(i,black_pawns)) {
            pawn_mobility_score -= pawn_bonus[i];
        }
    }

    mobility_score += pawn_mobility_score;
    mobility_score += knight_mobility_score;
    int king_safety = 0;
    if (white_queens || black_queens) { // Rough middlegame check
        if (white_king & 0x0000001818000000ULL) king_safety -= 20; // King in center
        if (black_king & 0x0000181800000000ULL) king_safety += 20;
        // castled king
        if (white_king & 0x0000000000000022ULL!=0) king_safety += 20;
        if (black_king & 0x2200000000000000ULL) king_safety -= 20;
        
    }

    //King safety bonus
    for(int i = 0; i < 64; i++) {
        if (get_bit(i, white_king)) {
            king_safety += king_bonus[i];
        }
        if (get_bit(i, black_king)) {
            king_safety += king_bonus[i];
        }
    }

    int pawn_structure = 0;
    for (int file = 0; file < 8; file++) {
        U64 file_mask = 0x0101010101010101ULL << file;
        int white_file_pawns = __builtin_popcountll(white_pawns & file_mask);
        int black_file_pawns = __builtin_popcountll(black_pawns & file_mask);
        if (white_file_pawns > 1) pawn_structure -= 10 * (white_file_pawns - 1);
        if (black_file_pawns > 1) pawn_structure += 10 * (black_file_pawns - 1);
    }


    return material_score+mobility_score+king_safety+pawn_structure;

}

int negamax(board_t * board,int depth,int alpha,int beta) {

    if(depth == 0) {
    
        return evaluate(board);
    }
    board_stack_t * stack = c_stack();
    move_t moves[300];
    get_legal_move_side(board, board->turn, moves);

    int best_score = -99999;
    for(int i=0;i<300;i++) {
        if(moves[i].from==0&&moves[i].to==0) {
            break;
        }
        
        make_move(board, &moves[i], stack);
        int score = -negamax(board,depth-1,-beta,-alpha);
        board = undo_move(stack, board);
        best_score = max(best_score,score);
        
        alpha = max(alpha,score);
        if(beta <= alpha) {
            return score;
        }
    }


}

move_t * move_find_best_move(board_t * board, int depth) {
    board_stack_t * stack = c_stack();
    move_t moves[300];
    get_legal_move_side(board, board->turn, moves);
    move_t * best_move = NULL;
    int best_score = -99999;
    
    for(int i=0;i<300;i++) {
        
        if(moves[i].from==0&&moves[i].to==0) {
            break;
        }
        
        make_move(board, &moves[i], stack);
        int score = -negamax(board,depth-1, -100000, 100000);
        printf("Best move score: %d\n",score);
        board = undo_move(stack, board);
        if(score > best_score) {
            best_score = score;
            best_move = &moves[i];
        }
    }
    if(best_move == NULL) {
        printf("No best move found\n");
        return NULL;
    }
    printf("Best move: %s\n",move_to_string(best_move));
    printf("Best move score: %d\n",best_score);
    return best_move;
}




