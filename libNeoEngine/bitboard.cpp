#include "bitboard.h"
#include <algorithm>
#include <bitset>



namespace bitboard {

	uint8_t PopCnt16[1 << 16];
	Bitboard SquareBB[SQUARE_NB];
	Bitboard LineBB[SQUARE_NB][SQUARE_NB];


	void setup()
	{
		for (unsigned i = 0; i < (1 << 16); ++i)
			PopCnt16[i] = uint8_t(std::bitset<16>(i).count());

		for (unsigned s = SQ_A1; s <= SQ_H8; ++s)
			SquareBB[s] = (1ULL << s);
	}

}