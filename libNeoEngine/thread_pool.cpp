#include "thread_pool.h"


ThreadPool::ThreadPool(size_t sz)
	: _running(true), _incDepth(false)
{
	setSize(sz);
	/*for (size_t i = 0; i < sz; ++i)
		emplace_back(
			[this]
			{
				for (;;)
				{
					std::function<void()> task;

					{
						std::unique_lock<std::mutex> lck(this->_mutex);
						this->_cv.wait(lck,
							[this] { return !this->_running || !this->_queue.empty(); });
						if (!this->_running && this->_queue.empty())
							return;
						task = std::move(this->_queue.front());
						this->_queue.pop();
					}

					task();
				}
			}
			);*/
}

template<class F, class... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args)
-> std::future<typename std::result_of<F(Args...)>::type>
{
	using retType = typename std::result_of<F(Args...)>::type;

	//package the task
	std::packaged_task<retType()> task(std::move(std::bind(f, args...)));

	std::future<retType> future = task->get_future();
	{
		std::unique_lock<std::mutex> lck(_mutex);

		// don't allow enqueueing after stopping the pool
		if (!_running)
			throw std::runtime_error("Error Thread pool is not running: enqueue is not allowed!");

		_queue.emplace([task]() { (*task)(); });
	}
	_cv.notify_one();
	return future;
}

template<typename Func, typename ...Args>
inline auto ThreadPool::push(Func&& f, Args && ...args) {

	//get return type of the function
	typedef decltype(f(args...)) retType;

	//package the task
	std::packaged_task<retType()> task(std::move(std::bind(f, args...)));

	//get the future from task before added to tasks queue
	std::future<retType> future = task.get_future();
	{
		std::unique_lock<std::mutex> lck(_mutex);

		// don't allow enqueueing after stopping the pool
		if (!_running)
			throw std::runtime_error("Error Thread pool is not running: enqueue is not allowed!");

		_queue.emplace(std::move(task));
	}
	_cv.notify_one();

	return future;
}

// the destructor joins all threads
ThreadPool::~ThreadPool()
{
	{
		std::unique_lock<std::mutex> lck(_mutex);
		_running = false;
	}
	_cv.notify_all();
	for (auto th : *this)
		th->join();
}



void ThreadPool::start(std::shared_ptr<Board> board, engine::EngineOption& options)
{
	active()->wait();
	active()->stopOnPonderhit = _running = true;
	active()->setPonder(options.ponder);
	_incDepth = true;
	//setup root nodes to use in setting up threads
	RootMoves rootMoves;
	/*for (const auto& m : board->legalMoves())
		if (options.searchmoves.empty()
			|| std::count(options.searchmoves.begin(), options.searchmoves.end(), m))
			rootMoves.emplace_back(m);*/
	//setup all search threads
	for (SearchThread* th : *this)
	{
		th->setup(board,rootMoves);
	}

	active()->start();
}
void ThreadPool::stop()
{
}
void ThreadPool::clearThreads()
{
	for (auto th : *this)
		th->clear();
}

void ThreadPool::setSize(size_t t)
{
	// destroy all existing threads
	if (size() > 0) {
		active()->wait();

		while (size() > 0)
			delete back(), pop_back();
	}

	if (t > 0) { 
	
		while (size() <= t)
			push_back(new SearchThread(size()));
		clear();

		
	}
}
