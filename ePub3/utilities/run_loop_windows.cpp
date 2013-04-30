//
//  run_loop_windows.cpp
//  ePub3
//
//  Created by Jim Dovey on 2013-04-22.
//  Copyright (c) 2012-2013 The Readium Foundation and contributors.
//  
//  The Readium SDK is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//  
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

// Common pieces used by all platforms
#include "run_loop_common.ipp"

#if EPUB_USE(CF)
# error Please use run_loop_cf.cpp for this platform
#elif EPUB_OS(ANDROID)
# error Please use run_loop_android.cpp for this platform
#elif !EPUB_OS(WINDOWS)
# error Please use run_loop_generic.cpp for this platform
#endif

EPUB3_BEGIN_NAMESPACE

#if EPUB_COMPILER_SUPPORTS(CXX_ALIAS_TEMPLATES)
using StackLock = std::lock_guard<std::recursive_mutex>;
#else
typedef std::lock_guard<std::recursive_mutex> StackLock;
#endif

// a std::duration type matching the integral type and frequency of
// a timer's LARGE_INTEGER fire date.
typedef std::chrono::duration<LONGLONG, std::ratio<1, 10000000>> winDuration;

#define _THROW_LAST_ERROR() throw std::system_error(static_cast<int>(GetLastError()), std::system_category())

template <class _Clock, class _Rep>
static inline void TimePointToLargeInteger(std::chrono::time_point<_Clock, _Rep>& tp, LARGE_INTEGER* ft)
{
    using namespace std::chrono;
    winDuration t = duration_cast<winDuration>(tp.time_since_epoch());
    ft->QuadPart = t.count() + 116444736000000000;
}

template <class _Clock, class _Rep>
static inline void LargeIntegerToTimePoint(LARGE_INTEGER* ft, std::chrono::time_point<_Clock, _Rep>& tp)
{
    using namespace std::chrono;
    winDuration t(ft->QuadPart);
    tp = duration_cast<_Rep>(winDuration);
}

RunLoop::RunLoop() : _wakeHandle(NULL), _timers(), _sources(), _observers(), _listLock(), _waiting(false), _stop(false), _resetHandles(false), _observerMask(0)
{
    _wakeHandle = CreateEvent(NULL, FALSE, FALSE, NULL);
    if ( _wakeHandle == NULL )
        _THROW_LAST_ERROR();
}
RunLoop::~RunLoop()
{
    if ( _wakeHandle != NULL )
        ::CloseHandle(_wakeHandle);
}
void RunLoop::PerformFunction(std::function<void ()> fn)
{
    RefCounted<EventSource> ev(new EventSource([fn](EventSource& __e){fn();}), adopt_ref);
    AddEventSource(ev);
    ev->Signal();
}
void RunLoop::AddTimer(Timer* timer)
{
    StackLock _(_listLock);
    if ( ContainsTimer(timer) )
        return;

    _timers[timer->_handle] = timer;
    _resetHandles = true;
    WakeUp();
}
bool RunLoop::ContainsTimer(Timer* timer) const
{
    StackLock _(const_cast<RunLoop*>(this)->_listLock);
    return _timers.find(timer->_handle) != _timers.end();
}
void RunLoop::RemoveTimer(Timer* timer)
{
    StackLock _(_listLock);
    auto iter = _timers.find(timer->_handle);
    if ( iter != _timers.end() )
    {
        _timers.erase(iter);
        _resetHandles = true;
        WakeUp();
    }
}
void RunLoop::AddObserver(Observer* obs)
{
    StackLock _(_listLock);
    if ( ContainsObserver(obs) )
        return;

    _observers.push_back(obs);
    _observerMask |= obs->_acts;
}
bool RunLoop::ContainsObserver(Observer* obs) const
{
    StackLock _(const_cast<RunLoop*>(this)->_listLock);
    for ( auto& o : _observers )
    {
        if ( *obs == *o )
            return true;
    }
    return false;
}
void RunLoop::RemoveObserver(Observer* obs)
{
    StackLock _(_listLock);
    for ( auto pos = _observers.begin(), end = _observers.end(); pos != end; ++pos )
    {
        if ( *(*pos) == *obs )
        {
            _observers.erase(pos);
            break;
        }
    }
}
void RunLoop::AddEventSource(EventSource* source)
{
    StackLock _(_listLock);
    if ( ContainsEventSource(source) )
        return;

    _sources[source->_event] = source;
    _resetHandles = true;
    WakeUp();
}
bool RunLoop::ContainsEventSource(EventSource* source) const
{
    StackLock _(const_cast<RunLoop*>(this)->_listLock);
    return _sources.find(source->_event) != _sources.end();
}
void RunLoop::RemoveEventSource(EventSource* source)
{
    StackLock _(_listLock);
    auto iter = _sources.find(source->_event);
    if ( iter != _sources.end() )
    {
        _sources.erase(iter);
        _resetHandles = true;
        WakeUp();
    }
}
void RunLoop::Run()
{
    ExitReason reason;
    do
    {
        std::chrono::nanoseconds timeout(std::chrono::nanoseconds::max());
        reason = RunInternal(false, timeout);

    } while (reason != ExitReason::RunStopped && reason != ExitReason::RunFinished);
}
void RunLoop::Stop()
{
    _stop = true;
    WakeUp();
}
bool RunLoop::IsWaiting() const
{
    return _waiting;
}
void RunLoop::WakeUp()
{
    SetEvent(_wakeHandle);
}
RunLoop::ExitReason RunLoop::RunInternal(bool returnAfterSourceHandled, std::chrono::nanoseconds& timeout)
{
    using namespace std::chrono;
    system_clock::time_point timeoutTime = system_clock::now() + duration_cast<system_clock::duration>(timeout);
    ExitReason reason(ExitReason::RunTimedOut);

    // catch a pending stop
    if ( _stop.exchange(false) )
        return ExitReason::RunStopped;

    _resetHandles = false;
    DWORD handleCount = _timers.size()+_sources.size()+1;
    if ( handleCount == 1 )
        return ExitReason::RunFinished;
    
    HANDLE *pHandles = new HANDLE[handleCount];
    int i = 0;
    for ( auto& pair : _timers )
    {
        pHandles[i++] = pair.first;
    }
    for ( auto& pair : _sources )
    {
        pHandles[i++] = pair.first;
    }
    pHandles[handleCount] = _wakeHandle;

    _listLock.lock();

    RunObservers(Observer::ActivityFlags::RunLoopEntry);

    do
    {
        if ( _resetHandles.exchange(false) )
        {
            delete [] pHandles;
            handleCount = _timers.size()+_sources.size()+1;
            if ( handleCount == 1 )
            {
                reason = ExitReason::RunFinished;
                break;
            }
            
            pHandles = new HANDLE[handleCount];
            int i = 0;
            for ( auto& pair : _timers )
            {
                pHandles[i++] = pair.first;
            }
            for ( auto& pair : _sources )
            {
                pHandles[i++] = pair.first;
            }
            pHandles[handleCount] = _wakeHandle;
        }
        
        RunObservers(Observer::ActivityFlags::RunLoopBeforeWaiting);
        _listLock.unlock();
        _waiting = true;

        milliseconds millis = duration_cast<milliseconds>(timeoutTime - system_clock::now());
        DWORD signaled = ::WaitForMultipleObjects(handleCount, pHandles, FALSE, static_cast<DWORD>(millis.count()));

        _waiting = false;
        _listLock.lock();

        if ( signaled == WAIT_TIMEOUT )
        {
            reason = ExitReason::RunTimedOut;
            break;
        }
        else if ( signaled = WAIT_FAILED )
        {
            // Q: Is throwing GetLastError() more appropriate?
            reason = ExitReason::RunFinished;
            break;
        }
        else if ( signaled < WAIT_OBJECT_0+handleCount )
        {
            HANDLE h = pHandles[signaled];
            if ( h == _wakeHandle )
            {
                if ( _stop.exchange(false) )
                {
                    reason = ExitReason::RunStopped;
                    break;
                }
            }
            else
            {
                // timer or source?
                auto timerIter = _timers.find(h);
                if ( timerIter != _timers.end() )
                {
                    ProcessTimer(timerIter->second);
                }
                else
                {
                    auto sourceIter = _sources.find(h);
                    if ( sourceIter != _sources.end() )
                    {
                        ProcessEventSource(sourceIter->second);
                        if ( returnAfterSourceHandled )
                        {
                            reason = ExitReason::RunHandledSource;
                            break;
                        }
                    }
                }
            }
        }
        else
        {
            reason = ExitReason::RunFinished;
            break;
        }
        
    } while (timeoutTime < system_clock::now());

    _listLock.unlock();
    return reason;
}
void RunLoop::RunObservers(Observer::Activity activity)
{
    // _listLock MUST ALREADY BE HELD
    if ( (_observerMask & activity) == 0 )
        return;

    std::vector<RefCounted<Observer>> observersToRemove;
    for ( auto& observer : _observers )
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

    for ( auto& observer : observersToRemove )
    {
        RemoveObserver(observer);
    }
}
void RunLoop::ProcessTimer(RefCounted<Timer> timer)
{
    if ( timer->IsCancelled() )
    {
        RemoveTimer(timer);
        return;
    }

    timer->_fn(*timer);     // DO CALLOUT

    if ( !timer->Repeats() || timer->IsCancelled() )
    {
        RemoveTimer(timer);
    }
}
void RunLoop::ProcessEventSource(RefCounted<EventSource> source)
{
    if ( source->IsCancelled() )
    {
        RemoveEventSource(source);
        return;
    }

    source->_fn(*source);       // DO CALLOUT

    if ( source->IsCancelled() )
    {
        RemoveEventSource(source);
    }
}

RunLoop::Observer::Observer(Activity activities, bool repeats, ObserverFn fn) : _fn(fn), _acts(activities), _repeats(repeats)
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

RunLoop::EventSource::EventSource(EventHandlerFn fn) : _event(NULL), _fn(fn)
{
    _event = CreateEvent(NULL, FALSE, FALSE, NULL);
    if ( _event == NULL )
        _THROW_LAST_ERROR();
}
RunLoop::EventSource::EventSource(const EventSource& o) : _event(NULL), _fn(o._fn)
{
    _event = CreateEvent(NULL, FALSE, FALSE, NULL);
    if ( _event == NULL )
        _THROW_LAST_ERROR();
}
RunLoop::EventSource::EventSource(EventSource&& o) : _event(o._event), _fn(std::move(o._fn))
{
    o._event = NULL;
}
RunLoop::EventSource::~EventSource()
{
    Cancel();
}
RunLoop::EventSource& RunLoop::EventSource::operator=(const EventSource& o)
{
    _fn = o._fn;
    if ( _event != NULL )
        ::CloseHandle(_event);
    
    _event = CreateEvent(NULL, FALSE, FALSE, NULL);
    if ( _event == NULL )
        _THROW_LAST_ERROR();
    return *this;
}
RunLoop::EventSource& RunLoop::EventSource::operator=(EventSource&& o)
{
    _fn = std::move(o._fn);
    if ( _event != NULL )
        ::CloseHandle(_event);
    
    _event = o._event; o._event = NULL;
    return *this;
}
bool RunLoop::EventSource::operator==(const EventSource& o) const
{
    return _fn.target<void>() == o._fn.target<void>();
}
bool RunLoop::EventSource::IsCancelled() const
{
    return _event == NULL;
}
void RunLoop::EventSource::Cancel()
{
    ::CloseHandle(_event);
    _event = NULL;
}
void RunLoop::EventSource::Signal()
{
    if ( _event == NULL )
        return;
    
    ::SetEvent(_event);
}

RunLoop::Timer::Timer(Clock::time_point& fireDate, Clock::duration& interval, TimerFn fn) : _fireDate(fireDate), _interval(interval), _fn(fn)
{
    using namespace std::chrono;

    _handle = ::CreateWaitableTimer(NULL, FALSE, NULL);
    if ( _handle == NULL )
        _THROW_LAST_ERROR();

    // the type Windows uses to set the timer's fire date
    LARGE_INTEGER due;
    TimePointToLargeInteger(fireDate, &due);
    
    if ( ::SetWaitableTimer(_handle, &due, static_cast<LONG>(duration_cast<milliseconds>(interval).count()),
                            NULL, NULL, FALSE) == FALSE )
    {
        ::CloseHandle(_handle);
        _THROW_LAST_ERROR();
    }
}
RunLoop::Timer::Timer(Clock::duration& interval, bool repeat, TimerFn fn) : _fireDate(Clock::now() + interval), _interval(interval), _fn(fn)
{
    using namespace std::chrono;

    _handle = ::CreateWaitableTimer(NULL, FALSE, NULL);
    if ( _handle == NULL )
        _THROW_LAST_ERROR();

    // the type Windows uses to set the timer's fire date
    LARGE_INTEGER due;
    TimePointToLargeInteger(_fireDate, &due);

    if ( ::SetWaitableTimer(_handle, &due,
                            (repeat ? static_cast<LONG>(duration_cast<milliseconds>(interval).count()) : 0L),
                            NULL, NULL, FALSE) == FALSE )
    {
        ::CloseHandle(_handle);
        _THROW_LAST_ERROR();
    }
}
RunLoop::Timer::Timer(const Timer& o) : _fireDate(o._fireDate), _interval(o._interval), _fn(o._fn)
{
    using namespace std::chrono;

    _handle = ::CreateWaitableTimer(NULL, FALSE, NULL);
    if ( _handle == NULL )
        _THROW_LAST_ERROR();

    // the type Windows uses to set the timer's fire date
    LARGE_INTEGER due;
    TimePointToLargeInteger(_fireDate, &due);

    if ( ::SetWaitableTimer(_handle, &due,
                            (o.Repeats() ? static_cast<LONG>(duration_cast<milliseconds>(_interval).count()) : 0L),
                            NULL, NULL, FALSE) == FALSE )
    {
        ::CloseHandle(_handle);
        _THROW_LAST_ERROR();
    }
}
RunLoop::Timer::Timer(Timer&& o) : _fireDate(std::move(_fireDate)), _interval(std::move(o._interval)), _fn(std::move(o._fn)), _handle(o._handle)
{
    o._handle = NULL;
}
RunLoop::Timer::~Timer()
{
    if ( _handle != NULL )
        ::CloseHandle(_handle);
}
RunLoop::Timer& RunLoop::Timer::operator=(const Timer& o)
{
    using namespace std::chrono;
    
    _fn = o._fn;
    _fireDate = o._fireDate;
    _interval = o._interval;
    _cancelled = o._cancelled;

    if ( _handle != NULL )
        ::CancelWaitableTimer(_handle);
    
    if ( !_cancelled )
    {
        if ( _handle == NULL )
        {
            _handle = ::CreateWaitableTimer(NULL, FALSE, NULL);
            if ( _handle == NULL )
                _THROW_LAST_ERROR();
        }

        LARGE_INTEGER due;
        TimePointToLargeInteger(_fireDate, &due);

        if ( ::SetWaitableTimer(_handle, &due,
                                (o.Repeats() ? static_cast<LONG>(duration_cast<milliseconds>(_interval).count()) : 0L),
                                NULL, NULL, FALSE) == FALSE )
        {
            ::CloseHandle(_handle);
            _THROW_LAST_ERROR();
        }
    }

    return *this;
}
RunLoop::Timer& RunLoop::Timer::operator=(Timer&& o)
{
    _fn = std::move(o._fn);
    _fireDate = std::move(o._fireDate);
    _interval = std::move(o._interval);
    _cancelled = o._cancelled;

    if ( _handle != NULL )
        ::CancelWaitableTimer(_handle);
    _handle = o._handle; o._handle = NULL;

    return *this;
}
bool RunLoop::Timer::operator==(const Timer& o) const
{
    return (_fireDate == o._fireDate) && (_interval == o._interval) && (_fn.target<void>() == o._fn.target<void>());
}
void RunLoop::Timer::Cancel()
{
    _cancelled = true;
    ::CancelWaitableTimer(_handle);
}
bool RunLoop::Timer::IsCancelled() const
{
    return _cancelled;
}
bool RunLoop::Timer::Repeats() const
{
    return _interval.count() != 0;
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
    using namespace std::chrono;
    if ( _cancelled )
        return;

    LARGE_INTEGER due;
    TimePointToLargeInteger(_fireDate, &due);
    ::SetWaitableTimer(_handle, &due, static_cast<LONG>(duration_cast<milliseconds>(_interval).count()),
                       NULL, NULL, FALSE);
}
RunLoop::Timer::Clock::duration RunLoop::Timer::GetNextFireDateDuration() const
{
    return _fireDate - Clock::now();
}
void RunLoop::Timer::SetNextFireDateDuration(Clock::duration& when)
{
    using namespace std::chrono;
    if ( _cancelled )
        return;

    _fireDate = Clock::now() + when;

    LARGE_INTEGER due;
    TimePointToLargeInteger(_fireDate, &due);
    ::SetWaitableTimer(_handle, &due, static_cast<LONG>(duration_cast<milliseconds>(_interval).count()),
                       NULL, NULL, FALSE);
}

EPUB3_END_NAMESPACE