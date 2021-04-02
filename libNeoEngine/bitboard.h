#pragma once


#include <vector>
#include <iostream>
#include <math.h>
#include <sstream>
#include <chrono>


#include "types.h"
#include "tables.h"





#ifdef _MSC_VER
#  include <nmmintrin.h>
#  define __builtin_popcountll _mm_popcnt_u64
#endif

namespace bitboard {

#define set_bit(bitboard, square) ((bitboard) |= (1ULL << (square)))
#define get_bit(bitboard, square) ((bitboard) & (1ULL << (square)))
#define pop_bit(bitboard, square) ((bitboard) &= ~(1ULL << (square)))

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
		i |= (1ULL << sq );
	}
	inline void unsetBit(Bitboard& i, Square sq) {
		i &= ~(1ULL << sq);
	}
	inline bool isBitSet(const Bitboard i, Square sq) {
		return i & (1ULL << (sq));
	}
	inline bool isBitSet(const Bitboard i, const unsigned int file, const unsigned int rank)
	{
		unsigned int sq = file + 8 * rank ;
		return i & (1ULL << sq);
		//return (0 | 1ULL << sq) & i;
	}

	inline bool areAligned(const Square s1, const Square s2, const Square s3) {
		return false;
		//return Tables::LINE_BB[s1][s2] & Tables::SquareBB[s3];
	}

	extern Bitboard SquareBB[SQUARE_NB];
	extern Bitboard LineBB[SQUARE_NB][SQUARE_NB];

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