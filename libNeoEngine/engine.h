#pragma once
#include <memory>

#include "board.h"
#include "search_thread.h"
#include "thread_pool.h"
#include "engine_options.h"
#include "transposition_table.h"


namespace engine {

	constexpr auto IDENTITY = "NEO Chess Engine";
	constexpr auto VERSION = "0.1";
	constexpr auto AUTHOR = "Adel Ben Hamadi";


	constexpr unsigned int MAX_DEPTH = 99;


	/*Engine
	* This is the main component of this project, it is responsible of what a chess engine meant to do:
	processing all legal moves for both player for a certain fixed depth or in a limited time interval
	*/

	class Engine {
		//private stuff
		std::shared_ptr<Board> _bord;
		std::shared_ptr<EngineOption> _options;
		ThreadPool _threadPool;
		unsigned int _ply;
		TTable _tt;
	public:
		static bool DEBUG_MODE;

		Engine(std::shared_ptr<Board> board);
		/*only search laucher:
		* Will take care of starting the thread pool search mechanism
		*/
		void start();
		void stop();
		void reset();
		void setOptions(std::shared_ptr<EngineOption> engOpt) {
			_options = engOpt;
			if (_options->depth > engine::MAX_DEPTH) _options->depth = engine::MAX_DEPTH;
			if (_options->isSetMoveTime()) {
				setMaxThinkTime(_options->movetime);
			}
			else if (_options->isSetInfinite()) {
				setMaxThinkTime(0x7FFFFFFF);
			}
			else if (_options->isSetSearchmoves()) {
				
			}
			if (_options->isForcedTime() == false) {

			}
		}
		void doMove(Move& m){}
		void printBoard(){}
	private:
		void setMaxThinkTime(unsigned int t) {
			_threadPool.setThinkTime(t);
		}
		
	
		
	};

}