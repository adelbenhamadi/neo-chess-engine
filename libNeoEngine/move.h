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

	DOUBLE_PAWN_PUSH = 1,
	CASTLING = 2,
	//PROMOTION = 3,
	CAPTURE = 4,
	EP_CAPTURE = 5

};


class Move
{
	unsigned int _move;
public:


	//Default constructor for null move
	inline Move() : _move(0) {}
	inline Move(int move) : _move(move) {}

	//constructor 
	inline Move(Square from, Square to, EMoveType mtype, EPieceType piece, EPieceType promoted = EPieceType::EMPTY)
	{
		_move = (from & 0b111111)
			| ((to & 0b111111) << 6)
			| ((piece & 0b1111) << 12)
			| ((promoted & 0b1111) << 16)
			| ((mtype & 0b111) << 20);

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
