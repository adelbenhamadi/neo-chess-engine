#include "bitboard.h"
#include <algorithm>
#include <bitset>

#include "tables.h"

namespace tables {
	
	Bitboard SquareBB[SQUARE_NB];
	Bitboard LineBB[SQUARE_NB][SQUARE_NB];
	uint8_t PopCnt16[1 << 16];
	uint8_t SquareDistance[SQUARE_NB][SQUARE_NB];
	Bitboard PseudoAttacks[PIECE_TYPE_NB][SQUARE_NB];
	Bitboard PawnAttacks[2/*sides*/][SQUARE_NB];
}
namespace bitboards {

	using namespace tables;

	void setup()
	{
		/*fill 16bit population counter:
		 PopCnt16[x]  = # of significant bits in x (x from 0 to 65535) */
		if (!population::USE_POPCNT) {
			for (unsigned i = 0; i < (1 << 16); ++i)
				PopCnt16[i] = uint8_t(std::bitset<16>(i).count());
		}
		/*fill square bitboards table*/
		for (int s = SQ_A1; s <= SQ_H8; ++s)
			SquareBB[s] = (1ULL << s);
		/*fill square distances table*/
		for (int s1 = SQ_A1; s1 <= SQ_H8; ++s1)
			for (int s2 = SQ_A1; s2 <= SQ_H8; ++s2)
				SquareDistance[s1][s2] = std::max(distance<File>((Square)s1, (Square)s2), distance<Rank>((Square)s1, (Square)s2));

        /*generate magics! And fill rook/bishop BB tables*/

        init_magics(ROOK, RookTable, RookMagics);
        init_magics(BISHOP, BishopTable, BishopMagics);
        /*fill PAWN attacks table and pseudo attacks for KING,KNIGHT,QUEEN,ROOK and BISHOP*/
        for (int s1 = SQ_A1; s1 <= SQ_H8; ++s1)
        {
            PawnAttacks[WHITE][s1] = pawn_attacks_bb<WHITE>(square_bb((Square)s1));
            PawnAttacks[BLACK][s1] = pawn_attacks_bb<BLACK>(square_bb((Square)s1));

            for (int step : {-9, -8, -7, -1, 1, 7, 8, 9})
                PseudoAttacks[KING][s1] |= safe_destination((Square)s1, step);

            for (int step : {-17, -15, -10, -6, 6, 10, 15, 17})
                PseudoAttacks[KNIGHT][s1] |= safe_destination((Square)s1, step);

            PseudoAttacks[BISHOP][s1] = attacks_bb(BISHOP, (Square)s1, 0);
            PseudoAttacks[ROOK][s1] = attacks_bb(ROOK, (Square)s1, 0);
            PseudoAttacks[QUEEN][s1] = PseudoAttacks[BISHOP][s1] | PseudoAttacks[ROOK][s1];

            for (EPieceType pt : { BISHOP, ROOK })
                for (int s2 = SQ_A1; s2 <= SQ_H8; ++s2)
                    if (PseudoAttacks[pt][s1] & SquareBB[s2])
                        LineBB[s1][s2] = (attacks_bb(pt, (Square)s1, 0) & attacks_bb(pt, (Square)s2, 0)) | SquareBB[s1] | SquareBB[s2];
        }
        	
    }

    /// Bitboards::pretty() returns an ASCII representation of a bitboard suitable
    /// to be printed to standard output. Useful for debugging.

    const std::string pretty(Bitboard b) {

        std::string s = "+---+---+---+---+---+---+---+---+\n";

        for (int r = RANK_8; r >= RANK_1; --r)
        {
            for (int f = FILE_A; f <= FILE_H; ++f)
                s += b & square::make_square((File)f,(Rank) r) ? "| X " : "|   ";

            s += "| " + std::to_string(1 + r) + "\n+---+---+---+---+---+---+---+---+\n";
        }
        s += "  a   b   c   d   e   f   g   h\n";

        return s;
    }

    


}