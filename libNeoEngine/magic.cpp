#include "magic.h"
#include "bb_utils.h"
#include "PRNG.h"

namespace magics {
	Magic BishopMagics[64];
	Magic RookMagics[64];

	Bitboard RookTable[0x19000];  // To store rook attacks
	Bitboard BishopTable[0x1480]; // To store bishop attacks

}

using namespace magics;

void print_magics() {
    printf("const uint64 RookMagics[64] = {\n");
    for (int square = 0; square < 64; square++)
        printf("  0x%llxULL,\n", RookMagics[square]);
    printf("};\n\n");

    printf("const uint64 BishopMagics[64] = {\n");
    for (int square = 0; square < 64; square++)
        printf("  0x%llxULL,\n", BishopMagics[square]);
    printf("};\n\n");

}
Bitboard sliding_attack(EPieceType pt, Square sq, Bitboard occupied) {

    Bitboard attacks = 0;
    Direction   RookDirections[4] = { NORTH, SOUTH, EAST, WEST };
    Direction BishopDirections[4] = { NORTH_EAST, SOUTH_EAST, SOUTH_WEST, NORTH_WEST };

    for (Direction d : (pt == ROOK ? RookDirections : BishopDirections))
    {
        Square s = sq;
        while (safe_destination(s, d) && !(occupied & s))
            attacks |= (s += d);
    }

    return attacks;
}


// init_magics() computes all rook and bishop attacks at startup. Magic
// bitboards are used to look up attacks of sliding pieces. As a reference see
// www.chessprogramming.org/Magic_Bitboards. In particular, here we use the so
// called "fancy" approach.
void init_magics(EPieceType pt, Bitboard table[], Magic magics[]) {

    // Optimal PRNG seeds to pick the correct magics in the shortest time
    int seeds[][RANK_NB] = { { 8977, 44560, 54343, 38998,  5731, 95205, 104912, 17020 },
                             {  728, 10316, 55013, 32803, 12281, 15100,  16645,   255 } };

    Bitboard* occupancy = new Bitboard[4096];
    Bitboard* reference = new Bitboard[4096];
    Bitboard edges, b;
    int* epoch = new int[4096];
    int cnt = 0, size = 0;

    for (int s = SQ_A1; s <= SQ_H8; ++s)
    {
        // Board edges are not considered in the relevant occupancies
        edges = ((MASK_RANK[0] | MASK_RANK[7]) & ~rank_bb((Square)s)) | ((MASK_FILE_A | MASK_FILE_H) & ~file_bb((Square)s));

        // Given a square 's', the mask is the bitboard of sliding attacks from
        // 's' computed on an empty board. The index must be big enough to contain
        // all the attacks for each possible subset of the mask and so is 2 power
        // the number of 1s of the mask. Hence we deduce the size of the shift to
        // apply to the 64 or 32 bits word to get the index.
        Magic& m = magics[s];
        m.mask = sliding_attack(pt, (Square)s, 0) & ~edges;
        m.shift = (Is64Bit ? 64 : 32) - population::populationCount(m.mask);

        // Set the offset for the attacks table of the square. We have individual
        // table sizes for each square with "Fancy Magic Bitboards".
        m.attacks = s == SQ_A1 ? table : magics[s - 1].attacks + size;

        // Use Carry-Rippler trick to enumerate all subsets of masks[s] and
        // store the corresponding sliding attack bitboard in reference[].
        b = size = 0;
        do {
            occupancy[size] = b;
            reference[size] = sliding_attack(pt, (Square)s, b);

            if (HasPext)
                m.attacks[pext(b, m.mask)] = reference[size];

            size++;
            b = (b - m.mask) & m.mask;
        } while (b);

        if (HasPext)
            continue;

        PRNG rng(seeds[Is64Bit][square::rank_of((Square)s)]);

        // Find a magic for square 's' picking up an (almost) random number
        // until we find the one that passes the verification test.
        for (int i = 0; i < size; )
        {
            for (m.magic = 0; population::populationCount((m.magic * m.mask) >> 56) < 6; )
                m.magic = rng.sparse_rand<Bitboard>();

            // A good magic must map every possible occupancy to an index that
            // looks up the correct sliding attack in the attacks[s] database.
            // Note that we build up the database for square 's' as a side
            // effect of verifying the magic. Keep track of the attempt count
            // and save it in epoch[], little speed-up trick to avoid resetting
            // m.attacks[] after every failed attempt.
            for (++cnt, i = 0; i < size; ++i)
            {
                unsigned idx = m.index(occupancy[i]);

                if (epoch[idx] < cnt)
                {
                    epoch[idx] = cnt;
                    m.attacks[idx] = reference[i];
                }
                else if (m.attacks[idx] != reference[i])
                    break;
            }
        }
    }

    delete[] occupancy;
    delete[] reference;
    delete[] epoch;

}
