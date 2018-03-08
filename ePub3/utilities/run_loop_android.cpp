//
//  run_loop.cpp
//  ePub3
//
//  Created by Jim Dovey on 2013-04-08.
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

// Common pieces used by all platforms
#include "run_loop_common.ipp"

#if EPUB_USE(CF)
# error Please use run_loop_cf.cpp for this platform
#elif EPUB_OS(WINDOWS)
# error Please use run_loop_windows.cpp for this platform
#elif !EPUB_OS(ANDROID)
# error Please use run_loop_generic.cpp for this platform
#endif

#include <android/looper.h>
#include <unistd.h>
#include <stdio.h>

enum
{
    RunLoopStopEvent
};

static bool _CheckRunLoopStopFD(int fd)
{
    struct timeval ts = {0, 0};
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(fd, &fds);
    ::select(fd+1, &fds, NULL, &fds, &ts);
    return FD_ISSET(fd, &fds);
}

EPUB3_BEGIN_NAMESPACE

using StackLock = std::lock_guard<std::recursive_mutex>;

RunLoop::RunLoop() : _looper(ALooper_prepare(ALOOPER_PREPARE_ALLOW_NON_CALLBACKS)), _handlers()
{
    if ( ::pipe(_wakeFDs) != 0 )
        throw std::system_error(errno, std::system_category(), "pipe() failed for RunLoop");
    
    ALooper_addFd(_looper, _wakeFDs[0], RunLoopStopEvent, ALOOPER_EVENT_INPUT, NULL, NULL);
}
RunLoop::~RunLoop()
{
    // this will signal the owning thread, if necessary
    close(_wakeFDs[0]);
    close(_wakeFDs[1]);
    
    // unregister all these fellas and delete them
    for ( auto& pair : _handlers )
    {
        ALooper_removeFd(_looper, pair.first);
    }
}
void RunLoop::PerformFunction(std::function<void ()> fn)
{
    shared_ptr<EventSource> ev(new EventSource([fn](EventSource& __e){fn();}));
    AddEventSource(ev);
    ev->Signal();
}
void RunLoop::AddTimer(TimerPtr timer)
{
    StackLock lock(_listLock);
    if ( ContainsTimer(timer) )
        return;
    
    _handlers[timer->_pipeFDs[0]] = timer;
    ALooper_addFd(_looper, timer->_pipeFDs[0], ALOOPER_POLL_CALLBACK, ALOOPER_EVENT_INPUT, &RunLoop::_ReceiveLoopEvent, reinterpret_cast<void*>(this));
}
bool RunLoop::ContainsTimer(TimerPtr timer) const
{
    StackLock lock(const_cast<RunLoop*>(this)->_listLock);
    return (_handlers.find(timer->_pipeFDs[0]) != _handlers.end());
}
void RunLoop::RemoveTimer(TimerPtr timer)
{
    StackLock lock(_listLock);
    auto found = _handlers.find(timer->_pipeFDs[0]);
    if ( found != _handlers.end() )
    {
        ALooper_removeFd(_looper, timer->_pipeFDs[0]);
        _handlers.erase(found);
    }
}
void RunLoop::AddObserver(ObserverPtr observer)
{
    StackLock lock(_listLock);
    if ( ContainsObserver(observer) )
        return;
    
    _observers.push_back(observer);
    _observerMask |= observer->_acts;
}
bool RunLoop::ContainsObserver(ObserverPtr obs) const
{
    StackLock lock(const_cast<RunLoop*>(this)->_listLock);
    for ( const ObserverPtr o : _observers )
    {
        if ( obs == o )
            return true;
    }
    return false;
}
void RunLoop::RemoveObserver(ObserverPtr obs)
{
    StackLock lock(_listLock);
    for ( auto iter = _observers.begin(), end = _observers.end(); iter != end; ++iter )
    {
        if ( *iter == obs )
        {
            _observers.erase(iter);
            break;
        }
    }
}
void RunLoop::AddEventSource(EventSourcePtr ev)
{
    StackLock lock(_listLock);
    if ( ContainsEventSource(ev) )
        return;
    
    _handlers[ev->_evt[0]] = ev;
    ALooper_addFd(_looper, ev->_evt[0], ALOOPER_POLL_CALLBACK, ALOOPER_EVENT_INPUT, &_ReceiveLoopEvent, reinterpret_cast<void*>(this));
}
bool RunLoop::ContainsEventSource(EventSourcePtr ev) const
{
    StackLock lock(const_cast<RunLoop*>(this)->_listLock);
    return (_handlers.find(ev->_evt[0]) != _handlers.end());
}
void RunLoop::RemoveEventSource(EventSourcePtr ev)
{
    StackLock lock(_listLock);
    auto found = _handlers.find(ev->_evt[0]);
    if ( found != _handlers.end() )
    {
        ALooper_removeFd(_looper, ev->_evt[0]);
        _handlers.erase(found);
    }
}
void RunLoop::Run()
{
    ExitReason reason;
    do
    {
        std::chrono::nanoseconds timeout(std::chrono::duration_values<std::chrono::nanoseconds::rep>::max());
        reason = RunInternal(false, timeout);
        
    } while (reason != ExitReason::RunStopped && reason != ExitReason::RunFinished);
}
void RunLoop::Stop()
{
    write(_wakeFDs[1], "stop", 4);
}
bool RunLoop::IsWaiting() const
{
    return _waiting;
}
void RunLoop::WakeUp()
{
    ALooper_wake(_looper);
}
RunLoop::ExitReason RunLoop::RunInternal(bool returnAfterSourceHandled, std::chrono::nanoseconds &timeout)
{
    using namespace std::chrono;
    system_clock::time_point timeoutTime = system_clock::now() + duration_cast<system_clock::duration>(timeout);
    ExitReason reason(ExitReason::RunTimedOut);
    
    if (_CheckRunLoopStopFD(_wakeFDs[0]) )
        return ExitReason::RunStopped;
    
    _listLock.lock();

    RunObservers(Observer::ActivityFlags::RunLoopEntry);

    do
    {
        if ( _handlers.empty() )
        {
            reason = ExitReason::RunFinished;
            break;
        }
        
        RunObservers(Observer::ActivityFlags::RunLoopBeforeWaiting);
        _listLock.unlock();
        _waiting = true;
        milliseconds millis = duration_cast<milliseconds>(timeoutTime - system_clock::now());
        
        int outFd, outEvents;
        void* outData;
        int waitStatus = ALooper_pollOnce(static_cast<int>(millis.count()), &outFd, &outEvents, &outData);
        
        _waiting = false;
        _listLock.lock();
        
        RunObservers(Observer::ActivityFlags::RunLoopAfterWaiting);
        
        // why did we wake up?
        if ( waitStatus == ALOOPER_POLL_TIMEOUT )
        {
            reason = ExitReason::RunTimedOut;
            break;
        }
        
        if ( waitStatus == RunLoopStopEvent )
        {
            char buf[12];
            ::read(outFd, buf, 12);
            reason = ExitReason::RunStopped;
            break;
        }
        else if ( waitStatus == ALOOPER_POLL_CALLBACK && returnAfterSourceHandled )
        {
            reason = ExitReason::RunHandledSource;
            break;
        }
    
    } while (timeoutTime > system_clock::now());

    RunObservers(Observer::ActivityFlags::RunLoopExit);
    
    _listLock.unlock();
    return reason;
}
void RunLoop::RunObservers(Observer::Activity activity)
{
    // _listLock MUST ALREADY BE HELD
    if ( (_observerMask & activity) == 0 )
        return;
    
    shared_vector<Observer> observersToRemove;
    for ( auto observer : _observers )
    {
        if ( observer->IsCancelled() )
        {
            observersToRemove.push_back(observer);
            continue;
        }
        
        if ( (observer->_acts & activity) == 0 )
            continue;
        observer->_fn(*observer, activity);
        if ( !observer->Repeats() )
            observersToRemove.push_back(observer);
    }
    
    for ( auto observer : observersToRemove )
    {
        RemoveObserver(observer);
    }
}
int RunLoop::_ReceiveLoopEvent(int fd, int events, void* data)
{
    RunLoop* p = reinterpret_cast<RunLoop*>(data);
    auto iter = p->_handlers.find(fd);
    if ( iter == p->_handlers.end() )
        return 0;   // unregister fd from the looper
    
    if ( !iter->second )
        return 0;
    
    // consume something
    char buf[12];
    ::read(fd, buf, 12);
    
    // what type is it?
    TimerPtr pTimer = std::dynamic_pointer_cast<Timer>(iter->second);
    if (pTimer != nullptr)
    {
        // it's a Timer!
        if ( (events & ALOOPER_EVENT_HANGUP) == ALOOPER_EVENT_HANGUP )
        {
            // remove it from the runloop
            p->RemoveTimer(pTimer);
            return 0;
        }
        
        pTimer->_fn(*pTimer);                 ///////// DO CALLOUT
        
        if ( !pTimer->Repeats() || pTimer->IsCancelled() )
        {
            // the underlying Linux timer_t is already disarmed
            p->RemoveTimer(pTimer);
            return 0;
        }
        
        // it'll repeat, so keep processing please
        return 1;
    }
    
    // not a Timer? Must be an EventSource then
    EventSourcePtr pSource = std::dynamic_pointer_cast<EventSource>(iter->second);
    if ( pSource != nullptr )
    {
        // it *is* an EventSource!
        if ( (events & ALOOPER_EVENT_HANGUP) == ALOOPER_EVENT_HANGUP )
        {
            // calcelled, so remove it from the runloop
            p->RemoveEventSource(pSource);
            return 0;
        }
        
        pSource->_fn(*pSource);               /////////// DO CALLOUT
        
        if ( pSource->IsCancelled() )
        {
            // now we want to remove it
            p->RemoveEventSource(pSource);
            return 0;
        }
        
        return 1;
    }
    
    // errrrr... who to the what, now?
    return 0;
}

RunLoop::Observer::Observer(Activity activities, bool repeats, ObserverFn fn) : _fn(fn), _acts(activities), _repeats(repeats), _cancelled(false)
{
}
RunLoop::Observer::Observer(const Observer& o) : _fn(o._fn), _acts(o._acts), _repeats(o._repeats)
{
}
RunLoop::Observer::Observer(Observer&& o) : _fn(std::move(o._fn)), _acts(o._acts), _repeats(o._repeats)
{
    o._acts = 0;
    o._repeats = false;
}
RunLoop::Observer::~Observer()
{
}
RunLoop::Observer& RunLoop::Observer::operator=(const Observer& o)
{
    _fn = o._fn;
    _acts = o._acts;
    _repeats = o._repeats;
    return *this;
}
RunLoop::Observer& RunLoop::Observer::operator=(Observer&& o)
{
    _fn = std::move(o._fn);
    _acts = o._acts; o._acts = 0;
    _repeats = o._repeats; o._repeats = false;
    return *this;
}
bool RunLoop::Observer::operator==(const Observer& o) const
{
    // cast as void* to compare function addresses
    return _fn.target<void>() == o._fn.target<void>();
}
RunLoop::Observer::Activity RunLoop::Observer::GetActivities() const
{
    return _acts;
}
bool RunLoop::Observer::Repeats() const
{
    return _repeats;
}
bool RunLoop::Observer::IsCancelled() const
{
    return _cancelled;
}
void RunLoop::Observer::Cancel()
{
    _cancelled = true;
}

RunLoop::EventSource::EventSource(EventHandlerFn fn) : _fn(fn)
{
    if ( ::pipe(_evt) != 0 )
        throw std::system_error(errno, std::system_category(), "pipe() failed for EventSource");
}
RunLoop::EventSource::EventSource(const EventSource& o) : _fn(o._fn)
{
    if ( ::pipe(_evt) != 0 )
        throw std::system_error(errno, std::system_category(), "pipe() failed for EventSource");
}
RunLoop::EventSource::EventSource(EventSource&& o) : _fn(std::move(o._fn))
{
    _evt[0] = o._evt[0];
    _evt[1] = o._evt[1];
    o._evt[0] = o._evt[1] = 0;
}
RunLoop::EventSource::~EventSource()
{
    ::close(_evt[0]);
    ::close(_evt[1]);
}
RunLoop::EventSource& RunLoop::EventSource::operator=(const EventSource& o)
{
    _fn = o._fn;
    _evt[0] = ::dup(o._evt[0]);
    _evt[1] = ::dup(o._evt[1]);
    return *this;
}
RunLoop::EventSource& RunLoop::EventSource::operator=(EventSource&& o)
{
    _evt[0] = o._evt[0];
    _evt[1] = o._evt[1];
    o._evt[0] = o._evt[1] = 0;
    _fn = std::move(o._fn);
    return *this;
}
bool RunLoop::EventSource::operator==(const EventSource& o) const
{
    return _evt[0] == o._evt[0];
}
bool RunLoop::EventSource::IsCancelled() const
{
    return _evt[1] == 0;
}
void RunLoop::EventSource::Cancel()
{
    ::close(_evt[1]);
    _evt[1] = 0;
}
void RunLoop::EventSource::Signal()
{
    if ( _evt[1] == 0 )
        return;
    
    ::write(_evt[1], "fire", 4);
}

static void _timer_handler(union sigval sv)
{
    // ping the file descriptor
    ::write(sv.sival_int, "fire", 4);
}
RunLoop::Timer::Timer(Clock::time_point& fireDate, Clock::duration& interval, TimerFn fn) : _fn(fn)
{
    using namespace std::chrono;
    
    if ( ::pipe(_pipeFDs) != 0 )
        throw std::system_error(errno, std::system_category(), "pipe() failed for Timer");
    
    struct sigevent ev;
    ev.sigev_signo = SIGALRM;
    ev.sigev_notify = SIGEV_THREAD;
    ev.sigev_value.sival_int = _pipeFDs[1];
    ev.sigev_notify_function = &_timer_handler;
    ev.sigev_notify_attributes = nullptr;
    ev.sigev_notify_thread_id = 0;
    
    if ( ::timer_create(CLOCK_REALTIME, &ev, &_timer) != 0 )
    {
        ::close(_pipeFDs[0]);
        ::close(_pipeFDs[1]);
        throw std::system_error(errno, std::system_category(), "timer_create() failed");
    }
    
    struct itimerspec its;
    its.it_value.tv_sec = duration_cast<seconds>(fireDate.time_since_epoch()).count();
    its.it_value.tv_nsec = duration_cast<nanoseconds>(fireDate.time_since_epoch()).count() % std::nano().den;
    its.it_interval.tv_sec = duration_cast<seconds>(interval).count();
    its.it_interval.tv_nsec = duration_cast<nanoseconds>(interval).count() % std::nano().den;
    
    // fireDate is an absolute time, not an interval relative to now()
    if ( ::timer_settime(_timer, TIMER_ABSTIME, &its, NULL) == -1 )
    {
        ::close(_pipeFDs[0]);
        ::close(_pipeFDs[1]);
        ::timer_delete(_timer);
        throw std::system_error(errno, std::system_category(), "timer_settime() failed");
    }
}
RunLoop::Timer::Timer(Clock::duration& interval, bool repeat, TimerFn fn) : _fn(fn)
{
    using namespace std::chrono;
    
    if ( ::pipe(_pipeFDs) != 0 )
        throw std::system_error(errno, std::system_category(), "pipe() failed for Timer");
    
    struct sigevent ev;
    ev.sigev_signo = SIGALRM;
    ev.sigev_notify = SIGEV_THREAD;
    ev.sigev_value.sival_int = _pipeFDs[1];
    ev.sigev_notify_function = &_timer_handler;
    ev.sigev_notify_attributes = nullptr;
    ev.sigev_notify_thread_id = 0;
    
    if ( ::timer_create(CLOCK_REALTIME, &ev, &_timer) != 0 )
    {
        ::close(_pipeFDs[0]);
        ::close(_pipeFDs[1]);
        throw std::system_error(errno, std::system_category(), "timer_create() failed");
    }
    
    nanoseconds inter = duration_cast<nanoseconds>(interval);
    
    struct itimerspec its;
    its.it_value.tv_sec = duration_cast<seconds>(inter).count();
    its.it_value.tv_nsec = inter.count() % std::nano().den;
    
    if ( repeat )
    {
        its.it_interval.tv_sec = its.it_value.tv_sec;
        its.it_interval.tv_nsec = its.it_value.tv_nsec;
    }
    else
    {
        its.it_interval.tv_sec = its.it_interval.tv_nsec = 0;
    }
    
    // its.it_value is an interval relative to now()
    if ( ::timer_settime(_timer, 0, &its, NULL) == -1 )
    {
        ::close(_pipeFDs[0]);
        ::close(_pipeFDs[1]);
        ::timer_delete(_timer);
        throw std::system_error(errno, std::system_category(), "timer_settime() failed");
    }
}
RunLoop::Timer::Timer(const Timer& o) : _fn(o._fn)
{
    struct itimerspec its;
    if ( ::timer_gettime(o._timer, &its) == -1 )
        throw std::system_error(errno, std::system_category(), "timer_gettime() failed");
    
    if ( ::pipe(_pipeFDs) != 0 )
        throw std::system_error(errno, std::system_category(), "pipe() failed for Timer");
    
    struct sigevent ev;
    ev.sigev_signo = SIGALRM;
    ev.sigev_notify = SIGEV_THREAD;
    ev.sigev_value.sival_int = _pipeFDs[1];
    ev.sigev_notify_function = &_timer_handler;
    ev.sigev_notify_attributes = nullptr;
    ev.sigev_notify_thread_id = 0;
    
    if ( ::timer_create(CLOCK_REALTIME, &ev, &_timer) != 0 )
    {
        ::close(_pipeFDs[0]);
        ::close(_pipeFDs[1]);
        throw std::system_error(errno, std::system_category(), "timer_create() failed");
    }
    
    // timer_gettime() always returns a fire date relative to now()
    if ( ::timer_settime(_timer, 0, &its, NULL) == -1 )
    {
        ::close(_pipeFDs[0]);
        ::close(_pipeFDs[1]);
        ::timer_delete(_timer);
        throw std::system_error(errno, std::system_category(), "timer_settime() failed");
    }
}
RunLoop::Timer::Timer(Timer&& o) : _timer(o._timer), _fn(std::move(o._fn))
{
    _pipeFDs[0] = o._pipeFDs[0];
    _pipeFDs[1] = o._pipeFDs[1];
    
    o._timer = 0;
    o._pipeFDs[0] = o._pipeFDs[1] = 0;
}
RunLoop::Timer::~Timer()
{
    timer_delete(_timer);
}
RunLoop::Timer& RunLoop::Timer::operator=(const Timer& o)
{
    struct itimerspec its;
    if ( ::timer_gettime(o._timer, &its) == -1 )
        throw std::system_error(errno, std::system_category(), "timer_gettime() failed");
    
    if ( ::timer_settime(_timer, 0, &its, NULL) == -1 )
        throw std::system_error(errno, std::system_category(), "timer_settime() failed");
    
    _fn = o._fn;
    return *this;
}
RunLoop::Timer& RunLoop::Timer::operator=(Timer&& o)
{
    _timer = o._timer;
    _pipeFDs[0] = o._pipeFDs[0];
    _pipeFDs[1] = o._pipeFDs[1];
    _fn = std::move(o._fn);
    
    o._timer = 0;
    o._pipeFDs[0] = o._pipeFDs[1] = 0;
    return *this;
}
bool RunLoop::Timer::operator==(const Timer& o) const
{
    return (GetNextFireDateTime() == o.GetNextFireDateTime()) && (RepeatIntervalInternal() == o.RepeatIntervalInternal()) && (_fn.target<void>() == o._fn.target<void>());
}
void RunLoop::Timer::Cancel()
{
    struct itimerspec its = {0};
    ::timer_settime(_timer, 0, &its, NULL);
}
bool RunLoop::Timer::IsCancelled() const
{
    struct itimerspec its = {0};
    ::timer_gettime(_timer, &its);
    return (its.it_value.tv_sec == 0 && its.it_value.tv_nsec == 0);
}
bool RunLoop::Timer::Repeats() const
{
    struct itimerspec its = {0};
    ::timer_gettime(_timer, &its);
    return (its.it_interval.tv_sec > 0 || its.it_interval.tv_nsec > 0);
}
RunLoop::Timer::Clock::duration RunLoop::Timer::RepeatIntervalInternal() const
{
    using namespace std::chrono;
    struct itimerspec its = {0};
    ::timer_gettime(_timer, &its);
    nanoseconds nsec = nanoseconds(its.it_interval.tv_sec * std::nano().den);
    nsec += nanoseconds(its.it_interval.tv_nsec);
    
    return duration_cast<Clock::duration>(nsec);
}
RunLoop::Timer::Clock::time_point RunLoop::Timer::GetNextFireDateTime() const
{
    return Clock::now() + GetNextFireDateDuration();
}
void RunLoop::Timer::SetNextFireDateTime(Clock::time_point& when)
{
    using namespace std::chrono;
    struct itimerspec its;
    ::timer_gettime(_timer, &its);
    
    its.it_value.tv_sec = duration_cast<seconds>(when.time_since_epoch()).count();
    its.it_value.tv_nsec = duration_cast<nanoseconds>(when.time_since_epoch()).count() % std::nano().den;
    
    ::timer_settime(_timer, TIMER_ABSTIME, &its, NULL);
}
RunLoop::Timer::Clock::duration RunLoop::Timer::GetNextFireDateDuration() const
{
    using namespace std::chrono;
    struct itimerspec its = {0};
    ::timer_gettime(_timer, &its);
    nanoseconds nsec = nanoseconds(its.it_value.tv_sec * std::nano().den);
    nsec += nanoseconds(its.it_value.tv_nsec);
    return duration_cast<Clock::duration>(nsec);
}
void RunLoop::Timer::SetNextFireDateDuration(Clock::duration& when)
{
    using namespace std::chrono;
    Clock::time_point date(Clock::now() + when);
    SetNextFireDateTime(date);
}

EPUB3_END_NAMESPACE

#endif //FUTURE_ENABLED
