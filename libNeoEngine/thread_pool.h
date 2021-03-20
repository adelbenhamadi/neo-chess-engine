#pragma once
#include <thread>
#include <vector>
#include <future>
#include <queue>
#include "types.h"
#include "search_thread.h"
#include "engine_options.h"
#include "board.h"

class ThreadPool : public std::vector<SearchThread*> {

    // tasks queue
    std::queue< std::function<void()> > _queue;

    // synchronization
    std::mutex _mutex;
    std::condition_variable _cv;
    //state
    bool _running;
    bool _incDepth;
public:
    ThreadPool(size_t);
    ~ThreadPool();
    void clearThreads();
    void setSize(size_t);
    SearchThread* active() { return front(); }
    template<class F, class... Args>
    auto enqueue(F&& f, Args&&... args)
        ->std::future<typename std::result_of<F(Args...)>::type>;
    template <typename Func, typename... Args > auto push(Func&& f, Args&&... args);
    void start(std::shared_ptr<Board> board, engine::EngineOption& options);
    void stop();
    /*set max thinking time in MS :
    we have to propagate this to all search threads*/
    void setThinkTime(unsigned int t) {
        for (SearchThread* th : *this) {
            th->setThinkTime(t);
        }
    }
};


/*
* usage:
int main()
{

    ThreadPool pool(4);
    std::vector< std::future<int> > results;

    for(int i = 0; i < 8; ++i) {
        results.emplace_back(
            pool.enqueue([i] {
                std::cout << "hello " << i << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(1));
                std::cout << "world " << i << std::endl;
                return i*i;
            })
        );
    }

    for(auto && result: results)
        std::cout << result.get() << ' ';
    std::cout << std::endl;

    return 0;
}
*/