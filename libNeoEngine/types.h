#pragma once
#include <cstdint>
#include <assert.h>

#ifdef NDEBUG
#define ASSERT_IF(_d) _d
#else
#define ASSERT_IF(_d) assert(_d)
#endif
constexpr auto _INFINITY = 0xffffff;
typedef std::uint64_t Bitboard;
typedef unsigned int Score;

enum GamePhase : unsigned { MG = 0U, EG, PHASE_NB };

enum Side : unsigned int { WHITE = 0U, BLACK = 1U };
enum EBitboardType : unsigned int
{
	W_PAWNS = 0,
	W_KNIGHTS = 1,
	W_BISHOPS = 2,
	W_ROOKS = 3,
	W_QUEENS = 4,
	W_KINGS = 5,
	
	B_PAWNS = 6,
	B_KNIGHTS = 7,
	B_BISHOPS = 8,
	B_ROOKS = 9,
	B_QUEENS = 10,
	B_KINGS = 11,

	ALL_PAWNS = 12,
	ALL_KNIGHTS = 13,
	ALL_BISHOPS = 14,
	ALL_ROOKS = 15,
	ALL_QUEENS = 16,
	ALL_KINGS = 17,

	ALL_W_PIECES = 18,
	ALL_B_PIECES = 19,

	ALL_PIECES = 20,

	//PINNED_PIECES = 21,
	BITBORD_NB = 21

};


struct CastlingRight {

	enum ECastlingRight : unsigned int { 		
		NO_CASTLE = 0b00,
		KING_SIDE = 0b01,
		QUEEN_SIDE = 0b10, 
		BOTH_SIDES =KING_SIDE | QUEEN_SIDE,
		CASTLING_NB = 4
	};

	//construct CastleRight from fen castle segment
	CastlingRight(std::string const& segment) : _white(NO_CASTLE), _black(NO_CASTLE)
	{
		if (segment[0] != '-')
			for (unsigned int i = 0; i < segment.size(); ++i)
			{
				if (segment[i] == 'K')
				{
					_white = CastlingRight::KING_SIDE;
				}
				else if (segment[i] == 'Q')
				{
					_white = CastlingRight::QUEEN_SIDE;
				}
				else if (segment[i] == 'k')
				{
					_black = CastlingRight::KING_SIDE ;
				}
				else if (segment[i] == 'q')
				{
					_black = CastlingRight::QUEEN_SIDE ;
				}
			}

	}

	//default constructor with full castle rights
	CastlingRight() : _white(BOTH_SIDES),_black(BOTH_SIDES) {};
	
	//add a given right to a given side 
	inline void add(Side side, unsigned right) {
		if (side == WHITE) {
			_white |= right;
		}
		else {
			_black |= right;
		}
	}
	//unset a given right from a given side 
	inline void remove(Side side, unsigned right) {
		if (side == WHITE) {
			_white &= right;
		}
		else {
			_black  &= right;
		}
	}
	//set a given right to a given side 
	inline void set(Side side, unsigned right) {
		if (side == WHITE) {
			_white = right;
		}
		else {
			_black = right;
		}
	}
	inline unsigned get(Side side)const {
		return  side == WHITE? _white : _black;
	}
	bool operator==(CastlingRight& rhs) {
		return get(WHITE) == rhs.get(WHITE) && get(BLACK) == rhs.get(BLACK);
	}
private:
	unsigned _white;
	unsigned _black;

};

/*square stuff*/

enum Square : short unsigned int {
	SQ_A1, SQ_B1, SQ_C1, SQ_D1, SQ_E1, SQ_F1, SQ_G1, SQ_H1,
	SQ_A2, SQ_B2, SQ_C2, SQ_D2, SQ_E2, SQ_F2, SQ_G2, SQ_H2,
	SQ_A3, SQ_B3, SQ_C3, SQ_D3, SQ_E3, SQ_F3, SQ_G3, SQ_H3,
	SQ_A4, SQ_B4, SQ_C4, SQ_D4, SQ_E4, SQ_F4, SQ_G4, SQ_H4,
	SQ_A5, SQ_B5, SQ_C5, SQ_D5, SQ_E5, SQ_F5, SQ_G5, SQ_H5,
	SQ_A6, SQ_B6, SQ_C6, SQ_D6, SQ_E6, SQ_F6, SQ_G6, SQ_H6,
	SQ_A7, SQ_B7, SQ_C7, SQ_D7, SQ_E7, SQ_F7, SQ_G7, SQ_H7,
	SQ_A8, SQ_B8, SQ_C8, SQ_D8, SQ_E8, SQ_F8, SQ_G8, SQ_H8,
	SQ_OUT,
	SQUARE_NB = 64
};

#define IF_VALID_SQUARE(_ind) if(_ind>=SQ_A1 && _ind<=SQ_H8)

enum File : short unsigned int {
	FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H, FILE_NB
};

enum Rank : short unsigned int {
	RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_NB
};

enum Direction : int {
	NORTH = 8,
	EAST = 1,
	SOUTH = -NORTH,
	WEST = -EAST,

	NORTH_EAST = NORTH + EAST,
	SOUTH_EAST = SOUTH + EAST,
	SOUTH_WEST = SOUTH + WEST,
	NORTH_WEST = NORTH + WEST
};


/*pieces */

enum EPieceType : unsigned int
{
	PAWN = 0,
	KNIGHT = 1,
	BISHOP = 2,
	ROOK = 3,
	QUEEN = 4,
	KING = 5,
	EMPTY = 6,
	PIECE_TYPE_NB = 6,
};

enum EPiece : unsigned int
{
	W_PAWN = 0,
	W_KNIGHT = 1,
	W_BISHOP = 2,
	W_ROOK = 3,
	W_QUEEN = 4,
	W_KING = 5,
	B_PAWN = 6,
	B_KNIGHT = 7,
	B_BISHOP = 8,
	B_ROOK = 9,
	B_QUEEN = 10,
	B_KING = 11,
	PIECE_NB = 12,
	NO_PIECE = 14

};

namespace square {
	constexpr Square make_square(File f, Rank r) {
		return Square(r * 8 + f);
	}

	constexpr bool is_ok(Square s) {
		return s >= SQ_A1 && s <= SQ_H8;
	}

	constexpr File file_of(Square s) {
		return File(s & 7);
	}

	constexpr Rank rank_of(Square s) {
		return Rank(s >> 3);
	}

}
