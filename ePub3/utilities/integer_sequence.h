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

#if !EPUB_PLATFORM(WIN_PHONE)

#include <type_traits>

EPUB3_BEGIN_NAMESPACE

// C++14 index_sequence implementation based on LLVM libc++ __tuple_indices

template <typename _Tp, _Tp... _Ip>
struct integer_sequence
{
	static_assert(std::is_integral<_Tp>::value, "integer_sequence requires an integral type");
	typedef _Tp value_type;
    
	static CONSTEXPR size_t size() _NOEXCEPT { return sizeof...(_Ip) };
};

// specialization for std::size_t to index an array or tuple
template <std::size_t... _Ip>
using index_sequence = integer_sequence<std::size_t, _Ip...>;	// __tuple_indices

namespace detail
{
    
    template <typename _Tp, _Tp _Sp, class _IntTuple, _Tp _Ep>
    struct iota;	// __make_indices_imp
    
    template <typename _Tp, _Tp _Sp, _Tp ..._Ip, _Tp _Ep>
    struct iota<_Tp, _Sp, integer_sequence<_Tp, _Ip...>, _Ep>
    {
        typedef typename iota<_Tp, _Sp+1, integer_sequence<_Tp, _Ip..., _Sp>, _Ep>::type type;
    };
    
    template <typename _Tp, _Tp _Ep, _Tp ..._Ip>
    struct iota<_Tp, _Ep, integer_sequence<_Tp, _Ip...>, _Ep>
    {
        typedef integer_sequence<_Tp, _Ip...> type;
    };
    
}

template <typename _Tp, _Tp _Ep, _Tp _Sp = 0>
struct make_integer_sequence	// __make_tuple_indices
{
    static_assert(_Sp <= _Ep, "make_integer_sequence input error");
    typedef typename detail::iota<_Tp, _Sp, integer_sequence<_Tp>, _Ep>::type type;		// __make_indices_imp
};

template <std::size_t _Ep, std::size_t _Sp = 0>
using make_index_sequence = make_integer_sequence<std::size_t, _Ep, _Sp>;

#if EPUB_COMPILER(MSVC)

// MSVC doesn't like to compile the above, but it's quite happy with the below (near-identical) code
// this is a renamed __tuple_indices and __make_tuple_indices from LLVM libc++

template <std::size_t...> struct __msvc_index_sequence {};

template <std::size_t _Sp, class _IntTuple, std::size_t _Ep>
struct __make_msvc_index_sequence_imp;

template <std::size_t _Sp, std::size_t ..._Indices, std::size_t _Ep>
struct __make_msvc_index_sequence_imp<_Sp, __msvc_index_sequence<_Indices...>, _Ep>
{
	typedef typename __make_msvc_index_sequence_imp<_Sp + 1, __msvc_index_sequence<_Indices..., _Sp>, _Ep>::type type;
};

template <std::size_t _Ep, std::size_t ..._Indices>
struct __make_msvc_index_sequence_imp<_Ep, __msvc_index_sequence<_Indices...>, _Ep>
{
	typedef __msvc_index_sequence<_Indices...> type;
};

template <std::size_t _Ep, std::size_t _Sp = 0>
struct __make_msvc_index_sequence
{
	static_assert(_Sp <= _Ep, "__make_msvc_index_sequence input error");
	typedef typename __make_msvc_index_sequence_imp<_Sp, __msvc_index_sequence<>, _Ep>::type type;
};

#define index_sequence __msvc_index_sequence
#define make_index_sequence __make_msvc_index_sequence

#endif

EPUB3_END_NAMESPACE

#endif	// !EPUB_PLATFORM(WIN_PHONE)

#endif
