#include "engine_options.h"

void engine::EngineOption::reset()
{
	//reset to default go options
	//if (searchmoves.size()) searchmoves.clear();
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

unsigned int engine::EngineOption::getAllocatedTime(Side side, unsigned int divider)
{

	if (movestogo == 0)
	{
		return (side == WHITE ? winc + (wtime - wtime / divider) / divider
			:
			binc + (btime - btime / divider) / divider);
	}
	else
	{
		return (side == WHITE ? winc + (wtime - wtime / (movestogo + 3)) / (0.7 * movestogo + 3)
			:
			binc + (btime - btime / (movestogo + 3)) / (0.7 * movestogo + 3));
	}

	return 0;
}

