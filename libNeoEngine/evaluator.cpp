#include "evaluator.h"

namespace Evaluator {
	const int TOTAL_MATERIAL = 2 * V_QUEEN + 4 * V_ROOK + 4 * V_BISHOP + 4 * V_KNIGHT + 16 * V_PAWN;
	const int DRAW_SCORE = 0;
	const int CHECKMATE_SCORE = 100000;

	template<Side side>
	int evaluate(Board& board) {
		return board.piecesCount(side, PAWN) * V_PAWN +
			board.piecesCount(side, KNIGHT) * V_KNIGHT +
			board.piecesCount(side, BISHOP) * V_BISHOP +
			board.piecesCount(side, ROOK) * V_ROOK +
			board.piecesCount(side, QUEEN) * V_QUEEN;
	}

	int evaluate(Board& board)
	{
		auto side = board.side();
		return (evaluate<WHITE>(board) - evaluate<BLACK>(board)) * (1 - 2 * side );

	}

	int getPiecesValue(Side side)
	{
		return 0;
	}

	void sortMoveList(MoveList& list)
	{
		/*
		std::sort(list.begin(), list.end(), [](const Move& lhs, const Move& rhs)
			{
				return evaluate(lhs) < evaluate(rhs);
			});
			*/
	}

}
