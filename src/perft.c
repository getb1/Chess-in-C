#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <pthread.h>
#include <string.h>

#include "board.h"
#include "misc.h"
#include "perft.h"

int perft(board_t * board, int depth, board_stack_t * stack) {
    if (depth == 0) {
        return 1;
    }
    move_t legal_moves[300];
    get_legal_move_side(board, board->turn, legal_moves);
    if(depth==1) {
        int count = 0;
        for(int i=0;i<300;++i) {
            if(legal_moves[i].from==0&&legal_moves[i].to==0) {
                break;
            }
            count++;
        }
        return count;
    }
    int nodes = 0;
    for (int i = 0; i < 300; i++) {

        if (legal_moves[i].from == 0 && legal_moves[i].to == 0) {
            break;
        }

        if(depth==8) {
            printf("%d",i);
        }
        make_move(board, &legal_moves[i], stack);
        nodes += perft(board, depth - 1, stack);
        board = undo_move(stack, board);
    }
    return nodes;
}


void perft_divide(board_t *board, int depth) {
    move_t moves[300];
    get_legal_move_side(board, board->turn, moves);
    int total = 0;
    
    for (int i = 0; i < 300; i++) {
        if (moves[i].from == 0 && moves[i].to == 0) {
            break;
        }
        board_stack_t * stack = c_stack();
        make_move(board, &moves[i], stack);

        int count = perft(board, depth - 1,stack);
        char * from = int_to_char_sq(moves[i].from);
        char * to = int_to_char_sq(moves[i].to);
        
        printf("%d:%s%s: %d\n",i, from, to, count);
        free(from);
        free(to);
        
        total += count;
        undo_move(stack, board);
    }
    printf("Total: %d\n", total);
}

void *thread_function(void* arg) {
    perft_args_t *args = (perft_args_t *)arg;
    board_stack_t * stack = c_stack();
    move_t moves[300];
    get_legal_move_side(args->board, args->board->turn, moves);
    int result = 0;
    for (int i = args->start; i < args->end; i++) {
        if (moves[i].from == 0 && moves[i].to == 0) {
            break;
        }
        make_move(args->board, &moves[i], stack);
        int count = perft(args->board, args->depth - 1, stack);
        result += count;
        undo_move(stack, args->board);
    }
    args->result = result;
    
    free(stack);
    
    
    return NULL;
}

board_t * full_copy_board(board_t * original) {
    board_t * copy = (board_t*)malloc(sizeof(board_t));
    if (!copy) return NULL;
    memcpy(copy, original, sizeof(board_t));
    return copy;
}

int perft_multi_threaded(board_t * board, int depth, int num_threads, int verbose) {

    pthread_t threads[num_threads];
    perft_args_t args[num_threads];

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

    int moves_per_thread = (end-start)/num_threads;
    for(int i=0;i<num_threads;++i) {
        //printf("Starting thread %d\n",i);
        args[i].board = full_copy_board(board);
        args[i].depth = depth;
        args[i].start= start;
        args[i].end = start + moves_per_thread;

        start = start + moves_per_thread;
        if(i==num_threads-1) {
            args[i].end = end;
        }
        args[i].result = 0;
        pthread_create(&threads[i], NULL, thread_function, (void*)&args[i]);

    }

    for(int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
        total += args[i].result;
        if(verbose) {
        printf("Thread %d result: %d\n", i, args[i].result);}
        free(args[i].board);
        
        
    }

    

    
    return total;
    

}

void compare_boards(board_t *board1, board_t *board2) {
    if (board1->WHITE != board2->WHITE) {
        printf("White pieces differ\n");
        display_bitBoard(board1->WHITE);
        display_bitBoard(board2->WHITE);
        printf("\n");
    }
    if (board1->BLACK != board2->BLACK) {
        printf("Black pieces differ\n");
    }
    if (board1->PAWNS != board2->PAWNS) {
        printf("Pawns differ\n");
    }
    if (board1->ROOKS != board2->ROOKS) {
        printf("Rooks differ\n");
    }
    if (board1->KNIGHTS != board2->KNIGHTS) {
        printf("Knights differ\n");
    }
    if (board1->BISHOPS != board2->BISHOPS) {
        printf("Bishops differ\n");
    }
    if (board1->QUEENS != board2->QUEENS) {
        printf("Queens differ\n");
    }
    if (board1->KINGS != board2->KINGS) {
        printf("Kings differ\n");
    }

    if (board1->castleFlags != board2->castleFlags) {
        printf("Castle flags differ\n");
    }
    if (board1->enPassantsq != board2->enPassantsq) {
        printf("En passant squares differ\n");
    }
    if (board1->turn != board2->turn) {
        printf("Turn differs\n");
    }
    if (board1->halfMoveCLock != board2->halfMoveCLock) {
        printf("Half move clock differs\n");
    }
    
}