#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>

#include "board.h"

#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

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

U64 generate_path_between_two_points(int from, int to, int exclude_to) {
    U64 path = 0ULL;
    
    int from_rank = get_rank(from);
    int from_file = get_file(from);
    int to_rank = get_rank(to);
    int to_file = get_file(to);
    path = set_bit(from, path, 1);
    path = set_bit(to, path, 1);
    if (from == to) {
        return path; // No path needed if from and to are the same
    }

    int old_to = to;
    

     

    int rank_step = (to_rank > from_rank) ? 1 : (to_rank < from_rank) ? -1 : 0;
    int file_step = (to_file > from_file) ? 1 : (to_file < from_file) ? -1 : 0;

    int current_rank = from_rank;
    int current_file = from_file;

    for(int i=0; i<max(abs(to_rank - from_rank), abs(to_file - from_file)); ++i) {
        current_rank += rank_step;
        current_file += file_step;
        int current_square = coordinates_to_number(current_rank, current_file);
        path = set_bit(current_square, path, 1);
        
        if (current_square == old_to) {
            break; // Stop if we've reached the destination square
        }
    }

    if(exclude_to) {
        path = clear_bit(old_to, path);
    }
    

    return path;
}