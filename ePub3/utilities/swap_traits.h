//
//  swap_traits.h
//  ePub3
//
//  Created by Jim Dovey on 2013-05-17.
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
