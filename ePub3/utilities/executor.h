//
//  executor.h
//  ePub3
//
//  Created by Jim Dovey on 2013-08-28.
//  Copyright (c) 2013 The Readium Foundation and contributors. All rights reserved.
//

#ifndef __ePub3__executor__
#define __ePub3__executor__

#include <ePub3/epub3.h>
#include <functional>
#include <chrono>
#include <queue>
#include <atomic>
#include <mutex>
#include <thread>
#include <cassert>
#include <condition_variable>

EPUB3_BEGIN_NAMESPACE

/*
 
    Header <executor> synopsis

        class executor;
        class scheduled_executor;
        static scheduled_executor* default_executor();
        static void set_default_executor(scheduled_executor*executor);

        executor* singleton_inline_executor();

        class executor {
        public:
            virtual ~executor();
            virtual void add(function<void()> closure) = 0;
            virtual size_t num_pending_closures() const = 0;
        };
        
        class scheduled_executor : public executor {
        public:
            virtual void add_at(chrono::system_clock::time_point abs_time, function<void()> closure) = 0;
            virtual void add_after(chrono::system_clock::duration rel_time, function<void()> closure) = 0;
        };
 
    Header <loop_executor> synopsis

        class loop_executor : public executor {
        public:
            loop_executor();
            virtual ~loop_executor();
            
            void loop();
            void run_queued_closures();
            bool try_run_one_closure();
 
            void make_loop_exit();
            
            // [executor methods omitted]
        };
 
    Header <serial_executor> synopsis

        class serial_executor : public executor {
        public:
            explicit serial_executor(executor* underlying_executor);
            virtual ~serial_executor();
            
            executor* underlying_executor();
            
            // [executor methods omitted]
        };
 
    Header <thread_pool> synopsis

        class thread_pool : public scheduled_executor {
        public:
            explicit thread_pool(int num_threads);
            virtual ~thread_pool();
            
            // [executor methods omitted]
        };
 
 */

#if 0
#pragma mark - <executor>
#endif

class executor;
class scheduled_executor;

scheduled_executor* default_executor();
void set_default_executor(scheduled_executor* executor);

executor* singleton_inline_executor();

class executor
{
public:
    typedef std::function<void()>   closure_type;
    
public:
    virtual ~executor() {}
    
    virtual void add(closure_type closure) = 0;
    virtual size_t num_pending_closures() const = 0;
    
    virtual closure_type GetDrainedHandler() const _NOEXCEPT { return _drained_handler; }
    virtual void SetDrainedHandler(closure_type handler) _NOEXCEPT { _drained_handler = handler; }
    
protected:
    inline FORCE_INLINE
    void _run_closure(closure_type closure) const {
        // terminate if a closure throws an exception
        // this matches the paper's guidance
        try {
            closure();
        } catch (...) {
            std::terminate();
        }
    }
    
    closure_type    _drained_handler;
    
};

class scheduled_executor : public executor
{
public:
    virtual void add_at(std::chrono::system_clock::time_point abs_time, closure_type closure) = 0;
    virtual void add_after(std::chrono::system_clock::duration rel_time, closure_type closure) = 0;
    
    template <class _Duration>
    void add_at(std::chrono::time_point<std::chrono::system_clock, _Duration> abs_time, closure_type closure) {
        add_at(std::chrono::time_point_cast<std::chrono::system_clock::duration>(abs_time), closure);
    }
    template <class _Duration>
    void add_after(_Duration rel_time, closure_type closure) {
        add_after(std::chrono::duration_cast<std::chrono::system_clock::duration>(rel_time), closure);
    }
    
};

#if 0
#pragma mark - <loop_executor>
#endif

class loop_executor : public executor
{
private:
    std::queue<closure_type>    _queue;
    std::atomic<bool>           _running_closures;
    std::atomic<bool>           _make_loop_exit;
    
public:
    loop_executor() : _queue(), _running_closures(false), _make_loop_exit(false) {}
    virtual ~loop_executor();
    
    virtual void add(closure_type closure)      OVERRIDE    { _queue.push(closure); }
    virtual size_t num_pending_closures() const OVERRIDE    { return _queue.size(); }
    
    void loop();
    void run_queued_closures();
    bool try_one_closure();
    
    void make_loop_exit() { _make_loop_exit = (bool)_running_closures; }
    
private:
    void _set_running(const char* errorText);
    
};

inline FORCE_INLINE
void loop_executor::_set_running(const char *errorText)
{
    bool notRunning = false;
    bool ok = _running_closures.compare_exchange_strong(notRunning, true);
    assert(ok);
    if ( !ok )
        throw std::system_error(std::make_error_code(std::errc::device_or_resource_busy), errorText);
}

inline FORCE_INLINE
loop_executor::~loop_executor()
{
    _set_running("loop_executor: destructor called while running closures");
}

#if 0
#pragma mark - <serial_executor>
#endif

class serial_executor : public executor
{
private:
    executor*                   _underlying_executor;
    std::queue<closure_type>    _queue;
    std::atomic_int_fast32_t    _running;
    std::atomic<bool>           _exiting;
    std::mutex                  _lock;
    std::condition_variable     _exit_condition;
    
public:
    explicit serial_executor(executor* underlying_executor) : _underlying_executor(underlying_executor) {
        if (_underlying_executor == nullptr) {
            throw std::invalid_argument("serial_executor: underlying_executor cannot be NULL");
        }
    }
    virtual ~serial_executor();
    
    executor* underlying_executor() { return _underlying_executor; }
    
    virtual void add(closure_type closure)      OVERRIDE;
    virtual size_t num_pending_closures() const OVERRIDE    { return _queue.size(); }
    
    virtual closure_type GetDrainedHandler() const _NOEXCEPT OVERRIDE {
        return _underlying_executor->GetDrainedHandler();
    }
    virtual void SetDrainedHandler(closure_type handler) _NOEXCEPT OVERRIDE {
        _underlying_executor->SetDrainedHandler(handler);
    }
    
};

#if 0
#pragma mark - <thread_pool>
#endif

struct __timed_closure_less;
typedef std::pair<std::chrono::system_clock::time_point, executor::closure_type>                timed_closure;
typedef std::priority_queue<timed_closure, std::vector<timed_closure>, __timed_closure_less>    timed_closure_queue;

struct __timed_closure_less : std::binary_function<timed_closure, timed_closure, bool>
{
    inline FORCE_INLINE
    bool operator ()(const timed_closure& __lhs, const timed_closure& __rhs) const {
        return __lhs.first < __rhs.first;
    }
};

class thread_pool : public scheduled_executor
{
private:
    std::queue<closure_type>            _queue;
    timed_closure_queue                 _timed_queue;
    
    std::vector<std::thread>            _threads;
    std::thread                         _timed_addition_thread;
    
    std::atomic_size_t                  _jobs_in_flight;
    
    std::mutex                          _mutex;
    std::atomic<bool>                   _exiting;
    std::condition_variable             _jobs_ready;
    std::condition_variable             _timers_updated;
    
public:
    static const int Automatic          = 0;
    
public:
    thread_pool(int num_threads=Automatic);
    virtual ~thread_pool();
    
    virtual void add(closure_type closure)      OVERRIDE;
    virtual size_t num_pending_closures() const OVERRIDE {
        return _queue.size() + _timed_queue.size() + _jobs_in_flight;
    }
    
    virtual void add_at(std::chrono::system_clock::time_point abs_time, closure_type closure) OVERRIDE;
    virtual void add_after(std::chrono::system_clock::duration rel_time, closure_type closure) OVERRIDE {
        add_at(std::chrono::system_clock::now() + rel_time, closure);
    }
    
private:
    void _RunWorker();
    void _RunTimer();
    
};

EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__executor__) */
