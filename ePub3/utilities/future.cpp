//
//  future.cpp
//  ePub3
//
//  Created by Jim Dovey on 11/15/2013.
//  Copyright (c) 2013 The Readium Foundation and contributors. All rights reserved.
//
//  Based on the future implementation from Boost, which carries the following
//  copyright information:
//
//  (C) Copyright 2008-10 Anthony Williams
//  (C) Copyright 2011-2013 Vicente J. Botet Escriba
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//

#include <ePub3/ePub3.h>
#include <system_error>
#include "future.h"

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
#if EPUB_COMPILER_SUPPORTS(CXX_THREAD_LOCAL)
    static thread_local _VecType __vec;
    return __vec;
#elif EPUB_COMPILER(MSVC)
    static __declspec(thread) _VecType __vec;
    return __vec;
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
