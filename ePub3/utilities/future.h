//
//  future.h
//  ePub3
//
//  Created by Jim Dovey on 11/12/2013.
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

#ifndef ePub3_future_h
#define ePub3_future_h

#include <ePub3/base.h>
#include <list>
#include <string>
#include <stdexcept>
#include <exception>
#include <system_error>
#include <thread>
#include <memory>
#include <atomic>
#include <ePub3/utilities/invoke.h>
#include <ePub3/utilities/executor.h>
#include <ePub3/utilities/condition_variable_any.h>


/*
    future synopsis

namespace std
{

enum class future_errc
{
    broken_promise,
    future_already_retrieved,
    promise_already_satisfied,
    no_state
};

enum class launch
{
    async = 1,
    deferred = 2,
    any = async | deferred
};

enum class future_status
{
    ready,
    timeout,
    deferred
};

template <> struct is_error_code_enum<future_errc> : public true_type { };
error_code make_error_code(future_errc e) _NOEXCEPT;
error_condition make_error_condition(future_errc e) _NOEXCEPT;

const error_category& future_category() _NOEXCEPT;

class future_error
    : public logic_error
{
public:
    future_error(error_code ec);  // exposition only

    const error_code& code() const _NOEXCEPT;
    const char*       what() const _NOEXCEPT;
};

template <class R>
class promise
{
public:
    promise();
    template <class Allocator>
        promise(allocator_arg_t, const Allocator& a);
    promise(promise&& rhs) _NOEXCEPT;
    promise(const promise& rhs) = delete;
    ~promise();

    // assignment
    promise& operator=(promise&& rhs) _NOEXCEPT;
    promise& operator=(const promise& rhs) = delete;
    void swap(promise& other) _NOEXCEPT;

    // retrieving the result
    future<R> get_future();

    // setting the result
    void set_value(const R& r);
    void set_value(R&& r);
    void set_exception(exception_ptr p);

    // setting the result with deferred notification
    void set_value_at_thread_exit(const R& r);
    void set_value_at_thread_exit(R&& r);
    void set_exception_at_thread_exit(exception_ptr p);
};

template <class R>
class promise<R&>
{
public:
    promise();
    template <class Allocator>
        promise(allocator_arg_t, const Allocator& a);
    promise(promise&& rhs) _NOEXCEPT;
    promise(const promise& rhs) = delete;
    ~promise();

    // assignment
    promise& operator=(promise&& rhs) _NOEXCEPT;
    promise& operator=(const promise& rhs) = delete;
    void swap(promise& other) _NOEXCEPT;

    // retrieving the result
    future<R&> get_future();

    // setting the result
    void set_value(R& r);
    void set_exception(exception_ptr p);

    // setting the result with deferred notification
    void set_value_at_thread_exit(R&);
    void set_exception_at_thread_exit(exception_ptr p);
};

template <>
class promise<void>
{
public:
    promise();
    template <class Allocator>
        promise(allocator_arg_t, const Allocator& a);
    promise(promise&& rhs) _NOEXCEPT;
    promise(const promise& rhs) = delete;
    ~promise();

    // assignment
    promise& operator=(promise&& rhs) _NOEXCEPT;
    promise& operator=(const promise& rhs) = delete;
    void swap(promise& other) _NOEXCEPT;

    // retrieving the result
    future<void> get_future();

    // setting the result
    void set_value();
    void set_exception(exception_ptr p);

    // setting the result with deferred notification
    void set_value_at_thread_exit();
    void set_exception_at_thread_exit(exception_ptr p);
};

template <class R> void swap(promise<R>& x, promise<R>& y) _NOEXCEPT;

template <class R, class Alloc>
    struct uses_allocator<promise<R>, Alloc> : public true_type {};

template <class R>
class future
{
public:
    future() _NOEXCEPT;
    future(future&&) _NOEXCEPT;
    future(future<future<R>>&& rhs) _NOEXCEPT;
    future(const future& rhs) = delete;
    ~future();
    future& operator=(const future& rhs) = delete;
    future& operator=(future&&) _NOEXCEPT;
    shared_future<R> share();

    // retrieving the value
    R get();

    // functions to check state
    bool valid() const _NOEXCEPT;

    void wait() const;
    template <class Rep, class Period>
        future_status
        wait_for(const chrono::duration<Rep, Period>& rel_time) const;
    template <class Clock, class Duration>
        future_status
        wait_until(const chrono::time_point<Clock, Duration>& abs_time) const;
 
    bool ready() const;
 
    template<typename F>
    auto then(F&& func) -> future<decltype(func(*this))>;
    template<typename F>
    auto then(executor &ex, F&& func) -> future<decltype(func(*this))>;
    template<typename F>
    auto then(launch policy, F&& func) -> future<decltype(func(*this))>;
 
    see below unwrap();
};

template <class R>
class future<R&>
{
public:
    future() _NOEXCEPT;
    future(future&&) _NOEXCEPT;
    future(const future& rhs) = delete;
    ~future();
    future& operator=(const future& rhs) = delete;
    future& operator=(future&&) _NOEXCEPT;
    shared_future<R&> share();

    // retrieving the value
    R& get();

    // functions to check state
    bool valid() const _NOEXCEPT;

    void wait() const;
    template <class Rep, class Period>
        future_status
        wait_for(const chrono::duration<Rep, Period>& rel_time) const;
    template <class Clock, class Duration>
        future_status
        wait_until(const chrono::time_point<Clock, Duration>& abs_time) const;
};

template <>
class future<void>
{
public:
    future() _NOEXCEPT;
    future(future&&) _NOEXCEPT;
    future(const future& rhs) = delete;
    ~future();
    future& operator=(const future& rhs) = delete;
    future& operator=(future&&) _NOEXCEPT;
    shared_future<void> share();

    // retrieving the value
    void get();

    // functions to check state
    bool valid() const _NOEXCEPT;

    void wait() const;
    template <class Rep, class Period>
        future_status
        wait_for(const chrono::duration<Rep, Period>& rel_time) const;
    template <class Clock, class Duration>
        future_status
        wait_until(const chrono::time_point<Clock, Duration>& abs_time) const;
};

template <class R>
class shared_future
{
public:
    shared_future() _NOEXCEPT;
    shared_future(const shared_future& rhs);
    shared_future(future<R>&&) _NOEXCEPT;
    shared_future(shared_future&& rhs) _NOEXCEPT;
    ~shared_future();
    shared_future& operator=(const shared_future& rhs);
    shared_future& operator=(shared_future&& rhs) _NOEXCEPT;

    // retrieving the value
    const R& get() const;

    // functions to check state
    bool valid() const _NOEXCEPT;

    void wait() const;
    template <class Rep, class Period>
        future_status
        wait_for(const chrono::duration<Rep, Period>& rel_time) const;
    template <class Clock, class Duration>
        future_status
        wait_until(const chrono::time_point<Clock, Duration>& abs_time) const;
};

template <class R>
class shared_future<R&>
{
public:
    shared_future() _NOEXCEPT;
    shared_future(const shared_future& rhs);
    shared_future(future<R&>&&) _NOEXCEPT;
    shared_future(shared_future&& rhs) _NOEXCEPT;
    ~shared_future();
    shared_future& operator=(const shared_future& rhs);
    shared_future& operator=(shared_future&& rhs) _NOEXCEPT;

    // retrieving the value
    R& get() const;

    // functions to check state
    bool valid() const _NOEXCEPT;

    void wait() const;
    template <class Rep, class Period>
        future_status
        wait_for(const chrono::duration<Rep, Period>& rel_time) const;
    template <class Clock, class Duration>
        future_status
        wait_until(const chrono::time_point<Clock, Duration>& abs_time) const;
};

template <>
class shared_future<void>
{
public:
    shared_future() _NOEXCEPT;
    shared_future(const shared_future& rhs);
    shared_future(future<void>&&) _NOEXCEPT;
    shared_future(shared_future&& rhs) _NOEXCEPT;
    ~shared_future();
    shared_future& operator=(const shared_future& rhs);
    shared_future& operator=(shared_future&& rhs) _NOEXCEPT;

    // retrieving the value
    void get() const;

    // functions to check state
    bool valid() const _NOEXCEPT;

    void wait() const;
    template <class Rep, class Period>
        future_status
        wait_for(const chrono::duration<Rep, Period>& rel_time) const;
    template <class Clock, class Duration>
        future_status
        wait_until(const chrono::time_point<Clock, Duration>& abs_time) const;
};

template <class F, class... Args>
  future<typename result_of<F(Args...)>::type>
  async(F&& f, Args&&... args);

template <class F, class... Args>
  future<typename result_of<F(Args...)>::type>
  async(launch policy, F&& f, Args&&... args);

template <class> class packaged_task; // undefined

template <class R, class... ArgTypes>
class packaged_task<R(ArgTypes...)>
{
public:
    typedef R result_type;

    // construction and destruction
    packaged_task() _NOEXCEPT;
    template <class F>
        explicit packaged_task(F&& f);
    template <class F, class Allocator>
        explicit packaged_task(allocator_arg_t, const Allocator& a, F&& f);
    ~packaged_task();

    // no copy
    packaged_task(const packaged_task&) = delete;
    packaged_task& operator=(const packaged_task&) = delete;

    // move support
    packaged_task(packaged_task&& other) _NOEXCEPT;
    packaged_task& operator=(packaged_task&& other) _NOEXCEPT;
    void swap(packaged_task& other) _NOEXCEPT;

    bool valid() const _NOEXCEPT;

    // result retrieval
    future<R> get_future();

    // execution
    void operator()(ArgTypes... );
    void make_ready_at_thread_exit(ArgTypes...);

    void reset();
};

template <class R>
  void swap(packaged_task<R(ArgTypes...)&, packaged_task<R(ArgTypes...)>&) _NOEXCEPT;

template <class R, class Alloc> struct uses_allocator<packaged_task<R>, Alloc>;

}  // std

*/

EPUB3_BEGIN_NAMESPACE

enum class future_errc
{
    broken_promise,
    future_already_retrieved,
    promise_already_satisfied,
    no_state,
    
    task_already_started
};

enum class launch
{
    none = 0,
    async = 1,
    deferred = 2,
    any = async | deferred
};

enum class future_status
{
    ready,
    timeout,
    deferred
};

const std::error_category& future_category() _NOEXCEPT;

EPUB3_END_NAMESPACE

namespace std
{
    
    template <> struct is_error_code_enum<ePub3::future_errc> : public true_type {};
    error_code make_error_code(ePub3::future_errc e) _NOEXCEPT;
    error_condition make_error_condition(ePub3::future_errc e) _NOEXCEPT;
    
}

#if EPUB_PLATFORM(ANDROID) && !defined(_LIBCPP_VERSION)
// some gubbins that doesn't appear to be implemented in GNU libstdc++
namespace std
{
    
    class __sp_mut
    {
        void* __lx;
    public:
        void lock() _NOEXCEPT;
        void unlock() _NOEXCEPT;
        
    private:
        CONSTEXPR __sp_mut(void*) _NOEXCEPT;
        __sp_mut(const __sp_mut&);
        __sp_mut& operator=(const __sp_mut&);
        
        friend __attribute__((__visibility__("default")))
            __sp_mut& __get_sp_mut(const void*);
    };
    
    __attribute__((__visibility__("default")))
    __sp_mut& __get_sp_mut(const void*);
    
    template <class _Tp>
    inline FORCE_INLINE
    bool
    atomic_is_lock_free(const shared_ptr<_Tp>*)
        {
            return false;
        }
    
    template <class _Tp>
    shared_ptr<_Tp>
    atomic_load(const shared_ptr<_Tp>* __p)
        {
            __sp_mut& __m = __get_sp_mut(__p);
            __m.lock();
            shared_ptr<_Tp> __q = *__p;
            __m.unlock();
            return __q;
        }
    
    template <class _Tp>
    inline FORCE_INLINE
    shared_ptr<_Tp>
    atomic_load_explicit(const shared_ptr<_Tp>* __p, memory_order)
        {
            return atomic_load(__p);
        }
    
    template <class _Tp>
    void
    atomic_store(shared_ptr<_Tp>* __p, shared_ptr<_Tp> __r)
        {
            __sp_mut& __m = __get_sp_mut(__p);
            __m.lock();
            __p->swap(__r);
            __m.unlock();
        }
    
    template <class _Tp>
    inline FORCE_INLINE
    void
    atomic_store_explicit(shared_ptr<_Tp>* __p, shared_ptr<_Tp> __r, memory_order)
        {
            atomic_store(__p, __r);
        }
    
    template <class _Tp>
    shared_ptr<_Tp>
    atomic_exchange(shared_ptr<_Tp>* __p, shared_ptr<_Tp> __r)
        {
            __sp_mut& __m = __get_sp_mut(__p);
            __m.lock();
            __p->swap(__r);
            __m.unlock();
            return __r;
        }
    
    template <class _Tp>
    shared_ptr<_Tp>
    atomic_exchange_explicit(shared_ptr<_Tp>* __p, shared_ptr<_Tp> __r, memory_order)
        {
            return atomic_exchange(__p, __r);
        }
    
    template <class _Tp>
    bool
    atomic_compare_exchange_strong(shared_ptr<_Tp>* __p, shared_ptr<_Tp>* __v, shared_ptr<_Tp> __w)
        {
            __sp_mut& __m = __get_sp_mut(__p);
            __m.lock();
            if (__p->owner_before(*__v) == false && __v->owner_before(*__p) == false)
            {
                *__p = __w;
                __m.unlock();
                return true;
            }
            *__v = *__p;
            __m.unlock();
            return false;
        }
    
    template <class _Tp>
    inline FORCE_INLINE
    bool
    atomic_compare_exchange_weak(shared_ptr<_Tp>* __p, shared_ptr<_Tp>* __v, shared_ptr<_Tp> __w)
        {
            return atomic_compare_exchange_strong(__p, __v, __w);
        }
    
    template <class _Tp>
    inline FORCE_INLINE
    bool
    atomic_compare_exchange_strong_explicit(shared_ptr<_Tp>* __p, shared_ptr<_Tp>* __v,
                                            shared_ptr<_Tp> __w, memory_order, memory_order)
        {
            return atomic_compare_exchange_strong(__p, __v, __w);
        }
    
    template <class _Tp>
    inline FORCE_INLINE
    bool
    atomic_compare_exchange_weak_explicit(shared_ptr<_Tp>* __p, shared_ptr<_Tp>* __v,
                                          shared_ptr<_Tp> __w, memory_order, memory_order)
        {
            return atomic_compare_exchange_weak(__p, __v, __w);
        }
    
}
#endif

EPUB3_BEGIN_NAMESPACE

class future_error
    : public std::logic_error
{
    std::error_code     __ec_;
    
public:
    FORCE_INLINE
    future_error(std::error_code __ec)
        : std::logic_error(__ec.message()), __ec_(__ec)
        {}
    
    FORCE_INLINE
    const std::error_code& code() const _NOEXCEPT
        { return __ec_; }

};

class future_uninitialized
    : public future_error
{
public:
    FORCE_INLINE
    future_uninitialized()
        : future_error(std::make_error_code(future_errc::no_state))
        {}
};

class broken_promise
    : public future_error
{
public:
    FORCE_INLINE
    broken_promise()
        : future_error(std::make_error_code(future_errc::broken_promise))
        {}
};

class future_already_retrieved
    : public future_error
{
public:
    FORCE_INLINE
    future_already_retrieved()
        : future_error(std::make_error_code(future_errc::future_already_retrieved))
        {}
};

class promise_already_satisfied
    : public future_error
{
public:
    promise_already_satisfied()
        : future_error(std::make_error_code(future_errc::promise_already_satisfied))
        {}
};

class task_already_started
    : public future_error
{
public:
    FORCE_INLINE
    task_already_started()
        : future_error(std::make_error_code(future_errc::task_already_started))
        {}
};

class task_moved
    : public future_error
{
public:
    FORCE_INLINE
    task_moved()
        : future_error(std::make_error_code(future_errc::no_state))
        {}
};

class promise_moved
    : public future_error
{
public:
    FORCE_INLINE
    promise_moved()
        : future_error(std::make_error_code(future_errc::no_state))
        {}
};

namespace __future_state
{
    enum state { uninitialized, waiting, ready, moved, deferred };
}

struct __relocker
{
    std::unique_lock<std::mutex>&   __lock_;
    bool                            __unlocked_;
    
    FORCE_INLINE
    __relocker(std::unique_lock<std::mutex>& __lk)
        : __lock_(__lk)
        {
            __lock_.unlock();
            __unlocked_ = true;
        }
    FORCE_INLINE
    ~__relocker()
        {
            if (__unlocked_)
                __lock_.lock();
        }
    FORCE_INLINE
    void lock()
        {
            if (__unlocked_)
            {
                __lock_.lock();
                __unlocked_ = false;
            }
        }
    
private:
    __relocker& operator=(const __relocker&) _DELETED_;
    
};

struct __shared_state_base
    : std::enable_shared_from_this<__shared_state_base>
{
    typedef std::list<condition_variable_any*>      _WaiterListType;
    typedef std::shared_ptr<__shared_state_base>    _ContinuationPtrType;
    
    std::exception_ptr      __exc_;
    bool                    __done_;
    bool                    __is_deferred_;
    launch                  __policy_;
    bool                    __is_constructed_;
    mutable std::mutex      __mutex_;
    std::condition_variable __waiters_;
    _WaiterListType         __external_waiters_;
    std::function<void()>   __callback_;
    _ContinuationPtrType    __continuation_ptr_;
    
    static
    std::vector<_ContinuationPtrType>&   __at_thread_exit();
    
    virtual FORCE_INLINE
    void launch_continuation(std::unique_lock<std::mutex>&)
        {}
    
    FORCE_INLINE
    __shared_state_base()
        : __done_(false),
          __is_deferred_(false),
          __policy_(launch::none),
          __is_constructed_(false),
          __continuation_ptr_()
        {}
    
    __shared_state_base(const __shared_state_base&) _DELETED_;
    __shared_state_base& operator=(const __shared_state_base&) _DELETED_;
    
    FORCE_INLINE
    void set_deferred()
        {
            __is_deferred_ = true;
            __policy_ = launch::deferred;
        }
    
    FORCE_INLINE
    void set_async()
        {
            __is_deferred_ = false;
            __policy_ = launch::async;
        }
    
    FORCE_INLINE
    _WaiterListType::iterator register_external_waiter(condition_variable_any& __cv)
        {
            std::unique_lock<std::mutex> __lk(__mutex_);
            do_callback(__lk);
            return __external_waiters_.insert(__external_waiters_.end(), &__cv);
        }
    
    FORCE_INLINE
    void remove_external_waiter(_WaiterListType::iterator __it)
        {
            std::lock_guard<std::mutex> _(__mutex_);
            __external_waiters_.erase(__it);
        }
    
    FORCE_INLINE
    void do_continuation(std::unique_lock<std::mutex>& __lk)
        {
            if (__continuation_ptr_)
            {
                __continuation_ptr_->launch_continuation(__lk);
                if (!__lk.owns_lock())
                    __lk.lock();
                __continuation_ptr_.reset();
            }
        }
    
    FORCE_INLINE
    void set_continuation_ptr(_ContinuationPtrType __c, std::unique_lock<std::mutex>& __lk)
        {
            __continuation_ptr_ = __c;
            if (__done_)
                do_continuation(__lk);
        }
    
    FORCE_INLINE
    void mark_finished_internal(std::unique_lock<std::mutex>& __lk)
        {
            __done_ = true;
            
            __waiters_.notify_all();
            for (auto& __w : __external_waiters_)
            {
                __w->notify_all();
            }
            if (!is_deferred(__lk))
                do_continuation(__lk);
        }
    
    FORCE_INLINE
    void make_ready()
        {
            std::unique_lock<std::mutex> __lk(__mutex_);
            mark_finished_internal(__lk);
        }
    
    FORCE_INLINE
    void do_callback(std::unique_lock<std::mutex>& __lk)
        {
            if (__callback_ && !__done_)
            {
                std::function<void()> __local = __callback_;
                __relocker relock(__lk);
                __local();
            }
        }
    
    FORCE_INLINE
    void wait_internal(std::unique_lock<std::mutex>& __lk, bool __rethrow=true)
        {
            do_callback(__lk);
            
            if (__is_deferred_)
            {
                __is_deferred_ = false;
                execute(__lk);
            }
            else
            {
                while(!__done_) {
                    __waiters_.wait(__lk);
                }
                if (__rethrow && __exc_) {
                    std::rethrow_exception(__exc_);
                }
            }
        }
    
    FORCE_INLINE
    virtual void wait(bool __rethrow=true)
        {
            std::unique_lock<std::mutex> __lk(__mutex_);
            wait_internal(__lk);
        }
    
    template <class _Clock, class _Duration = typename _Clock::duration>
    FORCE_INLINE
    future_status
    wait_until(const std::chrono::time_point<_Clock, _Duration>& abs_time)
        {
            std::unique_lock<std::mutex> __lk(__mutex_);
            if (__is_deferred_)
                return future_status::deferred;
            
            do_callback(__lk);
            
            while (!__done_)
            {
                const std::cv_status st = __waiters_.wait_until(__lk, abs_time);
                if (st == std::cv_status::timeout && !__done_)
                    return future_status::timeout;
            }
            
            return future_status::ready;
        }
    
    FORCE_INLINE
    void mark_exceptional_finish_internal(const std::exception_ptr& __e, std::unique_lock<std::mutex>& __lk)
        {
            __exc_ = __e;
            mark_finished_internal(__lk);
        }
    
    FORCE_INLINE
    void mark_exceptional_finish()
        {
            std::unique_lock<std::mutex> __lk(__mutex_);
            mark_exceptional_finish_internal(std::current_exception(), __lk);
        }
    
    FORCE_INLINE
    void set_exception_at_thread_exit(std::exception_ptr __e)
        {
            std::unique_lock<std::mutex> __lk(__mutex_);
            if (has_value(__lk))
                throw promise_already_satisfied();
            
            __exc_ = __e;
            this->__is_constructed_ = true;
            __at_thread_exit().push_back(this->shared_from_this());
        }
    
    FORCE_INLINE
    bool has_value() const
        {
            std::lock_guard<std::mutex> _(__mutex_);
            return __done_ && !__exc_;
        }
    
    FORCE_INLINE
    bool has_value(std::unique_lock<std::mutex>& __lk) const
        {
            return __done_ && !__exc_;
        }
    
    FORCE_INLINE
    bool has_exception() const
        {
            std::lock_guard<std::mutex> _(__mutex_);
            return __done_ && __exc_;
        }
    
    FORCE_INLINE
    bool has_exception(std::unique_lock<std::mutex>& __lk) const
        {
            return __done_ && __exc_;
        }
    
    FORCE_INLINE
    bool is_deferred(std::unique_lock<std::mutex>&) const
        {
            return __is_deferred_;
        }
    
    FORCE_INLINE
    launch launch_policy(std::unique_lock<std::mutex>& __lk) const
        {
            return __policy_;
        }
    
    FORCE_INLINE
    __future_state::state get_state() const
        {
            std::lock_guard<std::mutex> _(__mutex_);
            if (!__done_)
                return __future_state::waiting;
            else
                return __future_state::ready;
        }
    
    FORCE_INLINE
    std::exception_ptr get_exception_ptr()
        {
            std::unique_lock<std::mutex> __lk(__mutex_);
            return get_exception_ptr(__lk);
        }
    
    FORCE_INLINE
    std::exception_ptr get_exception_ptr(std::unique_lock<std::mutex>& __lk)
        {
            wait_internal(__lk, false);
            return __exc_;
        }
    
    template <typename _Fp, typename _Up>
    FORCE_INLINE
    void set_wait_callback(_Fp&& __f, _Up* __u)
        {
            std::lock_guard<std::mutex> __lk(__mutex_);
            __callback_ = std::bind(__f, std::ref(*__u));
        }
    
    virtual FORCE_INLINE
    void execute(std::unique_lock<std::mutex>&)
        {}
    
};

static void __KillVectorPtr(void* ptr)
{
    typedef std::vector<__shared_state_base::_ContinuationPtrType> _VecType;
    _VecType* __v = reinterpret_cast<_VecType*>(ptr);
    delete __v;
}

template <typename _Tp>
struct __future_traits
{
    typedef std::unique_ptr<_Tp>    storage_type;
    struct dummy;
    
    typedef _Tp&        source_reference_type;
    typedef _Tp&&       rvalue_source_type;
    typedef _Tp         move_dest_type;
    
    typedef const _Tp&  shared_future_get_result_type;
    
    static FORCE_INLINE
    void init(storage_type& __s, source_reference_type __t)
        {
            __s.reset(new _Tp(__t));
        }
    static FORCE_INLINE
    void init(storage_type& __s, rvalue_source_type __t)
        {
            __s.reset(new _Tp(std::move(__t)));
        }
    
    static FORCE_INLINE
    void cleanup(storage_type& __s)
        {
            __s.reset();
        }
};

template <typename _Tp>
struct __future_traits<_Tp&>
{
    typedef _Tp*    storage_type;
    typedef _Tp&    source_reference_type;
    typedef _Tp&    move_dest_type;
    typedef _Tp&    shared_future_get_result_type;
    
    static FORCE_INLINE
    void init(storage_type& __s, _Tp& __t)
        {
            __s = &__t;
        }
    
    static FORCE_INLINE
    void cleanup(storage_type& __s)
        {
            __s = nullptr;
        }
};

template <>
struct __future_traits<void>
{
    typedef bool    storage_type;
    typedef void    move_dest_type;
    typedef void    shared_future_get_result_type;
    
    static FORCE_INLINE
    void init(storage_type& __s)
        {
            __s = true;
        }
    
    static FORCE_INLINE
    void cleanup(storage_type& __s)
        {
            __s = false;
        }
};

template <typename _Tp>
struct __shared_state
    : __shared_state_base
{
    typedef __future_traits<_Tp>                                traits_type;
    typedef typename traits_type::storage_type                  storage_type;
    typedef typename traits_type::source_reference_type         source_reference_type;
    typedef typename traits_type::rvalue_source_type            rvalue_source_type;
    typedef typename traits_type::move_dest_type                move_dest_type;
    typedef typename traits_type::shared_future_get_result_type shared_future_get_result_type;
    
    storage_type    __result_;
    
    FORCE_INLINE
    __shared_state()
        : __result_(nullptr)
        {}
    
    FORCE_INLINE
    ~__shared_state()
        {}
    
    FORCE_INLINE
    void mark_finished_with_result_internal(source_reference_type __r, std::unique_lock<std::mutex>& __lk)
        {
            traits_type::init(__result_, __r);
            this->mark_finished_internal(__lk);
        }
    
    FORCE_INLINE
    void mark_finished_with_result_internal(rvalue_source_type __r, std::unique_lock<std::mutex>& __lk)
        {
            traits_type::init(__result_, std::move(__r));
            this->mark_finished_internal(__lk);
        }
    
    FORCE_INLINE
    void mark_finished_with_result(source_reference_type __r)
        {
            std::unique_lock<std::mutex> __lk(this->__mutex_);
            this->mark_finished_with_result_internal(__r, __lk);
        }
    
    FORCE_INLINE
    void mark_finished_with_result(rvalue_source_type __r)
        {
            std::unique_lock<std::mutex> __lk(this->__mutex_);
            this->mark_finished_with_result_internal(std::move(__r), __lk);
        }
    
    virtual FORCE_INLINE
    move_dest_type get()
        {
            wait();
            return std::move(*__result_);
        }
    
    virtual FORCE_INLINE
    shared_future_get_result_type get_sh()
        {
            wait();
            return *__result_;
        }
    
    FORCE_INLINE
    void set_value_at_thread_exit(source_reference_type __r)
        {
            std::unique_lock<std::mutex> __lk(this->__mutex_);
            if (this->has_value(__lk))
            {
                throw promise_already_satisfied();
            }
            
            __result_.reset(new _Tp(__r));
            
            __is_constructed_ = true;
            __at_thread_exit().push_back(this->shared_from_this());
        }
    
private:
    __shared_state(const __shared_state&) _DELETED_;
    __shared_state& operator=(const __shared_state&) _DELETED_;
};

template <typename _Tp>
struct __shared_state<_Tp&>
    : __shared_state_base
{
    typedef __future_traits<_Tp&>                               traits_type;
    typedef typename traits_type::storage_type                  storage_type;
    typedef typename traits_type::source_reference_type         source_reference_type;
    typedef typename traits_type::move_dest_type                move_dest_type;
    typedef typename traits_type::shared_future_get_result_type shared_future_get_result_type;
    
    _Tp*    __result_;
    
    FORCE_INLINE
    __shared_state()
        : __result_(nullptr)
        {}
    
    FORCE_INLINE
    ~__shared_state()
        {}
    
    FORCE_INLINE
    void mark_finished_with_result_internal(source_reference_type __r, std::unique_lock<std::mutex>& __lk)
        {
            __result_ = &__r;
            this->mark_finished_internal(__lk);
        }
    
    FORCE_INLINE
    void mark_finished_with_result(source_reference_type __r)
        {
            std::unique_lock<std::mutex> __lk(__mutex_);
            this->mark_finished_with_result_internal(__r, __lk);
        }
    
    virtual FORCE_INLINE
    move_dest_type get()
        {
            wait();
            return *__result_;
        }
    
    virtual FORCE_INLINE
    shared_future_get_result_type get_sh()
        {
            wait();
            return *__result_;
        }
    
    FORCE_INLINE
    void set_value_at_thread_exit(source_reference_type __r)
        {
            std::unique_lock<std::mutex> __lk(__mutex_);
            if (this->has_value(__lk))
                throw promise_already_satisfied();
            
            __result_ = &__r;
            
            __is_constructed_ = true;
            __at_thread_exit().push_back(shared_from_this());
        }
    
private:
    __shared_state(const __shared_state&) _DELETED_;
    __shared_state& operator=(__shared_state&) _DELETED_;
};

template <>
struct __shared_state<void>
    : __shared_state_base
{
    typedef void shared_future_get_result_type;
    
    FORCE_INLINE
    __shared_state()
        {}
    
    FORCE_INLINE
    ~__shared_state()
        {}
    
    FORCE_INLINE
    void mark_finished_with_result_internal(std::unique_lock<std::mutex>& __lk)
        {
            this->mark_finished_internal(__lk);
        }
    
    FORCE_INLINE
    void mark_finished_with_result()
        {
            std::unique_lock<std::mutex> __lk(__mutex_);
            this->mark_finished_with_result_internal(__lk);
        }
    
    virtual FORCE_INLINE
    void get()
        {
            this->wait();
        }
    
    virtual FORCE_INLINE
    shared_future_get_result_type get_sh()
        {
            this->wait();
        }
    
    FORCE_INLINE
    void set_value_at_thread_exit()
        {
            std::unique_lock<std::mutex> __lk(__mutex_);
            if (this->has_value(__lk))
                throw promise_already_satisfied();
            
            __is_constructed_ = true;
            __at_thread_exit().push_back(shared_from_this());
        }
    
private:
    __shared_state(const __shared_state&) _DELETED_;
    __shared_state& operator=(const __shared_state&) _DELETED_;
};

template <typename _Rp>
struct __future_async_shared_state_base
    : __shared_state<_Rp>
{
    typedef __shared_state<_Rp> _Base;
    
protected:
    std::thread __thr_;
    
    FORCE_INLINE
    void join()
        {
            if (__thr_.joinable())
                __thr_.join();
        }
    
public:
    FORCE_INLINE
    __future_async_shared_state_base()
        {
            this->set_async();
        }
    
    explicit FORCE_INLINE
    __future_async_shared_state_base(std::thread&& __th)
        : __thr_(std::move(__th))
        {
            this->set_async();
        }
    
    FORCE_INLINE
    ~__future_async_shared_state_base()
        {
            join();
        }
    
    virtual FORCE_INLINE
    void wait(bool rethrow)
        {
            join();
            this->_Base::wait();
        }
};

template <typename _Rp, typename _Fp>
struct __future_async_shared_state
    : __future_async_shared_state_base<_Rp>
{
    typedef __future_async_shared_state_base<_Rp>   _Base;
    
public:
    explicit FORCE_INLINE
    __future_async_shared_state(_Fp&& __f)
        : _Base(std::thread(&__future_async_shared_state::run, this, std::forward<_Fp>(__f)))
        {}
    
    static FORCE_INLINE
    void run(__future_async_shared_state* __that, _Fp&& __f)
        {
            try
            {
                __that->mark_finished_with_result(invoke(__f));
            }
            catch (...)
            {
                __that->mark_exceptional_finish();
            }
        }
};

template <typename _Rp, typename _Fp>
struct __future_async_shared_state<_Rp&, _Fp>
    : __future_async_shared_state_base<_Rp&>
{
    typedef __future_async_shared_state_base<_Rp&>   _Base;
    
public:
    explicit FORCE_INLINE
    __future_async_shared_state(_Fp&& __f)
        : _Base(std::thread(&__future_async_shared_state::run, this, std::forward<_Fp>(__f)))
        {}
    
    static FORCE_INLINE
    void run(__future_async_shared_state* __that, _Fp&& __f)
        {
            try
            {
                __that->mark_finished_with_result(invoke(__f));
            }
            catch (...)
            {
                __that->mark_exceptional_finish();
            }
        }
};

template <typename _Fp>
struct __future_async_shared_state<void, _Fp>
    : public __future_async_shared_state_base<void>
{
    typedef __future_async_shared_state_base<void>  _Base;
    
public:
    explicit FORCE_INLINE
    __future_async_shared_state(_Fp&& __f)
        : _Base(std::thread(&__future_async_shared_state::run, this, std::forward<_Fp>(__f)))
        {}
    
    static FORCE_INLINE
    void run(__future_async_shared_state* __that, _Fp&& __f)
        {
            try
            {
                invoke(__f);
                __that->mark_finished_with_result();
            }
            catch (...)
            {
                __that->mark_exceptional_finish();
            }
        }
};

template <typename _Rp, typename _Fp>
struct __future_deferred_shared_state
    : __shared_state<_Rp>
{
    typedef __shared_state<_Rp> _Base;
    _Fp __func_;
    
public:
    explicit FORCE_INLINE
    __future_deferred_shared_state(_Fp&& __f)
        : __func_(std::forward<_Fp>(__f))
        {
            this->set_deferred();
        }
    
    virtual FORCE_INLINE
    void execute(std::unique_lock<std::mutex>& __lk)
        {
            try
            {
                this->mark_finished_with_result_internal(invoke(__func_), __lk);
            }
            catch (...)
            {
                this->mark_exceptional_finish_internal(std::current_exception(), __lk);
            }
        }
};

template <typename _Rp, typename _Fp>
struct __future_deferred_shared_state<_Rp&, _Fp>
    : __shared_state<_Rp&>
{
    typedef __shared_state<_Rp&>    _Base;
    _Fp __func_;
    
public:
    explicit FORCE_INLINE
    __future_deferred_shared_state(_Fp&& __f)
        : __func_(std::forward<_Fp>(__f))
        {
            this->set_deferred();
        }
    
    virtual FORCE_INLINE
    void execute(std::unique_lock<std::mutex>& __lk)
        {
            try
            {
                this->mark_finished_with_result_internal(invoke(__func_), __lk);
            }
            catch (...)
            {
                this->mark_exceptional_finish_internal(std::current_exception(), __lk);
            }
        }
};

template <typename _Fp>
struct __future_deferred_shared_state<void, _Fp>
    : __shared_state<void>
{
    typedef __shared_state<void>    _Base;
    _Fp __func_;
    
public:
    explicit FORCE_INLINE
    __future_deferred_shared_state(_Fp&& __f)
        : __func_(std::forward<_Fp>(__f))
        {
            this->set_deferred();
        }
    
    virtual FORCE_INLINE
    void execute(std::unique_lock<std::mutex>& __lk)
        {
            try
            {
                invoke(__func_);
                this->mark_finished_with_result_internal(__lk);
            }
            catch (...)
            {
                this->mark_exceptional_finish_internal(std::current_exception(), __lk);
            }
        }
};

template <class _Tp>
class __scoped_array
{
private:
    _Tp*    __elems_;
    
public:
    typedef _Tp value_type;
    
    explicit FORCE_INLINE
    __scoped_array(_Tp* __p = nullptr) _NOEXCEPT
        : __elems_(__p)
        {}
    
    FORCE_INLINE
    ~__scoped_array()
        { if (__elems_ != nullptr) delete[] __elems_; }
    
    FORCE_INLINE
    void reset(_Tp* __p = nullptr)
        {
            if (__elems_ != nullptr)
                delete[] __elems_;
            __elems_ = __p;
        }
    
    FORCE_INLINE
    _Tp& operator[](std::ptrdiff_t __i) const _NOEXCEPT
        {
            return __elems_[__i];
        }
    
    FORCE_INLINE
    _Tp* get() const _NOEXCEPT
        {
            return __elems_;
        }
    
    FORCE_INLINE
#if EPUB_COMPILER_SUPPORTS(CXX_EXPLICIT_CONVERSIONS)
    explicit
#endif
    operator bool() const _NOEXCEPT
        {
            return __elems_ != nullptr;
        }
    
    void swap(__scoped_array& __o)
        {
            std::swap(__elems_, __o.__elems_);
        }
};

template <typename _Iter>
_Iter __try_lock_iter(_Iter, _Iter);
template <typename _Iter>
void __lock_iter(_Iter, _Iter);

template <typename _Iter>
struct __range_lock_guard
{
    _Iter   __begin_;
    _Iter   __end_;
    
    FORCE_INLINE
    __range_lock_guard(_Iter __b, _Iter __e)
        : __begin_(__b), __end_(__e)
        {
            __lock_iter(__begin_, __end_);
        }
    
    FORCE_INLINE
    void release()
        {
            __begin_ = __end_;
        }
    
    FORCE_INLINE
    ~__range_lock_guard()
        {
            for (; __begin_ != __end_; ++__begin_)
            {
                __begin_->unlock();
            }
        }
};

template <typename _Iter>
_Iter __try_lock_iter(_Iter __b, _Iter __e)
{
    if (__b == __e)
        return __e;
    
    typedef typename std::iterator_traits<_Iter>::value_type lock_type;
    std::unique_lock<lock_type> __guard(*__b, std::try_to_lock);
    
    if (!__guard.owns_lock())
        return __b;
    _Iter const __failed = __try_lock_iter(++__b, __e);
    if (__failed == __e)
        __guard.release();
    
    return __failed;
}

template <typename _Iter>
void __lock_iter(_Iter __b, _Iter __e)
{
    typedef typename std::iterator_traits<_Iter>::value_type lock_type;
    
    if (__b == __e)
        return;
    
    bool __start_with_begin = true;
    _Iter __second = __b;
    ++__second;
    _Iter __next = __second;
    
    for (;;)
    {
        std::unique_lock<lock_type> __begin_lock(*__b, std::defer_lock);
        if (__start_with_begin)
        {
            __begin_lock.lock();
            _Iter const __failed = __try_lock_iter(__next, __e);
            if (__failed == __e)
            {
                __begin_lock.release();
                return;
            }
            
            __start_with_begin = false;
            __next = __failed;
        }
        else
        {
            __range_lock_guard<_Iter> __guard(__next, __e);
            if (__begin_lock.try_lock())
            {
                _Iter const __failed = __try_lock_iter(__second, __next);
                if (__failed == __next)
                {
                    __begin_lock.release();
                    __guard.release();
                    return;
                }
                
                __start_with_begin = false;
                __next = __failed;
            }
            else
            {
                __start_with_begin = true;
                __next = __second;
            }
        }
    }
}

class __future_waiter
{
    struct registered_waiter;
    typedef std::vector<int>::size_type count_type;
    
    struct registered_waiter
    {
        std::shared_ptr<__shared_state_base>            __future_;
        __shared_state_base::_WaiterListType::iterator  __wait_iterator_;
        count_type                                      __idx_;
        
        FORCE_INLINE
        registered_waiter(const std::shared_ptr<__shared_state_base>& __fut,
                          __shared_state_base::_WaiterListType::iterator __it,
                          count_type __idx)
            : __future_(__fut), __wait_iterator_(__it), __idx_(__idx)
            {}
    };
    
    struct all_futures_lock
    {
        count_type                                      __count_;
        __scoped_array<std::unique_lock<std::mutex>>    __locks_;
        
        all_futures_lock(std::vector<registered_waiter>& __futures)
            : __count_(__futures.size()), __locks_(new std::unique_lock<std::mutex>[__count_])
            {
                for (count_type __i = 0; __i < __count_; __i++)
                {
                    __locks_[__i] = std::unique_lock<std::mutex>(__futures[__i].__future_->__mutex_);
                }
            }
        
        void lock()
            {
                __lock_iter(__locks_.get(), __locks_.get()+__count_);
            }
        
        void unlock()
            {
                for (count_type __i = 0; __i < __count_; ++__i)
                {
                    __locks_[__i].unlock();
                }
            }
    };
    
    condition_variable_any          __cv_;
    std::vector<registered_waiter>  __futures_;
    count_type                      __future_count_;
    
public:
    FORCE_INLINE
    __future_waiter()
        : __future_count_(0)
        {}
    
    template <typename _Fut>
    FORCE_INLINE
    void add(_Fut& __f)
        {
            if (__f.__future_)
            {
                __futures_.push_back(registered_waiter(__f.__future_, __f.__future_->register_external_waiter(__cv_), __future_count_));
            }
            ++__future_count_;
        }
    
    count_type wait()
        {
            all_futures_lock __alk(__futures_);
            std::unique_lock<all_futures_lock> __lk(__alk, std::defer_lock);
            for (;;)
            {
                for (count_type __i=0; __i<__futures_.size(); ++__i)
                {
                    if (__futures_[__i].__future_->__done_)
                        return __futures_[__i].__idx_;
                }
                __cv_.wait(__lk);
            }
        }
    
    ~__future_waiter()
        {
            for (count_type __i=0; __i<__futures_.size(); ++__i)
            {
                __futures_[__i].__future_->remove_external_waiter(__futures_[__i].__wait_iterator_);
            }
        }
};

////////////////////////////////////////////////////////////////////////////////////

#if 0
#pragma mark - Public API
#endif

template <typename _Rp>
class future;

template <typename _Rp>
class shared_future;

template <typename _Tp>
struct is_future_type : public std::false_type {};

template <typename _Tp>
struct is_future_type<future<_Tp>> : public std::true_type {};

template <typename _Tp>
struct is_future_type<shared_future<_Tp>> : public std::true_type {};

template <typename _Iter>
FORCE_INLINE
typename std::enable_if<!is_future_type<_Iter>::value, void>::type
wait_for_all(_Iter __b, _Iter __e)
{
    for (_Iter __c = __b; __c != __e; ++__c)
        __c->wait();
}

template <typename _F1>
FORCE_INLINE
typename std::enable_if<is_future_type<_F1>::value, void>::type
wait_for_all(_F1& __f1)
{
    __f1.wait();
}


template <typename _F1, typename ..._F2>
FORCE_INLINE
typename std::enable_if<is_future_type<_F1>::value, void>::type
wait_for_all(_F1& __f1, _F2&&... __f2)
{
    __f1.wait();
    wait_for_all(std::forward<_F2>(__f2)...);
}

template <typename _Iter>
FORCE_INLINE
typename std::enable_if<!is_future_type<_Iter>::value, _Iter>::type
wait_for_any(_Iter __b, _Iter __e)
{
    if (__b == __e)
        return __e;
    
    __future_waiter __waiter;
    for (_Iter __c = __b; __c != __e; ++__c)
        __waiter.add(*__c);
    return std::next(__b, __waiter.wait());
}

template <typename _F1>
FORCE_INLINE
typename std::enable_if<is_future_type<_F1>::value, unsigned>::type
wait_for_any(__future_waiter& __w, _F1& __f1)
{
    __w.add(__f1);
    return __w.wait();
}

template <typename _F1, typename ..._F2>
FORCE_INLINE
typename std::enable_if<is_future_type<_F1>::value, unsigned>::type
wait_for_any(__future_waiter& __w, _F1& __f1, _F2&&... __f2)
{
    __w.add(__f1);
    return wait_for_any(__w, std::forward<_F2>(__f2)...);
}

template <typename _F1, typename ..._F2>
typename std::enable_if<is_future_type<_F1>::value, unsigned>::type
wait_for_any(_F1& __f1, _F2&&... __f2)
{
    __future_waiter __waiter;
    wait_for_all(__waiter, __f1, std::forward<_F2>(__f2)...);
}

template <typename _Rp>
class promise;

template <typename _Rp>
class packaged_task;

template <typename _Rp>
class __basic_future
{
public:
    typedef std::shared_ptr<__shared_state<_Rp>>    future_ptr;
    future_ptr  __future_;
    
    FORCE_INLINE
    __basic_future(future_ptr __fut)
        : __future_(__fut)
        {}
    
    explicit
    __basic_future(const shared_future<_Rp>&);
    
    typedef __future_state::state state;
    
    __basic_future()
        : __future_()
        {}
    
    __basic_future(__basic_future&& __o) _NOEXCEPT
        : __future_(std::move(__o.__future_))
        {}
    
    __basic_future& operator=(__basic_future&& __o) _NOEXCEPT
        {
            __future_ = std::move(__o.__future_);
            return *this;
        }
private:
    __basic_future(const __basic_future&) _DELETED_;
    __basic_future& operator=(const __basic_future&) _DELETED_;
    
public:
    FORCE_INLINE
    void swap(__basic_future& __o) _NOEXCEPT
        {
            __future_.swap(__o.__future_);
        }
    
    FORCE_INLINE
    state get_state() const
        {
            if (!bool(__future_))
                return __future_state::uninitialized;
            return __future_->get_state();
        }
    
    FORCE_INLINE
    bool is_ready() const
        {
            return get_state() == __future_state::ready;
        }
    
    FORCE_INLINE
    bool has_exception() const
        {
            return bool(__future_) && __future_->has_exception();
        }
    
    FORCE_INLINE
    bool has_value() const
        {
            return bool(__future_) && __future_->has_value();
        }
    
    FORCE_INLINE
    launch launch_policy(std::unique_lock<std::mutex>& __lk) const
        {
            if (bool(__future_))
                return __future_->launch_policy(__lk);
            else
                return launch::none;
        }
    
    FORCE_INLINE
    std::exception_ptr get_exception_ptr()
        {
            return (bool(__future_)
                    ? __future_->get_exception_ptr()
                    : std::exception_ptr());
        }
    
    FORCE_INLINE
    bool valid() const _NOEXCEPT
        {
            return bool(__future_);
        }
    
    FORCE_INLINE
    void wait()
        {
            if (!bool(__future_))
                throw future_uninitialized();
            return __future_->wait();
        }
    
    template <class _Rep, class _Period>
    FORCE_INLINE
    future_status
    wait_for(const std::chrono::duration<_Rep, _Period>& __t) const
        {
            return wait_until(std::chrono::steady_clock::now() + __t);
        }
    
    template <class _Clock, class _Duration>
    FORCE_INLINE
    future_status
    wait_until(const std::chrono::time_point<_Clock, _Duration>& __t) const
        {
            if (!bool(__future_))
                throw future_uninitialized();
            return __future_->wait_until(__t);
        }
    
private:
    template <typename _Tp>
        friend future<typename std::decay<_Tp>::type>
        make_ready_future(_Tp&&);
    template <typename _Tp>
        friend future<_Tp>
        make_ready_future(std::exception_ptr);
    template <typename _Tp, typename _Exc>
        friend typename std::enable_if
            <
                !std::is_same<_Tp, _Exc>::value,
                future<_Tp>
            >::type
        make_ready_future(_Exc);
    
};

template <class _Rp, class _Fp>
FORCE_INLINE
future<_Rp>
__make_future_async_shared_state(_Fp&& __f);

template <class _Rp, class _Fp>
FORCE_INLINE
future<_Rp>
__make_future_deferred_shared_state(_Fp&& __f);

template <typename _Fut, typename _Rp, typename _Fp>
    struct __future_deferred_continuation_shared_state;
template <typename _Fut, typename _Rp, typename _Fp>
    struct __future_async_continuation_shared_state;
template <typename _Fut, typename _Rp, typename _Fp>
    struct __future_executor_continuation_shared_state;

template <class _Fut, class _Rp, class _Fp>
FORCE_INLINE
future<_Rp>
__make_future_async_continuation_shared_state(std::unique_lock<std::mutex>& __lk, _Fut&& __f, _Fp&& __c);

template <class _Fut, class _Rp, class _Fp>
FORCE_INLINE
future<_Rp>
__make_future_deferred_continuation_shared_state(std::unique_lock<std::mutex>& __lk, _Fut&& __f, _Fp&& __c);

template <class _Fut, class _Rp, class _Fp>
FORCE_INLINE
future<_Rp>
__make_future_executor_continuation_shared_state(std::unique_lock<std::mutex>& __lk, _Fut&& __f, executor* __e, _Fp&& __c);

template <typename _Fut, typename _Rp>
    struct __future_unwrap_shared_state;

template <class _Fut, class _Rp>
inline FORCE_INLINE
future<_Rp>
__make_future_unwrap_shared_state(std::unique_lock<std::mutex>& __lk, _Fut&& __f);

template <typename _Rp>
    struct __task_base_shared_state;

template <typename _Rp, typename ..._Args>
struct __task_base_shared_state<_Rp(_Args...)>
    : __shared_state<_Rp>
{
    bool __started_;
    
    FORCE_INLINE
    __task_base_shared_state()
        : __started_(false)
        {}
    
    FORCE_INLINE
    void reset()
        {
            __started_ = false;
        }
    
    virtual
    void do_run(_Args&&... __args) = 0;
    
    FORCE_INLINE
    void run(_Args&&... __args)
        {
            std::lock_guard<std::mutex> __lk(this->__mutex_);
            if(__started_)
                throw task_already_started();
            __started_ = true;
            do_run(std::forward<_Args>(__args)...);
        }
    
    virtual
    void do_apply(_Args&&... __args) = 0;
    
    FORCE_INLINE
    void apply(_Args&&... __args)
        {
            std::lock_guard<std::mutex> __lk(this->__mutex_);
            if (__started_)
                throw task_already_started();
            __started_ = true;
            do_apply(std::forward<_Args>(__args)...);
        }
    
    FORCE_INLINE
    void owner_destroyed()
        {
            std::unique_lock<std::mutex> __lk(this->__mutex_);
            if (!__started_)
            {
                __started_ = true;
                this->mark_exceptional_finish_internal(std::make_exception_ptr(broken_promise()), __lk);
            }
        }
};

template <typename _Fp, typename _Rp>
    struct __task_shared_state;

template <typename _Fp, typename _Rp, typename ..._Args>
struct __task_shared_state<_Fp, _Rp(_Args...)>
    : __task_base_shared_state<_Rp(_Args...)>
{
private:
    __task_shared_state(__task_shared_state&) _DELETED_;
    
public:
    _Fp __f_;
    
    FORCE_INLINE
    __task_shared_state(_Fp const& __f)
        : __f_(__f)
        {}
    
    FORCE_INLINE
    __task_shared_state(_Fp&& __f)
        : __f_(std::move(__f))
        {}
    
    virtual void do_apply(_Args&&... __args)
        {
            try
            {
                this->set_value_at_thread_exit(__f_(std::forward<_Args>(__args)...));
            }
            catch (...)
            {
                this->set_exception_at_thread_exit(std::current_exception());
            }
        }
    
    virtual void do_run(_Args&&... __args)
        {
            try
            {
                this->mark_finished_with_result(__f_(std::forward<_Args>(__args)...));
            }
            catch (...)
            {
                this->mark_exceptional_finish();
            }
        }
};

template <typename _Fp, typename _Rp, typename ..._Args>
struct __task_shared_state<_Fp, _Rp&(_Args...)>
    : __task_base_shared_state<_Rp&(_Args...)>
{
private:
    __task_shared_state(__task_shared_state&) = delete;
    
public:
    _Fp __f_;
    
    FORCE_INLINE
    __task_shared_state(_Fp const& __f)
        : __f_(__f)
        {}
    
    FORCE_INLINE
    __task_shared_state(_Fp&& __f)
        : __f_(__f)
        {}
    
    virtual void do_apply(_Args&&... __args)
        {
            try
            {
                this->set_value_at_thread_exit(__f_(std::forward<_Args>(__args)...));
            }
            catch (...)
            {
                this->set_exception_at_thread_exit(std::current_exception());
            }
        }
    
    virtual void do_run(_Args&&... __args)
    {
        try
        {
            this->mark_finished_with_result(__f_(std::forward<_Args>(__args)...));
        }
        catch (...)
        {
            this->mark_exceptional_finish();
        }
    }
};

template <typename _Rp, typename ..._Args>
struct __task_shared_state<std::function<_Rp(_Args...)>, _Rp(_Args...)>
    : __task_base_shared_state<_Rp(_Args...)>
{
private:
    __task_shared_state(__task_shared_state&) = delete;
    
public:
    std::function<_Rp(_Args...)> __f_;
    
    FORCE_INLINE
    __task_shared_state(std::function<_Rp(_Args...)> __f)
        : __f_(__f)
        {}
    
    virtual void do_apply(_Args&&... __args)
        {
            try
            {
                this->set_value_at_thread_exit(__f_(std::forward<_Args>(__args)...));
            }
            catch (...)
            {
                this->set_exception_at_thread_exit();
            }
        }
    
    virtual void do_run(_Args&&... __args)
        {
            try
            {
                this->mark_finished_with_result(__f_(std::forward<_Args>(__args)...));
            }
            catch (...)
            {
                this->mark_exceptional_finish();
            }
        }
};

template <typename _Fp, typename... _Args>
struct __task_shared_state<_Fp, void(_Args...)>
    : __task_base_shared_state<void(_Args...)>
{
private:
    __task_shared_state(__task_shared_state&) = delete;
    
public:
    _Fp __f_;
    
    FORCE_INLINE
    __task_shared_state(const _Fp& __f)
        : __f_(__f)
        {}
    
    FORCE_INLINE
    __task_shared_state(_Fp&& __f)
        : __f_(std::move(__f))
        {}
    
    virtual void do_apply(_Args&&... __args)
        {
            try
            {
                __f_(std::forward<_Args>(__args)...);
                this->set_value_at_thread_exit();
            }
            catch (...)
            {
                this->set_exception_at_thread_exit(std::current_exception());
            }
        }
    
    virtual void do_run(_Args&&... __args)
        {
            try
            {
                __f_(std::forward<_Args>(__args)...);
                this->mark_finished_with_result();
            }
            catch (...)
            {
                this->mark_exceptional_finish();
            }
        }
};

template <class _Rp>
class future
    : public __basic_future<_Rp>
{
private:
    typedef __basic_future<_Rp>         _Base;
    typedef typename _Base::future_ptr  future_ptr;
    
    // I'm *very* friendly...
    
    friend class shared_future<_Rp>;
    friend class promise<_Rp>;
    
    template <typename, typename, typename>
        friend struct __future_async_continuation_shared_state;
    template <typename, typename, typename>
        friend struct __future_deferred_continuation_shared_state;
    template <class _F, class _Up, class _Fp>
        friend future<_Up>
        __make_future_async_continuation_shared_state(std::unique_lock<std::mutex>&, _F&&, _Fp&&);
    template <class _F, class _Up, class _Fp>
        friend future<_Up>
        __make_future_deferred_continuation_shared_state(std::unique_lock<std::mutex>&, _F&&, _Fp&&);
    template <class _F, class _Up, class _Fp>
        friend future<_Up>
        __make_future_executor_continuation_shared_state(std::unique_lock<std::mutex>&, _F&&, executor*, _Fp&&);
    
    template <typename, typename>
        friend struct __future_unwrap_shared_state;
    template <class _F, class _Up>
        friend future<_Up>
        __make_future_unwrap_shared_state(std::unique_lock<std::mutex>&, _F&&);
    
    template <class>
        friend class packaged_task;
    
    friend class __future_waiter;
    
    template <class _Up, class _Fp>
        friend future<_Up>
        __make_future_async_shared_state(_Fp&&);
    template <class _Up, class _Fp>
        friend future<_Up>
        __make_future_deferred_shared_state(_Fp&& __f);
    
    template <class _Tp>
        friend future<typename std::decay<_Tp>::type>
        make_ready_future(_Tp&&);
    friend future<void>
        make_ready_future();
    template <typename _Tp>
        friend future<_Tp>
        make_ready_future(std::exception_ptr);
    template <typename _Tp, typename _Exc>
        friend typename std::enable_if
            <
                !std::is_same<_Tp, _Exc>::value,
                future<_Tp>
            >::type
        make_ready_future(_Exc);
    
    typedef typename __future_traits<_Rp>::move_dest_type move_dest_type;
    
    FORCE_INLINE
    future(future_ptr __f)
        : _Base(__f)
        {}
    
    future(const future&) _DELETED_;
    future& operator=(const future&) _DELETED_;
    
public:
    FORCE_INLINE
    CONSTEXPR future()
        {}
    
    FORCE_INLINE
    future(future&& __o) _NOEXCEPT
        : _Base(std::move(static_cast<_Base&>(__o)))
        {}
    
    future(future<future<_Rp>>&& __o);
    
    FORCE_INLINE
    ~future()
        {}
    
    FORCE_INLINE
    future& operator=(future&& __o)
        {
            this->_Base::operator=(std::move(static_cast<_Base&>(__o)));
            return *this;
        }
    
    FORCE_INLINE
    shared_future<_Rp> share()
        {
            return shared_future<_Rp>(std::move(*this));
        }
    
    FORCE_INLINE
    void swap(future& __o)
        {
            static_cast<_Base*>(this)->swap(__o);
        }
    
private:
    FORCE_INLINE
    void set_async()
        {
            this->__future_->set_async();
        }
    
    FORCE_INLINE
    void set_deferred()
        {
            this->__future_->set_deferred();
        }
    
public:
    
    /////////////////////////////////////////////////////
    // retrieving the encapsulated value
    
    FORCE_INLINE
    move_dest_type get()
        {
            if (!bool(this->__future_))
                throw future_uninitialized();
            future_ptr __fut = this->__future_;
            this->__future_.reset();
            return __fut->get();
        }
    
    template <typename _R2>
    FORCE_INLINE
    typename std::enable_if<!std::is_void<_R2>::value, move_dest_type>::type
    get_or(_R2&& __v)
        {
            if (!bool(this->__future_))
                throw future_uninitialized();
            
            this->__future_->wait(false);
            future_ptr __fut = this->__future_;
            this->__future_.reset();
            if (__fut->has_value())
                return __fut->get();
            else
                return std::move(__v);
        }
    
    template <typename _R2>
    FORCE_INLINE
    typename std::enable_if<!std::is_void<_R2>::value, move_dest_type>::type
    get_or(const _R2& __v)
        {
            if (!bool(this->__future_))
                throw future_uninitialized();
            
            this->__future_->wait(false);
            future_ptr __fut = this->__future_;
            this->__future_.reset();
            if (__fut->has_value())
                return __fut->get();
            else
                return __v;
        }
    
    /////////////////////////////////////////////////////
    // checking state
    
    // These are all implemented by __basic_future
    /*
    bool valid() const _NOEXCEPT;
    void wait() const
    
    template <class Rep, class Period>
        future_status
        wait_for(const chrono::duration<Rep, Period>& rel_time) const;
    
    template <class Clock, class Duration>
        future_status
        wait_until(const chrono::time_point<Clock, Duration>& abs_time) const;
     */
    
    bool ready() const
        {
            return this->is_ready();
        }
    
    ////////////////////////////////////////////////////
    // continuation support
    
    template <typename _Fp>
        inline FORCE_INLINE
        future<typename std::result_of<_Fp(future)>::type>
        then(_Fp&& __func);
    
    template <typename _Fp>
        inline FORCE_INLINE
        future<typename std::result_of<_Fp(future)>::type>
        then(executor& __exec, _Fp&& __func);
    
    template <typename _Fp>
        inline FORCE_INLINE
        future<typename std::result_of<_Fp(future)>::type>
        then(launch __policy, _Fp&& __func);
};

////////////////////////////////////////////////////////
// automatic unwrapping of nested futures

template <typename _R2>
class future<future<_R2>>
    : public __basic_future<_R2>
{
    typedef future<_R2> _Rp;
    
private:
    typedef __basic_future<_Rp>         _Base;
    typedef typename _Base::future_ptr  future_ptr;
    
    // I'm *very* friendly...
    
    friend class shared_future<_Rp>;
    friend class promise<_Rp>;
    
    template <typename, typename, typename>
        friend struct __future_async_continuation_shared_state;
    template <typename, typename, typename>
        friend struct __future_deferred_continuation_shared_state;
    template <class _F, class _Up, class _Fp>
        friend future<_Up>
        __make_future_async_continuation_shared_state(std::unique_lock<std::mutex>&, _F&&, _Fp&&);
    template <class _F, class _Up, class _Fp>
        friend future<_Up>
        __make_future_deferred_continuation_shared_state(std::unique_lock<std::mutex>&, _F&&, _Fp&&);
    template <class _F, class _Up, class _Fp>
        friend future<_Up>
        __make_future_executor_continuation_shared_state(std::unique_lock<std::mutex>&, _F&&, executor*, _Fp&&);
    
    template <typename, typename>
        friend struct __future_unwrap_shared_state;
    template <class _F, class _Up>
        friend future<_Up>
        __make_future_unwrap_shared_state(std::unique_lock<std::mutex>&, _F&&);
    
    template <class>
        friend class packaged_task;
    
    friend class __future_waiter;
    
    template <class _Up, class _Fp>
        friend future<_Up>
        __make_future_async_shared_state(_Fp&&);
    template <class _Up, class _Fp>
        friend future<_Up>
        __make_future_deferred_shared_state(_Fp&& __f);
    
    template <class _Tp>
        friend future<typename std::decay<_Tp>::type>
        make_ready_future(_Tp&&);
    friend future<void>
        make_ready_future();
    template <typename _Tp>
        friend future<_Tp>
        make_ready_future(std::exception_ptr);
    template <typename _Tp, typename _Exc>
        friend typename std::enable_if
            <
                !std::is_same<_Tp, _Exc>::value,
                future<_Tp>
            >::type
        make_ready_future(_Exc);
    
    typedef typename __future_traits<_Rp>::move_dest_type move_dest_type;
    
    future(future_ptr __f)
        : _Base(__f)
        {}
    
    future(const future&) _DELETED_;
    future& operator=(const future&) _DELETED_;
    
    
public:
    typedef __future_state::state state;
    typedef _Rp value_type;
    
    CONSTEXPR future()
        {}
    
    FORCE_INLINE
    future(future&& __o) _NOEXCEPT
        : _Base(std::move(static_cast<_Base&>(__o)))
        {}
    
    FORCE_INLINE
    ~future()
        {}
    
    FORCE_INLINE
    future& operator=(future&& __o) _NOEXCEPT
        {
            this->_Base::operator=(std::move(static_cast<_Base&>(__o)));
            return *this;
        }
    
    FORCE_INLINE
    shared_future<_Rp> share()
        {
            return shared_future<_Rp>(std::move(*this));
        }
    
    FORCE_INLINE
    void swap(future& __o)
        {
            static_cast<_Base*>(this)->swap(__o);
        }
    
private:
    FORCE_INLINE
    void set_async()
        {
            this->__future_->set_async();
        }
    
    FORCE_INLINE
    void set_deferred()
        {
            this->__future_->set_deferred();
        }
    
public:
    
    /////////////////////////////////////////////////////
    // retrieving the encapsulated value
    
    FORCE_INLINE
    move_dest_type get()
        {
            if (!bool(this->__future_))
                throw future_uninitialized();
            future_ptr __fut = this->__future_;
            this->__future_.reset();
            return __fut->get();
        }
    
    FORCE_INLINE
    move_dest_type get_or(_Rp&& __v)
        {
            if (!bool(this->__future_))
                throw future_uninitialized();
            
            this->__future_->wait(false);
            future_ptr __fut = this->__future_;
            this->__future_.reset();
            if (__fut->has_value())
                return __fut->get();
            else
                return std::move(__v);
        }
    
    FORCE_INLINE
    move_dest_type get_or(const _Rp& __v)
        {
            if (!bool(this->__future_))
                throw future_uninitialized();
            
            this->__future_->wait(false);
            future_ptr __fut = this->__future_;
            this->__future_.reset();
            if (__fut->has_value())
                return __fut->get();
            else
                return __v;
        }
    
    /////////////////////////////////////////////////////
    // checking state
    
    // These are all implemented by __basic_future
    /*
    bool valid() const _NOEXCEPT;
    void wait() const
    
    template <class Rep, class Period>
        future_status
        wait_for(const chrono::duration<Rep, Period>& rel_time) const;
    
    template <class Clock, class Duration>
        future_status
        wait_until(const chrono::time_point<Clock, Duration>& abs_time) const;
     */
    
    bool ready() const
        {
            return this->is_ready();
        }
    
    ////////////////////////////////////////////////////
    // continuation support
    
    template <typename _Fp>
        inline FORCE_INLINE
        future<typename std::result_of<_Fp(future)>::type>
        then(_Fp&& __func);
    
    template <typename _Fp>
        inline FORCE_INLINE
        future<typename std::result_of<_Fp(future)>::type>
        then(executor& __exec, _Fp&& __func);
    
    template <typename _Fp>
        inline FORCE_INLINE
        future<typename std::result_of<_Fp(future)>::type>
        then(launch __policy, _Fp&& __func);
    
    ////////////////////////////////////////////////////
    // unwrapping
    
    inline FORCE_INLINE
    future<_R2> unwrap();
};

template <typename _Rp>
class shared_future
    : public __basic_future<_Rp>
{
    typedef __basic_future<_Rp> _Base;
    typedef typename _Base::future_ptr future_ptr;
    
    friend class __future_waiter;
    friend class promise<_Rp>;
    
    template <typename, typename, typename>
        friend struct __future_async_continuation_shared_state;
    template <typename, typename, typename>
        friend struct __future_deferred_continuation_shared_state;
    template <class _F, class _Up, class _Fp>
        friend future<_Up>
        __make_future_async_continuation_shared_state(std::unique_lock<std::mutex>&, _F&&, _Fp&&);
    template <class _F, class _Up, class _Fp>
        friend future<_Up>
        __make_future_deferred_continuation_shared_state(std::unique_lock<std::mutex>&, _F&&, _Fp&&);
    
    template <typename, typename>
        friend struct __future_unwrap_shared_state;
    template <class _F, class _Up>
        friend future<_Up>
        __make_future_unwrap_shared_state(std::unique_lock<std::mutex>&, _F&&);
    
    template <class _Tp>
        friend shared_future<typename std::decay<_Tp>::type>
        make_ready_shared_future(_Tp&&);
    friend shared_future<void>
        make_ready_shared_future();
    template <typename _Tp>
        friend shared_future<_Tp>
        make_ready_shared_future(std::exception_ptr);
    template <typename _Tp, typename _Exc>
        friend typename std::enable_if
            <
                !std::is_same<_Tp, _Exc>::value,
                shared_future<_Tp>
            >::type
        make_ready_shared_future(_Exc);
    
    template <class>
        friend class packaged_task;
    
    FORCE_INLINE
    shared_future(future_ptr __f)
        : _Base(__f)
        {}
    
public:
    typedef __future_state::state state;
    typedef _Rp value_type;
    
    CONSTEXPR
    shared_future()
        {}
    
    FORCE_INLINE
    shared_future(const shared_future& __o)
        : _Base(__o)
        {}
    
    FORCE_INLINE
    shared_future(shared_future&& __o) _NOEXCEPT
        : _Base(std::move(__o))
        {
            __o.__future_.reset();
        }
    
    FORCE_INLINE
    shared_future(future<_Rp>&& __o) _NOEXCEPT
        : _Base(std::move(__o))
        {}
    
    FORCE_INLINE
    ~shared_future()
        {}
    
    shared_future& operator=(const shared_future& __o)
        {
            shared_future(__o).swap(*this);
            return *this;
        }
    
    shared_future& operator=(shared_future&& __o) _NOEXCEPT
        {
            this->_Base::operator=(std::move(static_cast<_Base&>(__o)));
            return *this;
        }
    
    void swap(shared_future& __o)
        {
            static_cast<_Base*>(this)->swap(__o);
        }
    
    /////////////////////////////////////////////////////
    // retrieving the encapsulated value
    
    FORCE_INLINE
    typename __shared_state<_Rp>::shared_future_get_result_type get()
        {
            if (!bool(this->__future_))
                throw future_uninitialized();
            return this->__future_->get_sh();
        }
    
    template <typename _R2>
    FORCE_INLINE
    typename std::enable_if<!std::is_void<_R2>::value, typename __shared_state<_Rp>::shared_future_get_result_type>::type
    get_or(_R2&& __v)
        {
            if (!bool(this->__future_))
                throw future_uninitialized();
            
            future_ptr __fut = this->__future_;
            __fut->wait();
            if (__fut->has_value())
                return __fut.get_sh();
            else
                return std::move(__v);
        }
    
    /////////////////////////////////////////////////////
    // checking state
    
    // These are all implemented by __basic_future
    /*
    bool valid() const _NOEXCEPT;
    void wait() const
    
    template <class Rep, class Period>
        future_status
        wait_for(const chrono::duration<Rep, Period>& rel_time) const;
    
    template <class Clock, class Duration>
        future_status
        wait_until(const chrono::time_point<Clock, Duration>& abs_time) const;
     */
    
    bool ready() const
        {
            return this->is_ready();
        }
    
    ////////////////////////////////////////////////////
    // continuation support
    
    template <typename _Fp>
        inline FORCE_INLINE
        future<typename std::result_of<_Fp(shared_future)>::type>
        then(_Fp&& __func);
    
    template <typename _Fp>
        inline FORCE_INLINE
        future<typename std::result_of<_Fp(shared_future)>::type>
        then(executor& __exec, _Fp&& __func);
    
    template <typename _Fp>
        inline FORCE_INLINE
        future<typename std::result_of<_Fp(shared_future)>::type>
        then(launch __policy, _Fp&& __func);
};

template <typename _Rp>
inline FORCE_INLINE
__basic_future<_Rp>::__basic_future(const shared_future<_Rp>& __o)
    : __future_(__o.__future_)
{
}

template <typename _Rp>
class promise
{
    typedef std::shared_ptr<__shared_state<_Rp>> future_ptr;
    
    future_ptr  __future_;
    bool        __future_obtained_;
    
    FORCE_INLINE
    void lazy_init()
        {
            if (!std::atomic_load(&__future_))
            {
                future_ptr __p;
                std::atomic_compare_exchange_strong(&__future_, &__p, future_ptr(new __shared_state<_Rp>));
            }
        }
    
    promise(const promise&) _DELETED_;
    promise& operator=(const promise&) _DELETED_;
    
public:
    template <class _Alloc>
    FORCE_INLINE
    promise(std::allocator_arg_t, _Alloc __a)
        {
            typedef typename _Alloc::template rebind<__shared_state<_Rp>>::other _A2;
            _A2 __a2(__a);
            __future_ = future_ptr(::new(__a2.allocate(1)) __shared_state<_Rp>());
            __future_obtained_ = false;
        }
    
    FORCE_INLINE
    promise()
        : __future_(), __future_obtained_(false)
        {}
    
    ~promise()
        {
            if (bool(__future_))
            {
                std::unique_lock<std::mutex> __lk(__future_->__mutex_);
                if (!__future_->__done_ && !__future_->__is_constructed_)
                {
                    __future_->mark_exceptional_finish_internal(std::make_exception_ptr(broken_promise()), __lk);
                }
            }
        }
    
    ///////////////////////////////////////////////////////////////
    // assignment
    
    FORCE_INLINE
    promise(promise&& __o) _NOEXCEPT
        : __future_(std::move(__o.__future_)), __future_obtained_(__o.__future_obtained_)
        {
            __o.__future_obtained_ = false;
        }
    
    promise& operator=(promise&& __o) _NOEXCEPT
        {
            __future_ = std::move(__o.__future_);
            __future_obtained_ = __o.__future_obtained_;
            __o.__future_obtained_ = false;
            return *this;
        }
    
    void swap(promise& __o)
        {
            __future_.swap(__o.__future_);
            std::swap(__future_obtained_, __o.__future_obtained_);
        }
    
    ///////////////////////////////////////////////////////////////
    // result handling
    
    future<_Rp> get_future()
        {
            lazy_init();
            if (!bool(__future_))
                throw promise_moved();
            if (__future_obtained_)
                throw future_already_retrieved();
            
            __future_obtained_ = true;
            return future<_Rp>(__future_);
        }
    
    void set_value(typename __future_traits<_Rp>::source_reference_type __r)
        {
            lazy_init();
            std::unique_lock<std::mutex> __lk(__future_->__mutex_);
            if (__future_->__done_)
                throw promise_already_satisfied();
            
            __future_->mark_finished_with_result_internal(std::forward<_Rp>(__r), __lk);
        }
    
    void set_value(typename __future_traits<_Rp>::rvalue_source_type __r)
        {
            lazy_init();
            std::unique_lock<std::mutex> __lk(__future_->__mutex_);
            if (__future_->__done_)
                throw promise_already_satisfied();
            
            __future_->mark_finished_with_result_internal(std::forward<_Rp>(__r), __lk);
        }
    
    void set_exception(std::exception_ptr __e)
        {
            lazy_init();
            std::unique_lock<std::mutex> __lk(__future_->__mutex_);
            if (__future_->__done_)
                throw promise_already_satisfied();
            
            __future_->mark_exceptional_finish_internal(__e, __lk);
        }
    
    template <typename _E>
    FORCE_INLINE
    void set_exception(_E __e)
        {
            set_exception(std::make_exception_ptr(__e));
        }
    
    ///////////////////////////////////////////////////////////////
    // deferred result notification
    
    FORCE_INLINE
    void set_value_at_thread_exit(const _Rp& __r)
        {
            if (!bool(__future_))
                throw promise_moved();
            __future_->set_value_at_thread_exit(__r);
        }
    
    FORCE_INLINE
    void set_value_at_thread_exit(_Rp&& __r)
        {
            if (!bool(__future_))
                throw promise_moved();
            __future_->set_value_at_thread_exit(std::move(__r));
        }
    
    FORCE_INLINE
    void set_exception_at_thread_exit(std::exception_ptr __e)
        {
            if(!bool(__future_))
                throw promise_moved();
            __future_->set_exception_at_thread_exit(__e);
        }
    
    template <typename _E>
    FORCE_INLINE
    void set_exception_at_thread_exit(_E __e)
        {
            set_exception_at_thread_exit(std::make_exception_ptr(__e));
        }
    
    /////////////////////////////////////////////////////////////
    // async wait
    
    template <typename _Fp>
    FORCE_INLINE
    void set_wait_callback(_Fp __f)
        {
            lazy_init();
            __future_->set_wait_callback(__f, this);
        }
};

template <typename _Rp>
class promise<_Rp&>
{
    typedef std::shared_ptr<__shared_state<_Rp&>> future_ptr;
    
    future_ptr  __future_;
    bool        __future_obtained_;
    
    FORCE_INLINE
    void lazy_init()
        {
            if (!std::atomic_load(&__future_))
            {
                future_ptr __p;
                std::atomic_compare_exchange_strong(&__future_, &__p, future_ptr(new __shared_state<_Rp&>));
            }
        }
    
    promise(const promise&) _DELETED_;
    promise& operator=(const promise&) _DELETED_;
    
public:
    template <class _Alloc>
    FORCE_INLINE
    promise(std::allocator_arg_t, _Alloc __a)
        {
            typedef typename _Alloc::template rebind<__shared_state<_Rp&>>::other _A2;
            _A2 __a2(__a);
            __future_ = future_ptr(::new(__a2.allocate(1)) __shared_state<_Rp&>());
            __future_obtained_ = false;
        }
    
    FORCE_INLINE
    promise()
        : __future_(), __future_obtained_(false)
        {}
    
    ~promise()
        {
            if (bool(__future_))
            {
                std::unique_lock<std::mutex> __lk(__future_->__mutex_);
                if (!__future_->__done_ && !__future_->__is_constructed_)
                {
                    __future_->mark_exceptional_finish_internal(std::make_exception_ptr(broken_promise()), __lk);
                }
            }
        }
    
    ///////////////////////////////////////////////////////////////
    // assignment
    
    FORCE_INLINE
    promise(promise&& __o) _NOEXCEPT
        : __future_(std::move(__o.__future_)), __future_obtained_(__o.__future_obtained_)
        {
            __o.__future_obtained_ = false;
        }
    
    promise& operator=(promise&& __o) _NOEXCEPT
        {
            __future_ = std::move(__o.__future_);
            __future_obtained_ = __o.__future_obtained_;
            __o.__future_obtained_ = false;
            return *this;
        }
    
    void swap(promise& __o)
        {
            __future_.swap(__o.__future_);
            std::swap(__future_obtained_, __o.__future_obtained_);
        }
    
    ///////////////////////////////////////////////////////////////
    // result handling
    
    future<_Rp&> get_future()
        {
            lazy_init();
            if (!bool(__future_))
                throw promise_moved();
            if (__future_obtained_)
                throw future_already_retrieved();
            
            __future_obtained_ = true;
            return future<_Rp&>(__future_);
        }
    
    void set_value(_Rp& __r)
        {
            lazy_init();
            std::unique_lock<std::mutex> __lk(__future_->__mutex_);
            if (__future_->__done_)
                throw promise_already_satisfied();
            
            __future_->mark_finished_with_result_internal(__r, __lk);
        }
    
    void set_exception(std::exception_ptr __e)
        {
            lazy_init();
            std::unique_lock<std::mutex> __lk(__future_->__mutex_);
            if (__future_->__done_)
                throw promise_already_satisfied();
            
            __future_->mark_exceptional_finish_internal(__e, __lk);
        }
    
    template <typename _E>
    FORCE_INLINE
    void set_exception(_E __e)
        {
            set_exception(std::make_exception_ptr(__e));
        }
    
    ///////////////////////////////////////////////////////////////
    // deferred result notification
    
    FORCE_INLINE
    void set_value_at_thread_exit(_Rp& __r)
        {
            if (!bool(__future_))
                throw promise_moved();
            __future_->set_value_at_thread_exit(__r);
        }
    
    FORCE_INLINE
    void set_exception_at_thread_exit(std::exception_ptr __e)
        {
            if(!bool(__future_))
                throw promise_moved();
            __future_->set_exception_at_thread_exit(__e);
        }
    
    template <typename _E>
    FORCE_INLINE
    void set_exception_at_thread_exit(_E __e)
        {
            set_exception_at_thread_exit(std::make_exception_ptr(__e));
        }
    
    /////////////////////////////////////////////////////////////
    // async wait
    
    template <typename _Fp>
    FORCE_INLINE
    void set_wait_callback(_Fp __f)
        {
            lazy_init();
            __future_->set_wait_callback(__f, this);
        }
};

template <>
class promise<void>
{
    typedef std::shared_ptr<__shared_state<void>> future_ptr;
    
    future_ptr  __future_;
    bool        __future_obtained_;
    
    FORCE_INLINE
    void lazy_init()
        {
            if (!std::atomic_load(&__future_))
            {
                future_ptr __p;
                std::atomic_compare_exchange_strong(&__future_, &__p, future_ptr(new __shared_state<void>));
            }
        }
    
    promise(const promise&) _DELETED_;
    promise& operator=(const promise&) _DELETED_;
    
public:
    template <class _Alloc>
    FORCE_INLINE
    promise(std::allocator_arg_t, _Alloc __a)
        {
            typedef typename _Alloc::template rebind<__shared_state<void>>::other _A2;
            _A2 __a2(__a);
            __future_ = future_ptr(::new(__a2.allocate(1)) __shared_state<void>());
            __future_obtained_ = false;
        }
    
    FORCE_INLINE
    promise()
        : __future_(), __future_obtained_(false)
        {}
    
    ~promise()
        {
            if (bool(__future_))
            {
                std::unique_lock<std::mutex> __lk(__future_->__mutex_);
                if (!__future_->__done_ && !__future_->__is_constructed_)
                {
                    __future_->mark_exceptional_finish_internal(std::make_exception_ptr(broken_promise()), __lk);
                }
            }
        }
    
    ///////////////////////////////////////////////////////////////
    // assignment
    
    FORCE_INLINE
    promise(promise&& __o) _NOEXCEPT
        : __future_(std::move(__o.__future_)), __future_obtained_(__o.__future_obtained_)
        {
            __o.__future_obtained_ = false;
        }
    
    promise& operator=(promise&& __o) _NOEXCEPT
        {
            __future_ = std::move(__o.__future_);
            __future_obtained_ = __o.__future_obtained_;
            __o.__future_obtained_ = false;
            return *this;
        }
    
    void swap(promise& __o)
        {
            __future_.swap(__o.__future_);
            std::swap(__future_obtained_, __o.__future_obtained_);
        }
    
    ///////////////////////////////////////////////////////////////
    // result handling
    
    future<void> get_future()
        {
            lazy_init();
            if (!bool(__future_))
                throw promise_moved();
            if (__future_obtained_)
                throw future_already_retrieved();
            
            __future_obtained_ = true;
            return future<void>(__future_);
        }
    
    void set_value()
        {
            lazy_init();
            std::unique_lock<std::mutex> __lk(__future_->__mutex_);
            if (__future_->__done_)
                throw promise_already_satisfied();
            
            __future_->mark_finished_with_result_internal(__lk);
        }
    
    void set_exception(std::exception_ptr __e)
        {
            lazy_init();
            std::unique_lock<std::mutex> __lk(__future_->__mutex_);
            if (__future_->__done_)
                throw promise_already_satisfied();
            
            __future_->mark_exceptional_finish_internal(__e, __lk);
        }
    
    template <typename _E>
    FORCE_INLINE
    void set_exception(_E __e)
        {
            set_exception(std::make_exception_ptr(__e));
        }
    
    ///////////////////////////////////////////////////////////////
    // deferred result notification
    
    FORCE_INLINE
    void set_value_at_thread_exit()
        {
            if (!bool(__future_))
                throw promise_moved();
            __future_->set_value_at_thread_exit();
        }
    
    FORCE_INLINE
    void set_exception_at_thread_exit(std::exception_ptr __e)
        {
            if(!bool(__future_))
                throw promise_moved();
            __future_->set_exception_at_thread_exit(__e);
        }
    
    template <typename _E>
    FORCE_INLINE
    void set_exception_at_thread_exit(_E __e)
        {
            set_exception_at_thread_exit(std::make_exception_ptr(__e));
        }
    
    /////////////////////////////////////////////////////////////
    // async wait
    
    template <typename _Fp>
    FORCE_INLINE
    void set_wait_callback(_Fp __f)
        {
            lazy_init();
            __future_->set_wait_callback(__f, this);
        }
};

EPUB3_END_NAMESPACE

namespace std
{
    
    template <class _Rp>
    void swap(EPUB3_NAMESPACE::promise<_Rp>& __x, EPUB3_NAMESPACE::promise<_Rp>& __y) _NOEXCEPT
        {
            __x.swap(__y);
        }
    
    template <class _Rp, class _Alloc>
    struct uses_allocator<EPUB3_NAMESPACE::promise<_Rp>, _Alloc>
        : public true_type {};
    
}

EPUB3_BEGIN_NAMESPACE

template <class _Fp, class ..._Args>
class __async_func
{
    std::tuple<_Fp, _Args...> __f_;
    
public:
    typedef typename __invoke_of<_Fp, _Args...>::type    _Rp;
    
    FORCE_INLINE
    explicit
    __async_func(_Fp&& __f, _Args&&... __args)
        : __f_(std::move(__f), std::move(__args)...)
        {}
    
    FORCE_INLINE
    __async_func(__async_func&& __o)
        : __f_(std::move(__o.__f_))
        {}
    
    _Rp operator()()
        {
            typedef typename make_index_sequence<1+sizeof...(_Args), 1>::type _Index;
            return __execute(_Index());
        }
    
private:
    template <std::size_t ..._Indices>
    _Rp __execute(index_sequence<_Indices...>)
        {
            return invoke(std::move(std::get<0>(__f_)), std::move(std::get<_Indices>(__f_))...);
        }
};

template <typename _Rp, typename ..._Args>
class packaged_task<_Rp(_Args...)>
{
    typedef __task_base_shared_state<_Rp(_Args...)> _Task;
    typedef std::shared_ptr<_Task>                  task_ptr;
    
    task_ptr    __task_;
    bool        __future_obtained_;
    struct      __dummy;
    
    packaged_task(const packaged_task&) _DELETED_;
    packaged_task& operator=(const packaged_task&) _DELETED_;
    
public:
    typedef _Rp result_type;
    
    FORCE_INLINE
    packaged_task()
        : __future_obtained_(false)
        {}
    
    template <class _Fp,
                typename std::enable_if
                    <
                        !std::is_same
                        <
                            typename std::decay<_Fp>::type,
                            packaged_task
                        >::value,
                        __dummy*
                    >::type = 0
                >
    explicit FORCE_INLINE
    packaged_task(_Fp&& __f)
        {
            typedef typename std::remove_cv<typename std::remove_reference<_Fp>::type>::type _FR;
            typedef __task_shared_state<_FR, _Rp(_Args...)> _State;
            
            __task_ = task_ptr(new _State(std::forward<_Fp>(__f)));
            __future_obtained_ = false;
        }
    
    template <class _Fp, class _Alloc>
    FORCE_INLINE
    packaged_task(std::allocator_arg_t, _Alloc __a, _Fp&& __f)
        {
            typedef typename std::remove_cv<typename std::remove_reference<_Fp>::type>::type _FR;
            typedef __task_shared_state<_FR, _Rp(_Args...)> _State;
            
            typedef typename _Alloc::template rebind<_State>::other _A2;
            _A2 __a2(__a);
            
            __task_ = task_ptr(::new (__a2.allocate(1)) _State(std::forward<_Fp>(__f)));
            __future_obtained_ = false;
        }
    
    FORCE_INLINE
    ~packaged_task()
        {
            if (!bool(__task_))
                __task_->owner_destroyed();
        }
    
    ////////////////////////////////////////////////////////////////////
    // assignment
    
    FORCE_INLINE
    packaged_task(packaged_task&& __o) _NOEXCEPT
        : __future_obtained_(__o.__future_obtained_)
        {
            __task_.swap(__o.__task_);
            __o.__future_obtained_ = false;
        }
    
    FORCE_INLINE
    packaged_task& operator=(packaged_task&& __o)
        {
            packaged_task __tmp(std::move(__o));
            swap(__tmp);
            return *this;
        }
    
    FORCE_INLINE
    void reset()
        {
            if (!valid())
                throw future_error(std::make_error_code(future_errc::no_state));
            __task_->reset();
            __future_obtained_ = false;
        }
    
    FORCE_INLINE
    void swap(packaged_task& __o)
        {
            __task_.swap(__o.__task_);
            std::swap(__future_obtained_, __o.__future_obtained_);
        }
    
    FORCE_INLINE
    bool valid() const _NOEXCEPT
        {
            return bool(__task_);
        }
    
    //////////////////////////////////////////////////////////////////
    // result retrieval
    
    future<_Rp> get_future()
        {
            if (!bool(__task_))
            {
                throw task_moved();
            }
            else if (!__future_obtained_)
            {
                __future_obtained_ = true;
                return future<_Rp>(__task_);
            }
            else
            {
                throw future_already_retrieved();
            }
        }
    
    //////////////////////////////////////////////////////////////////
    // execution
    
    void operator()(_Args&&... __args)
        {
            if (!bool(__task_))
                throw task_moved();
            __task_->run(std::forward<_Args>(__args)...);
        }
    
    void execute(executor& __exec, _Args&&... __args)
        {
            if (!bool(__task_))
                throw task_moved();
            
            task_ptr __t(__task_);
            executor::closure_type __run = [__t](_Args&&... __a) {
                __t->run(std::forward<_Args>(__a)...);
            };
            
            typedef typename std::decay<decltype(__run)>::type _F;
            typename __invoke_of<_F, typename std::decay<_Args>::type...>::type _R;
            typedef __async_func<_F, typename std::decay<_Args>::type...> _BF;
            
            __exec.add(_BF(decay_copy(std::move(__run), decay_copy(std::forward<_Args>(__args))...)));
        }
    
    FORCE_INLINE
    void make_ready_at_thread_exit(_Args&&... __args)
        {
            if (!bool(__task_))
                throw task_moved();
            if (__task_->has_value())
                throw promise_already_satisfied();
            __task_->apply(std::forward<_Args>(__args)...);
        }
    
    template <typename _Fp>
    FORCE_INLINE
    void set_wait_callback(_Fp __f)
        {
            __task_->set_wait_callback(__f, this);
        }
};

EPUB3_END_NAMESPACE

namespace std
{
    
    template <class _Rp, class _Alloc>
    struct uses_allocator<EPUB3_NAMESPACE::packaged_task<_Rp>, _Alloc>
        : public true_type {};
    
}

EPUB3_BEGIN_NAMESPACE

template <class _Rp, class _Fp>
future<_Rp>
__make_future_deferred_shared_state(_Fp&& __f)
{
    std::shared_ptr<__future_deferred_shared_state<_Rp, _Fp>> __h(new __future_deferred_shared_state<_Rp, _Fp>(std::forward<_Fp>(__f)));
    return future<_Rp>(__h);
}

template <class _Rp, class _Fp>
future<_Rp>
__make_future_async_shared_state(_Fp&& __f)
{
    std::shared_ptr<__future_async_shared_state<_Rp, _Fp>> __h(new __future_async_shared_state<_Rp, _Fp>(std::forward<_Fp>(__f)));
    return future<_Rp>(__h);
}

////////////////////////////////////////////////////////////////////////////
// Free functions

#if 0
#pragma mark - Free functions
#endif

template <class _Fp, class ..._Args>
future<typename __invoke_of<typename std::decay<_Fp>::type, typename std::decay<_Args>::type...>::type>
async(launch __policy, _Fp&& __f, _Args&&... __args)
{
    typedef __async_func<typename std::decay<_Fp>::type, typename std::decay<_Args>::type...> _BF;
    typedef typename _BF::_Rp _Rp;
    
    future<_Rp> __r;
    if (int(__policy) & int(launch::async))
    {
        __r = __make_future_async_shared_state<_Rp>(_BF(decay_copy(std::forward<_Fp>(__f)), decay_copy(std::forward<_Args>(__args))...));
    }
    else if (int(__policy) & int(launch::deferred))
    {
        __r = __make_future_deferred_shared_state<_Rp>(_BF(decay_copy(std::forward<_Fp>(__f)), decay_copy(std::forward<_Args>(__args))...));
    }
    
    return __r;
}
#if !EPUB_COMPILER(MSVC)
template <class _Fp, class ..._Args>
future<typename __invoke_of<typename std::decay<_Fp>::type, typename std::decay<_Args>::type...>::type>
async(_Fp&& __f, _Args&&... __args)
{
    return async(launch::any, std::forward<_Fp>(__f), std::forward<_Args>(__args)...);
}
#endif
template <typename _Tp>
FORCE_INLINE
future<typename std::decay<_Tp>::type>
make_ready_future(_Tp&& __v)
{
    typedef typename std::decay<_Tp>::type _T2;
    typedef typename future<_T2>::future_ptr _F;
    _F __state(new __shared_state<_T2>);
    __state->mark_finished_with_result(std::move(__v));
    return std::move(future<_T2>(__state));
}

inline FORCE_INLINE
future<void>
make_ready_future()
{
#if EPUB_COMPILER(MSVC)
	typedef future<void>::future_ptr _F;
#else
    typedef typename future<void>::future_ptr _F;
#endif
    _F __state(new __shared_state<void>);
    __state->mark_finished_with_result();
    return future<void>(__state);
}

template <typename _Tp>
FORCE_INLINE
future<_Tp>
make_ready_future(std::exception_ptr __exc)
{
    typedef typename future<_Tp>::future_ptr _F;
    _F __state(new __shared_state<_Tp>);
    std::unique_lock<std::mutex> __lk(__state->__mutex_);
    __state->mark_exceptional_finish_internal(__exc, __lk);
    return future<_Tp>(__state);
}

template <typename _Tp, typename _E>
FORCE_INLINE
typename std::enable_if
<
    !std::is_same<_Tp, _E>::value,
    future<_Tp>
>::type
make_ready_future(_E __exc)
{
    typedef typename future<_Tp>::future_ptr _F;
    _F __state(new __shared_state<_Tp>);
    std::unique_lock<std::mutex> __lk(__state->__mutex_);
    __state->mark_exceptional_finish_internal(std::make_exception_ptr(__exc), __lk);
    return future<_Tp>(__state);
}

template <typename _Tp>
FORCE_INLINE
shared_future<typename std::decay<_Tp>::type>
make_ready_shared_future(_Tp&& __v)
{
    typedef typename std::decay<_Tp>::type _T2;
    typedef typename shared_future<_T2>::future_ptr _F;
    _F __state(new __shared_state<_T2>);
    __state->mark_finished_with_result(std::move(__v));
    return shared_future<_T2>(__state);
}

inline FORCE_INLINE
shared_future<void>
make_ready_shared_future()
{
#if EPUB_COMPILER(MSVC)
	typedef shared_future<void>::future_ptr _F;
#else
    typedef typename shared_future<void>::future_ptr _F;
#endif
    _F __state(new __shared_state<void>);
    __state->mark_finished_with_result();
    return shared_future<void>(__state);
}

template <typename _Tp>
FORCE_INLINE
shared_future<_Tp>
make_ready_shared_future(std::exception_ptr __exc)
{
    typedef typename shared_future<_Tp>::future_ptr _F;
    _F __state(new __shared_state<_Tp>);
    std::unique_lock<std::mutex> __lk(__state->__mutex_);
    __state->mark_exceptional_finish_internal(__exc, __lk);
    return shared_future<_Tp>(__state);
}

template <typename _Tp, typename _E>
FORCE_INLINE
typename std::enable_if
<
    !std::is_same<_Tp, _E>::value,
    shared_future<_Tp>
>::type
make_ready_shared_future(_E __e)
{
    typedef typename shared_future<_Tp>::future_ptr _F;
    _F __state(new __shared_state<_Tp>);
    std::unique_lock<std::mutex> __lk(__state->__mutex_);
    __state->mark_exceptional_finish_internal(std::make_exception_ptr(__e), __lk);
    return shared_future<_Tp>(__state);
}

template <typename _Fut, typename _Rp, typename _Fp>
struct __future_async_continuation_shared_state
    : __future_async_shared_state_base<_Rp>
{
    _Fut    __parent_;
    _Fp     __continuation_;
    
public:
    FORCE_INLINE
    __future_async_continuation_shared_state(_Fut&& __f, _Fp&& __c)
        : __parent_(std::move(__f)), __continuation_(std::move(__c))
        {}
    
    FORCE_INLINE
    void launch_continuation(std::unique_lock<std::mutex>& __lk)
        {
            __lk.unlock();
            this->__thr_ = std::thread(&__future_async_continuation_shared_state::run, this);
        }
    
    static
    void run(__future_async_continuation_shared_state* __that)
    {
        try
        {
            __that->mark_finished_with_result(__that->__continuation_(std::move(__that->__parent_)));
        }
        catch (...)
        {
            __that->mark_exceptional_finish();
        }
    }
    
    FORCE_INLINE
    ~__future_async_continuation_shared_state()
        {
            this->join();
        }
};

template <typename _Fut, typename _Fp>
struct __future_async_continuation_shared_state<_Fut, void, _Fp>
    : public __future_async_shared_state_base<void>
{
    _Fut    __parent_;
    _Fp     __continuation_;
    
public:
    FORCE_INLINE
    __future_async_continuation_shared_state(_Fut&& __f, _Fp&& __c)
        : __parent_(std::move(__f)), __continuation_(std::move(__c))
        {}
    
    FORCE_INLINE
    void launch_continuation(std::unique_lock<std::mutex>& __lk)
        {
            __lk.unlock();
            this->__thr_ = std::thread(&__future_async_continuation_shared_state::run, this);
        }
    
    static
    void run(__future_async_continuation_shared_state& __that)
    {
        try
        {
            __that->__continuation_(std::move(__that->__parent_));
            __that->mark_finished_with_result();
        }
        catch (...)
        {
            __that->mark_exceptional_finish();
        }
    }
    
    FORCE_INLINE
    ~__future_async_continuation_shared_state()
        {
            this->join();
        }
};

template <typename _Fut, typename _Rp, typename _Fp>
struct __future_deferred_continuation_shared_state
    : __shared_state<_Rp>
{
    _Fut    __parent_;
    _Fp     __continuation_;
    
public:
    FORCE_INLINE
    __future_deferred_continuation_shared_state(_Fut&& __f, _Fp&& __c)
    : __parent_(std::move(__f)), __continuation_(std::move(__c))
        {
            this->set_deferred();
        }
    
    virtual
    void launch_continuation(std::unique_lock<std::mutex>& __lk)
        {
            execute(__lk);
        }
    
    virtual
    void execute(std::unique_lock<std::mutex>& __lk)
        {
            //__relocker __relock(__lk);
            
            try
            {
                // stop the parent from calling this continuation when its value is set
                __parent_.__future_->__continuation_ptr_.reset();
                this->mark_finished_with_result_internal(__continuation_(std::move(__parent_)), __lk);
            }
            catch (...)
            {
                this->mark_exceptional_finish_internal(std::current_exception(), __lk);
            }
        }
};

template <typename _Fut, typename _Fp>
struct __future_deferred_continuation_shared_state<_Fut, void, _Fp>
    : __shared_state<void>
{
    _Fut    __parent_;
    _Fp     __continuation_;
    
public:
    FORCE_INLINE
    __future_deferred_continuation_shared_state(_Fut&& __f, _Fp&& __c)
        : __parent_(std::move(__f)), __continuation_(std::move(__c))
        {
            this->set_deferred();
        }
    
    virtual
    void launch_continuation(std::unique_lock<std::mutex>& __lk)
        {
            execute(__lk);
        }
    
    virtual
    void execute(std::unique_lock<std::mutex>& __lk)
        {
            //__relocker __relock(__lk);
            
            try
            {
                // stop the parent from calling this continuation when its value is set
                __parent_.__future_->__continuation_ptr_.reset();
                __continuation_(std::move(__parent_));
                this->mark_finished_with_result_internal(__lk);
            }
            catch (...)
            {
                this->mark_exceptional_finish_internal(std::current_exception(), __lk);
            }
        }
};

template <typename _Fut, typename _Rp, typename _Fp>
struct __future_executor_continuation_shared_state
    : __shared_state<_Rp>
{
    _Fut        __parent_;
    _Fp         __continuation_;
    executor*   __target_;
    
public:
    FORCE_INLINE
    __future_executor_continuation_shared_state(_Fut&& __f, executor* __exec, _Fp&& __c)
        : __parent_(std::move(__f)), __continuation_(std::move(__c)), __target_(__exec)
        {}
    
    virtual
    void launch_continuation(std::unique_lock<std::mutex>& __lk)
        {
            assert(__target_ != nullptr);
            __relocker __relock(__lk);
            
            auto self = this->shared_from_this();
            __target_->add([this, self]() {
                try
                {
                    this->mark_finished_with_result(this->__continuation_(std::move(this->__parent_)));
                }
                catch (...)
                {
                    this->mark_exceptional_finish();
                }
            });
        }
};

template <typename _Fut, typename _Fp>
struct __future_executor_continuation_shared_state<_Fut, void, _Fp>
    : __shared_state<void>
{
    _Fut        __parent_;
    _Fp         __continuation_;
    executor*   __target_;
    
public:
    FORCE_INLINE
    __future_executor_continuation_shared_state(_Fut&& __f, executor* __exec, _Fp&& __c)
        : __parent_(std::move(__f)), __continuation_(std::move(__c)), __target_(__exec)
        {}
    
    virtual
    void launch_continuation(std::unique_lock<std::mutex>& __lk)
        {
            assert(__target_ != nullptr);
            __lk.unlock();
            
            auto self = this->shared_from_this();
            __target_->add([this, self]() {
                try
                {
                    this->__continuation_(std::move(this->__parent_));
                    this->mark_finished_with_result();
                }
                catch (...)
                {
                    this->mark_exceptional_finish();
                }
            });
        }
};

template <typename _Fut, typename _Rp, typename _Fp>
future<_Rp>
__make_future_deferred_continuation_shared_state(std::unique_lock<std::mutex>& __lk, _Fut&& __f, _Fp&& __c)
{
    std::shared_ptr<__future_deferred_continuation_shared_state<_Fut, _Rp, _Fp>> __h(new __future_deferred_continuation_shared_state<_Fut, _Rp, _Fp>(std::move(__f), std::forward<_Fp>(__c)));
    __h->__parent_.__future_->set_continuation_ptr(__h, __lk);
    return future<_Rp>(__h);
}

template <typename _Fut, typename _Rp, typename _Fp>
future<_Rp>
__make_future_async_continuation_shared_state(std::unique_lock<std::mutex>& __lk, _Fut&& __f, _Fp&& __c)
{
    std::shared_ptr<__future_async_continuation_shared_state<_Fut, _Rp, _Fp>> __h(new __future_async_continuation_shared_state<_Fut, _Rp, _Fp>(std::move(__f), std::forward<_Fp>(__c)));
    __h->__parent_.__future_->set_continuation_ptr(__h, __lk);
    return future<_Rp>(__h);
}

template <typename _Fut, typename _Rp, typename _Fp>
future<_Rp>
__make_future_executor_continuation_shared_state(std::unique_lock<std::mutex>& __lk, _Fut&& __f, executor* __exec, _Fp&& __c)
{
    std::shared_ptr<__future_executor_continuation_shared_state<_Fut, _Rp, _Fp>> __h(new __future_executor_continuation_shared_state<_Fut, _Rp, _Fp>(std::move(__f), __exec, std::forward<_Fp>(__c)));
    __h->__parent_.__future_->set_continuation_ptr(__h, __lk);
    return future<_Rp>(__h);
}

//////////////////////////////////////////////////////////////////////////////
// future::then

template <typename _Rp>
template <typename _Fp>
inline FORCE_INLINE
future<typename std::result_of<_Fp(future<_Rp>)>::type>
future<_Rp>::then(launch __policy, _Fp&& __func)
{
    typedef typename std::result_of<_Fp(future<_Rp>)>::type _Fut;
    if (!bool(this->__future_))
        throw future_uninitialized();
    
    std::unique_lock<std::mutex> __lk(this->__future_->__mutex_);
    if (int(__policy) & int(launch::async))
    {
        return std::move(__make_future_async_continuation_shared_state<future<_Rp>, _Fut, _Fp>(__lk, std::move(*this), std::forward<_Fp>(__func)));
    }
    else if (int(__policy) & int(launch::deferred))
    {
        return std::move(__make_future_deferred_continuation_shared_state<future<_Rp>, _Fut, _Fp>(__lk, std::move(*this), std::forward<_Fp>(__func)));
    }
    else
    {
        return std::move(__make_future_async_continuation_shared_state<future<_Rp>, _Fut, _Fp>(__lk, std::move(*this), std::forward<_Fp>(__func)));
    }
}

template <typename _Rp>
template <typename _Fp>
inline FORCE_INLINE
future<typename std::result_of<_Fp(future<_Rp>)>::type>
future<_Rp>::then(_Fp&& __func)
{
    typedef typename std::result_of<_Fp(future<_Rp>)>::type _Fut;
    if (!bool(this->__future_))
        throw future_uninitialized();
    
    std::unique_lock<std::mutex> __lk(this->__future_->__mutex_);
    if (int(this->launch_policy(__lk)) & int(launch::async))
    {
        return std::move(__make_future_async_continuation_shared_state<future<_Rp>, _Fut, _Fp>(__lk, std::move(*this), std::forward<_Fp>(__func)));
    }
    else if (int(this->launch_policy(__lk)) & int(launch::deferred))
    {
        return std::move(__make_future_deferred_continuation_shared_state<future<_Rp>, _Fut, _Fp>(__lk, std::move(*this), std::forward<_Fp>(__func)));
    }
    else
    {
        return std::move(__make_future_async_continuation_shared_state<future<_Rp>, _Fut, _Fp>(__lk, std::move(*this), std::forward<_Fp>(__func)));
    }
}

template <typename _Rp>
template <typename _Fp>
inline FORCE_INLINE
future<typename std::result_of<_Fp(future<_Rp>)>::type>
future<_Rp>::then(executor& __exec, _Fp&& __func)
{
    typedef typename std::result_of<_Fp(future<_Rp>)>::type _Fut;
    if (!bool(this->__future_))
        throw future_uninitialized();
    
    std::unique_lock<std::mutex> __lk(this->__future_->__mutex_);
    return std::move(__make_future_executor_continuation_shared_state<future<_Rp>, _Fut, _Fp>(__lk, std::move(*this), &__exec, std::forward<_Fp>(__func)));
}

template <typename _Rp>
template <typename _Fp>
inline FORCE_INLINE
future<typename std::result_of<_Fp(shared_future<_Rp>)>::type>
shared_future<_Rp>::then(launch __policy, _Fp&& __func)
{
    typedef typename std::result_of<_Fp(future<_Rp>)>::type _Fut;
    if (!bool(this->__future_))
        throw future_uninitialized();
    
    std::unique_lock<std::mutex> __lk(this->__future_->__mutex_);
    if (int(__policy) & int(launch::async))
    {
        return std::move(__make_future_async_continuation_shared_state<shared_future<_Rp>, _Fut, _Fp>(__lk, std::move(*this), std::forward<_Fp>(__func)));
    }
    else if (int(__policy) & int(launch::deferred))
    {
        return std::move(__make_future_deferred_continuation_shared_state<shared_future<_Rp>, _Fut, _Fp>(__lk, std::move(*this), std::forward<_Fp>(__func)));
    }
    else
    {
        return std::move(__make_future_async_continuation_shared_state<shared_future<_Rp>, _Fut, _Fp>(__lk, std::move(*this), std::forward<_Fp>(__func)));
    }
}

template <typename _Rp>
template <typename _Fp>
inline FORCE_INLINE
future<typename std::result_of<_Fp(shared_future<_Rp>)>::type>
shared_future<_Rp>::then(_Fp&& __func)
{
    typedef typename std::result_of<_Fp(future<_Rp>)>::type _Fut;
    if (!bool(this->__future_))
        throw future_uninitialized();
    
    std::unique_lock<std::mutex> __lk(this->__future_->__mutex_);
    if (int(this->launch_policy(__lk)) & int(launch::async))
    {
        return std::move(__make_future_async_continuation_shared_state<shared_future<_Rp>, _Fut, _Fp>(__lk, std::move(*this), std::forward<_Fp>(__func)));
    }
    else if (int(this->launch_policy(__lk)) & int(launch::deferred))
    {
        return std::move(__make_future_deferred_continuation_shared_state<shared_future<_Rp>, _Fut, _Fp>(__lk, std::move(*this), std::forward<_Fp>(__func)));
    }
    else
    {
        return std::move(__make_future_async_continuation_shared_state<shared_future<_Rp>, _Fut, _Fp>(__lk, std::move(*this), std::forward<_Fp>(__func)));
    }
}

template <typename _Rp>
template <typename _Fp>
inline FORCE_INLINE
future<typename std::result_of<_Fp(shared_future<_Rp>)>::type>
shared_future<_Rp>::then(executor& __exec, _Fp&& __func)
{
    typedef typename std::result_of<_Fp(future<_Rp>)>::type _Fut;
    if (!bool(this->__future_))
        throw future_uninitialized();
    
    std::unique_lock<std::mutex> __lk(this->__future_->__mutex_);
    return std::move(__make_future_executor_continuation_shared_state<shared_future<_Rp>, _Fut, _Fp>(__lk, std::move(*this), &__exec, std::forward<_Fp>(__func)));
}

EPUB3_END_NAMESPACE

#endif
