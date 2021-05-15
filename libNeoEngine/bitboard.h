/*
  Stockfish, a UCI chess playing engine derived from Glaurung 2.1
  Copyright (C) 2004-2021 The Stockfish developers (see AUTHORS file)

  Stockfish is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Stockfish is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include <string>

#include "bb_utils.h"
#include "tables.h"
#include "magic.h"


using namespace tables;
using namespace magics;

namespace bitboards {

    void setup();
    const std::string pretty(Bitboard b);
    
}


template<Side side>
constexpr Bitboard pawn_attacks_bb(Bitboard b) {
    return (side == WHITE ?
        ((b & ~MASK_FILE_A) << 7) | ((b & ~MASK_FILE_H) << 9)
        :
        ((b & ~MASK_FILE_A) >> 9) | ((b & ~MASK_FILE_H) >> 7)
        );
}

inline Bitboard pawn_attacks_bb(Side side, Square sq) {
    assert(square::is_ok(sq));
    return PawnAttacks[side][sq];
}

constexpr Bitboard pawn_fork_attacks_bb(Side side,Bitboard b) {
    return (side == WHITE ?
        ((b & ~MASK_FILE_A) << 7) & ((b & ~MASK_FILE_H) << 9)
        :
        ((b & ~MASK_FILE_A) >> 7) & ((b & ~MASK_FILE_H) >> 9)
        );
}


/// pawn_double_attacks_bb() returns the squares doubly attacked by pawns of the
/// given color from the squares in the given bitboard.

template<Side C>
constexpr Bitboard pawn_double_attacks_bb(Bitboard b) {
    return C == WHITE ? ((b & ~MASK_FILE_A) << 7)& ((b & ~MASK_FILE_H) << 9)
        : ((b & ~MASK_FILE_A) >> 9)& ((b & ~MASK_FILE_H) >> 7);
}


/// adjacent_files_bb() returns a bitboard representing all the squares on the
/// adjacent files of a given square.
constexpr Bitboard adjacent_files_bb(Square s) {
    return ((file_bb(s) & ~MASK_FILE_H) << 1) | ((file_bb(s) & ~MASK_FILE_A) >> 1);
}


/// line_bb() returns a bitboard representing an entire line (from board edge
/// to board edge) that intersects the two given squares. If the given squares
/// are not on a same file/rank/diagonal, the function returns 0. For instance,
/// line_bb(SQ_C4, SQ_F7) will return a bitboard with the A2-G8 diagonal.
inline Bitboard line_bb(Square s1, Square s2) {

    assert(square::is_ok(s1) && square::is_ok(s2));
    return tables::LineBB[s1][s2];
}


/// between_bb() returns a bitboard representing squares that are linearly
/// between the two given squares (excluding the given squares). If the given
/// squares are not on a same file/rank/diagonal, we return 0. For instance,
/// between_bb(SQ_C4, SQ_F7) will return a bitboard with squares D5 and E6.
inline Bitboard between_bb(Square s1, Square s2) {
    Bitboard b = line_bb(s1, s2) & ((tables::AllSquares << s1) ^ (tables::AllSquares << s2));
    return b & (b - 1); //exclude lsb
}


/// forward_ranks_bb() returns a bitboard representing the squares on the ranks
/// in front of the given one, from the point of view of the given color. For instance,
/// forward_ranks_bb(BLACK, SQ_D3) will return the 16 squares on ranks 1 and 2.
constexpr Bitboard forward_ranks_bb(Side c, Square s) {
    return c == WHITE ? ~MASK_RANK[0] << 8 * square::relative_rank(WHITE, s)
        : ~MASK_RANK[8] >> 8 * square::relative_rank(BLACK, s);
}


/// forward_file_bb() returns a bitboard representing all the squares along the
/// line in front of the given one, from the point of view of the given color.
constexpr Bitboard forward_file_bb(Side c, Square s) {
    return forward_ranks_bb(c, s) & file_bb(s);
}


/// pawn_attack_span() returns a bitboard representing all the squares that can
/// be attacked by a pawn of the given color when it moves along its file, starting
/// from the given square.
constexpr Bitboard pawn_attack_span(Side c, Square s) {
    return forward_ranks_bb(c, s) & adjacent_files_bb(s);
}


/// passed_pawn_span() returns a bitboard which can be used to test if a pawn of
/// the given color and on the given square is a passed pawn.
constexpr Bitboard passed_pawn_span(Side c, Square s) {
    return pawn_attack_span(c, s) | forward_file_bb(c, s);
}


/// aligned() returns true if the squares s1, s2 and s3 are aligned either on a
/// straight or on a diagonal line.
inline bool aligned(Square s1, Square s2, Square s3) {
    return line_bb(s1, s2) & s3;
}

/// shift() moves a bitboard one or two steps as specified by the direction D

constexpr Bitboard shift(Direction D,Bitboard b) {
    return  D == NORTH ? b << 8 : D == SOUTH ? b >> 8
        : D == NORTH + NORTH ? b << 16 : D == SOUTH + SOUTH ? b >> 16
        : D == EAST ? (b & ~MASK_FILE_H) << 1 : D == WEST ? (b & ~MASK_FILE_A) >> 1
        : D == NORTH_EAST ? (b & ~MASK_FILE_H) << 9 : D == NORTH_WEST ? (b & ~MASK_FILE_A) << 7
        : D == SOUTH_EAST ? (b & ~MASK_FILE_H) >> 7 : D == SOUTH_WEST ? (b & ~MASK_FILE_A) >> 9
        : 0;
}