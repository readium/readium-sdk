//
//  rw_lock.h
//  ePub3
//
//  Created by Jim Dovey on 2013-02-05.
//  Copyright (c) 2013 The Readium Foundation. All rights reserved.
//

#ifndef __ePub3__rw_lock__
#define __ePub3__rw_lock__

#include "epub3.h"
#include <atomic>

#if EPUB3_TARGET_WINDOWS
# include <WinBase.h>
# include <synchapi.h>      // get SRWLOCK API
#else
# include <pthread.h>
#endif

EPUB3_BEGIN_NAMESPACE

class RWLock
{
public:
#if EPUB3_TARGET_WINDOWS
    typedef SRWLOCK             native_handle_type;
#else
    typedef pthread_rwlock_t    native_handle_type;
#endif
    
public:
#if EPUB3_TARGET_WINDOWS
    constexpr   RWLock()    noexcept : __lock_(SRWLOCK_INIT), __is_writing_(false) {}
#else
    constexpr   RWLock()    noexcept : __lock_(PTHREAD_RWLOCK_INITIALIZER) {}
#endif
                ~RWLock();
    
    void        lock();
    bool        try_lock();
    void        unlock();
    
    void        readlock();
    bool        try_readlock();
    
protected:
    native_handle_type  __lock_;
#if EPUB3_TARGET_WINDOWS
    bool                __is_writing_;
#endif
    
};

class readlock_guard
{
public:
    inline explicit readlock_guard(RWLock& __m) : __m_(__m) { __m.readlock(); }
    inline          readlock_guard(RWLock& __m, std::adopt_lock_t) : __m_(__m) {}
    inline          ~readlock_guard() { __m_.unlock(); }
    
                    readlock_guard(readlock_guard const&)   = delete;
    readlock_guard& operator=(readlock_guard const&)        = delete;
    
private:
    RWLock& __m_;
    
};

EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__rw_lock__) */
