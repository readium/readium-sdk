//
//  backup_atomics.h
//  ePub3
//
//  Created by Jim Dovey on 2013-04-16.
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

#ifndef __BACKUP_ATOMICS_H__
#define __BACKUP_ATOMICS_H__

#include <pthread.h>
#include <string.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif
    
void __lock_atomics();
void __unlock_atomics();

#ifdef __cplusplus
};

template <class T>
T
__sw_atomic_load(_Atomic(T) const volatile* obj, int __m)
{
    register T __r;
    __lock_atomics();
    __r = *obj;
    __unlock_atomics();
    return __r;
}

template <class T>
void
__sw_atomic_store(_Atomic(T) volatile* obj, T desr, int __m)
{
    __lock_atomics();
    *obj = desr;
    __unlock_atomics();
}

template <class T>
T
__sw_atomic_exchange(_Atomic(T) volatile* obj, T desr, int __m)
{
    register T __r;
    __lock_atomics();
    __r = *obj;
    *obj = desr;
    __unlock_atomics();
    return __r;
}
template <class T>
bool
__sw_atomic_compare_exchange_strong(_Atomic(T) volatile* obj, T* exp, T desr, int __m)
{
    bool __r;
    __lock_atomics();
    if (::memcmp(const_cast<T*>(obj), exp, sizeof(T)) == 0) // if (*obj == *exp)
    {
        ::memcpy(const_cast<T*>(obj), &desr, sizeof(T)); // *obj = desr;
        __r = true;
    }
    ::memcpy(exp, const_cast<T*>(obj), sizeof(T)); // *exp = *obj;
    __r = false;
    __unlock_atomics();
    return __r;
}

// May spuriously return false (even if *obj == *exp)
template <class T>
bool
__sw_atomic_compare_exchange_weak(_Atomic(T) volatile* obj, T* exp, T desr, int __m)
{
    bool __r;
    __lock_atomics();
    if (::memcmp(const_cast<T*>(obj), exp, sizeof(T)) == 0) // if (*obj == *exp)
    {
        ::memcpy(const_cast<T*>(obj), &desr, sizeof(T)); // *obj = desr;
        __r = true;
    }
    ::memcpy(exp, const_cast<T*>(obj), sizeof(T)); // *exp = *obj;
    __r = false;
    __unlock_atomics();
    return __r;
}

template <class T>
T
__sw_atomic_fetch_add(_Atomic(T) volatile* obj, T operand, int __m)
{
    register T __r;
    __lock_atomics();
    __r = *obj;
    *obj += operand;
    __unlock_atomics();
    return __r;
}

template <class T>
T
__sw_atomic_fetch_sub(_Atomic(T) volatile* obj, T operand, int __m)
{
    register T __r;
    __lock_atomics();
    __r = *obj;
    *obj -= operand;
    __unlock_atomics();
    return __r;
}

template <class T>
T
__sw_atomic_fetch_and(_Atomic(T) volatile* obj, T operand, int __m)
{
    register T __r;
    __lock_atomics();
    __r = *obj;
    *obj &= operand;
    __unlock_atomics();
    return __r;
    
}

template <class T>
T
__sw_atomic_fetch_or(_Atomic(T) volatile* obj, T operand, int __m)
{
    register T __r;
    __lock_atomics();
    __r = *obj;
    *obj |= operand;
    __unlock_atomics();
    return __r;
}

template <class T>
T
__sw_atomic_fetch_xor(_Atomic(T) volatile* obj, T operand, int __m)
{
    register T __r;
    __lock_atomics();
    __r = *obj;
    *obj ^= operand;
    __unlock_atomics();
    return __r;
}
/*
void*
__atomic_fetch_add(void* volatile* obj, ptrdiff_t operand)
{
    std::unique_lock<std::mutex> _(___OMFG_WHERE_ARE_ALL_THE_ATOMIC_INTRINSICS__);
    void* r = *obj;
    (char*&)(*obj) += operand;
    return r;
}

void*
__atomic_fetch_sub(void* volatile* obj, ptrdiff_t operand)
{
    std::unique_lock<std::mutex> _(___OMFG_WHERE_ARE_ALL_THE_ATOMIC_INTRINSICS__);
    void* r = *obj;
    (char*&)(*obj) -= operand;
    return r;
}
*/

static __attribute__ ((__always_inline__)) void __sw_atomic_thread_fence(int __m)
{
    __lock_atomics();
    __unlock_atomics();
}

static __attribute__ ((__always_inline__)) void __sw_atomic_signal_fence(int __m)
{
    __lock_atomics();
    __unlock_atomics();
}

//#define __c11_atomic_load                       __sw_atomic_load
//#define __c11_atomic_store                      __sw_atomic_store
//#define __c11_atomic_exchange                   __sw_atomic_exchange
#define __c11_atomic_compare_exchange_strong    __sw_atomic_compare_exchange_strong
#define __c11_atomic_compare_exchange_weak      __sw_atomic_compare_exchange_weak
#define __c11_atomic_fetch_add                  __sw_atomic_fetch_add
#define __c11_atomic_fetch_sub                  __sw_atomic_fetch_sub
#define __c11_atomic_fetch_and                  __sw_atomic_fetch_and
#define __c11_atomic_fetch_or                   __sw_atomic_fetch_or
#define __c11_atomic_fetch_xor                  __sw_atomic_fetch_xor

#endif  /* __cplusplus */

#endif  /* __BACKUP_ATOMICS_H__ */