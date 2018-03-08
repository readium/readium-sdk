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
#elif EPUB_OS(ANDROID)
# error Please use run_loop_android.cpp for this platform
#elif EPUB_OS(WINDOWS)
# error Please use run_loop_windows.cpp for this platform
#endif

EPUB3_BEGIN_NAMESPACE

using StackLock = std::lock_guard<std::recursive_mutex>;

RunLoop::RunLoop() : _timers(), _observers(), _sources(), _listLock(), _conditionLock(), _wakeUp(), _waiting(false), _stop(false), _observerMask(0), _waitingUntilTimer(nullptr)
{
}
RunLoop::~RunLoop()
{
    if ( _waiting )
        Stop();
}
void RunLoop::PerformFunction(std::function<void ()> fn)
{
    EventSourcePtr ev = EventSource::New([fn](EventSource& __e) {
        fn();
    });
    AddEventSource(ev);
    ev->Signal();
}
void RunLoop::AddTimer(TimerPtr timer)
{
    StackLock lock(_listLock);
    if ( ContainsTimer(timer) )
        return;
    
    _timers.push_back(timer);
    _timers.sort();
    
    if ( _waiting && _waitingUntilTimer != nullptr && _waitingUntilTimer->GetNextFireDate() < timer->GetNextFireDate() )
    {
        // signal a Run() invocation that it needs to adjust its timeout to the fire
        // date of this new timer
        WakeUp();
    }
}
bool RunLoop::ContainsTimer(TimerPtr timer) const
{
    StackLock lock(const_cast<RunLoop*>(this)->_listLock);
    for ( const TimerPtr t : _timers )
    {
        if ( timer == t )
            return true;
    }
    return false;
}
void RunLoop::RemoveTimer(TimerPtr timer)
{
    StackLock lock(_listLock);
    for ( auto iter = _timers.begin(), end = _timers.end(); iter != end; ++iter )
    {
        if ( *iter == timer )
        {
            _timers.erase(iter);
            break;
        }
    }
    
    if ( _waiting )
    {
        if ( _waitingUntilTimer == timer )
        {
            // a Run() invocation is waiting until the removed timer's fire date
            // wake up the runloop so it can adjust its timeout accordingly
            _waitingUntilTimer = nullptr;
            WakeUp();
        }
        else if ( _timers.empty() && _sources.empty() )
        {
            // run out of useful things to wait upon
            WakeUp();
        }
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
    
    _sources.push_back(ev);
}
bool RunLoop::ContainsEventSource(EventSourcePtr ev) const
{
    StackLock lock(const_cast<RunLoop*>(this)->_listLock);
    for ( const EventSourcePtr e : _sources )
    {
        if ( ev == e )
            return true;
    }
    return false;
}
void RunLoop::RemoveEventSource(EventSourcePtr ev)
{
    StackLock lock(_listLock);
    for ( auto iter = _sources.begin(), end = _sources.end(); iter != end; ++iter )
    {
        if ( *iter == ev )
        {
            _sources.erase(iter);
            break;
        }
    }
    
    if ( _waiting && _timers.empty() && _sources.empty() )
    {
        // run out of useful things to wait upon
        WakeUp();
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
    _stop = true;
    if ( _waiting )
        WakeUp();
}
bool RunLoop::IsWaiting() const
{
    return _waiting;
}
void RunLoop::WakeUp()
{
    if ( _conditionLock.try_lock() )
    {
        _wakeUp.notify_all();
        _conditionLock.unlock();
    }
}
RunLoop::ExitReason RunLoop::RunInternal(bool returnAfterSourceHandled, std::chrono::nanoseconds &timeout)
{
    using namespace std::chrono;
    system_clock::time_point timeoutTime = system_clock::now() + duration_cast<system_clock::duration>(timeout);
    ExitReason reason(ExitReason::RunTimedOut);
    
    // catch a pending stop
    if ( _stop.exchange(false) )
        return ExitReason::RunStopped;
    
    _listLock.lock();

    RunObservers(Observer::ActivityFlags::RunLoopEntry);

    do
    {
        if ( _timers.empty() && _sources.empty() )
        {
            reason = ExitReason::RunFinished;
            break;
        }
        
        std::vector<Timer*> timersToFire = CollectFiringTimers();
        if ( !timersToFire.empty() )
        {
            RunObservers(Observer::ActivityFlags::RunLoopBeforeTimers);
            for ( auto timer : timersToFire )
            {
                // we'll reset repeating timers after the callback returns, so it doesn't
                // arm again while we're 
                Timer::Clock::time_point date = timer->GetNextFireDate();
                
                // fire the callback now
                timer->_fn(*timer);
                
                // only reset a repeating timer if the fire date hasn't been changed
                //  by the callback
                if ( timer->Repeats() && timer->GetNextFireDate() == date )
                {
                    timer->SetNextFireDate(timer->_interval);
                }
            }
        }
        
        std::vector<EventSource*> sourcesToFire = CollectFiringSources(returnAfterSourceHandled);
        if ( !sourcesToFire.empty() )
        {
            RunObservers(Observer::ActivityFlags::RunLoopBeforeSources);
            for ( auto source : sourcesToFire )
            {
                source->_fn(*source);
            }
            
            if ( returnAfterSourceHandled )
            {
                reason = ExitReason::RunHandledSource;
                break;
            }
        }
        
        if ( timeout <= nanoseconds(0) )
        {
            reason = ExitReason::RunTimedOut;
            break;
        }
        
        RunObservers(Observer::ActivityFlags::RunLoopBeforeWaiting);
        _listLock.unlock();
        _waiting = true;
        
        system_clock::time_point waitUntil = TimeoutOrTimer(timeoutTime);
        
        std::unique_lock<std::mutex> _condLock(_conditionLock);
        std::cv_status waitStatus = _wakeUp.wait_until(_condLock, waitUntil);
        _condLock.unlock();
        
        _waiting = false;
        _listLock.lock();
        
        RunObservers(Observer::ActivityFlags::RunLoopAfterWaiting);
        
        // why did we wake up?
        if ( waitStatus == std::cv_status::timeout )
        {
            reason = ExitReason::RunTimedOut;
            break;
        }
        
        if ( _stop )
        {
            reason = ExitReason::RunStopped;
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
    
    std::vector<Observer*> observersToRemove;
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
std::vector<RunLoop::Timer*> RunLoop::CollectFiringTimers()
{
    // _listLock MUST ALREADY BE HELD
    auto currentTime = std::chrono::system_clock::now();
    std::vector<Timer*> result;
    
    std::vector<Timer*> timersToRemove;
    for ( TimerPtr timer : _timers )
    {
        if ( timer->IsCancelled() )
        {
            timersToRemove.push_back(timer);
            continue;
        }
        
        // Timers are sorted in ascending order, so as soon as we've encountered one
        // which hasn't yet fired, we can stop searching.
        if ( timer->GetNextFireDate() > currentTime )
            break;
        
        result.push_back(timer);
    }
    
    for ( auto timer : timersToRemove )
    {
        RemoveTimer(timer);
    }
    
    return result;
}
shared_vector<RunLoop::EventSource> RunLoop::CollectFiringSources(bool onlyOne)
{
    // _listLock MUST ALREADY BE HELD
    shared_vector<EventSourcePtr> result;
    
    shared_vector<EventSource> cancelledSources;
    for ( EventSourcePtr source : _sources )
    {
        if ( source->IsCancelled() )
        {
            cancelledSources.push_back(source);
            continue;
        }
        
        // we atomically set it to false while reading to ensure only one RunLoop
        // picks up the source
        if ( source->_signalled.exchange(false) )
            result.push_back(source);
        
        if ( onlyOne )
            break;      // don't unset the signal on any other sources
    }
    
    for ( auto source : cancelledSources )
    {
        RemoveEventSource(source);
    }
    
    return result;
}
std::chrono::system_clock::time_point RunLoop::TimeoutOrTimer(std::chrono::system_clock::time_point& timeout)
{
    // _listLock MUST ALREADY BE HELD
    if ( _timers.empty() )
        return timeout;
    
    std::chrono::system_clock::time_point fireDate = _timers.front()->GetNextFireDate();
    if ( fireDate < timeout )
    {
        _waitingUntilTimer = _timers.front();
        return fireDate;
    }
    
    return timeout;
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
    // cast as voidPtr to compare function addresses
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

RunLoop::EventSource::EventSource(EventHandlerFn fn) : _signalled(false), _cancelled(false), _fn(fn)
{
}
RunLoop::EventSource::EventSource(const EventSource& o) : _signalled((bool)o._signalled), _cancelled(o._cancelled), _fn(o._fn)
{
}
RunLoop::EventSource::EventSource(EventSource&& o) : _signalled(o._signalled.exchange(false)), _cancelled(o._cancelled), _fn(std::move(o._fn))
{
    o._cancelled = false;
}
RunLoop::EventSource::~EventSource()
{
}
RunLoop::EventSource& RunLoop::EventSource::operator=(const EventSource& o)
{
    _signalled = (bool)o._signalled;
    _fn = o._fn;
    _cancelled = o._cancelled;
    return *this;
}
RunLoop::EventSource& RunLoop::EventSource::operator=(EventSource&& o)
{
    _signalled = o._signalled.exchange(false);
    _fn = std::move(o._fn);
    _cancelled = o._cancelled; o._cancelled = false;
    return *this;
}
bool RunLoop::EventSource::operator==(const EventSource& o) const
{
    return _fn.target<void>() == o._fn.target<void>();
}
bool RunLoop::EventSource::IsCancelled() const
{
    return _cancelled;
}
void RunLoop::EventSource::Cancel()
{
    _cancelled = true;
}
void RunLoop::EventSource::Signal()
{
    _signalled = true;
}

RunLoop::Timer::Timer(Clock::time_point& fireDate, Clock::duration& interval, TimerFn fn) : _fireDate(fireDate), _interval(interval), _fn(fn)
{
}
RunLoop::Timer::Timer(Clock::duration& interval, bool repeat, TimerFn fn) : _fireDate(Clock::now()+interval), _interval(interval), _fn(fn)
{
}
RunLoop::Timer::Timer(const Timer& o) : _fireDate(o._fireDate), _interval(o._interval), _fn(o._fn)
{
}
RunLoop::Timer::Timer(Timer&& o) : _fireDate(std::move(o._fireDate)), _interval(std::move(o._interval)), _fn(std::move(o._fn))
{
}
RunLoop::Timer::~Timer()
{
}
RunLoop::Timer& RunLoop::Timer::operator=(const Timer& o)
{
    _fireDate = o._fireDate;
    _interval = o._interval;
    _fn = o._fn;
    return *this;
}
RunLoop::Timer& RunLoop::Timer::operator=(Timer&& o)
{
    _fireDate = std::move(o._fireDate);
    _interval = std::move(o._interval);
    _fn = std::move(o._fn);
    return *this;
}
bool RunLoop::Timer::operator==(const Timer& o) const
{
    return (_fireDate == o._fireDate) && (_interval == o._interval) && (_fn.target<void>() == o._fn.target<void>());
}
void RunLoop::Timer::Cancel()
{
    _cancelled = true;
}
bool RunLoop::Timer::IsCancelled() const
{
    return _cancelled;
}
bool RunLoop::Timer::Repeats() const
{
    return _interval > Clock::duration(0);
}
RunLoop::Timer::Clock::duration RunLoop::Timer::RepeatIntervalInternal() const
{
    return _interval;
}
RunLoop::Timer::Clock::time_point RunLoop::Timer::GetNextFireDateTime() const
{
    return _fireDate;
}
void RunLoop::Timer::SetNextFireDateTime(Clock::time_point& when)
{
    _fireDate = when;
}
RunLoop::Timer::Clock::duration RunLoop::Timer::GetNextFireDateDuration() const
{
    return _fireDate - Clock::now();
}
void RunLoop::Timer::SetNextFireDateDuration(Clock::duration& when)
{
    _fireDate = Clock::now() + when;
}

EPUB3_END_NAMESPACE

#endif //FUTURE_ENABLED

