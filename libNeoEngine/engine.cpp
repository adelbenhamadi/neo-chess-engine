#include "engine.h"

namespace engine {
	bool Engine::DEBUG_MODE = false;
	
	Engine::Engine(std::shared_ptr<Board> board) :_bord(board), _threadPool(1), _tt() {}

	void Engine::start() {
		_threadPool.start(_bord, *_options);
	}
	void Engine::stop() {
		//TODO : stop engine on uci command : quit
		if(_threadPool.size())
			_threadPool.active()->wait();
	}
	void Engine::reset()
	{
		stop();
		//reset TT
		_tt.reset();
		_threadPool.clearThreads();
	}
	Engine::Engine() : _options(),_states(new std::deque<BoardState>(1)) {
		PESTO::init_tables();
	}

	void Engine::start() {
		_stop = false;
		if (_options.infinite) {
			search(MAX_DEPTH);
		}
		else if (_options.depth > 1) {
			search(_options.depth);
		}
		else {
			searchTime(_options.getAllocatedTime(_board.side()));
		}

	}

	void Engine::stop() {
		_stop = true;
	}

	void Engine::reset()
	{
		stop();
		_options.reset();
	}

	//Quiescence Search
	int Engine::quiescence(int alpha, const int beta)
	{
		int stand_pat = evaluate();

		if (stand_pat >= beta)
		{
			return beta;
		}
		if (alpha < stand_pat)
		{
			alpha = stand_pat;
		}
		BoardState st;
		MoveList moveList;

		MoveGenerator::generateLegalMoves(_board, &moveList);

		for (auto move : moveList)
		{

			if (!_board.capture_or_promotion(move))
				continue;

			int score = 0;
			_board.doMove(move, st);
			_ply++;

			score = -quiescence(-beta, -alpha);

			_board.undoMove(move);
			_ply--;

			if (score >= beta)
			{
				return beta;
			}
			else if (score > alpha)
			{
				alpha = score;
			}

		}

		return alpha;
	}

	int Engine::searchTime(const unsigned utime) {
		int alpha = -_INFINITY;
		int beta = _INFINITY;
		int score = 0;
		_searchStats.start();
		int minDepth = 2;

		_searchDepth = 1;

		while (1) {
			alpha = -_INFINITY;
			beta = _INFINITY;
			score = 0;
			//check for time
			if (_searchStats.duration() > 0.5 * utime && _searchDepth >= minDepth)
			{
				return alpha;
			}
			BoardState st;
			MoveList bestMoves;
			MoveList rootMoves;
			MoveGenerator::generateLegalMoves(_board, &rootMoves);
			int j = 0;
			for (auto move : rootMoves) {
				j++;


				bool isMovelistHalfDone = 2 * j > rootMoves.size();
				_searchStats.reset();
				if (_stop || (_searchStats.duration() > 0.5 * utime && _searchDepth >= minDepth && isMovelistHalfDone))
				{
					return alpha;
				}
				_board.doMove(move, st);
				score = -negaMax(_searchDepth - 1, -beta, -alpha);
#if DEBUG_LVL > 0
				_searchStats.verbose<SearchStats::DEBUG>(score, _searchDepth, move, j);
#endif
				if (score > alpha)
				{
					alpha = score;
					bestMoves.push_back(move);

				}
				_board.undoMove(move);
				_searchStats.incProcessed();
				_searchStats.reset();
				_searchStats.verbose<SearchStats::INFO>(alpha, _searchDepth, move, j);
			}

			_searchDepth++;
		}

		return alpha;
	}

	int Engine::search(const int searchDepth)
	{
		int alpha = -_INFINITY;
		int beta = _INFINITY;
		int score = 0;
		_searchDepth = searchDepth;
		_searchStats.start();
		BoardState st;
		MoveList bestMoves;
		MoveList rootMoves;
		MoveGenerator::generateLegalMoves(_board, &rootMoves);
		int j = 0;
		for (auto move : rootMoves) {
			j++;
			
			_board.doMove(move, st);
			score = -negaScout(_searchDepth - 1, -beta, -alpha);
			
#if DEBUG_LVL>0
			if (DEBUG_MODE)
				_searchStats.verbose<SearchStats::DEBUG>(score, _searchDepth, move, j);
#endif
			if (score > alpha)
			{
				alpha = score;
				bestMoves.push_back(move);
				_searchStats.reset();
				_searchStats.verbose<SearchStats::INFO>(alpha, _searchDepth, move, j);
			}
			_board.undoMove(move);
			_searchStats.incProcessed();
		}

#if DEBUG_LVL>0
		std::cout << "\ninfo string Tot. processed root-moves: " << rootMoves.size();
		std::cout << "\ninfo string bestmoves: ";
		for (auto move : bestMoves) {
			std::cout << move.toString() << ",";
	}
#endif
		_best = bestMoves.back();
		return alpha;
}

	int Engine::negaMax(int depth, int alpha, int beta)
	{
		
		int depth_ext = _board.checkersBB() && _ply <= MAX_DEPTH ? 1 : 0;

		if (depth + depth_ext <= 0)
		{
			/*if (_board.historyMoves().size())
			{
				Move last = _board.historyMoves().back();
				if (_board.captured_piece() != NO_PIECE) {
					//std::cout << "\ninfo string quiescence move: " << last.toString();
					return quiescence(alpha, beta);
				}
			}*/
			if (_board.state()->captured != NO_PIECE) {
				return quiescence(alpha, beta);
			}
			return evaluate();

		}


		int score = -_INFINITY;
		BoardState st;
		MoveList moves;
		
		MoveGenerator::generateLegalMoves(_board, &moves);

		//Check for stalemate/checkmate
		if (!moves.size())
		{
			if (_board.checkersBB())
			{
				//checkmate
				return -(Evaluator::CHECKMATE_SCORE + depth);
			}
			//stalemate
			return Evaluator::DRAW_SCORE;

		}
		
		for (auto move : moves) {
			
			//std::cout << "\n string " << move.toString() << " d=" << depth;
			
			_board.doMove(move, st);
			auto max = -negaMax(depth + depth_ext - 1, -beta, -alpha);

			if (max > score) score = max;
			//adjust the search window 
			if (score > alpha) alpha = score;
			_board.undoMove(move);
			_searchStats.incProcessed();
			
			//_searchStats.verbose<SearchStats::DEBUG>(max, depth, move, -1);
			

			if (alpha >= beta) {
#if DEBUG_LVL > 2
				std::cout << "\ninfo string cutoff [ alpha: " << alpha << " , beta: " << beta << "]\n";
#endif
				return alpha; // cut off
		}

	}


		assert(score > -_INFINITY && score < _INFINITY);
		return score;
	}


	int Engine::negaScout(int depth, int alpha, int beta)
	{

		int depth_ext = _board.checkersBB() && _ply <= MAX_DEPTH ? 1 : 0;

		if (depth + depth_ext <= 0)
		{
			
			if (_board.state()->captured != NO_PIECE) {
				return quiescence(alpha, beta);
			}
			return evaluate();

		}

		int score = -_INFINITY;
		BoardState st;
		MoveList moves;
		
		MoveGenerator::generateLegalMoves(_board, &moves);

		//Check for stalemate/checkmate
		
		if (!moves.size())
		{
			if (_board.checkersBB())
			{
				//checkmate
				return -(Evaluator::CHECKMATE_SCORE + depth);
			}
			//stalemate
			return Evaluator::DRAW_SCORE;

		}
		
		int n = beta;
		for (auto move : moves) {

			//std::cout << "\n string " << move.toString() << " d=" << depth;

			_board.doMove(move, st);
			auto max = -negaScout(depth + depth_ext - 1, -n, -alpha);

			if(max > score) {
				if (n == beta || depth <= 2)
					score = max; 
				else
					score = -negaScout(depth + depth_ext - 1, -beta, -max);
			}
			
			//adjust the search window 
			if (score > alpha) alpha = score;
			_board.undoMove(move);
			_searchStats.incProcessed();

			//_searchStats.verbose<SearchStats::DEBUG>(max, depth, move, -1);


			if (alpha >= beta) {
#if DEBUG_LVL > 2
				std::cout << "\ninfo string cutoff [ alpha: " << alpha << " , beta: " << beta << "]\n";
#endif
				return alpha; // cut off
			}
			n = alpha + 1;
		}


		assert(score > -_INFINITY && score < _INFINITY);
		return score;
	}

	void Engine::newBoard(std::string fen)
	{
			_board =fen.size()? Board(fen) : Board();
		
	}

	inline int Engine::evaluate()
	{
		return PESTO::evaluate(_board);
		//return Evaluator::evaluate(*_board);
	}
	
	bool Engine::executeMove(Move move) {
		
		if (!_board.doMove(move, _states->back())) {
			std::cout << " debug cannot execute move: " << move.toString() << std::endl;
			return false;
		}
		return true;
	}
	
	bool Engine::executeMove(std::string str)
	{		
		MoveList moves;
		MoveGenerator::generateLegalMoves(_board, &moves);
		for (auto move : moves)
		{
			if (str == move.toString() && !move.isNull()) {
				return executeMove(move);
			}

		}
		
		return false;

	}
#endif

}