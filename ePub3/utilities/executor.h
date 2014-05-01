//
//  executor.h
//  ePub3
//
//  Created by Jim Dovey on 2013-08-28.
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//  
//  This program is distributed in the hope that it will be useful, but WITHOUT ANY 
//  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
//  
//  Licensed under Gnu Affero General Public License Version 3 (provided, notwithstanding this notice, 
//  Readium Foundation reserves the right to license this material under a different separate license, 
//  and if you have done so, the terms of that separate license control and the following references 
//  to GPL do not apply).
//  
//  This program is free software: you can redistribute it and/or modify it under the terms of the GNU 
//  Affero General Public License as published by the Free Software Foundation, either version 3 of 
//  the License, or (at your option) any later version. You should have received a copy of the GNU 
//  Affero General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.

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
#include <ePub3/utilities/invoke.h>

EPUB3_BEGIN_NAMESPACE

/*
 
    Header <executor> synopsis

        class executor;
        class scheduled_executor;

        executor* singleton_inline_executor();

        class executor {
        public:
            virtual ~executor();
            virtual void add(function<void()> closure) = 0;
            virtual size_t uninitiated_task_count() const = 0;
        };
        
        class scheduled_executor : public executor {
        public:
            virtual void add_at(chrono::system_clock::time_point& abs_time, function<void()> closure) = 0;
            virtual void add_after(chrono::system_clock::duration& rel_time, function<void()> closure) = 0;
        };
 
    Header <thread_pool> synopsis

        class thread_pool;
 
        class thread_pool : public scheduled_executor {
        public:
            explicit thread_pool(int num_threads);
            virtual ~thread_pool();
            
            // [executor methods omitted]
        };
 
    Header <serial_executor> synopsis

        class serial_executor;
 
        class serial_executor : public executor {
        public:
            explicit serial_executor(executor& underlying_executor);
            virtual ~serial_executor();
            
            executor* underlying_executor();
            
            // [executor methods omitted]
        };
 
    Header <loop_executor> synopsis

        class loop_executor;
 
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
 
    Header <inline_executor> synopsis

        class inline_executor;
 
        class inline_executor : public executor {
        public:
            explicit inline_executor();
            // [executor methods omitted]
        };
 
    Header <thread_executor> synopsis

        class thread_executor;
 
        class thread_executor : public executor {
        public:
            explicit thread_executor();
            // [executor methods omitted]
        };
 
 */

#if 0
#pragma mark - <executor>
#endif

class executor;
class scheduled_executor;
class thread_pool;
class serial_executor;
class loop_executor;
class inline_executor;
class thread_executor;

class __thread_pool_impl_stdcpp;
#if EPUB_PLATFORM(WINRT)
class __thread_pool_impl_winrt;
#endif

class executor
    : public std::enable_shared_from_this<executor>
{
public:
    typedef std::function<void()>   closure_type;
    
public:
    virtual FORCE_INLINE
    ~executor()
        {}
    
    virtual
    void add(closure_type closure) = 0;
    
    virtual
    size_t uninitiated_task_count() const = 0;
    
protected:
    inline FORCE_INLINE
    static
    void _run_closure(closure_type closure)
        {
            // terminate if a closure throws an exception
            // this matches the paper's guidance
            try {
                invoke(closure);
            } catch (...) {
#ifndef NDEBUG
                std::exception_ptr __exc = std::current_exception();
                try
                {
                    std::rethrow_exception(__exc);
                }
                catch (std::exception& __e)
                {
                    std::cerr << "executor::_run_closure: caught exception" << std::endl;
                    std::cerr << __e.what() << std::endl;
                }
#endif
                std::terminate();
            }
        }

	friend class __thread_pool_impl_stdcpp;
#if EPUB_PLATFORM(WINRT)
	friend class __thread_pool_impl_winrt;
#endif
    
};

class scheduled_executor
    : public executor
{
public:
    virtual
    void add_at(std::chrono::system_clock::time_point& abs_time, closure_type closure) = 0;
    
    virtual
    void add_after(std::chrono::system_clock::duration& rel_time, closure_type closure) = 0;
    
    template <class _Clock, class _Duration>
    void add_at(std::chrono::time_point<_Clock, _Duration>& abs_time, closure_type closure)
        {
            add_at(std::chrono::time_point_cast<std::chrono::system_clock::duration>(abs_time), closure);
        }
    
    template <class _Rep, class _Period>
    void add_after(std::chrono::duration<_Rep, _Period>& rel_time, closure_type closure)
        {
            add_after(std::chrono::duration_cast<std::chrono::system_clock::duration>(rel_time), closure);
        }
    
};

#if 0
#pragma mark - <loop_executor>
#endif

class loop_executor
    : public executor
{
private:
    std::queue<closure_type>    _queue;
    std::atomic<bool>           _running_closures;
    std::atomic<bool>           _make_loop_exit;
    
public:
    FORCE_INLINE
    loop_executor()
        : _queue(), _running_closures(false), _make_loop_exit(false)
        {}
    
    virtual
    ~loop_executor();
    
    virtual
    void add(closure_type closure) OVERRIDE
        {
            _queue.push(closure);
        }
    
    virtual
    size_t uninitiated_task_count() const OVERRIDE
        {
            return _queue.size();
        }
    
    void loop();
    void run_queued_closures();
    bool try_one_closure();
    
    FORCE_INLINE
    void make_loop_exit()
        {
            _make_loop_exit = (bool)_running_closures;
        }
    
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

class serial_executor
    : public executor
{
private:
    executor*                   _underlying_executor;
    std::queue<closure_type>    _queue;
    std::atomic_int_fast32_t    _running;
    std::atomic<bool>           _exiting;
    std::mutex                  _lock;
    std::condition_variable     _exit_condition;
    
public:
    explicit FORCE_INLINE
    serial_executor(executor& underlying_executor)
        : _underlying_executor(&underlying_executor),
          _queue(),
          _running(0),
          _exiting(false),
          _lock(),
          _exit_condition()
        {
            if (_underlying_executor == nullptr) {
                throw std::invalid_argument("serial_executor: underlying_executor cannot be NULL");
            }
        }
    
    virtual
    ~serial_executor();
    
    FORCE_INLINE
    executor& underlying_executor()
        {
            return *_underlying_executor;
        }
    
    virtual
    void add(closure_type closure) OVERRIDE;
    
    virtual
    size_t uninitiated_task_count() const OVERRIDE
        {
            return _queue.size();
        }
    
};

#if 0
#pragma mark - <inline_executor>
#endif

class inline_executor
    : public executor
{
public:
    explicit FORCE_INLINE
    inline_executor()
        {}
    
    virtual FORCE_INLINE
    ~inline_executor()
        {}
    
    virtual
    void add(closure_type closure) OVERRIDE
        {
            _run_closure(closure);
        }
    
    virtual
    size_t uninitiated_task_count() const OVERRIDE
        {
            return 0;
        }
    
};

#if 0
#pragma mark - <thread_executor>
#endif

class thread_executor
    : public executor
{
private:
//    std::map<std::thread::id, std::thread>  __threads_;
    std::mutex                              __lock_;
    std::condition_variable                 __cleanup_cv_;
    std::atomic_size_t                      __pending_count_;
    std::atomic_size_t                      __running_count_;
    std::atomic<bool>                       __destruct_;
    /*
    class __thread_reaper
    {
        typedef std::pair<thread_executor*, std::thread::id>    _ReapPair;
        
        std::thread             __reaper_thread_;
        std::queue<_ReapPair>   __to_reap_;
        std::mutex              __lock_;
        std::condition_variable __wake_;
        std::atomic<bool>       __destruct_;
        
    public:
        FORCE_INLINE
        __thread_reaper()
            : __to_reap_(),
              __lock_(),
              __wake_(),
              __destruct_(),
              __reaper_thread_(std::mem_fn(&__thread_reaper::_run_reaper_thread), this)
            {}
        
        ~__thread_reaper()
            {
                __destruct_ = true;
                __wake_.notify_all();
                if (__reaper_thread_.joinable())
                    __reaper_thread_.join();
            }
        
        FORCE_INLINE
        void reap_thread(thread_executor* __exec, std::thread::id __thr)
            {
                if (__destruct_)
                    return;
                
                std::unique_lock<std::mutex> __lk(__lock_);
                __to_reap_.emplace(__exec, __thr);
                __wake_.notify_all();
            }
        
        void reap_all(thread_executor* __exec, std::vector<std::thread::id>& __thr)
            {
                if (__destruct_)
                    return;
                
                std::unique_lock<std::mutex> __lk(__lock_);
                for (auto& __id : __thr)
                {
                    __to_reap_.emplace(__exec, __id);
                }
                __wake_.notify_all();
            }
        
    private:
        void _run_reaper_thread()
            {
                while (!__destruct_)
                {
                    std::unique_lock<std::mutex> __lk(__lock_);
                    __wake_.wait(__lk, [this](){return __destruct_ || !__to_reap_.empty();});
                    
                    while (!__to_reap_.empty())
                    {
                        auto __p = __to_reap_.front();
                        auto __thr = __p.first->__threads_.find(__p.second);
                        if (__thr == __p.first->__threads_.end())
                        {
                            __p.first->__cleanup_cv_.notify_all();
                            continue;
                        }
                        
                        if (__thr->second.joinable())
                        {
                            // unlock the mutex before calling join()
                            __lk.unlock();
                            
                            try
                            {
                                // race between joinable() and join(), grrr...
                                __thr->second.join();
                            }
                            catch (...)
                            {
                            }
                            
                            __lk.lock();
                        }
                        
                        __p.first->__threads_.erase(__thr);
                        
                        if (__p.first->__threads_.empty())
                            __p.first->__cleanup_cv_.notify_all();
                        
                        __to_reap_.pop();
                    }
                }
            }
        
    };
    
    static __thread_reaper                  __reaper_;
    */
public:
    explicit FORCE_INLINE
    thread_executor()
        : /*__threads_(), */__lock_(), __cleanup_cv_(), __pending_count_(0), __running_count_(0)
        {}
    
    virtual
    ~thread_executor()
        {
            __destruct_ = true;
            
            std::unique_lock<std::mutex> __lk(__lock_);
            /*
            std::vector<std::thread::id> __vec;
            for (auto& __item : __threads_)
            {
                __vec.emplace_back(__item.first);
            }
            
            if (!__vec.empty())
            {
                __reaper_.reap_all(this, __vec);
                __cleanup_cv_.wait(__lk, [this](){return __threads_.empty();});
            }
             */
            __cleanup_cv_.wait(__lk, [this](){return __running_count_ == 0;});
        }
    
    virtual
    void add(closure_type closure) OVERRIDE
        {
            if (__destruct_)
                return;
            
            __pending_count_++;
            
            // this lock stops the thread from invoking the closure until this method exits
            std::lock_guard<std::mutex> __lk(__lock_);
            std::thread __thr(std::mem_fn(&thread_executor::__run_closure_thread), this, closure);
//#if EPUB_HAVE(CXX_MAP_EMPLACE)
//            __threads_.emplace(__thr.get_id(), std::move(__thr));
//#else
//            __threads_[__thr.get_id()] = std::move(__thr);
//#endif
            __thr.detach();
        }
    
    virtual
    size_t uninitiated_task_count() const OVERRIDE
        {
            return __pending_count_;
        }
    
private:
    /*
    void __reap_thread(std::thread::id __tid)
        {
            __reaper_.reap_thread(this, __tid);
        }
     */
    
    void __run_closure_thread(closure_type __c)
        {
            // wait until the thread has been safely installed in the executor's
            // state before running the closure
            std::unique_lock<std::mutex> __lk(__lock_);
            if (!__destruct_)
            {
                __lk.unlock();
                
                __running_count_++;
                __pending_count_--;
                
                _run_closure(__c);
                
                __running_count_--;
                
                __lk.lock();
            }
            
            if (__running_count_ == 0)
                __cleanup_cv_.notify_all();
//            __reap_thread(std::this_thread::get_id());
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
    bool operator ()(const timed_closure& __lhs, const timed_closure& __rhs) const
        {
            return __lhs.first < __rhs.first;
        }
};

class __thread_pool_impl_stdcpp
{
	std::queue<executor::closure_type>	_queue;
	timed_closure_queue                 _timed_queue;

	std::vector<std::thread>            _threads;
	std::thread                         _timed_addition_thread;

	std::atomic_size_t                  _jobs_in_flight;

	std::mutex                          _mutex;
	std::atomic<bool>                   _exiting;
	std::condition_variable             _jobs_ready;
	std::condition_variable             _timers_updated;
	
	__thread_pool_impl_stdcpp(int num_threads);
    
	virtual
    ~__thread_pool_impl_stdcpp();

	void add(executor::closure_type closure);
    
    FORCE_INLINE
	size_t uninitiated_task_count() const
        {
            return _queue.size() + _timed_queue.size();
        }

	void add_at(std::chrono::system_clock::time_point abs_time, executor::closure_type closure);
    
    FORCE_INLINE
	void add_after(std::chrono::system_clock::duration rel_time, executor::closure_type closure)
        {
            add_at(std::chrono::system_clock::now() + rel_time, closure);
        }

private:
	void _RunWorker();
	void _RunTimer();

	friend class thread_pool;
};

#if EPUB_PLATFORM(WINRT)
class __thread_pool_impl_winrt : public std::enable_shared_from_this<__thread_pool_impl_winrt>
{
	typedef ::Windows::System::Threading::ThreadPool		thread_pool;
	typedef ::Windows::System::Threading::ThreadPoolTimer	timer;
	typedef ::Windows::Foundation::IAsyncAction				work_item;

	std::vector<work_item^>				_work_items;
	std::vector<timer^>					_timers;

	std::mutex                          _mutex;
	std::atomic<bool>                   _exiting;

	__thread_pool_impl_winrt(int num_threads);
    
	virtual
    ~__thread_pool_impl_winrt();

	void add(executor::closure_type closure);
    
    FORCE_INLINE
	size_t uninitiated_task_count() const
        {
            return _work_items.size() + _timers.size();
        }

    FORCE_INLINE
	void add_at(std::chrono::system_clock::time_point& abs_time, executor::closure_type closure)
        {
            add_after(abs_time - std::chrono::system_clock::now(), closure);
        }
    
	void add_after(std::chrono::system_clock::duration& rel_time, executor::closure_type closure);

private:
	friend class ::ePub3::thread_pool;

};
#endif

class thread_pool : public scheduled_executor
{
private:
#if EPUB_PLATFORM(WINRT)
	typedef __thread_pool_impl_winrt	impl_t;
#else
	typedef __thread_pool_impl_stdcpp	impl_t;
#endif
	impl_t								__impl_;
    
public:
    static const int Automatic          = 0;
    
public:
	thread_pool(int num_threads = Automatic)
		: __impl_(num_threads)
		{}
	virtual
    ~thread_pool()
		{}

	virtual
    void add(closure_type closure) OVERRIDE
		{
			__impl_.add(closure);
		}
	virtual
    size_t uninitiated_task_count() const OVERRIDE
		{
			return __impl_.uninitiated_task_count();
		}

	virtual
    void add_at(std::chrono::system_clock::time_point& abs_time, closure_type closure) OVERRIDE
		{
			__impl_.add_at(abs_time, closure);
		}
	virtual
    void add_after(std::chrono::system_clock::duration& rel_time, closure_type closure) OVERRIDE
		{
			__impl_.add_after(rel_time, closure);
		}
    
};

std::shared_ptr<executor> main_thread_executor();

EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__executor__) */
