#pragma once


#include <vector>
#include <iostream>
#include <math.h>
#include <sstream>
#include <chrono>
#include <assert.h>

#include "types.h"
#include "tables.h"





#ifdef _MSC_VER
#  include <nmmintrin.h>
#  define __builtin_popcountll _mm_popcnt_u64
#endif

namespace bitboard {



	inline bool moreThanOne(Bitboard i)
	{
		return i & (i - 1);
	}
	/*
	PopulationCount:3 static routines for population count on a given bitboard
	 populationCount(Bitboard i) from Bit Hacks
	 populationCount64swar(Bitboard i) : Swar algo for 64b
	 populationCount64sparse(Bitboard i) : for Sparse bitboards
	*/
	struct PopulationCount {
		// Source : https://graphics.stanford.edu/~seander/bithacks.html
		static inline unsigned int populationCount(Bitboard i)
		{
#ifdef NO_POPCNT
			return popcount64sparse(Bitboard i);
#else
			return (unsigned int)__builtin_popcountll(i);
#endif
		}

		// SWAR algorithm for 64 bits
		static inline unsigned int populationCount64swar(Bitboard i)
		{
			i = i - ((i >> 1) & 0x5555555555555555);
			i = (i & 0x3333333333333333) + ((i >> 2) & 0x3333333333333333);
			return (((i + (i >> 4)) & 0x0f0f0f0f0f0f0f0f) *
				0x0101010101010101) >> 56;
		}

		// "Brian Kernighan's way" https://chessprogramming.wikispaces.com/Population+Count
		// efficient for sparse bitboard
		static inline unsigned int populationCount64sparse(Bitboard i)
		{
			int c = 0;
			for (; i > 0U; c++) {
				i &= i - 1; // reset LS1B
			}
			return c;
		}
	};


	//see https://chessprogramming.wikispaces.com/BitScan

	inline Square msb(const Bitboard& i)
	{
#ifdef _MSC_VER
		unsigned long idx;
		_BitScanReverse64(&idx, i);
		return (Square)idx;

#else
		Bitboard idx;
		__asm__("bsrq %1, %0": "=r"(idx) : "rm"(i));

#endif
		return (Square)idx;

	}
	// Assembly code by Heinz van Saanen

	inline Square lsb(Bitboard i) {
#ifdef _MSC_VER
		unsigned long idx;
		_BitScanForward64(&idx, i);
		return (Square)idx;

#else
		Bitboard idx;
		__asm__("bsfq %1, %0": "=r"(idx) : "rm"(i));
		return (Square)idx;

#endif
	}

	inline Square pop_lsb(Bitboard* b) {
		const Square s = lsb(*b);
		*b &= *b - 1;
		return s;
	}
	inline void setBit(Bitboard& i, Square sq) {
		i |= (1ULL << sq);
	}
	inline void unsetBit(Bitboard& i, Square sq) {
		i &= ~(1ULL << sq);
	}
	inline bool isBitSet(const Bitboard i, Square sq) {
		assert(square::is_ok(sq));
		return i & (1ULL << (sq));
	}
	inline bool isBitSet(const Bitboard i, const unsigned int file, const unsigned int rank)
	{
		Square sq = square::make_square((File)file, (Rank)rank);
		return isBitSet(i, sq);
	}

	extern Bitboard SquareBB[SQUARE_NB];
	extern Bitboard LineBB[SQUARE_NB][SQUARE_NB];

	/// line_bb() returns a bitboard representing an entire line (from board edge
	/// to board edge) that intersects the two given squares. If the given squares
	/// are not on a same file/rank/diagonal, the function returns 0. For instance,
	/// line_bb(SQ_C4, SQ_F7) will return a bitboard with the A2-G8 diagonal.
	inline Bitboard line_bb(Square s1, Square s2) {
		assert(square::is_ok(s1) && square::is_ok(s2));
		return LineBB[s1][s2];
	}

	/// aligned() returns true if the squares s1, s2 and s3 are aligned either on a
	inline bool aligned(Square s1, Square s2, Square s3) {
		return line_bb(s1, s2) & SquareBB[s3];
		//	return LineBB[s1][s2] & SquareBB[s3];
	}




	void setup();

	//Operator overloads for Bitboard and Square
	inline Bitboard operator&(Bitboard b, Square s) {
		return b & SquareBB[s];
	}

	inline Bitboard operator|(Bitboard b, Square s) {
		return b | SquareBB[s];
	}

	inline Bitboard operator^(Bitboard b, Square s) {
		return b ^ SquareBB[s];
	}

	inline Bitboard& operator&=(Bitboard& b, Square s) {
		return b &= SquareBB[s];
	}

	inline Bitboard& operator|=(Bitboard& b, Square s) {
		return b |= SquareBB[s];
	}

	inline Bitboard& operator^=(Bitboard& b, Square s) {
		return b ^= SquareBB[s];
	}

}