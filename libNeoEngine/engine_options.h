#pragma once
#include <vector>
#include <string>

namespace engine {

	class EngineOption {
	public:
		bool ponder;
		std::vector<std::string> searchmoves;
		unsigned int wtime;
		unsigned int btime;
		unsigned int winc;
		unsigned int binc;
		unsigned int movestogo;
		unsigned int depth;
		unsigned int nodes;
		unsigned int mate;
		unsigned int movetime;
		bool infinite;

		EngineOption() :searchmoves(), ponder(false), wtime(0), btime(0), winc(0), binc(0), movestogo(0), depth(0), nodes(0), mate(0), movetime(0), infinite(false) {}
		void reset();
		bool isForcedTime() const { return (depth > 0 || movetime > 0 || infinite); }
		bool isSetMoveTime()const { return movetime > 0; }
		bool isSetInfinite()const { return infinite; }
		bool isSetDepth()const { return depth > 0; }
		bool isSetMovetogo()const { return movestogo > 0; }
		bool isSetSearchmoves()const { return movestogo > 0; }
	};


}