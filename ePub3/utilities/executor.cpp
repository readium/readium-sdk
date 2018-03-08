//
//  executor.cpp
//  ePub3
//
//  Created by Jim Dovey on 2013-08-28.
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//  
//  Redistribution and use in source and binary forms, with or without modification, 
//  are permitted provided that the following conditions are met:
//  1. Redistributions of source code must retain the above copyright notice, this 
//  list of conditions and the following disclaimer.
//  2. Redistributions in binary form must reproduce the above copyright notice, 
//  this list of conditions and the following disclaimer in the documentation and/or 
//  other materials provided with the distribution.
//  3. Neither the name of the organization nor the names of its contributors may be 
//  used to endorse or promote products derived from this software without specific 
//  prior written permission.
//  
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
//  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
//  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
//  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
//  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
//  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
//  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
//  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE 
//  OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED 
//  OF THE POSSIBILITY OF SUCH DAMAGE.

#if FUTURE_ENABLED

#include "epub3.h"
#include "executor.h"
#include <iostream>
#include <future>

#if EPUB_PLATFORM(MAC)
#include <CoreFoundation/CoreFoundation.h>
#elif EPUB_PLATFORM(WINRT)
#include <ppltasks.h>
#endif

// because without an implementation file including a header, it doesn't get
//  indexed/coloured in Xcode (grr)
#include "optional.h"

EPUB3_BEGIN_NAMESPACE

//thread_executor::__thread_reaper thread_executor::__reaper_;

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
    
    std::unique_lock<std::mutex> lk(_lock);
    _queue.push(closure);
    
    // if there's already a closure on the underlying executor, it will consume the new closure for us.
    if (_running > 0)
        return;
    
    // unlock our mutex -- the underlying executor could be an inline_executor
    lk.unlock();
    
    _underlying_executor->add([&]{
        std::unique_lock<std::mutex> ulk(_lock);
        
        _exit_condition.wait(ulk, [this](){
            return _running == 0 || _exiting;
        });
        
        if (_exiting) {
            return;
        }
        
        while (!_queue.empty())
        {
            ++_running;
            
            closure_type closure_to_run = _queue.front();
            _queue.pop();
            
            // unlock the mutex before the closure runs
            ulk.unlock();
            
            _run_closure(closure_to_run);
            
            ulk.lock();
            
            --_running;
        }
        
        _exit_condition.notify_all();
    });
}

#if 0
#pragma mark -
#endif

__thread_pool_impl_stdcpp::__thread_pool_impl_stdcpp(int num_threads) : _queue(), _timed_queue(), _threads(), _jobs_in_flight(0), _mutex(), _exiting(false), _jobs_ready(), _timers_updated(), _timed_addition_thread()
{
    if ( num_threads < 1 )
        num_threads = std::thread::hardware_concurrency();
    if ( num_threads < 1 )
        num_threads = 1;
    
    for ( int i = 0; i < num_threads; i++ ) {
		_threads.emplace_back(&__thread_pool_impl_stdcpp::_RunWorker, this);
    }
    
    _timed_addition_thread = std::thread(&__thread_pool_impl_stdcpp::_RunTimer, this);
}
__thread_pool_impl_stdcpp::~__thread_pool_impl_stdcpp()
{
    _mutex.lock();
    _exiting = true;
    _mutex.unlock();
    
    // wake up all threads -- any that are waiting will see _exiting and exit immediately
    _jobs_ready.notify_all();
    _timers_updated.notify_all();
    
    // wait until all threads have exited
    for ( std::thread& thr : _threads ) {
        thr.join();
    }
    
    _timed_addition_thread.join();
}
void __thread_pool_impl_stdcpp::add(executor::closure_type closure)
{
    std::unique_lock<std::mutex> _(_mutex);
    
    // enqueue the job
    _queue.push(closure);
    
    // wake one available thread
    _jobs_ready.notify_one();
}
void __thread_pool_impl_stdcpp::add_at(std::chrono::system_clock::time_point abs_time, executor::closure_type closure)
{
    std::unique_lock<std::mutex> _(_mutex);
    
    // enqueue the time and the closure-- the priority_queue will sort it into place automatically
    _timed_queue.emplace(abs_time, closure);
    
    // notify the timer thread that changes have been made
    _timers_updated.notify_all();
}
void __thread_pool_impl_stdcpp::_RunWorker()
{
    do
    {
        std::unique_lock<std::mutex> lk(_mutex);
        if (!_exiting && _queue.size() == 0)
            _jobs_ready.wait(lk);
        
        if ( _exiting )
            break;
        
        if (_queue.empty())
            continue;       // is this why the copy below is crashing sometimes?
        
        // NB: mutex is locked at this point, remember
		executor::closure_type closure = _queue.front();
        _queue.pop();
        ++_jobs_in_flight;
        
        // unlock the mutex
        lk.unlock();
        
        // run the closure
        executor::_run_closure(closure);
        
//        if ( --_jobs_in_flight == 0 && bool(_drained_handler) )
//            _drained_handler();
        
    } while (1);
}
void __thread_pool_impl_stdcpp::_RunTimer()
{
    while (!_exiting)
    {
        std::unique_lock<std::mutex> lk(_mutex);
        if (_exiting)
            break;
        
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
			executor::closure_type closure = _timed_queue.top().second;
            _timed_queue.pop();
            
            // unlock the mutex before calling add(), which will want to own the lock itself
            lk.unlock();
            
            // add the closure to the execution queue
            add(closure);
        }
        
    }
}

#if EPUB_PLATFORM(WINRT)
__thread_pool_impl_winrt::__thread_pool_impl_winrt(int num_threads)
	: _work_items(),
	  _timers(),
	  _mutex(), 
	  _exiting(false)
{
}
__thread_pool_impl_winrt::~__thread_pool_impl_winrt()
{
	std::lock_guard<std::mutex> _(_mutex);
	_exiting = true;

	for (auto item : _timers)
	{
		item->Cancel();
	}
	for (auto item : _work_items)
	{
		item->Cancel();
	}
}
void __thread_pool_impl_winrt::add(executor::closure_type closure)
{
	using namespace ::Windows::System::Threading;
	std::unique_lock<std::mutex> _(_mutex);
	if (_exiting)
		return;

	// enqueue the job
	thread_pool::RunAsync(ref new WorkItemHandler([closure](work_item^ action) {
		if (action->Status == ::Windows::Foundation::AsyncStatus::Started)
			closure();
	}));
}
void __thread_pool_impl_winrt::add_after(std::chrono::system_clock::duration& rel_time, executor::closure_type closure)
{
	using namespace ::Windows::System::Threading;
	std::lock_guard<std::mutex> _(_mutex);
	if (_exiting)
		return;

	typedef std::chrono::duration<long long, std::ratio<1, 10000000>> _Ticks;
	::Windows::Foundation::TimeSpan span;
	span.Duration = std::chrono::duration_cast<_Ticks>(rel_time).count();

	// create a timer which will enqueue the job
	auto self = shared_from_this();
	_timers.push_back(timer::CreateTimer(ref new TimerElapsedHandler([self, closure](timer^ theTimer) {
		self->add(closure);
	}), span));
}
#endif

#if EPUB_PLATFORM(WINRT) || EPUB_PLATFORM(MAC)
class __main_thread_executor : public scheduled_executor
{
private:
#if EPUB_PLATFORM(WINRT)
	static ::Windows::UI::Core::CoreDispatcher^	_mainDispatcher;
	static void SetMainDispatcher(::Windows::UI::Core::CoreDispatcher^ dispatcher);
#endif

	std::atomic_int_fast32_t    _num_closures;

public:
    
    explicit FORCE_INLINE
	__main_thread_executor()
        : scheduled_executor()
        {}
    
	virtual
    ~__main_thread_executor()
        {}

	virtual
    void add(closure_type closure) OVERRIDE;
	
    virtual
    size_t uninitiated_task_count() const OVERRIDE;

	virtual
    void add_at(std::chrono::system_clock::time_point& abs_time, closure_type closure) OVERRIDE;
	
    virtual
    void add_after(std::chrono::system_clock::duration& rel_time, closure_type closure) OVERRIDE;

};

std::shared_ptr<executor>
main_thread_executor()
{
    return std::make_shared<__main_thread_executor>();
}
#endif

#if EPUB_PLATFORM(MAC)
template <typename _Rep, typename _Period>
CFTimeInterval CFTimeIntervalFromDuration(std::chrono::duration<_Rep, _Period>& __d)
{
    using namespace std::chrono;
    typedef duration<CFTimeInterval> CFSeconds;
    CFSeconds __cfs = duration_cast<CFSeconds>(__d);
    return __cfs.count();
}

template <typename _Clock, typename _Duration = typename _Clock::duration>
CFAbsoluteTime CFAbsoluteTimeFromTimePoint(std::chrono::time_point<_Clock, _Duration>& __t)
{
    using namespace std::chrono;
    typedef duration<CFAbsoluteTime> CFSeconds;
    CFSeconds __s1970 = duration_cast<CFSeconds>(__t.time_since_epoch());
    return __s1970.count() + kCFAbsoluteTimeIntervalSince1970;
}

void __main_thread_executor::add(closure_type closure)
{
	_num_closures++;
    std::weak_ptr<__main_thread_executor> weakThis(std::dynamic_pointer_cast<__main_thread_executor>(shared_from_this()));
    CFRunLoopPerformBlock(CFRunLoopGetMain(), kCFRunLoopCommonModes, ^{
        auto self = weakThis.lock();
        if (bool(self))
            self->_num_closures--;
        closure();
    });
    CFRunLoopWakeUp(CFRunLoopGetMain());
}
size_t __main_thread_executor::uninitiated_task_count() const
{
	return _num_closures;
}
void __main_thread_executor::add_at(std::chrono::system_clock::time_point& abs_time, closure_type closure)
{
	using namespace std::chrono;
    CFAbsoluteTime cfTime = CFAbsoluteTimeFromTimePoint(abs_time);

	auto self = std::dynamic_pointer_cast<__main_thread_executor>(shared_from_this());
	CFRunLoopTimerRef timer = CFRunLoopTimerCreateWithHandler(kCFAllocatorDefault, cfTime, 0.0, 0, 0, ^(CFRunLoopTimerRef theTimer) {
		closure();
		self->_num_closures--;
	});

	_num_closures++;
	CFRunLoopAddTimer(CFRunLoopGetMain(), timer, kCFRunLoopCommonModes);
}
void __main_thread_executor::add_after(std::chrono::system_clock::duration& rel_time, closure_type closure)
{
    auto abs_time = std::chrono::system_clock::now() + rel_time;
	add_at(abs_time, closure);
}
#elif EPUB_PLATFORM(WINRT)
::Windows::UI::Core::CoreDispatcher^ __main_thread_executor::_mainDispatcher = nullptr;
void __main_thread_executor::SetMainDispatcher(::Windows::UI::Core::CoreDispatcher^ dispatcher)
{
	_mainDispatcher = dispatcher;
}
void __main_thread_executor::add(closure_type closure)
{
	using namespace ::Windows::UI::Core;
	_num_closures++;
	CoreDispatcher^ dispatcher = _mainDispatcher;
	if (dispatcher == nullptr)
		throw std::system_error(std::error_code(E_HANDLE, std::system_category()), "You must call main_thread_executor::SetMainDispatcher() from the main UI thread before you can use this method");

	if (dispatcher->HasThreadAccess)
	{
		closure();
	}
	else
	{
		auto self = std::dynamic_pointer_cast<__main_thread_executor>(shared_from_this());
		dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([self, closure]() {
			closure();
			self->_num_closures--;
		}));
	}
}
size_t __main_thread_executor::uninitiated_task_count() const
{
	return _num_closures;
}
void __main_thread_executor::add_at(std::chrono::system_clock::time_point& abs_time, closure_type closure)
{
	using namespace std::chrono;
    auto rel_time = abs_time - system_clock::now();
	add_after(rel_time, closure);
}
void __main_thread_executor::add_after(std::chrono::system_clock::duration& rel_time, closure_type closure)
{
	using namespace ::Windows::System::Threading;
	using namespace std::chrono;

	typedef duration<long long, std::ratio<1, 10000000>> _Ticks;
	::Windows::Foundation::TimeSpan span;
	span.Duration = duration_cast<_Ticks>(rel_time).count();

	// create a timer which will enqueue the job
	auto self = shared_from_this();
	ThreadPoolTimer::CreateTimer(ref new TimerElapsedHandler([self, closure](ThreadPoolTimer^ theTimer) {
		self->add(closure);
	}), span);
}
#endif

EPUB3_END_NAMESPACE

#endif //FUTURE_ENABLED

