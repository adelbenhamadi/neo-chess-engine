#pragma once
#include <chrono>
#include <mutex>
#include <thread>


#include "history.h"
#include "board.h"


class SearchThread{

private:
    bool _running = true;
    bool _exit = false;
    size_t _id;
    std::condition_variable _condVar;
    std::mutex _mutex;
    std::thread _thisThread;
    History _history;
    bool _ponder;
    unsigned int _thinkTime;
public:
    bool stopOnPonderhit;

    explicit SearchThread(size_t);
    ~SearchThread() {
        assert(!_running);
        _exit = true;
        start();
        join();
    }

    void wait();
    // wake up one waiter
    void wakeupOne() {
        _condVar.notify_one();
    }
    // wake up all waiters
    void wakeupAll() {
        _condVar.notify_all();
    }
    void clear();
    void idle_loop();
    void start();
    void stop();
    void setup(std::shared_ptr<Board> board, RootMoves& rmoves);
    //setters
    void setPonder(const bool p) { _ponder = p; }
    void join() {
        if (_thisThread.joinable()) {
            _thisThread.join();
        }
    }

    void detach() {
        _thisThread.detach();
    }

    size_t getId() const {
        return _id;
    }

    void setId(size_t id) {
        _id = id;
    }


    bool joinable() {
        return _thisThread.joinable();
    }

    void setState(bool b) {
        _running = !b;
    }
    void setThinkTime(unsigned int t) {
        _thinkTime = t;
    }
};


