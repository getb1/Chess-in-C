#ifndef __BOARD_H__
#define __BOARD_H__
#include <stdlib.h>
#include <stdint.h>
typedef unsigned long long int U64;
typedef __uint64_t hash_t;


extern const int RBits[64];
extern const int BBits[64];

typedef struct Move {
    int from;
    int to;
    char piece;
    int colour;

    int prev_enPassantSq;
    int prev_halfMoveClock;
    int prev_castleFlags;
    char capturedPiece;
    char promotedPiece;
    
    
    int enPassantCaptureSq;
} move_t;



typedef struct MoveStack {
    move_t * moves[4096];
    int top;
} move_stack_t;

typedef struct check_info {
    int in_check;
    U64 checkers;
    U64 block_or_captures;
    U64 pinned_pieces;
    int num_checkers;
    int num_pinned;
    U64 pinned_rays;
    U64 rook_pinned_rays;
    U64 bishop_pinned_rays;
    int en_passant_pinned[2];
    int en_passant_pinned_squares[2];
    
} check_info_t;


typedef struct BOARD {
    //bitboards
    U64 WHITE;
    U64 BLACK;
    U64 PAWNS;
    U64 ROOKS;
    U64 KNIGHTS;
    U64 BISHOPS;
    U64 QUEENS;
    U64 KINGS;
    // other data
    int enPassantsq;
    int moves;
    int turn;
    int halfMoveCLock;
    int castleFlags;// 4 bit number 1111 where bits 0 and 1 are WHITE QR and same for BLACK

    hash_t zorbist_table[12][64];
    hash_t zorbist_castle_table[4];
    hash_t zorbist_en_passant[8];
    hash_t zorbist_to_move;
    hash_t zorbist_hash;
    // Moves
    U64 WHITE_PAWN_MOVES[64];
    U64 BLACK_PAWN_MOVES[64];
    U64 KNIGHT_MOVES[64];
    U64 ROOK_MOVES[64];
    U64 BISHOP_MOVES[64];
    U64 QUEEN_MOVES[64];
    U64 KING_MOVES[64];

    U64 ROOK_BLOCKERS[64][4096];

    U64 ROOK_MAGICS[64];
    U64 BISHOP_MAGICS[64];
    U64 ROOK_MASKS[64];
    U64 BISHOP_MASKS[64];
    U64 ROOK_ATTACKS[64][4096];
    U64 BISHOP_ATTACKS[64][512];
    U64 ROOK_ATTACK_TABLE[64][4096];
    U64 BISHOP_ATTACK_TABLE[64][4096];

    U64 WHITE_ATTACK_MAP;
    U64 BLACK_ATTACK_MAP;

    
} board_t;



typedef struct BOARD_STACK {
    board_t * stack[4096];
    int top;
} board_stack_t;

void display_bitBoard(U64 bitboard);
char get_piece_at_square(board_t * board, int square);
void display_board(board_t * board);
int turn_to_int(char turn);
int char_to_intsq(char sq[]);
U64 precomputePawnMove(int square, int direction);
void precomputePawnMoves(board_t * board);
void precomputeKnightMoves(board_t * board);
void precompute_rook_moves(board_t * board);
int on_board(int pos);
int on_board_rank_file(int rank, int file);
hash_t get_hash_for_piece_at_square(board_t* board, int pos);
hash_t update_hash(board_t * board, move_t * move);
hash_t init_zorbisttable(board_t * board);
board_t * init_from_FEN(char fen[]);
board_t * init_board();
int get_rank(int sq);
int get_file(int sq);
void cool();
U64 get_attacks_for_knight_at_square(board_t * board,int pos,int colour);
U64 get_attacks_for_bishop_at_square(board_t * board,int pos, int colour);
U64 get_attacks_for_rook_at_square(board_t * board,int pos, int colour);
U64 generate_attack_maps(board_t * board,int colour);
int in_check(board_t * board, int colour);
U64 get_legal_moves_for_knight_at_square(board_t * board, int pos, int colour, check_info_t info);
U64 get_legal_moves_for_rook_at_sqaure(board_t *board, int pos, int colour, check_info_t info);
U64 get_legal_moves_for_king_at_sqaure(board_t *board, int pos, int colour);
U64 get_legal_moves_for_pawn_at_sqaure(board_t * board,int pos, int colour, check_info_t info);
U64 get_legal_moves_for_side_bitboards(board_t * board,int colour);
move_t * get_legal_move_side(board_t * board, int colour, move_t * legal_moves);
int make_move(board_t* board,move_t * move,board_stack_t * stack);
int coordinates_to_number(int rank, int file);
void play();
board_t * undo_move(board_stack_t * stack, board_t * board);
void move_test();
void display_stack(board_stack_t *stack);
board_stack_t * c_stack();
void generate_blocker_boards_rooks(board_t * board);
int checkmate(board_t * board);
int stalemate(board_t * board);
int is_terminal(board_t * board);
board_t * copy_board(board_t * original);
check_info_t generate_check_info(board_t * board);
void display_check_info(check_info_t info);
U64 get_diagonal_mask(int square, int is_a1h8);
#endif
