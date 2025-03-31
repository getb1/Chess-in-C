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
const int pawn_sq_scores[64] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    5, 5, 5, 5, 5, 5, 5, 5,
    10,10,10,10,10,10,10,10,
    20,20,20,600,600,20,20,20,
    30,30,30,600,600,30,30,30,
    40,40,40,40,40,40,40,40,
    50,50,50,50,50,50,50,50,
    60,60,60,60,60,60,60,60
};

int evaluate_board(board_t * board) {
    int score = 0;

    if(checkmate(board)) {
        return -100000;
    }

    U64 white_pawns = board->PAWNS&board->WHITE;
    U64 black_pawns = board->PAWNS&board->BLACK;
    U64 white_knights = board->KNIGHTS&board->WHITE;
    U64 black_knights = board->KNIGHTS&board->BLACK;    
    U64 white_bishops = board->BISHOPS&board->WHITE;
    U64 black_bishops = board->BISHOPS&board->BLACK;
    U64 white_rooks = board->ROOKS&board->WHITE;
    U64 black_rooks = board->ROOKS&board->BLACK;
    U64 white_queens = board->QUEENS&board->WHITE;
    U64 black_queens = board->QUEENS&board->BLACK;
    U64 white_king = board->KINGS&board->WHITE;
    U64 black_king = board->KINGS&board->BLACK;
    

    // Evaluate pawns
    U64 white_pawn_sq = white_pawns;
    U64 black_pawn_sq = black_pawns;
    while(white_pawn_sq) {
        int sq = find_lsb(white_pawn_sq);
        score += pawn_sq_scores[sq];
        white_pawn_sq = clear_bit(sq, white_pawn_sq);
    }
    

    score += __builtin_popcountll(white_pawns) * 10;
    score -= __builtin_popcountll(black_pawns) * 10;
    
    score += __builtin_popcountll(white_knights) * 30;
    score -= __builtin_popcountll(black_knights) * 30;

    score += __builtin_popcountll(white_bishops) * 30;
    score -= __builtin_popcountll(black_bishops) * 30;

    score += __builtin_popcountll(white_rooks) * 50;
    score -= __builtin_popcountll(black_rooks) * 50;

    score += __builtin_popcountll(white_queens) * 90;
    score -= __builtin_popcountll(black_queens) * 90;

    score += __builtin_popcountll(white_king) * 900;
    score -= __builtin_popcountll(black_king) * 900;
    return score;
}

int minimax(board_t * board, int depth, int alpha, int beta) {
    if(depth == 0 || is_terminal(board)) {
        return evaluate_board(board);
    }
    
    move_t moves[300];
    get_legal_move_side(board, board->turn, moves);
    board_stack_t * stack = c_stack();
    if(moves[0].from==0&&moves[0].to==0) {
        return evaluate_board(board);
    }
    if(is_terminal(board)) {
        return evaluate_board(board);
    }
    if(depth == 0) {
        return evaluate_board(board);
    }
    if(board->turn) {
        int maxEval = -100000;
        for(int i=0;i<300;i++) {
            if(moves[i].from==0&&moves[i].to==0) {
                break;
            }
            make_move(board, &moves[i], stack);
            int eval = minimax(board, depth-1, alpha, beta);
            board = undo_move(stack, board);
            maxEval = max(maxEval, eval);
            alpha = max(alpha, eval);
            if(beta <= alpha) {
                break;
            }
        }
        return maxEval;
    } else {
        int minEval = 100000;
        for(int i=0;i<300;i++) {
            if(moves[i].from==0&&moves[i].to==0) {
                break;
            }
            make_move(board, &moves[i], stack);
            int eval = minimax(board, depth-1, alpha, beta);
            board = undo_move(stack, board);
            minEval = min(minEval, eval);
            beta = min(beta, eval);
            if(beta <= alpha) {
                break;
            }
        }
        return minEval;
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
        int score = minimax(board,depth-1, -100000, 100000);
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

    return best_move;
}




