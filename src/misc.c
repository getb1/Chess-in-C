#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>

#include "board.h"

int get_bit(int pos, U64 number) {
    
    return (number>>pos)&1==1;
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

U64 clear_bit(int pos,U64 number) {
  return number & ~((U64)1 << pos);
}

U64 toggle_bit(int pos, U64 number) {
    return (number ^ (1 << (pos)));
}

int coordinates_to_number(int rank, int file) {
    return (8*rank)+file;
}

char * int_to_char_sq(int sq) {
    char * sq_str = (char *)malloc(3);
    int rank = get_rank(sq);
    int file = get_file(sq);
    sq_str[0] = 'h' - file;
    sq_str[1] = '1' + rank;
    sq_str[2] = '\0';
    return sq_str;
}

char * move_to_string(move_t * move) {
    char * move_str = (char *)malloc(5);
    char * from = int_to_char_sq(move->from);
    char * to = int_to_char_sq(move->to);
    printf("%c",move->promotedPiece);
    move_str[0] = from[0];
    move_str[1] = from[1];
    move_str[2] = to[0];
    move_str[3] = to[1];
    move_str[4] = '\0';
    free(from);
    free(to);
    return move_str;  
}