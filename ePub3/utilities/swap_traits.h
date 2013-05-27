//
//  swap_traits.h
//  ePub3
//
//  Created by Jim Dovey on 2013-05-17.
//  Copyright (c) 2013 The Readium Foundation and contributors. All rights reserved.
//

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
struct __is_nothrow_swappable<false, _Tp> : public false_type
{
};

template <class _Tp>
struct __is_nothrow_swappable : public __is_nothrow_swappable<__is_swappable<_Tp>::value, _Tp>
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
