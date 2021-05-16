#pragma once
#include "types.h"
#include "board.h"
#include "move_generator.h"

namespace Evaluator
{
	
	extern const int TOTAL_MATERIAL;
	extern const int DRAW_SCORE;
	extern const int CHECKMATE_SCORE;

	int evaluate(Board& board);

	int getPiecesValue(Side side);


	/* Move ordering */
	inline int evaluate(Board& board, Side side, Move move) {
		int s = 0;
		if (move.isPromotion()) {
			s += piece::typeToValue((EPieceType)move.promoted_type()) - piece::typeToValue(PAWN);
		}

		if (board.capture(move)) {
			auto captured = board.captured(move);
			s += piece::typeToValue(piece::type_of(captured));
		}
		

		return s;
	}
	inline int evaluate(Move move) {
		int s = 0;
		if (move.isPromotion()) {
			s += piece::typeToValue((EPieceType)move.promoted_type()) - piece::typeToValue(PAWN);
		}
		return s;
	}
	static void sortMoveList(MoveList& list);

};

