#pragma once
#include <cstdint>
#include <iostream>
#include <bitset>
#include <random>

namespace ZHash
{
	typedef uint64_t ZKey ;
	//random generator
	extern std::mt19937_64 rdGen;

	extern ZKey psq[ 2/*sides*/][7/*piece types*/][64/*squares*/];
	extern ZKey enPassant[8/*files*/];
	extern ZKey castling[4/*cases*/];
	extern ZKey side;

	void initZobristKeys();
}
