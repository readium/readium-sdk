//
//  future.cpp
//  ePub3
//
//  Created by Jim Dovey on 11/15/2013.
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

#include <ePub3/epub3.h>
#include <system_error>
#include "future.h"

/*
#if EPUB_PLATFORM(ANDROID)
namespace std
{
    
    static const std::size_t __sp_mut_count = 16;
    static pthread_mutex_t mut_back_imp[__sp_mut_count] =
    {
        PTHREAD_MUTEX_INITIALIZER, PTHREAD_MUTEX_INITIALIZER, PTHREAD_MUTEX_INITIALIZER, PTHREAD_MUTEX_INITIALIZER,
        PTHREAD_MUTEX_INITIALIZER, PTHREAD_MUTEX_INITIALIZER, PTHREAD_MUTEX_INITIALIZER, PTHREAD_MUTEX_INITIALIZER,
        PTHREAD_MUTEX_INITIALIZER, PTHREAD_MUTEX_INITIALIZER, PTHREAD_MUTEX_INITIALIZER, PTHREAD_MUTEX_INITIALIZER,
        PTHREAD_MUTEX_INITIALIZER, PTHREAD_MUTEX_INITIALIZER, PTHREAD_MUTEX_INITIALIZER, PTHREAD_MUTEX_INITIALIZER
    };
    
    static mutex* mut_back = reinterpret_cast<std::mutex*>(mut_back_imp);
    
    CONSTEXPR __sp_mut::__sp_mut(void* p) _NOEXCEPT
    : __lx(p)
    {
    }
    
    void
    __sp_mut::lock() _NOEXCEPT
    {
        mutex& m = *static_cast<mutex*>(__lx);
        unsigned count = 0;
        while (!m.try_lock())
        {
            if (++count > 16)
            {
                m.lock();
                break;
            }
            this_thread::yield();
        }
    }
    
    void
    __sp_mut::unlock() _NOEXCEPT
    {
        static_cast<mutex*>(__lx)->unlock();
    }
    
    __sp_mut&
    __get_sp_mut(const void* p)
    {
        static __sp_mut muts[__sp_mut_count]
        {
            &mut_back[ 0], &mut_back[ 1], &mut_back[ 2], &mut_back[ 3],
            &mut_back[ 4], &mut_back[ 5], &mut_back[ 6], &mut_back[ 7],
            &mut_back[ 8], &mut_back[ 9], &mut_back[10], &mut_back[11],
            &mut_back[12], &mut_back[13], &mut_back[14], &mut_back[15]
        };
        return muts[hash<const void*>()(p) & (__sp_mut_count-1)];
    }
    
}
#endif
*/

#if EPUB_COMPILER(CLANG) && defined(ANDROID)
#ifdef __cplusplus
extern "C" {
#endif

int __cxa_thread_atexit(void (*func)(), void *obj,
                                   void *dso_symbol) {
  return 0;
}
#ifdef __cplusplus
}
#endif
#endif

EPUB3_BEGIN_NAMESPACE

class _LIBCPP_HIDDEN __future_category
    : public std::error_category
{
public:
    virtual const char* name() const _NOEXCEPT OVERRIDE;
    virtual std::string message(int ev) const OVERRIDE;
};

const char* __future_category::name() const _NOEXCEPT
{
    return "future";
}

std::string __future_category::message(int ev) const
{
    switch (static_cast<future_errc>(ev))
    {
        case future_errc::broken_promise:
            return std::string("The associated promise has been destructed prior "
                          "to the associated state becoming ready.");
        case future_errc::future_already_retrieved:
            return std::string("The future has already been retrieved from "
                          "the promise or packaged_task.");
        case future_errc::promise_already_satisfied:
            return std::string("The state of the promise has already been set.");
        case future_errc::no_state:
            return std::string("Operation not permitted on an object without "
                          "an associated state.");
        case future_errc::task_already_started:
            return std::string("The packaged_task has already been started.");
    }
    
    return std::string("unspecified future_errc value\n");
}

const std::error_category& future_category() _NOEXCEPT
{
    static __future_category category;
    return category;
}

std::vector<__shared_state_base::_ContinuationPtrType>&
__shared_state_base::__at_thread_exit()
{
    typedef std::vector<_ContinuationPtrType> _VecType;
#if EPUB_COMPILER_SUPPORTS(CXX_THREAD_LOCAL) && !EPUB_OS(IOS)
    static thread_local _VecType __vec;
    return __vec;
#elif EPUB_COMPILER(MSVC)
    // grumbles something about having to use internal API...
	static _Tss_t __key;
	static std::once_flag __once;
	std::call_once(__once, [&]() {
		_Tss_create(&__key, &__KillVectorPtr);
	});

	void* __p = _Tss_get(__key);
	if (__p == nullptr) {
		__p = new _VecType();
		_Tss_set(__key, __p);
	}

	_VecType* __pv = reinterpret_cast<_VecType*>(__p);
	return *__pv;
#elif EPUB_OS(UNIX)
    static pthread_key_t __key;
    static std::once_flag __once;
    std::call_once(__once, [&](){
        pthread_key_create(&__key, &__KillVectorPtr);
    });
    
    void* __p = pthread_getspecific(__key);
    if (__p == nullptr) {
        __p = new _VecType();
        pthread_setspecific(__key, __p);
    }
    
    std::vector<_ContinuationPtrType>* __pv = reinterpret_cast<_VecType*>(__p);
    return *__pv;
#else
# error No TLS implementation for this OS/Compiler
#endif
}

EPUB3_END_NAMESPACE

std::error_code std::make_error_code(EPUB3_NAMESPACE::future_errc e) _NOEXCEPT
{
    return std::error_code(static_cast<int>(e), EPUB3_NAMESPACE::future_category());
}

std::error_condition std::make_error_condition(EPUB3_NAMESPACE::future_errc e) _NOEXCEPT
{
    return std::error_condition(static_cast<int>(e), EPUB3_NAMESPACE::future_category());
}

#endif //FUTURE_ENABLED
