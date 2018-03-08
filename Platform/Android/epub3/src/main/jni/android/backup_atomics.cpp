//
//  backup_atomics.cpp
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

//#if 1
//#include <pthread.h>
//int ___OMFG_WHERE_ARE_ALL_THE_ATOMIC_INTRINSICS__ = 0;
//extern "C" void __lock_atomics(void)
//{
//    while (1)
//    {
//        int i;
//        for (i=0; i<10000;i++)
//        {
//            if (__sync_bool_compare_and_swap(&___OMFG_WHERE_ARE_ALL_THE_ATOMIC_INTRINSICS__, 0, 1))
//            {
//                return;
//            }
//        }
//        sched_yield();
//    }
//}
//extern "C" void __unlock_atomics(void)
//{
//    __asm__ __volatile__ ("" ::: "memory");
//    *(&___OMFG_WHERE_ARE_ALL_THE_ATOMIC_INTRINSICS__) = 0;
//}
//
//extern "C" void* __atomic_load(void* const volatile* obj, int __m)
//{
//    return *obj;
//}
//#else
//#include <mutex>
//
//std::mutex ___OMFG_WHERE_ARE_ALL_THE_ATOMIC_INTRINSICS__;
//
//extern "C" void*
//__atomic_load(void* const volatile* obj)
//{
//    std::unique_lock<std::mutex> _(___OMFG_WHERE_ARE_ALL_THE_ATOMIC_INTRINSICS__);
//    return *obj;
//}
//
//extern "C" void
//__atomic_store(void* volatile* obj, void* desr)
//{
//    std::unique_lock<std::mutex> _(___OMFG_WHERE_ARE_ALL_THE_ATOMIC_INTRINSICS__);
//    *obj = desr;
//}
//
//extern "C" void*
//__atomic_exchange(void* volatile* obj, void* desr)
//{
//    std::unique_lock<std::mutex> _(___OMFG_WHERE_ARE_ALL_THE_ATOMIC_INTRINSICS__);
//    void* r = *obj;
//    *obj = desr;
//    return r;
//}
//
//extern "C" void*
//__atomic_fetch_add(void* volatile* obj, ptrdiff_t operand)
//{
//    std::unique_lock<std::mutex> _(___OMFG_WHERE_ARE_ALL_THE_ATOMIC_INTRINSICS__);
//    void* r = *obj;
//    (char*&)(*obj) += operand;
//    return r;
//}
//
//extern "C" void*
//__atomic_fetch_sub(void* volatile* obj, ptrdiff_t operand)
//{
//    std::unique_lock<std::mutex> _(___OMFG_WHERE_ARE_ALL_THE_ATOMIC_INTRINSICS__);
//    void* r = *obj;
//    (char*&)(*obj) -= operand;
//    return r;
//}
//
//extern "C" void __atomic_thread_fence()
//{
//    std::unique_lock<std::mutex> _(___OMFG_WHERE_ARE_ALL_THE_ATOMIC_INTRINSICS__);
//}
//
//extern "C" void __atomic_signal_fence()
//{
//    std::unique_lock<std::mutex> _(___OMFG_WHERE_ARE_ALL_THE_ATOMIC_INTRINSICS__);
//}
//#endif
