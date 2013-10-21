//
//  executor.cpp
//  ePub3
//
//  Created by Jim Dovey on 2013-08-28.
//  Copyright (c) 2013 The Readium Foundation and contributors. All rights reserved.
//

#include "executor.h"
#include <iostream>
#include <future>

EPUB3_BEGIN_NAMESPACE

class inline_executor : public executor
{
private:
    std::mutex                  _mutex;
    std::atomic_int_fast32_t    _running;
    std::atomic<bool>           _shutdown;
    std::condition_variable     _shutdown_wait;
    
public:
    inline_executor();
    virtual ~inline_executor();
    
    virtual void add(closure_type closure)      OVERRIDE;
    virtual size_t num_pending_closures() const OVERRIDE;
    
};

#if 0
#pragma mark -
#endif

//static thread_pool InitialDefaultExecutor;
static scheduled_executor* DefaultExecutor = nullptr;

scheduled_executor* default_executor()
{
	static std::once_flag __once;
	std::call_once(__once, [=](){
		DefaultExecutor = new thread_pool;
	});
    return DefaultExecutor;
}

void set_default_executor(scheduled_executor* executor)
{
    if ( executor == nullptr )
        throw std::invalid_argument("set_default_executor: null executor argument");

	if (DefaultExecutor != nullptr)
		std::async([](){delete DefaultExecutor;});
    DefaultExecutor = executor;
}

executor* singleton_inline_executor()
{
    static inline_executor InlineExecutor;
    return &InlineExecutor;
}

#if 0
#pragma mark -
#endif

inline_executor::inline_executor() : _mutex(), _running(0), _shutdown(false), _shutdown_wait()
{
}
inline_executor::~inline_executor()
{
    bool cur = false;
    _shutdown.compare_exchange_strong(cur, true);
    assert(cur == false);
    
    // wait for all pending closures to complete
    std::unique_lock<std::mutex> lk(_mutex);
    _shutdown_wait.wait(lk, [&]{return _running == 0;});
    
    if ( bool(_drained_handler) )
        _drained_handler();
}
void inline_executor::add(closure_type closure)
{
    if ( _shutdown )
        return;
    
    // bump the run count
    ++_running;
    
    // execute the closure
    _run_closure(closure);
    
    // drop the run count
    --_running;
    
    if ( _running == 0 && bool(_drained_handler) && !_shutdown )
        _drained_handler();
    
    // signal completion
    _shutdown_wait.notify_all();
}
size_t inline_executor::num_pending_closures() const
{
    int_fast32_t num = _running;
    return (num > 0 ? static_cast<size_t>(num) : 0);
}

#if 0
#pragma mark -
#endif

void loop_executor::loop()
{
    _set_running("loop_executor: closure execution methods called in parallel");
    while ( !_make_loop_exit && !_queue.empty() )
    {
        closure_type closure = _queue.front();
        _run_closure(closure);
        _queue.pop();
    }
    
    _running_closures = false;
    _make_loop_exit = false;
}
void loop_executor::run_queued_closures()
{
    _set_running("loop_executor: closure execution methods called in parallel");
    auto myQueue = _queue;
    while ( !_make_loop_exit && !myQueue.empty() )
    {
        closure_type closure = myQueue.front();
        _run_closure(closure);
        myQueue.pop();
        _queue.pop();
    }
    
    _running_closures = false;
    _make_loop_exit = false;
    
    if ( _queue.empty() && bool(_drained_handler) )
        _drained_handler();
}
bool loop_executor::try_one_closure()
{
    _set_running("loop_executor: closure execution methods called in parallel");
    bool result = !_queue.empty();
    if ( result )
    {
        closure_type closure = _queue.front();
        _run_closure(closure);
        _queue.pop();
    }
    
    _running_closures = false;
    if ( _queue.empty() && bool(_drained_handler) )
        _drained_handler();
    
    return result;
}

#if 0
#pragma mark -
#endif

serial_executor::~serial_executor()
{
    _exiting = true;
    std::unique_lock<std::mutex> lk(_lock);
    _exit_condition.wait(lk, [&]{return _running <= 0;});
}
void serial_executor::add(closure_type closure)
{
    if ( _exiting )
        return;
    
    _queue.push(closure);
    _underlying_executor->add([&]{
        if (_exiting || _queue.empty()) {
            return;
        }
        
        ++_running;
        
        // synchronize the queue fetch/pop operation
        _lock.lock();
        closure_type closure_to_run = _queue.front();
        _queue.pop();
        _lock.unlock();
        
        _run_closure(closure_to_run);
        
        --_running;
        _exit_condition.notify_all();
    });
}

#if 0
#pragma mark -
#endif

thread_pool::thread_pool(int num_threads) : _queue(), _timed_queue(), _threads(), _timed_addition_thread(&thread_pool::_RunTimer, this), _jobs_in_flight(0), _mutex(), _exiting(false), _jobs_ready(), _timers_updated()
{
    if ( num_threads < 1 )
        num_threads = std::thread::hardware_concurrency();
    if ( num_threads < 1 )
        num_threads = 1;
    
    for ( int i = 0; i < num_threads; i++ ) {
        _threads.emplace_back(&thread_pool::_RunWorker, this);
    }
}
thread_pool::~thread_pool()
{
    _exiting = true;
    
    // wake up all threads -- any that are waiting will see _exiting and exit immediately
    _jobs_ready.notify_all();
    _timers_updated.notify_all();
    
    // wait until all threads have exited
    for ( std::thread& thr : _threads ) {
        thr.join();
    }
    
    _timed_addition_thread.join();
}
void thread_pool::add(closure_type closure)
{
    std::unique_lock<std::mutex> _(_mutex);
    
    // enqueue the job
    _queue.push(closure);
    
    // wake one available thread
    _jobs_ready.notify_one();
}
void thread_pool::add_at(std::chrono::system_clock::time_point abs_time, closure_type closure)
{
    std::unique_lock<std::mutex> _(_mutex);
    
    // enqueue the time and the closure-- the priority_queue will sort it into place automatically
    _timed_queue.emplace(abs_time, closure);
    
    // notify the timer thread that changes have been made
    _timers_updated.notify_all();
}
void thread_pool::_RunWorker()
{
    do
    {
        std::unique_lock<std::mutex> lk(_mutex);
        _jobs_ready.wait(lk);
        
        if ( _exiting )
            break;
        
        // NB: mutex is locked at this point, remember
        closure_type closure = _queue.front();
        _queue.pop();
        ++_jobs_in_flight;
        
        // unlock the mutex
        lk.unlock();
        
        // run the closure
        _run_closure(closure);
        
        if ( --_jobs_in_flight == 0 && bool(_drained_handler) )
            _drained_handler();
        
    } while (1);
}
void thread_pool::_RunTimer()
{
    do
    {
        std::unique_lock<std::mutex> lk(_mutex);
        
        std::cv_status status = std::cv_status::no_timeout;
        if ( _timed_queue.empty() )
        {
            // wait to be notified of additions
            _timers_updated.wait(lk);
        }
        else
        {
            // wait until either notified or a timer expires
            status = _timers_updated.wait_until(lk, _timed_queue.top().first);
        }
        
        if ( _exiting )
            break;
        
        // if we hit the timeout, add the topmost item from the timed queue to the real queue
        if ( status == std::cv_status::timeout )
        {
            // remember that the mutex is still locked at this point
            closure_type closure = _timed_queue.top().second;
            _timed_queue.pop();
            
            // unlock the mutex before calling add(), which will want to own the lock itself
            lk.unlock();
            
            // add the closure to the execution queue
            add(closure);
        }
        
    } while (1);
}

EPUB3_END_NAMESPACE
