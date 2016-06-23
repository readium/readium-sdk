//
//  run_loop_windows.cpp
//  ePub3
//
//  Created by Jim Dovey on 2013-04-22.
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
    _wakeHandle = CreateEventEx(NULL, NULL, 0, 0);
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
    EventSourcePtr ev = EventSource::New([fn](EventSource& __e){fn();});
    AddEventSource(ev);
    ev->Signal();
}
void RunLoop::AddTimer(TimerPtr timer)
{
    StackLock _(_listLock);
    if ( ContainsTimer(timer) )
        return;

    _timers[timer->_handle] = timer;
    _resetHandles = true;
    WakeUp();
}
bool RunLoop::ContainsTimer(TimerPtr timer) const
{
    StackLock _(const_cast<RunLoop*>(this)->_listLock);
    return _timers.find(timer->_handle) != _timers.end();
}
void RunLoop::RemoveTimer(TimerPtr timer)
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
void RunLoop::AddObserver(ObserverPtr obs)
{
    StackLock _(_listLock);
    if ( ContainsObserver(obs) )
        return;

    _observers.push_back(obs);
    _observerMask |= obs->_acts;
}
bool RunLoop::ContainsObserver(ObserverPtr obs) const
{
    StackLock _(const_cast<RunLoop*>(this)->_listLock);
    for ( auto& o : _observers )
    {
        if ( *obs == *o )
            return true;
    }
    return false;
}
void RunLoop::RemoveObserver(ObserverPtr obs)
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
void RunLoop::AddEventSource(EventSourcePtr source)
{
    StackLock _(_listLock);
    if ( ContainsEventSource(source) )
        return;

    _sources[source->_event] = source;
    _resetHandles = true;
    WakeUp();
}
bool RunLoop::ContainsEventSource(EventSourcePtr source) const
{
    StackLock _(const_cast<RunLoop*>(this)->_listLock);
    return _sources.find(source->_event) != _sources.end();
}
void RunLoop::RemoveEventSource(EventSourcePtr source)
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
    DWORD handleCount = static_cast<DWORD>(_timers.size()+_sources.size()+1);
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
    pHandles[handleCount-1] = _wakeHandle;

    _listLock.lock();

    RunObservers(Observer::ActivityFlags::RunLoopEntry);

    do
    {
        if ( _resetHandles.exchange(false) )
        {
            delete [] pHandles;
            handleCount = static_cast<DWORD>(_timers.size()+_sources.size()+1);
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
            pHandles[handleCount-1] = _wakeHandle;
        }
        
        RunObservers(Observer::ActivityFlags::RunLoopBeforeWaiting);
        _listLock.unlock();
        _waiting = true;

        milliseconds millis = duration_cast<milliseconds>(timeoutTime - system_clock::now());
        DWORD signaled = ::WaitForMultipleObjectsEx(handleCount, pHandles, FALSE, static_cast<DWORD>(millis.count()), FALSE);

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

    shared_vector<Observer> observersToRemove;
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
void RunLoop::ProcessTimer(TimerPtr timer)
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
void RunLoop::ProcessEventSource(EventSourcePtr source)
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

RunLoop::EventSource::EventSource(EventHandlerFn fn) : _event(NULL), _runLoops(), _fn(fn)
{
	_event = CreateEventEx(NULL, NULL, 0, 0);
    if ( _event == NULL )
        _THROW_LAST_ERROR();
}
RunLoop::EventSource::EventSource(const EventSource& o) : _event(NULL), _runLoops(), _fn(o._fn)
{
	_event = CreateEventEx(NULL, NULL, 0, 0);
    if ( _event == NULL )
        _THROW_LAST_ERROR();
}
RunLoop::EventSource::EventSource(EventSource&& o) : _event(o._event), _runLoops(std::move(o._runLoops)), _fn(std::move(o._fn))
{
	auto self = shared_from_this();
	auto so = o.shared_from_this();
	for (auto& wk : _runLoops)
	{
		auto ptr = wk.lock();
		if (bool(ptr))
		{
			ptr->RemoveEventSource(so);
			ptr->AddEventSource(self);
		}
	}

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
    
	_event = CreateEventEx(NULL, NULL, 0, 0);
    if ( _event == NULL )
        _THROW_LAST_ERROR();
    return *this;
}
RunLoop::EventSource& RunLoop::EventSource::operator=(EventSource&& o)
{
    _fn = std::move(o._fn);

	auto self = shared_from_this();
	for (auto& wk : _runLoops)
	{
		auto ptr = wk.lock();
		if (bool(ptr))
			ptr->RemoveEventSource(self);
	}
	_runLoops.clear();

    if ( _event != NULL )
        ::CloseHandle(_event);
	_runLoops = std::move(o._runLoops);
	_event = o._event;

	auto so = o.shared_from_this();
	for (auto& wk : _runLoops)
	{
		auto ptr = wk.lock();
		if (bool(ptr))
		{
			ptr->RemoveEventSource(so);
			ptr->AddEventSource(self);
		}
	}
    
    o._event = NULL;
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
	if (IsCancelled())
		return;

	auto self = shared_from_this();
	for (auto& wk : _runLoops)
	{
		auto ptr = wk.lock();
		if (bool(ptr))
			ptr->RemoveEventSource(self);
	}
	_runLoops.clear();

    ::CloseHandle(_event);
    _event = NULL;
}
void RunLoop::EventSource::Signal()
{
    if ( IsCancelled() )
        return;
    
    ::SetEvent(_event);
}

#if EPUB_PLATFORM(WINRT)
ref class TimerCallbackWrapper
{
private:
	typedef std::function<void()> CallbackType;

	friend class ePub3::RunLoop::Timer;
	TimerCallbackWrapper(CallbackType cb) : m_callback(cb) {}
	void Fire(ThreadPoolTimer^ unused) {
		m_callback();
	}

private:
	CallbackType m_callback;
};
#endif

RunLoop::Timer::Timer(Clock::time_point& fireDate, Clock::duration& interval, TimerFn fn) : _runLoops(), _fireDate(fireDate), _interval(interval), _fn(fn)
{
    using namespace std::chrono;

#if EPUB_PLATFORM(WINRT)
	_handle = ::CreateEventEx(NULL, NULL, 0, 0);
	if (_handle == NULL)
		_THROW_LAST_ERROR();

	std::weak_ptr<Timer> weakThis(shared_from_this());
	auto doneFn = ref new TimerCallbackWrapper([weakThis](){
		auto self = weakThis.lock();
		if (!bool(self))
			return;

		if (self->_handle != NULL)
			::SetEvent(self->_handle);
	});
	auto elapsedHandler = ref new TimerElapsedHandler(doneFn, &TimerCallbackWrapper::Fire, Platform::CallbackContext::Any, true);

	if (interval > Clock::duration(0))
	{
		// Periodic Timer
		Windows::Foundation::TimeSpan timeSpan{ interval.count() };
		_timer = ThreadPoolTimer::CreatePeriodicTimer(elapsedHandler, timeSpan);
	}
	else
	{
		// One-shot timer
		long long theInterval = duration_cast<milliseconds>(fireDate.time_since_epoch() - Clock::now().time_since_epoch()).count();
		Windows::Foundation::TimeSpan timeSpan{ theInterval };
		_timer = ThreadPoolTimer::CreateTimer(elapsedHandler, timeSpan);
	}

#else
    _handle = CreateWaitableTimer(nullptr, FALSE, nullptr);
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
#endif
}
RunLoop::Timer::Timer(Clock::duration& interval, bool repeat, TimerFn fn) : _runLoops(), _fireDate(Clock::now() + interval), _interval(interval), _fn(fn)
{
    using namespace std::chrono;
#if EPUB_PLATFORM(WINRT)
	_handle = ::CreateEventEx(NULL, NULL, 0, 0);
    if ( _handle == NULL )
		_THROW_LAST_ERROR();

	std::weak_ptr<Timer> weakThis(shared_from_this());
	auto doneFn = ref new TimerCallbackWrapper([weakThis](){
		auto self = weakThis.lock();
		if (!bool(self))
			return;

		if (self->_handle != NULL)
			::SetEvent(self->_handle);
	});
	auto elapsedHandler = ref new TimerElapsedHandler(doneFn, &TimerCallbackWrapper::Fire, Platform::CallbackContext::Any, true);

	Windows::Foundation::TimeSpan timeSpan{ interval.count() };
	_timer = ThreadPoolTimer::CreatePeriodicTimer(elapsedHandler, timeSpan);
#else
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
#endif
}
RunLoop::Timer::Timer(const Timer& o) : _runLoops(), _fireDate(o._fireDate), _interval(o._interval), _fn(o._fn)
{
    using namespace std::chrono;
#if EPUB_PLATFORM(WINRT)
	_handle = ::CreateEventEx(NULL, NULL, 0, 0);
	if (_handle == NULL)
		_THROW_LAST_ERROR();

	std::weak_ptr<Timer> weakThis(shared_from_this());
	auto doneFn = ref new TimerCallbackWrapper([weakThis](){
		auto self = weakThis.lock();
		if (!bool(self))
			return;

		if (self->_handle != NULL)
			::SetEvent(self->_handle);
	});
	auto elapsedHandler = ref new TimerElapsedHandler(doneFn, &TimerCallbackWrapper::Fire, Platform::CallbackContext::Any, true);

	if (o._timer->Period.Duration != 0)
	{
		// Periodic Timer
		_timer = ThreadPoolTimer::CreatePeriodicTimer(elapsedHandler, o._timer->Period);
	}
	else
	{
		// One-shot timer
		_timer = ThreadPoolTimer::CreateTimer(elapsedHandler, o._timer->Delay);
	}
#else
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
#endif
}
RunLoop::Timer::Timer(Timer&& o) : _runLoops(std::move(o._runLoops)), _fireDate(std::move(_fireDate)), _interval(std::move(o._interval)), _fn(std::move(o._fn)),
#if EPUB_PLATFORM(WINRT)
	_timer(o._timer),
#endif
	_handle(o._handle)
{
#if EPUB_PLATFORM(WINRT)
	o._timer = nullptr;
#endif
	auto self = shared_from_this();
	auto so = o.shared_from_this();
	for (auto& wk : _runLoops)
	{
		auto ptr = wk.lock();
		if (bool(ptr))
		{
			ptr->RemoveTimer(so);
			ptr->AddTimer(self);
		}
	}

    o._handle = NULL;
}
RunLoop::Timer::~Timer()
{
#if EPUB_PLATFORM(WINRT)
	if (!IsCancelled())
		_timer->Cancel();
	_timer = nullptr;
#endif
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
#if EPUB_PLATFORM(WINRT)
	if (_handle != NULL)
	{
		CloseHandle(_handle);
		_handle = NULL;
	}
	if (_timer != nullptr)
	{
		_timer->Cancel();
		_timer = nullptr;
	}

	_handle = ::CreateEventEx(NULL, NULL, 0, 0);
	if (_handle == NULL)
		_THROW_LAST_ERROR();

	auto doneFn = ref new TimerCallbackWrapper([this](){if (_handle != NULL)::SetEvent(_handle); });
	auto elapsedHandler = ref new TimerElapsedHandler(doneFn, &TimerCallbackWrapper::Fire, Platform::CallbackContext::Any, true);

	if (o._timer->Period.Duration != 0)
	{
		// Periodic Timer
		_timer = ThreadPoolTimer::CreatePeriodicTimer(elapsedHandler, o._timer->Period);
	}
	else
	{
		// One-shot timer
		_timer = ThreadPoolTimer::CreateTimer(elapsedHandler, o._timer->Delay);
	}
#else
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
#endif
    return *this;
}
RunLoop::Timer& RunLoop::Timer::operator=(Timer&& o)
{
    _fn = std::move(o._fn);
    _fireDate = std::move(o._fireDate);
    _interval = std::move(o._interval);
    _cancelled = o._cancelled;
#if EPUB_PLATFORM(WINRT)
	_timer = o._timer;
	o._timer = nullptr;

	_runLoops = std::move(o._runLoops);
	_handle = o._handle;

	auto self = shared_from_this();
	auto so = o.shared_from_this();
	for (auto& wk : _runLoops)
	{
		auto ptr = wk.lock();
		if (bool(ptr))
		{
			ptr->RemoveTimer(so);
			ptr->AddTimer(self);
		}
	}
#else
    if ( _handle != NULL )
        ::CancelWaitableTimer(_handle);
#endif
	
	o._handle = NULL;
    return *this;
}
bool RunLoop::Timer::operator==(const Timer& o) const
{
    return (_fireDate == o._fireDate) && (_interval == o._interval) && (_fn.target<void>() == o._fn.target<void>());
}
void RunLoop::Timer::Cancel()
{
	if (IsCancelled())
		return;

    _cancelled = true;
#if EPUB_PLATFORM(WINRT)
	if (_timer != nullptr)
		_timer->Cancel();

	auto self = shared_from_this();
	for (auto& wk : _runLoops)
	{
		auto ptr = wk.lock();
		if (bool(ptr))
			ptr->RemoveTimer(self);
	}
	_runLoops.clear();

	::CloseHandle(_handle);
#else
    ::CancelWaitableTimer(_handle);
#endif
	_handle = NULL;
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
	_fireDate = when;
#if EPUB_PLATFORM(WINRT)
	if (_timer != nullptr)
	{
		_timer->Cancel();
		_timer = nullptr;
	}

	auto doneFn = ref new TimerCallbackWrapper([this](){if (_handle != NULL)::SetEvent(_handle); });
	auto elapsedHandler = ref new TimerElapsedHandler(doneFn, &TimerCallbackWrapper::Fire, Platform::CallbackContext::Any, true);

	long long delay = duration_cast<milliseconds>(_fireDate.time_since_epoch() - Clock::now().time_since_epoch()).count();
	Windows::Foundation::TimeSpan timeSpan{delay};
	_timer = ThreadPoolTimer::CreateTimer(elapsedHandler, timeSpan);
#else
    LARGE_INTEGER due;
    TimePointToLargeInteger(_fireDate, &due);
    ::SetWaitableTimer(_handle, &due, static_cast<LONG>(duration_cast<milliseconds>(_interval).count()),
                       NULL, NULL, FALSE);
#endif
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
#if EPUB_PLATFORM(WINRT)
	if (_timer != nullptr)
	{
		_timer->Cancel();
		_timer = nullptr;
	}

	auto doneFn = ref new TimerCallbackWrapper([this](){if (_handle != NULL)::SetEvent(_handle); });
	auto elapsedHandler = ref new TimerElapsedHandler(doneFn, &TimerCallbackWrapper::Fire, Platform::CallbackContext::Any, true);

	Windows::Foundation::TimeSpan timeSpan{ when.count() };
	_timer = ThreadPoolTimer::CreatePeriodicTimer(elapsedHandler, timeSpan);
#else
    LARGE_INTEGER due;
    TimePointToLargeInteger(_fireDate, &due);
    ::SetWaitableTimer(_handle, &due, static_cast<LONG>(duration_cast<milliseconds>(_interval).count()),
                       NULL, NULL, FALSE);
#endif
}

#if EPUB_PLATFORM(WINRT)

DWORD RunLoop::RunLoopTLSKey = TLS_OUT_OF_INDEXES;

void RunLoop::KillRunLoopTLSKey() {
	if (RunLoop::RunLoopTLSKey != TLS_OUT_OF_INDEXES) {
		TlsFree(RunLoop::RunLoopTLSKey);
	}
}

void RunLoop::InitRunLoopTLSKey() {
	RunLoopTLSKey = TlsAlloc();
	if (RunLoopTLSKey == TLS_OUT_OF_INDEXES)
	{
		OutputDebugString(L"No TLS Indexes for RunLoop!\n");
		std::terminate();
	}
	atexit(RunLoop::KillRunLoopTLSKey);
}

#endif

EPUB3_END_NAMESPACE
