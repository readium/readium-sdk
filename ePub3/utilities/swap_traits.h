//
//  swap_traits.h
//  ePub3
//
//  Created by Jim Dovey on 2013-05-17.
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

#ifndef ePub3_swap_traits_h
#define ePub3_swap_traits_h

#include "basic.h"
#include <type_traits>

EPUB3_BEGIN_NAMESPACE

#ifdef _LIBCPP_TYPE_TRAITS

// we know that libc++ supports template aliasing
template <class _Tp>
using __is_swappable = std::__is_swappable<_Tp>;
template <class _Tp>
using __is_nothrow_swappable = std::__is_nothrow_swappable<_Tp>;

#else   // _LIBCPP_TYPE_TRAITS

struct __nat
{
#if EPUB_COMPILER_SUPPORTS(CXX_DELETED_FUNCTIONS)
    __nat() = delete;
    __nat(const __nat&) = delete;
    __nat& operator=(const __nat&) = delete;
    ~__nat() = delete;
#endif
};
struct __any
{
    __any(...);
};

namespace __detail
{
    using std::swap;
    __nat swap(__any, __any);
    
    template <class _Tp>
    struct __swappable
    {
        typedef decltype(swap(std::declval<_Tp&>(), std::declval<_Tp&>())) type;
        static const bool value = !std::is_same<type, __nat>::value;
    };
};

template <class _Tp>
struct __is_swappable : public std::integral_constant<bool, __detail::__swappable<_Tp>::value>
{
};

#if EPUB_COMPILER_SUPPORTS(CXX_NOEXCEPT)

template <bool, class _Tp>
struct __is_nothrow_swappable_imp : public std::integral_constant<bool, noexcept(swap(std::declval<_Tp&>(),
                                                                                      std::declval<_Tp&>()))>
{
};

template <class _Tp>
struct __is_nothrow_swappable_imp<false, _Tp> : public false_type
{
};

template <class _Tp>
struct __is_nothrow_swappable : public __is_nothrow_swappable_imp<__is_swappable<_Tp>::value, _Tp>
{
};

#else   // EPUB_COMPILER_SUPPORTS(CXX_NOEXCEPT)

template <class _Tp>
struct __is_nothrow_swappable : public false_type
{
};

#endif  // EPUB_COMPILER_SUPPORTS(CXX_NOEXCEPT)

#endif  // _LIBCPP_TYPE_TRAITS

EPUB3_END_NAMESPACE

#endif
