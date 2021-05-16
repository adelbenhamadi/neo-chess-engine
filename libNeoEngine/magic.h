#pragma once
#include "types.h"
#include "tables.h"
#include "bb_utils.h"

using namespace tables;

namespace magics
{
	/// safe_destination() returns the bitboard of target square for the given step
	/// from the given square. If the step is off the board, returns empty bitboard.
	inline Bitboard safe_destination(Square s, int step) {
		Square to = Square(s + step);
		return square::is_ok(to) && distance(s, to) <= 2 ? square_bb(to) : Bitboard(0);
	}

	struct Magic {
		Bitboard  mask;
		Bitboard  magic;
		Bitboard* attacks;
		//Bitboard attacks[64];
		unsigned  shift;

		/*this is what's "magic" int the Magic technic*/
		unsigned index(Bitboard occ) const{
			if (HasPext)
				return unsigned(pext(occupied, mask));
			if (Is64Bit)
				return unsigned(((occ & mask) * magic) >> shift);
			/*for 32bit it's little tricky*/
			unsigned lo = unsigned(occ) & unsigned(mask);
			unsigned hi = unsigned(occ >> 32) & unsigned(mask >> 32);
			return (lo * unsigned(magic) ^ hi * unsigned(magic >> 32)) >> shift;
		}
		
	};

	extern Magic BishopMagics[64];
	extern Magic RookMagics[64];
	extern Bitboard RookTable[0x19000]; 
	extern Bitboard BishopTable[0x1480]; 

};

/// attacks_bb(Square) returns the pseudo attacks of the give piece type
/// assuming an empty board.
template<EPieceType Pt>
inline Bitboard attacks_bb(Square s) {

	assert((Pt != PAWN) && (square::is_ok(s)));

	return tables::PseudoAttacks[Pt][s];
}

inline Bitboard attacks_bb(const EPieceType Pt, const Square sq, Bitboard occ) {

	using namespace magics;
	assert((Pt != PAWN) && (square::is_ok(sq)));
	switch (Pt)
	{
	case BISHOP: return BishopMagics[sq].attacks[BishopMagics[sq].index(occ)];
	case ROOK: return   RookMagics[sq].attacks[RookMagics[sq].index(occ)];
	case QUEEN: return BishopMagics[sq].attacks[BishopMagics[sq].index(occ)] | RookMagics[sq].attacks[RookMagics[sq].index(occ)];
	//case QUEEN: return attacks_bb(BISHOP, sq, occ) | attacks_bb(ROOK, sq, occ);
	default: return tables::PseudoAttacks[Pt][sq];
	}
}

void init_magics(EPieceType pt, Bitboard table[], magics::Magic magics[]);

