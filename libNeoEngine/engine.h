#pragma once
#include <memory>

#include "board.h"
#include "search_thread.h"
#include "thread_pool.h"
#include "engine_options.h"
#include "transposition_table.h"
#include "move_generator.h"

#define USE_THREADING 0
#define USE_TT 0

using namespace std::chrono;

namespace engine {

	constexpr auto IDENTITY = "NEO Chess Engine";
	constexpr auto VERSION = "0.1";
	constexpr auto AUTHOR = "Adel Ben Hamadi";


	constexpr unsigned int MAX_DEPTH = 99;

	struct SearchStats {
		enum EVerboseMode  { INFO , DEBUG};
		steady_clock::time_point _startTime;
		int _processedNodes;
		long long _searchTime;
		unsigned _thinkTime;
		SearchStats() :_searchTime(0), _thinkTime(0) {}
		inline void start() {
			_processedNodes = 0;
			_searchTime = 0;
			_startTime = steady_clock::now();
		}
		inline void incProcessed() {
			_processedNodes++;
		}
		int processed() { return _processedNodes; }
		long long duration() { return _searchTime; }
		unsigned nps() {
			if(_searchTime)
			 return (1000 * _processedNodes /_searchTime);
			return 0;
		}
		inline void reset() {
			_searchTime = duration_cast<milliseconds>(steady_clock::now() - _startTime).count();
			_startTime = steady_clock::now();
		}
		inline void setThinkTime(unsigned int t) {
			_thinkTime = t;
		}
		inline unsigned thinkTime() { return _thinkTime; }
		template<EVerboseMode Mode>
		inline void verbose(int sc,int depth,Move cur,int curi)
		{
			if (Mode == INFO) {
				std::cout << "\ninfo";
				if (sc > piece::typeToValue(KING) - MAX_DEPTH) {
					std::cout << " depth " << depth << " score mate 1";
				}
				else {
					std::cout << " depth " << depth /*- extension*/ << " score cp " << sc;
				}
				
				std::cout << " currmove " << cur.toString();
				std::cout << " currmovenumber " << curi;
				std::cout << " nodes " << _processedNodes;
				std::cout << " nps " << nps();
				std::cout << " time " << _searchTime;
				//std::cout << std::endl;
			}
			else if (Mode == DEBUG) {
				std::cout <<  "\ndebug";
				std::cout << " depth " << depth;
				std::cout << " score cp " << sc;
				std::cout << " currmove " << cur.toString();
				std::cout << " currmovenumber " << curi;
				std::cout << " nodes " << _processedNodes;
				std::cout << " nps " << nps();
				std::cout << " time " << _searchTime;
			}
			//	std::cout << " pv "; TODO
			std::cout << std::endl;
		}
	};

	/*Engine
	*
	Responsible of what a chess engine meant to do:
	processing all legal moves for both player for a certain fixed depth or for a limited time interval
	*/
	class Engine {
		
		EngineOption _options;
		Board _board;
		std::unique_ptr<std::deque<BoardState>> _states;
		unsigned int _ply;
		Move _best;
		int _searchDepth;
		bool _stop = false;
		Move _curMove;
#if USE_THREADING
		ThreadPool _threadPool;
#else
		SearchStats _searchStats;
#endif
#if USE_TT
		TTable _tt;
#endif
	public:
		static bool DEBUG_MODE;
		Engine();
#if USE_THREADING

#else
		int search(const int searchDepth);
		int searchTime(const unsigned utime);
		int negaMax(int depth, int alpha, int beta);
		int negaScout(int depth, int alpha, int beta);
		int quiescence(int alpha, const int beta);
		int evaluate();
#endif
		/*only search laucher:
		* Will take care of starting the thread pool search mechanism
		*/
		void start();
		void stop();
		void reset();
		void newBoard(std::string fen);
		Board& board() { 
			return _board; 
		}
		inline void setOptions(EngineOption engOpt) {
			_options = engOpt;
			if (_options.depth > engine::MAX_DEPTH) _options.depth = engine::MAX_DEPTH;
			if (_options.isSetMoveTime()) {
				_searchStats.setThinkTime(_options.movetime);
			}
			else if (_options.isSetInfinite()) {
				_searchStats.setThinkTime(0x7FFFFFFF);
			}
			else if (_options.isSetSearchmoves()) {
				
			}
			if (_options.isForcedTime() == false) {

			}
		}
		EngineOption options() {
			return _options;}
		
		inline void printBoard(){
			std::cout << _board << std::endl;
		}
		bool executeMove(Move move);
		bool executeMove(std::string str);

		Move bestMove() const{ return _best; }
		
		
	private:
		void setMaxThinkTime(unsigned int t) {
			_threadPool.setThinkTime(t);
		}
		
	
		
	};

}