//
//  run_loop.h
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

#ifndef __ePub3__run_loop__
#define __ePub3__run_loop__

#if FUTURE_ENABLED

//#undef EPUB_USE_CF
//#define EPUB_OS_ANDROID 1

#include <ePub3/base.h>

#if EPUB_USE(CF)
# include <CoreFoundation/CoreFoundation.h>
#elif EPUB_OS(ANDROID)
# include <time.h>
struct ALooper;
#elif EPUB_OS(WINDOWS)
# include <windows.h>
#else
# include <condition_variable>      // GNU libstdc++ 4.7 has this guy in a separate header
# include <pthread.h>
# include <time.h>
#endif

#include <ePub3/epub3.h>
#include <ePub3/utilities/basic.h>
#include <ePub3/utilities/utfstring.h>
#include <chrono>
#include <list>
#include <mutex>
#include <atomic>

#if EPUB_USE(CF)
#include "cf_helpers.h"
#endif

#if EPUB_PLATFORM(WINRT)
#include "ThreadEmulation.h"
using namespace Windows::System::Threading;
#endif

EPUB3_BEGIN_NAMESPACE

class RunLoop : public PointerType<RunLoop>
{
public:
    enum class EPUB3_EXPORT ExitReason : uint8_t
    {
        RunFinished         = 1,    ///< The RunLoop has no timers or event sources to process.
        RunStopped          = 2,    ///< The RunLoop was stopped by a call to RunLoop::Stop().
        RunTimedOut         = 3,    ///< The RunLoop timed out while waiting for an event or timer.
        RunHandledSource    = 4     ///< The RunLoop processed a single source and was told to return after doing so.
    };
    
    class Observer;
    class EventSource;
    class Timer;
        
    typedef std::shared_ptr<Observer>       ObserverPtr;
    typedef std::shared_ptr<EventSource>    EventSourcePtr;
    typedef std::shared_ptr<Timer>          TimerPtr;
    
protected:
    class _SourceBase
    {
    public:
        _SourceBase() {}
        virtual ~_SourceBase() {}
    };
    typedef std::shared_ptr<_SourceBase>    _SourceBasePtr;
    
public:
    class Observer : public PointerType<Observer>
    {
    public:
        ///
        /// A bitfield type, used to specify the activities to observe.
        typedef uint32_t Activity;
        ///
        /// The different Activity flags.
        enum ActivityFlags : uint32_t
        {
            RunLoopEntry            = (1U << 0),    ///< Observe entry to the run loop.
            RunLoopBeforeTimers     = (1U << 1),    ///< Fires before any timers are processed.
            RunLoopBeforeSources    = (1U << 2),    ///< Fires before any event sources are processed.
            RunLoopBeforeWaiting    = (1U << 5),    ///< Fires before the RunLoop waits for events/timers to fire.
            RunLoopAfterWaiting     = (1U << 6),    ///< Fires when the RunLoop finishes waiting (event/timer/timeout).
            RunLoopExit             = (1U << 7),    ///< Fired when RunLoop::Run(bool, std::chrono::duration) is about to return.
            RunLoopAllActivities    = 0x0FFFFFFFU
        };
        
        ///
        /// The type of function invoked by an observer.
        typedef std::function<void(Observer&, Activity)> ObserverFn;
        
    private:
#if EPUB_USE(CF)
        CFRefCounted<CFRunLoopObserverRef>  _cf;        ///< The underlying CF type of the observer.
#else
        ObserverFn              _fn;        ///< The observer callback function.
        Activity                _acts;      ///< The activities to apply.
        bool                    _repeats;   ///< Whether the observer handles multiple events.
        bool                    _cancelled; ///< Whether the observer is cancelled.
#endif
        
        friend class RunLoop;

                                Observer()                  _DELETED_;
        
    public:
        /**
         Creates a RunLoop observer.
         @param activities The ActivityFlags defining the activities to observe.
         @param repeats Whether the observer should fire more than once.
         @param fn The function to call when an observed activity occurs.
         */
        EPUB3_EXPORT
        Observer(Activity activities, bool repeats, ObserverFn fn);
        ///
        /// Copy constructor
        EPUB3_EXPORT
        Observer(const Observer&);
        ///
        /// Move constructor
        EPUB3_EXPORT
        Observer(Observer&& o);

        EPUB3_EXPORT
        ~Observer();

        EPUB3_EXPORT
        Observer&       operator=(const Observer&);
        ///
        /// Move assignment
        EPUB3_EXPORT
        Observer&       operator=(Observer&&o);
        
        ///
        /// Test for equality with another observer
        EPUB3_EXPORT
        bool            operator==(const Observer&)     const;
        bool            operator!=(const Observer&o)    const   { return this->operator==(o) == false; }
        
        ///
        /// Retrieves the activities monitored by this observer.
        EPUB3_EXPORT
        Activity        GetActivities()                 const;
        ///
        /// Whether this observer will post multiple events.
        EPUB3_EXPORT
        bool            Repeats()                       const;
        ///
        /// Whether the observer has been cancelled.
        EPUB3_EXPORT
        bool            IsCancelled()                   const;
        
        ///
        /// Cancels the observer, causing it never to fire again.
        EPUB3_EXPORT
        void            Cancel();
    };
    
public:
    class EventSource : public PointerType<EventSource>, public _SourceBase
    {
    public:
        typedef std::function<void(EventSource&)>   EventHandlerFn;
        
    private:
#if EPUB_USE(CF)
        CFRefCounted<CFRunLoopSourceRef>    _cf;    ///< The underlying CF type of the event source.
        std::map<CFRunLoopRef, int>         _rl;    ///< The CFRunLoops with which this CF source is registered.
#elif EPUB_OS(ANDROID)
        int                                 _evt[2];    ///< The event's pipe file descriptors.
#elif EPUB_OS(WINDOWS)
        HANDLE                              _event;
		std::vector<std::weak_ptr<RunLoop>>	_runLoops;
#else
        std::atomic<bool>                   _signalled; ///< Whether the source has been signalled.
        bool                                _cancelled; ///< Whether the source is cancelled.
#endif
        
        EventHandlerFn              _fn;    ///< The function to invoke when the event fires.
        
        friend class RunLoop;
        
                        EventSource()                   _DELETED_;
        
    public:
        EPUB3_EXPORT    EventSource(EventHandlerFn fn);
        EPUB3_EXPORT    EventSource(const EventSource& o);
        EPUB3_EXPORT    EventSource(EventSource&& o);
        EPUB3_EXPORT    ~EventSource();
        
        ///
        /// Copy assignment
        EPUB3_EXPORT
        EventSource&    operator=(const EventSource&);
        ///
        /// Move assignment
        EPUB3_EXPORT
        EventSource&    operator=(EventSource&&);
        
        ///
        /// Test for equality.
        EPUB3_EXPORT
        bool            operator==(const EventSource&)  const;
        bool            operator!=(const EventSource&o) const   { return this->operator==(o) == false; }
        
        ///
        /// Whether the event source has been cancelled.
        EPUB3_EXPORT
        bool            IsCancelled()                   const;
        
        ///
        /// Cancel the event source, so it will never fire again.
        EPUB3_EXPORT
        void            Cancel();
        
        ///
        /// Signal the event source, causing it to fire on one of its associated RunLoops.
        EPUB3_EXPORT
        void            Signal();
        
    protected:
#if EPUB_USE(CF)
        static void _FireCFSourceEvent(void* __i);
        static void _ScheduleCF(void*, CFRunLoopRef, CFStringRef);
        static void _CancelCF(void*, CFRunLoopRef, CFStringRef);
#endif
        
    };
    
    class Timer : public PointerType<Timer>, public _SourceBase
    {
    public:
        typedef std::function<void(Timer&)>  TimerFn;
        
        ///
        /// Timers always use the system clock.
#if EPUB_COMPILER_SUPPORTS(CXX_ALIAS_TEMPLATES)
# if EPUB_USE(CF)
        using Clock = cf_clock;
# else
        using Clock = std::chrono::system_clock;
# endif
#else
# if EPUB_USE(CF)
        typedef cf_clock Clock;
# else
        typedef std::chrono::system_clock Clock;
# endif
#endif
        
    private:
#if EPUB_USE(CF)
        CFRefCounted<CFRunLoopTimerRef>		_cf;        ///< The underlying CF type of the timer.
#elif EPUB_OS(ANDROID)
        timer_t								_timer;     ///< The underlying Linux timer.
        int									_pipeFDs[2];///< The pipe endpoints used with ALooper.
        TimerFn								_fn;        ///< The function to call when the timer fires.
#elif EPUB_OS(WINDOWS)
#if EPUB_PLATFORM(WINRT)
		ThreadPoolTimer^					_timer;
#endif
		HANDLE								_handle;
		std::vector<std::weak_ptr<RunLoop>>	_runLoops;
        Clock::time_point					_fireDate;
        Clock::duration						_interval;
        TimerFn								_fn;
        bool								_cancelled;
#else
        Clock::time_point					_fireDate;  ///< The date at which the timer will fire.
        TimerFn								_fn;        ///< The function to call when the timer fires.
        Clock::duration						_interval;  ///< The interval at which the timer repeats (if any)
        bool								_cancelled; ///< Set to `true` when the timer is cancelled.
#endif
        
        friend class RunLoop;
        
        ///
        /// No default constructor.
                        Timer()                 _DELETED_;
        
    protected:
        EPUB3_EXPORT
        Timer(Clock::time_point& fireDate, Clock::duration& interval, TimerFn fn);
        EPUB3_EXPORT
        Timer(Clock::duration& interval, bool repeat, TimerFn fn);
        
    public:
        /**
         Create a timer with an absolute fire date.
         @param fireDate The time/date at which the timer should first fire.
         @param interval The repeat interval. Pass `0` for a non-repeating timer.
         @param fn The function to call whenever the timer fires.
         */
        template <class _Rep>
        Timer(std::chrono::time_point<Clock, _Rep>& fireDate,
              std::chrono::duration<_Rep>& interval,
              TimerFn fn) : Timer(std::chrono::time_point_cast<Clock::duration>(fireDate), std::chrono::duration_cast<Clock::duration>(interval), fn) {}
        
        /**
         Create a timer with a relative fire date.
         @param interval The interval after which the timer should fire.
         @param repeat Whether the timer should fire multiple times.
         @param fn The function to call whenever the timer fires.
         */
        template <class _Rep>
        Timer(std::chrono::duration<_Rep>& interval,
              bool repeat, TimerFn fn) : Timer(std::chrono::duration_cast<Clock::duration>(interval), repeat, fn) {}
        
        ///
        /// Copy constructor
        EPUB3_EXPORT    Timer(const Timer& o);
        
        ///
        /// Move constructor
        EPUB3_EXPORT    Timer(Timer&& o);

        EPUB3_EXPORT    ~Timer();
        
        ///
        /// Copy assignment
        EPUB3_EXPORT
        Timer&          operator=(const Timer&);
        ///
        /// Move assignment
        EPUB3_EXPORT
        Timer&          operator=(Timer&&);
        
        ///
        /// Test for equality
        EPUB3_EXPORT
        bool            operator==(const Timer&) const;
        bool            operator!=(const Timer&o) const { return this->operator==(o) == false; }
        
        ///
        /// Cancels the timer, causing it to never fire again.
        EPUB3_EXPORT
        void            Cancel();
        ///
        /// Tests whether a timer has been cancelled.
        EPUB3_EXPORT
        bool            IsCancelled()   const;
        
        ///
        /// Tests whether a timer is set to repeat.
        EPUB3_EXPORT
        bool            Repeats()       const;
        
        ///
        /// Retrieves the repeat interval of a timer.
        template <class _Rep, class _Period _DEFAULT_(std::ratio<1>)>
        std::chrono::duration<_Rep, _Period>    RepeatInterval()    const {
            using namespace std::chrono;
            return duration_cast<decltype(RepeatInterval<_Rep,_Period>())>(RepeatIntervalInternal());
        }
        
        ///
        /// Retrieves the date at which the timer will next fire.
        template <class _Duration _DEFAULT_(typename Clock::duration)>
        std::chrono::time_point<Clock, _Duration>  GetNextFireDate()   const {
            using namespace std::chrono;
            return time_point_cast<_Duration>(GetNextFireDateTime());
        }
        
        ///
        /// Sets the date at whech the timer will next fire.
        template <class _Duration _DEFAULT_(typename Clock::duration)>
        void            SetNextFireDate(std::chrono::time_point<Clock, _Duration>& when) {
            using namespace std::chrono;
            Clock::time_point __t = time_point_cast<Clock::duration>(when);
            return SetNextFireDateTime(__t);
        }
        
        ///
        /// Retrieves the timer's next fire date as an interval from the current time.
        template <class _Rep, class _Period _DEFAULT_(std::ratio<1>)>
        std::chrono::duration<_Rep, _Period>    GetNextFireDate()   const {
            using namespace std::chrono;
            return duration_cast<_Rep>(GetNextFireDateDuration());
        }
        
        ///
        /// Sets the timer's next fire date using a relative time interval (from now).
        template <class _Rep, class _Period _DEFAULT_(std::ratio<1>)>
        void            SetNextFireDate(std::chrono::duration<_Rep, _Period>& when) {
            using namespace std::chrono;
            Clock::duration __d = duration_cast<Clock::duration>(when);
            return SetNextFireDateDuration(__d);
        }
        
    protected:
        EPUB3_EXPORT
        Clock::duration RepeatIntervalInternal() const;

        EPUB3_EXPORT
        Clock::time_point GetNextFireDateTime() const;
        EPUB3_EXPORT
        void SetNextFireDateTime(Clock::time_point& when);

        EPUB3_EXPORT
        Clock::duration GetNextFireDateDuration() const;
        EPUB3_EXPORT
        void SetNextFireDateDuration(Clock::duration& when);
    };
    
public:
    ///
    /// This is the only way to obtain a RunLoop. Use it wisely.
    EPUB3_EXPORT
    static RunLoopPtr CurrentRunLoop();

    EPUB3_EXPORT    ~RunLoop();
    
    ///
    /// Call a function on the run loop's assigned thread.
    EPUB3_EXPORT
    void            PerformFunction(std::function<void()> fn);
    
    ///
    /// Adds a timer to the run loop.
    EPUB3_EXPORT
    void            AddTimer(TimerPtr timer);
    ///
    /// Whether a timer is registered on this runloop.
    EPUB3_EXPORT
    bool            ContainsTimer(TimerPtr timer)            const;
    ///
    /// Removes the timer from this RunLoop (without cancelling it).
    EPUB3_EXPORT
    void            RemoveTimer(TimerPtr timer);
    
    ///
    /// Adds an event source to the run loop.
    EPUB3_EXPORT
    void            AddEventSource(EventSourcePtr source);
    ///
    /// Whether an event source is registered on this runloop.
    EPUB3_EXPORT
    bool            ContainsEventSource(EventSourcePtr source)  const;
    ///
    /// Removes an event source from this RunLoop (without cancelling it).
    EPUB3_EXPORT
    void            RemoveEventSource(EventSourcePtr source);
    
    ///
    /// Adds an observer to the run loop.
    EPUB3_EXPORT
    void            AddObserver(ObserverPtr observer);
    ///
    /// Whether an observer is registered on this runloop.
    EPUB3_EXPORT
    bool            ContainsObserver(ObserverPtr observer)      const;
    ///
    /// Removes an observer from this RunLoop (without cancelling it).
    EPUB3_EXPORT
    void            RemoveObserver(ObserverPtr observer);
    
    /**
     Run the RunLoop, either indefinitely, for a specific duration, and/or until an event occurs.
     @param returnAfterSourceHandled Return from this method after a single
     EventSource has fired. Note that this *only* applies to EventSources-- Timers
     will not cause this method to return.
     @param timeout The maximum amount of time to run the run loop as a result of this
     invocation. If `0` or less, this will poll the RunLoop exactly once, firing any
     pending timers and a miximum of one event source.
     @result Returns a value defining the reason that the method returned.
     */
    template <class _Rep _DEFAULT_(long long), class _Period _DEFAULT_(std::ratio<1>)>
    ExitReason      Run(bool returnAfterSourceHandled,
                        std::chrono::duration<_Rep,_Period> timeout) {
        using namespace std::chrono;
        nanoseconds ns = duration_cast<nanoseconds>(timeout);
        return RunInternal(returnAfterSourceHandled, ns);
    }
    
    ///
    /// Runs the RunLoop forever, or until Stop() is called.
    EPUB3_EXPORT
    void            Run();
    
    ///
    /// Stops the RunLoop, exiting any invocations of Run() or Run(bool, std::chrono::duration).
    EPUB3_EXPORT
    void            Stop();
    
    ///
    /// Whether the RunLoop is currently waiting for an event or timer to fire.
    EPUB3_EXPORT
    bool            IsWaiting()                                     const;
    
    ///
    /// Explicitly wake the RunLoop, causing it to check timers and event sources.
    EPUB3_EXPORT
    void            WakeUp();
    
# if EPUB_OS(WINDOWS) && EPUB_PLATFORM(WINRT)
	static void InitRunLoopTLSKey();
	static void KillRunLoopTLSKey();
# endif

protected:
    ///
    /// Internal Run function which takes an explicit timeout duration type.
    EPUB3_EXPORT
    ExitReason      RunInternal(bool returnAfterSourceHandled, std::chrono::nanoseconds& timeout);
    
    ///
    /// Obtains the run loop for the current thread.
    EPUB3_EXPORT    RunLoop();

private:
    ///
    /// No copy constructor
                    RunLoop(const RunLoop& o) _DELETED_;
    ///
    /// No move constructor
                    RunLoop(RunLoop&& o) _DELETED_;
    
#if EPUB_OS(ANDROID)
    static int      _ReceiveLoopEvent(int fd, int events, void* data);
#endif
#if !EPUB_USE(CF)
    ///
    /// Runs all observers matching the given activity
    void            RunObservers(Observer::Activity activity);
#endif
    
#if !EPUB_OS(ANDROID) && !EPUB_OS(WINDOWS) && !EPUB_USE(CF)
    ///
    /// Collects all timers ready to fire
    shared_vector<Timer>        CollectFiringTimers();
    ///
    /// Collects all sources that have been signalled
    shared_vector<EventSource>  CollectFiringSources(bool onlyOne);
    ///
    /// If a timer will fire before the given timeout, returns a new timeout
    std::chrono::system_clock::time_point   TimeoutOrTimer(std::chrono::system_clock::time_point& timeout);
#elif EPUB_OS(WINDOWS)
    ///
    /// Process a firing timer
    void            ProcessTimer(TimerPtr timer);
    ///
    /// Process a firing event source
    void            ProcessEventSource(EventSourcePtr source);
#endif
    
private:
#if EPUB_USE(CF)
    CFRunLoopRef        _cf;            ///< The underlying CF type of the run loop.
#elif EPUB_OS(ANDROID)
    ALooper*            _looper;        ///< The Android NDK event loop construct.
    int                 _wakeFDs[2];    ///< The pipe for waking/stopping/killing the runloop.

    std::recursive_mutex            _listLock;
    
    typedef std::map<int,_SourceBasePtr> SourceMap_t;
    SourceMap_t         _handlers;      ///< Maps event fds to their owners.
    
    shared_list<Observer>           _observers;
    Observer::Activity              _observerMask;
    std::atomic<bool>               _waiting;
#elif EPUB_OS(WINDOWS)
    HANDLE                              _wakeHandle;
    std::map<HANDLE, TimerPtr>          _timers;
    std::map<HANDLE, EventSourcePtr>    _sources;
    shared_list<Observer>               _observers;
    std::recursive_mutex                _listLock;
    std::atomic<bool>                   _waiting;
    std::atomic<bool>                   _stop;
    std::atomic<bool>                   _resetHandles;
    Observer::Activity                  _observerMask;


# if EPUB_PLATFORM(WINRT)
	static DWORD RunLoopTLSKey;
# endif
#else
    shared_list<Timer>                  _timers;
    shared_list<Observer>               _observers;
    shared_list<EventSource>            _sources;
    std::recursive_mutex                _listLock;
    std::mutex                          _conditionLock;
    std::condition_variable             _wakeUp;
    std::atomic<bool>                   _waiting;
    std::atomic<bool>                   _stop;
    Observer::Activity                  _observerMask;
    const TimerPtr                      _waitingUntilTimer;
#endif

};

EPUB3_END_NAMESPACE

#endif //FUTURE_ENABLED

#endif /* defined(__ePub3__run_loop__) */
