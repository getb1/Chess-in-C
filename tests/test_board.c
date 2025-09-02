
#include "../Unity/src/unity.h"
#include "../src/board.h"
#include "../src/perft.h"
#include "../src/misc.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
void setUp(void) {
    // Initialize necessary structures if needed
}

// Teardown function (runs after each test)
void tearDown(void) {
    // Clean up resources if needed
}




void test_move_generation(void) {
	char fen[] = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    char fen2[] = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
    char fen3[] = "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1";
    char fen4[] = "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1";
    char fen5[] = "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8";
    char fen6[] = "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10";
    


    
    board_t * board = init_from_FEN(fen);
    display_board(board);
    board_stack_t * s = c_stack();
    TEST_ASSERT_EQUAL(20, perft_multi_threaded(board, 1, 4,0));
    TEST_ASSERT_EQUAL(400, perft_multi_threaded(board, 2, 4,0));
    TEST_ASSERT_EQUAL(8902, perft_multi_threaded(board, 3, 4,0));
    TEST_ASSERT_EQUAL(197281, perft_multi_threaded(board, 4, 4,0));
    TEST_ASSERT_EQUAL(4865609, perft_multi_threaded(board, 5, 4,0));
    printf("1 Done\n");

    board_t * board2 = init_from_FEN(fen2);
    display_board(board2);
    board_stack_t * s2 = c_stack();
    TEST_ASSERT_EQUAL(48, perft_multi_threaded(board2, 1, 4,0));
    TEST_ASSERT_EQUAL(2039, perft_multi_threaded(board2, 2, 4,0)); 
    TEST_ASSERT_EQUAL(97862, perft_multi_threaded(board2, 3, 4,0));
    TEST_ASSERT_EQUAL(4085603, perft_multi_threaded(board2, 4, 4,0));
    //TEST_ASSERT_EQUAL(193690690, perft_multi_threaded(board2, 5, 6,0));
    printf("2 Done\n");

    board_t * board3 = init_from_FEN(fen3);
    board_stack_t * s3 = c_stack();
    TEST_ASSERT_EQUAL(14, perft_multi_threaded(board3, 1, 4,0));
    TEST_ASSERT_EQUAL(191, perft_multi_threaded(board3, 2, 4,0));
    TEST_ASSERT_EQUAL(2812, perft_multi_threaded(board3, 3, 4,0));
    TEST_ASSERT_EQUAL(43238, perft_multi_threaded(board3, 4, 4,0));
    TEST_ASSERT_EQUAL(674624, perft_multi_threaded(board3, 5, 4,0));
    printf("3 Done\n");

    board_t * board4 = init_from_FEN(fen4);
    board_stack_t * s4 = c_stack();
    TEST_ASSERT_EQUAL(6, perft_multi_threaded(board4, 1, 4,0));
    TEST_ASSERT_EQUAL(264, perft_multi_threaded(board4, 2, 4,0));
    TEST_ASSERT_EQUAL(9467, perft_multi_threaded(board4, 3, 4,0));
    TEST_ASSERT_EQUAL(422333, perft_multi_threaded(board4, 4, 4,0));
    TEST_ASSERT_EQUAL(15833292, perft_multi_threaded(board4, 5, 4,0));
    printf("4 Done\n");

    board_t * board5 = init_from_FEN(fen5);
    board_stack_t * s5 = c_stack();
    TEST_ASSERT_EQUAL(44, perft_multi_threaded(board5, 1, 4,0));
    TEST_ASSERT_EQUAL(1486, perft_multi_threaded(board5, 2, 4,0));
    TEST_ASSERT_EQUAL(62379, perft_multi_threaded(board5, 3, 4,0));
    TEST_ASSERT_EQUAL(2103487, perft_multi_threaded(board5, 4, 4,0));
    TEST_ASSERT_EQUAL(89941194, perft_multi_threaded(board5, 5, 4,0));
    printf("5 Done\n");

    board_t * board6 = init_from_FEN(fen6);
    board_stack_t * s6 = c_stack();
    TEST_ASSERT_EQUAL(46, perft_multi_threaded(board6, 1, 4,0));
    TEST_ASSERT_EQUAL(2079, perft_multi_threaded(board6, 2, 4,0));
    TEST_ASSERT_EQUAL(89890, perft_multi_threaded(board6, 3, 4,0));
    TEST_ASSERT_EQUAL(3894594, perft_multi_threaded(board6, 4, 4,0));
    printf("6 Done\n");

    free(board);
    free(s);
    free(board2);
    free(s2);
    free(board3);
    free(s3);
    free(board4);
    free(s4);
    free(board5);
    free(s5);
    free(board6);
    free(s6);

}

void calculate_move_gen_speed() {
    char fen[] = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    board_t * board = init_from_FEN(fen);
    board_stack_t * stack = c_stack();
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    int num_moves = 0;
    for(int i=0;i<10;++i) {
    num_moves+=perft_multi_threaded(board, 6, 6,0);}

    clock_gettime(CLOCK_MONOTONIC, &end);
    double elapsed = (end.tv_sec - start.tv_sec) + 
                    (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("Time: %f ms\n", elapsed);
    int moves_per_second = num_moves / elapsed;
    printf("Perft Result: %d\n", num_moves);
    printf("Moves Per second %d\n", moves_per_second);
    
    free(board);
    free(stack);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_move_generation);
    RUN_TEST(calculate_move_gen_speed);
    return UNITY_END();
	return 0;
}
