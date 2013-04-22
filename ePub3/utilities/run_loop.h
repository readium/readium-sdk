//
//  run_loop.h
//  ePub3
//
//  Created by Jim Dovey on 2013-04-08.
//  Copyright (c) 2013 The Readium Foundation and contributors. All rights reserved.
//

#ifndef __ePub3__run_loop__
#define __ePub3__run_loop__

#include "basic.h"
#include "utfstring.h"
#include <chrono>
#include <list>
#include <mutex>
#include <atomic>
#include "ref_counted.h"

//#undef EPUB_USE_CF
//#define EPUB_OS_ANDROID 1

#if EPUB_USE(CF)
# include <CoreFoundation/CoreFoundation.h>
#elif EPUB_OS(ANDROID)
# include <time.h>
struct ALooper;
#elif EPUB_OS(WINDOWS)
# error Add Windows code to RunLoop.h
#else
# include <condition_variable>      // GNU libstdc++ 4.7 has this guy in a separate header
# include <pthread.h>
# include <time.h>
#endif

#if EPUB_USE(CF)
#include "cf_helpers.h"
#endif

EPUB3_BEGIN_NAMESPACE

class RunLoop
{
public:
    enum class ExitReason : uint8_t
    {
        RunFinished         = 1,    ///< The RunLoop has no timers or event sources to process.
        RunStopped          = 2,    ///< The RunLoop was stopped by a call to RunLoop::Stop().
        RunTimedOut         = 3,    ///< The RunLoop timed out while waiting for an event or timer.
        RunHandledSource    = 4     ///< The RunLoop processed a single source and was told to return after doing so.
    };
    
    class Observer : public RefCountable
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
#elif EPUB_OS(WINDOWS)
#error No Windows RunLoop implementation defined
#else
        ObserverFn              _fn;        ///< The observer callback function.
        Activity                _acts;      ///< The activities to apply.
        bool                    _repeats;   ///< Whether the observer handles multiple events.
        bool                    _cancelled; ///< Whether the observer is cancelled.
#endif
        
        friend class RunLoop;
        
                    Observer()                  = delete;
        
    public:
        /**
         Creates a RunLoop observer.
         @param activities The ActivityFlags defining the activities to observe.
         @param repeats Whether the observer should fire more than once.
         @param fn The function to call when an observed activity occurs.
         */
        Observer(Activity activities, bool repeats, ObserverFn fn);
        ///
        /// Copy constructor
        Observer(const Observer&);
        ///
        /// Move constructor
        Observer(Observer&& o);
        
        ~Observer();
        
        Observer&       operator=(const Observer&);
        ///
        /// Move assignment
        Observer&       operator=(Observer&&o);
        
        ///
        /// Test for equality with another observer
        bool            operator==(const Observer&)     const;
        bool            operator!=(const Observer&o)    const   { return this->operator==(o) == false; }
        
        ///
        /// Retrieves the activities monitored by this observer.
        Activity        GetActivities()                 const;
        ///
        /// Whether this observer will post multiple events.
        bool            Repeats()                       const;
        ///
        /// Whether the observer has been cancelled.
        bool            IsCancelled()                   const;
        
        ///
        /// Cancels the observer, causing it never to fire again.
        void            Cancel();
    };
    
    class EventSource : public RefCountable
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
#error No Windows RunLoop implementation defined
#else
        std::atomic<bool>                   _signalled; ///< Whether the source has been signalled.
        bool                                _cancelled; ///< Whether the source is cancelled.
#endif
        
        EventHandlerFn              _fn;    ///< The function to invoke when the event fires.
        
        friend class RunLoop;
        
                        EventSource()                   = delete;
        
    public:
                        EventSource(EventHandlerFn fn);
                        EventSource(const EventSource& o);
                        EventSource(EventSource&& o);
                        ~EventSource();
        
        ///
        /// Copy assignment
        EventSource&    operator=(const EventSource&);
        ///
        /// Move assignment
        EventSource&    operator=(EventSource&&);
        
        ///
        /// Test for equality.
        bool            operator==(const EventSource&)  const;
        bool            operator!=(const EventSource&o) const   { return this->operator==(o) == false; }
        
        ///
        /// Whether the event source has been cancelled.
        bool            IsCancelled()                   const;
        
        ///
        /// Cancel the event source, so it will never fire again.
        void            Cancel();
        
        ///
        /// Signal the event source, causing it to fire on one of its associated RunLoops.
        void            Signal();
        
    protected:
#if EPUB_USE(CF)
        static void _FireCFSourceEvent(void* __i);
        static void _ScheduleCF(void*, CFRunLoopRef, CFStringRef);
        static void _CancelCF(void*, CFRunLoopRef, CFStringRef);
#endif
        
    };
    
    class Timer : public RefCountable
    {
    public:
        typedef std::function<void(Timer&)>  TimerFn;
        
        ///
        /// Timers always use the system clock.
#if EPUB_USE(CF)
        using Clock = cf_clock;
#else
        using Clock = std::chrono::system_clock;
#endif
        
    private:
#if EPUB_USE(CF)
        CFRefCounted<CFRunLoopTimerRef> _cf;        ///< The underlying CF type of the timer.
#elif EPUB_OS(ANDROID)
        timer_t                         _timer;     ///< The underlying Linux timer.
        int                             _pipeFDs[2];///< The pipe endpoints used with ALooper.
        TimerFn                         _fn;        ///< The function to call when the timer fires.
#elif EPUB_OS(WINDOWS)
#error No Windows RunLoop implementation defined
#else
        Clock::time_point               _fireDate;  ///< The date at which the timer will fire.
        TimerFn                         _fn;        ///< The function to call when the timer fires.
        Clock::duration                 _interval;  ///< The interval at which the timer repeats (if any)
        bool                            _cancelled; ///< Set to `true` when the timer is cancelled.
#endif
        
        friend class RunLoop;
        
        ///
        /// No default constructor.
                        Timer()                 = delete;
        
    protected:
        Timer(Clock::time_point& fireDate, Clock::duration& interval, TimerFn fn);
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
                        Timer(const Timer& o);
        
        ///
        /// Move constructor
                        Timer(Timer&& o);
        
                        ~Timer();
        
        ///
        /// Copy assignment
        Timer&          operator=(const Timer&);
        ///
        /// Move assignment
        Timer&          operator=(Timer&&);
        
        ///
        /// Test for equality
        bool            operator==(const Timer&) const;
        bool            operator!=(const Timer&o) const { return this->operator==(o) == false; }
        
        ///
        /// Cancels the timer, causing it to never fire again.
        void            Cancel();
        ///
        /// Tests whether a timer has been cancelled.
        bool            IsCancelled()   const;
        
        ///
        /// Tests whether a timer is set to repeat.
        bool            Repeats()       const;
        
        ///
        /// Retrieves the repeat interval of a timer.
        template <class _Rep, class _Period = std::ratio<1>>
        std::chrono::duration<_Rep, _Period>    RepeatInterval()    const {
            using namespace std::chrono;
            return duration_cast<decltype(RepeatInterval<_Rep,_Period>())>(RepeatIntervalInternal());
        }
        
        ///
        /// Retrieves the date at which the timer will next fire.
        template <class _Duration = typename Clock::duration>
        std::chrono::time_point<Clock, _Duration>  GetNextFireDate()   const {
            using namespace std::chrono;
            return time_point_cast<_Duration>(GetNextFireDateTime());
        }
        
        ///
        /// Sets the date at whech the timer will next fire.
        template <class _Duration = typename Clock::duration>
        void            SetNextFireDate(std::chrono::time_point<Clock, _Duration>& when) {
            using namespace std::chrono;
            Clock::time_point __t = time_point_cast<Clock::duration>(when);
            return SetNextFireDateTime(__t);
        }
        
        ///
        /// Retrieves the timer's next fire date as an interval from the current time.
        template <class _Rep, class _Period = std::ratio<1>>
        std::chrono::duration<_Rep, _Period>    GetNextFireDate()   const {
            using namespace std::chrono;
            return duration_cast<_Rep>(GetNextFireDateDuration());
        }
        
        ///
        /// Sets the timer's next fire date using a relative time interval (from now).
        template <class _Rep, class _Period = std::ratio<1>>
        void            SetNextFireDate(std::chrono::duration<_Rep, _Period>& when) {
            using namespace std::chrono;
            Clock::duration __d = duration_cast<Clock::duration>(when);
            return SetNextFireDateDuration(__d);
        }
        
    protected:
        Clock::duration RepeatIntervalInternal() const;
        
        Clock::time_point GetNextFireDateTime() const;
        void SetNextFireDateTime(Clock::time_point& when);
        
        Clock::duration GetNextFireDateDuration() const;
        void SetNextFireDateDuration(Clock::duration& when);
    };
    
public:
    ///
    /// This is the only way to obtain a RunLoop. Use it wisely.
    static RunLoop* CurrentRunLoop();
    
                    ~RunLoop();
    
    ///
    /// Call a function on the run loop's assigned thread.
    void            PerformFunction(std::function<void()> fn);
    
    ///
    /// Adds a timer to the run loop.
    void            AddTimer(Timer* timer);
    ///
    /// Whether a timer is registered on this runloop.
    bool            ContainsTimer(Timer* timer)               const;
    ///
    /// Removes the timer from this RunLoop (without cancelling it).
    void            RemoveTimer(Timer* timer);
    
    ///
    /// Adds an event source to the run loop.
    void            AddEventSource(EventSource* source);
    ///
    /// Whether an event source is registered on this runloop.
    bool            ContainsEventSource(EventSource* source)  const;
    ///
    /// Removes an event source from this RunLoop (without cancelling it).
    void            RemoveEventSource(EventSource* source);
    
    ///
    /// Adds an observer to the run loop.
    void            AddObserver(Observer* observer);
    ///
    /// Whether an observer is registered on this runloop.
    bool            ContainsObserver(Observer* observer)      const;
    ///
    /// Removes an observer from this RunLoop (without cancelling it).
    void            RemoveObserver(Observer* observer);
    
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
    template <class _Rep = long long, class _Period = std::ratio<1>>
    ExitReason      Run(bool returnAfterSourceHandled,
                        std::chrono::duration<_Rep,_Period> timeout) {
        using namespace std::chrono;
        nanoseconds ns = duration_cast<nanoseconds>(timeout);
        return RunInternal(returnAfterSourceHandled, ns);
    }
    
    ///
    /// Runs the RunLoop forever, or until Stop() is called.
    void            Run();
    
    ///
    /// Stops the RunLoop, exiting any invocations of Run() or Run(bool, std::chrono::duration).
    void            Stop();
    
    ///
    /// Whether the RunLoop is currently waiting for an event or timer to fire.
    bool            IsWaiting()                                     const;
    
    ///
    /// Explicitly wake the RunLoop, causing it to check timers and event sources.
    void            WakeUp();
    
protected:
    ///
    /// Internal Run function which takes an explicit timeout duration type.
    ExitReason      RunInternal(bool returnAfterSourceHandled, std::chrono::nanoseconds& timeout);
    
    
    ///
    /// Obtains the run loop for the current thread.
                    RunLoop();
    
private:
    ///
    /// No copy constructor
                    RunLoop(const RunLoop& o) = delete;
    ///
    /// No move constructor
                    RunLoop(RunLoop&& o) = delete;
    
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
    std::vector<Timer*>         CollectFiringTimers();
    ///
    /// Collects all sources that have been signalled
    std::vector<EventSource*>   CollectFiringSources(bool onlyOne);
    ///
    /// If a timer will fire before the given timeout, returns a new timeout
    std::chrono::system_clock::time_point   TimeoutOrTimer(std::chrono::system_clock::time_point& timeout);
#endif
    
private:
#if EPUB_USE(CF)
    CFRunLoopRef        _cf;            ///< The underlying CF type of the run loop.
#elif EPUB_OS(ANDROID)
    ALooper*            _looper;        ///< The Android NDK event loop construct.
    int                 _wakeFDs[2];    ///< The pipe for waking/stopping/killing the runloop.

    std::recursive_mutex            _listLock;
    
    typedef std::map<int,RefCounted<RefCountable>> SourceMap_t;
    SourceMap_t         _handlers;      ///< Maps event fds to their owners.
    
    std::list<RefCounted<Observer>> _observers;
    Observer::Activity              _observerMask;
    std::atomic<bool>               _waiting;
#elif EPUB_OS(WINDOWS)
#error No Windows RunLoop implementation defined
#else
    std::list<RefCounted<Timer>>        _timers;
    std::list<RefCounted<Observer>>     _observers;
    std::list<RefCounted<EventSource>>  _sources;
    std::recursive_mutex                _listLock;
    std::mutex                          _conditionLock;
    std::condition_variable             _wakeUp;
    std::atomic<bool>                   _waiting;
    std::atomic<bool>                   _stop;
    Observer::Activity                  _observerMask;
    const Timer*                        _waitingUntilTimer;
#endif

};

EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__run_loop__) */
