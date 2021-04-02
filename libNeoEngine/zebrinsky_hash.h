#pragma once
#include <cstdint>
#include <iostream>
#include <bitset>
#include <random>

#include "types.h"

typedef uint64_t ZKey ;

namespace ZHash
{
	
	//random generator
	extern std::mt19937_64 rdGen;

	extern ZKey piecesKeys[2/*sides*/][PIECE_TYPE_NB][SQUARE_NB];
	extern ZKey epKeys[8/*files*/];
	extern ZKey castleKeys[2/*sides*/][4/*cases: KS , QS , BOTH , NO CASTLE*/];
	extern ZKey sideKey;

	

	void init();
}
