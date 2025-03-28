
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



void test_move_generation(void) {
	char fen[] = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    
    board_t * board = init_from_FEN(fen);
    board_stack_t * s = c_stack();
    TEST_ASSERT_EQUAL(20, perft(board, 1, s));
    TEST_ASSERT_EQUAL(400, perft(board, 2, s));
    TEST_ASSERT_EQUAL(8902, perft(board, 3, s));
    TEST_ASSERT_EQUAL(197281, perft(board, 4, s));
    TEST_ASSERT_EQUAL(4865609, perft(board, 5, s));
    //TEST_ASSERT_EQUAL(119060324, perft(board, 6, s));
    free(board);
    free(s);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_move_generation);
    return UNITY_END();
	return 0;
}
