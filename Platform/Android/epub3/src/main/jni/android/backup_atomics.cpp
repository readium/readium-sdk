//
//  backup_atomics.cpp
//  ePub3
//
//  Created by Jim Dovey on 2013-04-16.
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
#if 1
#include <pthread.h>
int ___OMFG_WHERE_ARE_ALL_THE_ATOMIC_INTRINSICS__ = 0;
extern "C" void __lock_atomics(void)
{
    while (1)
    {
        int i;
        for (i=0; i<10000;i++)
        {
            if (__sync_bool_compare_and_swap(&___OMFG_WHERE_ARE_ALL_THE_ATOMIC_INTRINSICS__, 0, 1))
            {
                return;
            }
        }
        sched_yield();
    }
}
extern "C" void __unlock_atomics(void)
{
    __asm__ __volatile__ ("" ::: "memory");
    *(&___OMFG_WHERE_ARE_ALL_THE_ATOMIC_INTRINSICS__) = 0;
}

extern "C" void* __atomic_load(void* const volatile* obj, int __m)
{
    return *obj;
}
#else
#include <mutex>

std::mutex ___OMFG_WHERE_ARE_ALL_THE_ATOMIC_INTRINSICS__;

extern "C" void*
__atomic_load(void* const volatile* obj)
{
    std::unique_lock<std::mutex> _(___OMFG_WHERE_ARE_ALL_THE_ATOMIC_INTRINSICS__);
    return *obj;
}

extern "C" void
__atomic_store(void* volatile* obj, void* desr)
{
    std::unique_lock<std::mutex> _(___OMFG_WHERE_ARE_ALL_THE_ATOMIC_INTRINSICS__);
    *obj = desr;
}

extern "C" void*
__atomic_exchange(void* volatile* obj, void* desr)
{
    std::unique_lock<std::mutex> _(___OMFG_WHERE_ARE_ALL_THE_ATOMIC_INTRINSICS__);
    void* r = *obj;
    *obj = desr;
    return r;
}

extern "C" void*
__atomic_fetch_add(void* volatile* obj, ptrdiff_t operand)
{
    std::unique_lock<std::mutex> _(___OMFG_WHERE_ARE_ALL_THE_ATOMIC_INTRINSICS__);
    void* r = *obj;
    (char*&)(*obj) += operand;
    return r;
}

extern "C" void*
__atomic_fetch_sub(void* volatile* obj, ptrdiff_t operand)
{
    std::unique_lock<std::mutex> _(___OMFG_WHERE_ARE_ALL_THE_ATOMIC_INTRINSICS__);
    void* r = *obj;
    (char*&)(*obj) -= operand;
    return r;
}

extern "C" void __atomic_thread_fence()
{
    std::unique_lock<std::mutex> _(___OMFG_WHERE_ARE_ALL_THE_ATOMIC_INTRINSICS__);
}

extern "C" void __atomic_signal_fence()
{
    std::unique_lock<std::mutex> _(___OMFG_WHERE_ARE_ALL_THE_ATOMIC_INTRINSICS__);
}
#endif
