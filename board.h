#ifndef __BOARD_H__
#define __BOARD_H__

typedef unsigned long long int U64;
typedef __uint64_t hash_t;

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
    int castleFlags; // 4 bit number 1111 where bits 0 and 1 are WHITE QR and same for BLACK

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
} board_t;

typedef struct Move {
    int from;
    int to;
    int piece;
    int colour;
}move_t;

typedef struct  Node
{
    move_t * data;
     struct  Node* next;
} node_t;

typedef struct linkedList
{
    node_t* head;
} list_t;

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
U64 get_legal_moves_for_knight_at_square(board_t * board, int pos, int colour);
U64 get_legal_moves_for_rook_at_sqaure(board_t *board, int pos, int colour);
U64 get_legal_moves_for_king_at_sqaure(board_t *board, int pos, int colour);
U64 get_legal_moves_for_pawn_at_sqaure(board_t * board,int pos, int colour);
U64 get_legal_moves_for_side(board_t * board,int colour);
#endif