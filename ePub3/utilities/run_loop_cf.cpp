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

#if !EPUB_USE(CF)
# error Please use the appropriate run_loop_*.cpp for your platform
#endif

EPUB3_BEGIN_NAMESPACE

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
void RunLoop::AddTimer(TimerPtr timer)
{
    CFRunLoopAddTimer(_cf, timer->_cf, ADD_MODE_ARG);
}
bool RunLoop::ContainsTimer(TimerPtr timer) const
{
    return (CFRunLoopContainsTimer(_cf, timer->_cf, ADD_MODE_ARG) == TRUE);
}
void RunLoop::RemoveTimer(TimerPtr timer)
{
    CFRunLoopRemoveTimer(_cf, timer->_cf, ADD_MODE_ARG);
}
void RunLoop::AddEventSource(EventSourcePtr source)
{
    CFRunLoopAddSource(_cf, source->_cf, ADD_MODE_ARG);
}
bool RunLoop::ContainsEventSource(EventSourcePtr source) const
{
    return (CFRunLoopContainsSource(_cf, source->_cf, ADD_MODE_ARG) == TRUE);
}
void RunLoop::RemoveEventSource(EventSourcePtr source)
{
    CFRunLoopRemoveSource(_cf, source->_cf, ADD_MODE_ARG);
}
void RunLoop::AddObserver(ObserverPtr observer)
{
    CFRunLoopAddObserver(_cf, observer->_cf, ADD_MODE_ARG);
}
bool RunLoop::ContainsObserver(ObserverPtr observer) const
{
    return (CFRunLoopContainsObserver(_cf, observer->_cf, ADD_MODE_ARG) == TRUE);
}
void RunLoop::RemoveObserver(ObserverPtr observer)
{
    CFRunLoopRemoveObserver(_cf, observer->_cf, ADD_MODE_ARG);
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
    _cf = std::move(o._cf);
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
    _cf = std::move(o._cf);
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
bool RunLoop::Observer::IsCancelled() const
{
    return (CFRunLoopObserverIsValid(_cf) == FALSE);
}
void RunLoop::Observer::Cancel()
{
    CFRunLoopObserverInvalidate(_cf);
}

RunLoop::EventSource::EventSource(EventHandlerFn fn) : _cf(nullptr), _rl(), _fn(fn)
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
}
RunLoop::EventSource::EventSource(EventSource&& o) : _cf(std::move(o._cf))
{
}
RunLoop::EventSource::~EventSource()
{
}
RunLoop::EventSource& RunLoop::EventSource::operator=(EventSource && o)
{
    _cf = std::move(o._cf);
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

EPUB3_END_NAMESPACE

#endif //FUTURE_ENABLED
