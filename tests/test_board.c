
#include "../Unity/src/unity.h"
#include "../src/board.h"
#include "../src/perft.h"
#include "../src/misc.h"
#include <stdlib.h>
void setUp(void) {
    // Initialize necessary structures if needed
}

// Teardown function (runs after each test)
void tearDown(void) {
    // Clean up resources if needed
}

void magic_number_rook_tests(void) {
    board_t * board = init_board();
    generate_blocker_boards_rooks(board);
    
    free(board);
}


void test_move_generation(void) {
	char fen[] = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    char fen2[] = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
    char fen3[] = "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1";
    char fen4[] = "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1";
    char fen5[] = "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8 ";
    char fen6[] = "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10";
    


    
    board_t * board = init_from_FEN(fen);
    board_stack_t * s = c_stack();
    TEST_ASSERT_EQUAL(20, perft(board, 1, s));
    TEST_ASSERT_EQUAL(400, perft(board, 2, s));
    TEST_ASSERT_EQUAL(8902, perft(board, 3, s));
    TEST_ASSERT_EQUAL(197281, perft(board, 4, s));
    TEST_ASSERT_EQUAL(4865609, perft(board, 5, s));
    printf("1 Done\n");

    board_t * board2 = init_from_FEN(fen2);
    board_stack_t * s2 = c_stack();
    TEST_ASSERT_EQUAL(48, perft(board2, 1, s2));
    TEST_ASSERT_EQUAL(2039, perft(board2, 2, s2)); 
    TEST_ASSERT_EQUAL(97862, perft(board2, 3, s2));
    TEST_ASSERT_EQUAL(4085603, perft(board2, 4, s2));
    TEST_ASSERT_EQUAL(193690690, perft(board2, 5, s2));
    printf("2 Done\n");

    board_t * board3 = init_from_FEN(fen3);
    board_stack_t * s3 = c_stack();
    TEST_ASSERT_EQUAL(14, perft(board3, 1, s3));
    TEST_ASSERT_EQUAL(191, perft(board3, 2, s3));
    TEST_ASSERT_EQUAL(2812, perft(board3, 3, s3));
    TEST_ASSERT_EQUAL(43238, perft(board3, 4, s3));
    TEST_ASSERT_EQUAL(674624, perft(board3, 5, s3));
    printf("3 Done\n");

    board_t * board4 = init_from_FEN(fen4);
    board_stack_t * s4 = c_stack();
    TEST_ASSERT_EQUAL(6, perft(board4, 1, s4));
    TEST_ASSERT_EQUAL(264, perft(board4, 2, s4));
    TEST_ASSERT_EQUAL(9467, perft(board4, 3, s4));
    TEST_ASSERT_EQUAL(422333, perft(board4, 4, s4));
    TEST_ASSERT_EQUAL(15833292, perft(board4, 5, s4));
    printf("4 Done\n");

    board_t * board5 = init_from_FEN(fen5);
    board_stack_t * s5 = c_stack();
    TEST_ASSERT_EQUAL(44, perft(board5, 1, s5));
    TEST_ASSERT_EQUAL(1486, perft(board5, 2, s5));
    TEST_ASSERT_EQUAL(62379, perft(board5, 3, s5));
    TEST_ASSERT_EQUAL(2103487, perft(board5, 4, s5));
    TEST_ASSERT_EQUAL(89941194, perft(board5, 5, s5));
    printf("5 Done\n");

    board_t * board6 = init_from_FEN(fen6);
    board_stack_t * s6 = c_stack();
    TEST_ASSERT_EQUAL(46, perft(board6, 1, s6));
    TEST_ASSERT_EQUAL(2079, perft(board6, 2, s6));
    TEST_ASSERT_EQUAL(89890, perft(board6, 3, s6));
    TEST_ASSERT_EQUAL(164075551, perft(board6, 4, s6));
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

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_move_generation);
    return UNITY_END();
	return 0;
}
