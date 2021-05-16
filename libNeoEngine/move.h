#pragma once
#include <iostream>
#include <bitset>
#include <string>
#include <array>
#include "piece.h"
#include "types.h"


/* Move types :
	0	0	0	null move				0
	0	0	1	double pawn push		1
	0	1	0	castling				2
	0	1	1	promotion				3	unused
	1	0	0	normal capture			4
	1	0	1	"en passant" capture	5

*/
enum EMoveType : unsigned {
	QUIET = 0,
	DOUBLE_PAWN_PUSH = 1 << 20,
	CASTLING = 2 << 20,
	PROMOTION = 3 << 20,
	CAPTURE = 4 << 20,
	EP_CAPTURE = 5 << 20

};


class Move
{
	unsigned int _move;
public:


	//Default constructor for null move
	inline Move() noexcept : _move(0),score(0) {}
	inline Move(unsigned move) noexcept : _move(move),score(0) {}

	//constructor 
	inline Move(Square from, Square to, EMoveType mtype, EPiece captured, EPieceType promoted = EPieceType::EMPTY) noexcept
	{
		_move = ((int) from )
			| ((int)to  << 6)
			| (captured  << 12)
			| (promoted  << 16)
			+ mtype ;

	}
	inline unsigned get() const { return _move; }

	inline Square from_sq() const { return static_cast<Square>(_move & 0b111111); }

	inline Square to_sq() const { return static_cast<Square>((_move & 0b111111000000) >> 6); }

	inline unsigned int piece_type() const { return (_move & 0b1111000000000000) >> 12; }

	inline unsigned int promoted_piece() const {
		return (_move & 0b11110000000000000000) >> 16;
	}

	inline EMoveType type() const { return (EMoveType)(_move >> 20); }

	inline unsigned int isDoublePawnPush() const { return type() == DOUBLE_PAWN_PUSH; }
	inline unsigned int isCastling() const { return type() == CASTLING; }
	inline unsigned int isCapture() const {
		return type() & CAPTURE;
	}
	inline unsigned int isEnPassant() const { return type() == EP_CAPTURE; }
	inline unsigned int isPromotion() const { return promoted_piece() /*type() & PROMOTION*/; }

	//isQuiet returns true for null AND double pawn push move
	inline bool isQuiet() const { return !(type() & 0b110); }
	inline bool isNull() const { return _move == 0U; }


};


class RootMove {

};
