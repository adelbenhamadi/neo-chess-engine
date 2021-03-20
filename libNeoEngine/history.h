#pragma once
#include <type_traits>
#include <limits>
#include <array>

#include "square.h"
#include "move.h"

template<typename T,int D, int Size,  int ... Sizes>
struct Stats {
	typedef Stats<T, D, Size, Sizes...> stats;

	void fill(const T& v) {
	}
};


enum NB { NB_COLOR , NB_SQUARE, NB_PIECE, NB_PIECE_TYPE};
/*Butterfly Boards:
Two - dimensional arrays(typically of various history counters for each color), 
indexed by the from - and to - square coordinates of(valid and likely quiet) moves,
which appear inside the search.Those counters can then be used for move ordering as
mentioned in the history heuristic, or to decide about late move reductions and history
leaf pruning.Another application is a kind of killer - or refutation table, to store
a refutation of a specific move, also base of the countermove heuristic
[from: www.chessprogramming.org/Butterfly_Boards]
*/
typedef Stats<int16_t, 13365, NB_COLOR, int(NB_SQUARE)* int(NB_SQUARE)> ButterflyHistory;


/*Countermove Heuristic:
a dynamic move ordering heuristic introduced by Jos Uiterwijk in 1992, which shows some
similarities with the killer-, refutation- and history heuristic [2] . This heuristic 
assumes that many moves have a "natural" response, irrespective of the actual position,
and is easy to implement either with a 64 * 64 butterfly table or alternatively a more
memory friendly 6 * 64 array for each side to move, indexed by [from][to] or by [piece][to] [3]
of the previous move, containing the counter move. The nature of the countermove heuristic
renders it complementary to the killer heuristic, substituting position with same distance to
the root with the last move played.
[from: www.chessprogramming.org/Countermove_Heuristic]
*/
typedef Stats<Move,  NB_PIECE, SQUARE_NB> CounterMoveHistory;

/// CapturePieceToHistory is addressed by a move's [piece][to][captured piece type]
typedef Stats<int16_t, 10692, NB_PIECE, SQUARE_NB, NB_PIECE_TYPE> CapturePieceToHistory;
/// PieceToHistory is like ButterflyHistory but is addressed by a move's [piece][to]
typedef Stats<int16_t, 29952, NB_PIECE, SQUARE_NB> PieceToHistory;
/// ContinuationHistory is the combined history of a given pair of moves, usually
/// the current one given a previous one. The nested history table is based on
/// PieceToHistory instead of ButterflyBoards.
typedef Stats<PieceToHistory,  NB_PIECE, SQUARE_NB> ContinuationHistory;

class History
{
	CounterMoveHistory _counterMoves;
	ButterflyHistory _mainHistory;
	CapturePieceToHistory _captureHistory;

public:
	void clear(const bool bAll=false);
};

