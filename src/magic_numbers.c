#include "magic_numbers.h"
#include "board.h"
#include "misc.h"
#include <stdio.h>
#include <stdlib.h>


U64 random_number () {
    U64 u1,u2,u3,u4;

    u1 = (U64)(random()) & 0xFFFF; u2 = (U64)(random()) & 0xFFFF;
    u3 = (U64)(random()) & 0xFFFF; u4 = (U64)(random()) & 0xFFFF;
    return u1 | (u2 << 16) | (u3 << 32) | (u4 << 48);
}

U64 random_sparse_bits() {
    return random_number() & random_number() & random_number();
}


