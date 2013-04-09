//
//  run_loop.cpp
//  ePub3
//
//  Created by Jim Dovey on 2013-04-08.
//  Copyright (c) 2013 The Readium Foundation and contributors. All rights reserved.
//

#include "run_loop.h"

EPUB3_BEGIN_NAMESPACE

static pthread_key_t RunLoopTLSKey;

static void _DestroyTLSRunLoop(void* data)
{
    RunLoop* rl = reinterpret_cast<RunLoop*>(data);
    delete rl;
}
/// FIXME: alternatives to __attribute__((constructor)) etc.?
__attribute__((constructor))
static void InitRunLoopTLSKey()
{
    pthread_key_create(&RunLoopTLSKey, _DestroyTLSRunLoop);
}
__attribute__((destructor))
static void KillRunLoopTLSKey()
{
    pthread_key_delete(RunLoopTLSKey);
}

RunLoop* RunLoop::CurrentRunLoop()
{
    RunLoop* p = reinterpret_cast<RunLoop*>(pthread_getspecific(RunLoopTLSKey));
    if ( p == nullptr )
    {
        p = new RunLoop();
        pthread_setspecific(RunLoopTLSKey, reinterpret_cast<void*>(p));
    }
    return p;
}

#if EPUB_USE(CF)

#define ADD_MODE_ARG kCFRunLoopCommonModes
#define RUN_MODE_ARG kCFRunLoopDefaultMode

typedef std::chrono::duration<CFTimeInterval>   CFInterval;
typedef std::chrono::duration<CFAbsoluteTime>   CFAbsolute;

const cf_adopt_ref_t cf_adopt_ref = cf_adopt_ref_t();

cf_clock::time_point cf_clock::now() noexcept
{
    return time_point(duration(CFAbsoluteTimeGetCurrent()));
}
time_t cf_clock::to_time_t(const time_point &__t) noexcept
{
    return time_t(__t.time_since_epoch().count() + kCFAbsoluteTimeIntervalSince1970);
}
cf_clock::time_point cf_clock::from_time_t(const time_t &__t) noexcept
{
    return time_point(duration(CFAbsoluteTime(__t) - kCFAbsoluteTimeIntervalSince1970));
}

RunLoop::RunLoop() : _cf(CFRunLoopGetCurrent())
{
}
RunLoop::~RunLoop()
{
}
void RunLoop::PerformFunction(std::function<void ()> fn)
{
    CFRunLoopPerformBlock(_cf, ADD_MODE_ARG, ^{fn();});
}
void RunLoop::AddTimer(const Timer &timer)
{
    CFRunLoopAddTimer(_cf, timer._cf, ADD_MODE_ARG);
}
bool RunLoop::ContainsTimer(const Timer &timer) const
{
    return (CFRunLoopContainsTimer(_cf, timer._cf, ADD_MODE_ARG) == TRUE);
}
void RunLoop::RemoveTimer(const Timer &timer)
{
    CFRunLoopRemoveTimer(_cf, timer._cf, ADD_MODE_ARG);
}
void RunLoop::AddEventSource(const EventSource &source)
{
    CFRunLoopAddSource(_cf, source._cf, ADD_MODE_ARG);
}
bool RunLoop::ContainsEventSource(const EventSource &source) const
{
    return (CFRunLoopContainsSource(_cf, source._cf, ADD_MODE_ARG) == TRUE);
}
void RunLoop::RemoveEventSource(const EventSource &source)
{
    CFRunLoopRemoveSource(_cf, source._cf, ADD_MODE_ARG);
}
void RunLoop::AddObserver(const Observer &observer)
{
    CFRunLoopAddObserver(_cf, observer._cf, ADD_MODE_ARG);
}
bool RunLoop::ContainsObserver(const Observer &observer) const
{
    return (CFRunLoopContainsObserver(_cf, observer._cf, ADD_MODE_ARG) == TRUE);
}
void RunLoop::RemoveObserver(const Observer &observer)
{
    CFRunLoopRemoveObserver(_cf, observer._cf, ADD_MODE_ARG);
}
void RunLoop::Run()
{
    if (CFRunLoopGetCurrent() != _cf)
        return;     // Q: Should I throw here?
    CFRunLoopRun();
}
void RunLoop::Stop()
{
    CFRunLoopStop(_cf);
}
bool RunLoop::IsWaiting() const
{
    return CFRunLoopIsWaiting(_cf);
}
void RunLoop::WakeUp()
{
    CFRunLoopWakeUp(_cf);
}
RunLoop::ExitReason RunLoop::RunInternal(bool returnAfterSourceHandled, std::chrono::nanoseconds& timeout)
{
    using namespace std::chrono;
    return ExitReason(CFRunLoopRunInMode(RUN_MODE_ARG, duration_cast<cf_clock::duration>(timeout).count(), returnAfterSourceHandled));
}

RunLoop::Timer::Timer(Clock::time_point& fireDate, Clock::duration& interval, TimerFn fn)
{
    using namespace std::chrono;
    _cf = CFRunLoopTimerCreateWithHandler(kCFAllocatorDefault, fireDate.time_since_epoch().count(), interval.count(), 0, 0, ^(CFRunLoopTimerRef timer) {
        fn(*this);
    });
}
RunLoop::Timer::Timer(Clock::duration& interval, bool repeat, TimerFn fn)
{
    using namespace std::chrono;
    CFAbsoluteTime fireDate = CFAbsoluteTimeGetCurrent() + interval.count();
    _cf = CFRunLoopTimerCreateWithHandler(kCFAllocatorDefault, fireDate, interval.count(), 0, 0, ^(CFRunLoopTimerRef timer) {
        fn(*this);
    });
}
RunLoop::Timer::Timer(const Timer& o) : _cf(o._cf)
{
}
RunLoop::Timer::Timer(Timer&& o) : _cf(std::move(o._cf))
{
}
RunLoop::Timer::~Timer()
{
}
RunLoop::Timer& RunLoop::Timer::operator=(const Timer & o)
{
    _cf = o._cf;
    CFRetain(_cf);
    return *this;
}
RunLoop::Timer& RunLoop::Timer::operator=(Timer&& o)
{
    _cf.swap(std::move(o._cf));
    return *this;
}
bool RunLoop::Timer::operator==(const Timer& o) const
{
    return _cf == o._cf;
}
void RunLoop::Timer::Cancel()
{
    CFRunLoopTimerInvalidate(_cf);
}
bool RunLoop::Timer::IsCancelled() const
{
    return (CFRunLoopTimerIsValid(_cf) == FALSE);
}
bool RunLoop::Timer::Repeats() const
{
    return CFRunLoopTimerDoesRepeat(_cf);
}
RunLoop::Timer::Clock::duration RunLoop::Timer::RepeatIntervalInternal() const
{
    return Clock::duration(CFRunLoopTimerGetInterval(_cf));
}
RunLoop::Timer::Clock::time_point RunLoop::Timer::GetNextFireDateTime() const
{
    return Clock::time_point(Clock::duration(CFRunLoopTimerGetNextFireDate(_cf)));
}
void RunLoop::Timer::SetNextFireDateTime(Clock::time_point& when)
{
    CFRunLoopTimerSetNextFireDate(_cf, when.time_since_epoch().count());
}
RunLoop::Timer::Clock::duration RunLoop::Timer::GetNextFireDateDuration() const
{
    return Clock::duration(CFRunLoopTimerGetNextFireDate(_cf)) - Clock::now().time_since_epoch();
}
void RunLoop::Timer::SetNextFireDateDuration(Clock::duration& when)
{
    CFRunLoopTimerSetNextFireDate(_cf, (Clock::now()+when).time_since_epoch().count());
}

RunLoop::Observer::Observer(Activity activities, bool repeats, ObserverFn fn)
{
    _cf = CFRunLoopObserverCreateWithHandler(kCFAllocatorDefault, CFOptionFlags(activities), Boolean(repeats), 0, ^(CFRunLoopObserverRef observer, CFRunLoopActivity activity) {
        fn(*this, activities);
    });
}
RunLoop::Observer::Observer(const Observer& o) : _cf(o._cf)
{
}
RunLoop::Observer::Observer(Observer&& o) : _cf(std::move(o._cf))
{
}
RunLoop::Observer::~Observer()
{
}
RunLoop::Observer& RunLoop::Observer::operator=(const Observer & o)
{
    _cf = o._cf;
    return *this;
}
RunLoop::Observer& RunLoop::Observer::operator=(Observer &&o)
{
    _cf.swap(std::move(o._cf));
    return *this;
}
bool RunLoop::Observer::operator==(const Observer &o) const
{
    return _cf == o._cf;
}
RunLoop::Observer::Activity RunLoop::Observer::GetActivities() const
{
    return Activity(CFRunLoopObserverGetActivities(_cf));
}
bool RunLoop::Observer::Repeats() const
{
    return (CFRunLoopObserverDoesRepeat(_cf) == TRUE);
}
bool RunLoop::Observer::Cancelled() const
{
    return (CFRunLoopObserverIsValid(_cf) == FALSE);
}
void RunLoop::Observer::Cancel()
{
    CFRunLoopObserverInvalidate(_cf);
}

RunLoop::EventSource::EventSource(EventHandlerFn fn) : _cf(nullptr), _rl(), _fn()
{
    CFRunLoopSourceContext ctx = {
        .version            = 0,
        .info               = reinterpret_cast<void*>(this),
        .retain             = nullptr,
        .release            = nullptr,
        .copyDescription    = nullptr,
        .equal              = nullptr,
        .hash               = nullptr,
        .schedule           = &_ScheduleCF,
        .cancel             = &_CancelCF,
        .perform            = &_FireCFSourceEvent,
    };
    _cf = CFRunLoopSourceCreate(kCFAllocatorDefault, 0, &ctx);
}
RunLoop::EventSource::EventSource(const EventSource& o) : _cf(o._cf)
{
    CFRetain(_cf);
}
RunLoop::EventSource::EventSource(EventSource&& o) : _cf(o._cf)
{
    o._cf = nullptr;
}
RunLoop::EventSource& RunLoop::EventSource::operator=(EventSource && o)
{
    _cf.swap(std::move(o._cf));
    return *this;
}
bool RunLoop::EventSource::operator==(const EventSource & o) const
{
    return _cf == o._cf;
}
bool RunLoop::EventSource::IsCancelled() const
{
    return (CFRunLoopSourceIsValid(_cf) == FALSE);
}
void RunLoop::EventSource::Cancel()
{
    CFRunLoopSourceInvalidate(_cf);
}
void RunLoop::EventSource::Signal()
{
    CFRunLoopSourceSignal(_cf);
    for ( auto& item : _rl )
    {
        if ( item.second > 0 )
            CFRunLoopWakeUp(item.first);
    }
}
void RunLoop::EventSource::_FireCFSourceEvent(void *info)
{
    EventSource* p = reinterpret_cast<EventSource*>(info);
    p->_fn(*p);
}
void RunLoop::EventSource::_ScheduleCF(void* info, CFRunLoopRef rl, CFStringRef mode)
{
    EventSource* p = reinterpret_cast<EventSource*>(info);
    p->_rl[rl]++;
}
void RunLoop::EventSource::_CancelCF(void* info, CFRunLoopRef rl, CFStringRef mode)
{
    EventSource* p = reinterpret_cast<EventSource*>(info);
    if ( --(p->_rl[rl]) == 0 )
    {
        p->_rl.erase(rl);
    }
}

#elif EPUB_OS(ANDROID)

using StackLock = std::lock_guard<std::recursive_mutex>;

RunLoop::RunLoop() : _timers(), _observers(), _sources()
{
}
RunLoop::~RunLoop()
{
}
void RunLoop::PerformFunction(std::function<void ()> fn)
{
    EventSource ev([fn](EventSource& __e) {
        fn();
    });
    AddEventSource(ev);
    ev.Signal();
}
void RunLoop::AddTimer(const Timer &timer)
{
    StackLock lock(_listLock);
    if ( ContainsTimer(timer) )
        return;
    
    _timers.push_back(timer);
    const_cast<Timer&>(timer).Arm();
}
bool RunLoop::ContainsTimer(const Timer &timer) const
{
    StackLock lock(const_cast<RunLoop*>(this)->_listLock);
    for ( const Timer& t : _timers )
    {
        if ( timer == t )
            return true;
    }
    return false;
}
void RunLoop::RemoveTimer(const Timer &timer)
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
}
void RunLoop::AddObserver(const Observer &observer)
{
    StackLock lock(_listLock);
    if ( ContainsObserver(observer) )
        return;
    
    _observers.push_back(observer);
}
bool RunLoop::ContainsObserver(const Observer &obs) const
{
    StackLock lock(const_cast<RunLoop*>(this)->_listLock);
    for ( const Observer& o : _observers )
    {
        if ( obs == o )
            return true;
    }
    return false;
}
void RunLoop::RemoveObserver(const Observer &obs)
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
void RunLoop::AddEventSource(const EventSource &ev)
{
    StackLock lock(_listLock);
    if ( ContainsEventSource(ev) )
        return;
    
    _sources.push_back(ev);
}
bool RunLoop::ContainsEventSource(const EventSource &ev) const
{
    StackLock lock(const_cast<RunLoop*>(this)->_listLock);
    for ( const EventSource& e : _sources )
    {
        if ( ev == e )
            return true;
    }
    return false;
}
void RunLoop::RemoveEventSource(const EventSource &ev)
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
}

#elif EPUB_OS(WINDOWS)
# error RunLoop on Windows missing implementation
#else
# error Don't know how to implement a RunLoop on this system
#endif

EPUB3_END_NAMESPACE
