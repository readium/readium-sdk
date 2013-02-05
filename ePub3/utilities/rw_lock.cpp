//
//  rw_lock.cpp
//  ePub3
//
//  Created by Jim Dovey on 2013-02-05.
//  Copyright (c) 2013 The Readium Foundation. All rights reserved.
//

#include "rw_lock.h"

EPUB3_BEGIN_NAMESPACE

#if EPUB3_TARGET_WINDOWS
RWLock::~RWLock()
{
}
void RWLock::lock()
{
    AcquireSRWLockExclusive(&__lock_);
    __is_writing = true;
}
bool RWLock::try_lock()
{
    bool r = static_cast<bool>(TryAcquireSRWLockExclusive(&__lock_));
    if ( r )
        __is_writing_ = true;
    return r;
}
void RWLock::unlock()
{
    if ( __is_writing_ )
        ReleaseSRWLockExclusive(&__lock_);
    else
        ReleaseSRWLockShared(&__lock_);
    __is_writing = false;
}
void RWLock::readlock()
{
    AcquireSRWLockShared(&__lock_);
}
bool RWLock::try_readlock()
{
    return static_cast<bool>(TryAcquireSRWLockShared(&__lock_));
}
#else
RWLock::~RWLock()
{
    pthread_rwlock_destroy(&__lock_);
}
void RWLock::lock()
{
    pthread_rwlock_wrlock(&__lock_);
}
bool RWLock::try_lock()
{
    return (pthread_rwlock_trywrlock(&__lock_) == 0);
}
void RWLock::unlock()
{
    pthread_rwlock_unlock(&__lock_);
}
void RWLock::readlock()
{
    pthread_rwlock_rdlock(&__lock_);
}
bool RWLock::try_readlock()
{
    return (pthread_rwlock_tryrdlock(&__lock_) == 0);
}
#endif

EPUB3_END_NAMESPACE
