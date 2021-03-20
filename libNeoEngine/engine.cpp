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
}