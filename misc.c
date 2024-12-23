#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>

#include "board.h"

int get_bit(int pos, U64 number) {
    return 1 & (number>>pos);
}

hash_t rand64(void) {
  uint64_t r = 0;
  for (int i=0; i<64; i += 15 /*30*/) {
    r = r*((uint64_t)RAND_MAX + 1) + rand();
  }
  return r;
}

U64 set_bit(int pos, U64 number, int new_bit) {
    return ((U64)1<<pos) | number;
    
}

U64 toggle_bit(int pos, U64 number) {
    return (number ^ (1 << (pos)));
}

int coordinates_to_number(int rank, int file) {
    return (8*rank)+file;
}
