//
//  integer_sequence.h
//  ePub3
//
//  Created by Jim Dovey on 11/7/2013.
//  Copyright (c) 2013 The Readium Foundation and contributors. All rights reserved.
//

#ifndef ePub3_integer_sequence_h
#define ePub3_integer_sequence_h

#include <ePub3/base.h>

EPUB3_BEGIN_NAMESPACE

// C++14 index_sequence implementation based on LLVM libc++ __tuple_indices

template <typename _Int, _Int ..._I>
struct integer_sequence
{
    static_assert(std::is_integral<_Int>::value, "Integral type");
    typedef _Int type;
    
#if EPUB_COMPILER_SUPPORTS(CXX_CONSTEXPR)
    static constexpr _Int size = sizeof...(_I);
    
    template <_Int _Num>
    using append = integer_sequence<_Int, _I..., _Num>;
    
    typedef append<size> next;
#endif
};

// specialization for std::size_t to index an array or tuple
template <std::size_t ..._I>
using index_sequence = integer_sequence<std::size_t, _I...>;

namespace detail
{
    
    template <typename _Int, _Int _Sp, class _IntTuple, _Int _Ep>
    struct iota;
    
    template <typename _Int, _Int _Sp, _Int ..._Indices, _Int _Ep>
    struct iota<_Int, _Sp, integer_sequence<_Int, _Indices...>, _Ep>
    {
        typedef typename iota<_Int, _Sp+1, integer_sequence<_Int, _Indices..., _Sp>, _Ep>::type type;
    };
    
    template <typename _Int, _Int _Ep, _Int ..._Indices>
    struct iota<_Int, _Ep, integer_sequence<_Int, _Indices...>, _Ep>
    {
        typedef integer_sequence<_Int, _Indices...> type;
    };
    
}

template <typename _Int, _Int _Ep, _Int _Sp = 0>
struct make_integer_sequence
{
    static_assert(_Sp <= _Ep, "make_integer_sequence input error");
    typedef typename detail::iota<_Int, _Sp, integer_sequence<_Int>, _Ep>::type type;
};

template <std::size_t _Ep, std::size_t _Sp = 0>
using make_index_sequence = make_integer_sequence<std::size_t, _Ep, _Sp>;

EPUB3_END_NAMESPACE

#endif
