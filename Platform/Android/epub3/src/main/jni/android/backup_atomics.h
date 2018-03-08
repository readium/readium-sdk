//
//  backup_atomics.h
//  ePub3
//
//  Created by Jim Dovey on 2013-04-16.
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

#ifndef __BACKUP_ATOMICS_H__
#define __BACKUP_ATOMICS_H__
//
//#include <pthread.h>
//#include <string.h>
//#include <stdlib.h>
//
//#ifdef __cplusplus
//extern "C" {
//#endif
//
//void __lock_atomics();
//void __unlock_atomics();
//
//#ifdef __cplusplus
//};
//
//template <class T>
//T
//__sw_atomic_load(_Atomic(T) const volatile* obj, int __m)
//{
//    register T __r;
//    __lock_atomics();
//    __r = *obj;
//    __unlock_atomics();
//    return __r;
//}
//
//template <class T>
//void
//__sw_atomic_store(_Atomic(T) volatile* obj, T desr, int __m)
//{
//    __lock_atomics();
//    *obj = desr;
//    __unlock_atomics();
//}
//
//template <class T>
//T
//__sw_atomic_exchange(_Atomic(T) volatile* obj, T desr, int __m)
//{
//    register T __r;
//    __lock_atomics();
//    __r = *obj;
//    *obj = desr;
//    __unlock_atomics();
//    return __r;
//}
//template <class T>
//bool
//__sw_atomic_compare_exchange_strong(_Atomic(T) volatile* obj, T* exp, T desr, int __m)
//{
//    bool __r;
//    __lock_atomics();
//    if (::memcmp(const_cast<T*>(obj), exp, sizeof(T)) == 0) // if (*obj == *exp)
//    {
//        ::memcpy(const_cast<T*>(obj), &desr, sizeof(T)); // *obj = desr;
//        __r = true;
//    }
//    ::memcpy(exp, const_cast<T*>(obj), sizeof(T)); // *exp = *obj;
//    __r = false;
//    __unlock_atomics();
//    return __r;
//}
//
//// May spuriously return false (even if *obj == *exp)
//template <class T>
//bool
//__sw_atomic_compare_exchange_weak(_Atomic(T) volatile* obj, T* exp, T desr, int __m)
//{
//    bool __r;
//    __lock_atomics();
//    if (::memcmp(const_cast<T*>(obj), exp, sizeof(T)) == 0) // if (*obj == *exp)
//    {
//        ::memcpy(const_cast<T*>(obj), &desr, sizeof(T)); // *obj = desr;
//        __r = true;
//    }
//    ::memcpy(exp, const_cast<T*>(obj), sizeof(T)); // *exp = *obj;
//    __r = false;
//    __unlock_atomics();
//    return __r;
//}
//
//template <class T>
//T
//__sw_atomic_fetch_add(_Atomic(T) volatile* obj, T operand, int __m)
//{
//    register T __r;
//    __lock_atomics();
//    __r = *obj;
//    *obj += operand;
//    __unlock_atomics();
//    return __r;
//}
//
//template <class T>
//T
//__sw_atomic_fetch_sub(_Atomic(T) volatile* obj, T operand, int __m)
//{
//    register T __r;
//    __lock_atomics();
//    __r = *obj;
//    *obj -= operand;
//    __unlock_atomics();
//    return __r;
//}
//
//template <class T>
//T
//__sw_atomic_fetch_and(_Atomic(T) volatile* obj, T operand, int __m)
//{
//    register T __r;
//    __lock_atomics();
//    __r = *obj;
//    *obj &= operand;
//    __unlock_atomics();
//    return __r;
//
//}
//
//template <class T>
//T
//__sw_atomic_fetch_or(_Atomic(T) volatile* obj, T operand, int __m)
//{
//    register T __r;
//    __lock_atomics();
//    __r = *obj;
//    *obj |= operand;
//    __unlock_atomics();
//    return __r;
//}
//
//template <class T>
//T
//__sw_atomic_fetch_xor(_Atomic(T) volatile* obj, T operand, int __m)
//{
//    register T __r;
//    __lock_atomics();
//    __r = *obj;
//    *obj ^= operand;
//    __unlock_atomics();
//    return __r;
//}
///*
//void*
//__atomic_fetch_add(void* volatile* obj, ptrdiff_t operand)
//{
//    std::unique_lock<std::mutex> _(___OMFG_WHERE_ARE_ALL_THE_ATOMIC_INTRINSICS__);
//    void* r = *obj;
//    (char*&)(*obj) += operand;
//    return r;
//}
//
//void*
//__atomic_fetch_sub(void* volatile* obj, ptrdiff_t operand)
//{
//    std::unique_lock<std::mutex> _(___OMFG_WHERE_ARE_ALL_THE_ATOMIC_INTRINSICS__);
//    void* r = *obj;
//    (char*&)(*obj) -= operand;
//    return r;
//}
//*/
//
//static __attribute__ ((__always_inline__)) void __sw_atomic_thread_fence(int __m)
//{
//    __lock_atomics();
//    __unlock_atomics();
//}
//
//static __attribute__ ((__always_inline__)) void __sw_atomic_signal_fence(int __m)
//{
//    __lock_atomics();
//    __unlock_atomics();
//}
//
////#define __c11_atomic_load                       __sw_atomic_load
////#define __c11_atomic_store                      __sw_atomic_store
////#define __c11_atomic_exchange                   __sw_atomic_exchange
//#define __c11_atomic_compare_exchange_strong    __sw_atomic_compare_exchange_strong
//#define __c11_atomic_compare_exchange_weak      __sw_atomic_compare_exchange_weak
//#define __c11_atomic_fetch_add                  __sw_atomic_fetch_add
//#define __c11_atomic_fetch_sub                  __sw_atomic_fetch_sub
//#define __c11_atomic_fetch_and                  __sw_atomic_fetch_and
//#define __c11_atomic_fetch_or                   __sw_atomic_fetch_or
//#define __c11_atomic_fetch_xor                  __sw_atomic_fetch_xor
//
//#endif  /* __cplusplus */

#endif  /* __BACKUP_ATOMICS_H__ */
