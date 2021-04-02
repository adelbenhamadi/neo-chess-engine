#include "zebrinsky_hash.h"

namespace ZHash {
	std::mt19937_64 rdGen;

	ZKey piecesKeys[2/*sides*/][PIECE_TYPE_NB][SQUARE_NB];
	ZKey epKeys[8/*files*/];
	ZKey castleKeys[2/*sides*/][4/*castle cases: KS , QS , BOTH , NO CASTLE*/];
	ZKey sideKey;

}
void ZHash::init()
{
	rdGen.seed(1);

	for (unsigned int c = 0; c <= 1; ++c)
	{
		//loop over all piece types and over all squares
		for (unsigned int pt = 0; pt < PIECE_TYPE_NB; ++pt)
		{
			for (unsigned int s = 0; s < SQUARE_NB; ++s)
			{
				piecesKeys[c][pt][s] = rdGen();
			}
		}

		for (unsigned int i = 0; i < 4 /*4 castle cases*/; i++)
		{
			castleKeys[c][i] = rdGen();
		}
	}

	for (unsigned int f = 0; f <= 7; ++f)
	{
		epKeys[f] = rdGen();
	}

	sideKey = rdGen();
}


