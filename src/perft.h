#ifndef __PERFT_H__
#define __PERFT_H__

#include "board.h"
#include "misc.h"

int perft(board_t * board, int depth);
void p(board_t * board, int depth);
#endif