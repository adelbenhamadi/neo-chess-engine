#include "engine_options.h"

void engine::EngineOption::reset()
{
	//reset to default go options
	if (searchmoves.size()) searchmoves.clear();
	ponder = false;
	wtime = 0;
	btime = 0;
	winc = 0;
	binc = 0;
	movestogo = 0;
	mate = 0;
	depth = 0;
	nodes = 0;
	movetime = 0;
	infinite = false;

}

