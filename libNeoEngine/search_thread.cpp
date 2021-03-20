#include "search_thread.h"
#include <thread>
#include <mutex>
#include <assert.h>


SearchThread::SearchThread(size_t id) : _id(id), _thisThread(&SearchThread::idle_loop, this) {
    wait();
}

void SearchThread::wait() {
    std::unique_lock<std::mutex> lck(_mutex);
    _condVar.wait(lck, [&] { return !_running; });

}

void SearchThread::clear()
{
    //clear all history
    _history.clear();
}

inline void SearchThread::idle_loop() {
    while (true)
    {
        _running = false;
        std::unique_lock<std::mutex> lk(_mutex);
        wakeupOne();
        _condVar.wait(lk, [&] { return _running; });
        //break on exit
        if (_exit)  return;
        lk.unlock();
        start();
    }
}
//Wake up thread in idle_loop()
inline void SearchThread::start() {
    std::lock_guard<std::mutex> lk(_mutex);
    _running = true;
    wakeupOne();
}

void SearchThread::stop()
{
}

void SearchThread::setup(std::shared_ptr<Board> board, RootMoves& rmoves)
{
}

