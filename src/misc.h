#ifndef __MISC_H__
#define __MISC_H__


U64 generate_path_between_two_points(int from, int to, int exclude_to);
int get_bit(int pos, U64 number);
hash_t rand64(void);
U64 set_bit(int pos, U64 number, int new_bit);
U64 toggle_bit(int pos, U64 number);
U64 clear_bit(int pos, U64 number);
int coordinates_to_number(int rank, int file);
char * int_to_char_sq(int sq);
char * move_to_string(move_t * move);
#endif