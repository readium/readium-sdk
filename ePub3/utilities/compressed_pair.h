//
//  compressed_pair.h
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

#ifndef __ePub3__compressed_pair__
#define __ePub3__compressed_pair__

#include <ePub3/utilities/basic.h>
#include "swap_traits.h"

EPUB3_BEGIN_NAMESPACE

// internal template classes
// based on LLVM libc++ std::__compressed_pair implementation (why is there no compressed_pair in C++11 STL?)
//  which is released under MIT and University of Illinios Open Source licenses.

#if EPUB_COMPILER_SUPPORTS(CXX_VARIADIC_TEMPLATES)
// helper pretty much copied verbatim from libc++, where it's an internal class used to help with some tuple stuff
template <std::size_t...> struct __tuple_indices {};

template <std::size_t _Sp, class _IntTuple, std::size_t _Ep>
struct __make_indices_imp;

template <std::size_t _Sp, std::size_t ..._Indices, std::size_t _Ep>
struct __make_indices_imp<_Sp, __tuple_indices<_Indices...>, _Ep>
{
    typedef typename __make_indices_imp<_Sp+1, __tuple_indices<_Indices..., _Sp>, _Ep>::type type;
};

template <std::size_t _Ep, std::size_t ..._Indices>
struct __make_indices_imp<_Ep, __tuple_indices<_Indices...>, _Ep>
{
    typedef __tuple_indices<_Indices...> type;
};

template <std::size_t _Ep, std::size_t _Sp = 0>
struct __make_tuple_indices
{
    static_assert(_Sp < _Ep, "__make_tuple_indices input error");
    typedef typename __make_indices_imp<_Sp, __tuple_indices<>, _Ep>::type type;
};
#endif

// basic type used to switch() on traits
template <class _T1, class _T2, bool = std::is_same<typename std::remove_cv<_T1>::type,
                                                    typename std::remove_cv<_T2>::type>::value,
                                bool = std::is_empty<_T1>::value
#if EPUB_COMPILER(CLANG) && __has_feature(is_final)
                                       && !__is_final(_T1)
#endif
                                ,
                                bool = std::is_empty<_T2>::value
#if EPUB_COMPILER(CLANG) && __has_feature(is_final)
                                       && !__is_final(_T2)
#endif
         >
struct __epub3_compressed_pair_switch;

// specializations to define value for different sets of inputs

// neither member is empty (or neither can be subclassed)
template <class _T1, class _T2, bool IsSame>
struct __epub3_compressed_pair_switch<_T1, _T2, IsSame, false, false>   {enum {value = 0};};

// only first member is empty (ahnd subclassable)
template <class _T1, class _T2, bool IsSame>
struct __epub3_compressed_pair_switch<_T1, _T2, IsSame, true, false>    {enum {value = 1};};

// only second member is empty (and subclassable)
template <class _T1, class _T2, bool IsSame>
struct __epub3_compressed_pair_switch<_T1, _T2, IsSame, false, true>    {enum {value = 2};};

// two differing empty/subclassable members
template <class _T1, class _T2>
struct __epub3_compressed_pair_switch<_T1, _T2, false, true, true>      {enum {value = 3};};

// two identical empty/subclassable members
template <class _T1, class _T2>
struct __epub3_compressed_pair_switch<_T1, _T2, true, true, true>       {enum {value = 1};};

// implementation type -- specialized based on above switch constants
template <class _T1, class _T2, unsigned = __epub3_compressed_pair_switch<_T1, _T2>::value>
class __epub3_compressed_pair_imp;

// implementation for two non-empty members
template <class _T1, class _T2>
class __epub3_compressed_pair_imp<_T1, _T2, 0>
{
private:
    _T1 __first_;
    _T2 __second_;
    
public:
    typedef _T1 _T1_param;
    typedef _T2 _T2_param;
    
    typedef typename std::remove_reference<_T1>::type& _T1_reference;
    typedef typename std::remove_reference<_T2>::type& _T2_reference;
    
    typedef const typename std::remove_reference<_T1>::type& _T1_const_reference;
    typedef const typename std::remove_reference<_T2>::type& _T2_const_reference;
    
    FORCE_INLINE __epub3_compressed_pair_imp() {}
    FORCE_INLINE explicit __epub3_compressed_pair_imp(_T1_param __t1)
        : __first_(std::forward<_T1_param>(__t1)) {}
    FORCE_INLINE explicit __epub3_compressed_pair_imp(_T2_param __t2)
        : __second_(std::forward<_T2_param>(__t2)) {}
    FORCE_INLINE __epub3_compressed_pair_imp(_T1_param __t1, _T2_param __t2)
        : __first_(std::forward<_T1_param>(__t1)), __second_(std::forward<_T2_param>(__t2)) {}
    
    FORCE_INLINE
    __epub3_compressed_pair_imp(const __epub3_compressed_pair_imp& __p)
        _NOEXCEPT_(std::is_nothrow_copy_constructible<_T1>::value &&
                   std::is_nothrow_copy_constructible<_T2>::value)
        : __first_(__p.first()), __second_(__p.second()) {}
    
    FORCE_INLINE
    __epub3_compressed_pair_imp& operator=(const __epub3_compressed_pair_imp& __p)
        _NOEXCEPT_(std::is_nothrow_copy_assignable<_T1>::value &&
                   std::is_nothrow_copy_assignable<_T2>::value)
    {
        __first_ = __p.first();
        __second_ = __p.second();
        return *this;
    }
    
    FORCE_INLINE
    __epub3_compressed_pair_imp(__epub3_compressed_pair_imp&& __p)
        _NOEXCEPT_(std::is_nothrow_move_constructible<_T1>::value &&
                   std::is_nothrow_move_constructible<_T2>::value)
        : __first_(std::forward<_T1>(__p.first())), __second_(std::forward<_T2>(__p.second())) {}
    
    FORCE_INLINE
    __epub3_compressed_pair_imp& operator=(__epub3_compressed_pair_imp&& __p)
        _NOEXCEPT_(std::is_nothrow_move_assignable<_T1>::value &&
                   std::is_nothrow_move_assignable<_T2>::value)
    {
        __first_ = std::forward<_T1>(__p.first());
        __second_ = std::forward<_T2>(__p.second());
        return *this;
    }
    
#if EPUB_COMPILER_SUPPORTS(CXX_VARIADIC_TEMPLATES)
    template <class... _Args1, class... _Args2, size_t... _I1, size_t... _I2>
    FORCE_INLINE
    __epub3_compressed_pair_imp(std::piecewise_construct_t __pc,
                                std::tuple<_Args1...> __first_args,
                                std::tuple<_Args2...> __second_args,
                                __tuple_indices<_I1...>,
                                __tuple_indices<_I2...>)
        : __first_(std::forward<_Args1>(std::get<_I1>(__first_args))...),
          __second_(std::forward<_Args2>(std::get<_I2>(__second_args))...)
        {}
#endif
    
    FORCE_INLINE _T1_reference          first() _NOEXCEPT           { return __first_; }
    FORCE_INLINE _T1_const_reference    first() const _NOEXCEPT     { return __first_; }
    
    FORCE_INLINE _T2_reference          second() _NOEXCEPT          { return __second_; }
    FORCE_INLINE _T2_const_reference    second() const _NOEXCEPT    { return __second_; }
    
    FORCE_INLINE void swap(__epub3_compressed_pair_imp& __x)
        _NOEXCEPT_(__is_nothrow_swappable<_T1>::value &&
                   __is_nothrow_swappable<_T2>::value)
    {
        using std::swap;
        swap(__first_, __x.__first);
        swap(__second_, __x.__second_);
    }
    
};

// Implementation where only first member is empty & subclassable
template <class _T1, class _T2>
class __epub3_compressed_pair_imp<_T1, _T2, 1>
    : private _T1
{
private:
    _T2 __second_;
    
public:
    typedef _T1 _T1_param;
    typedef _T2 _T2_param;
    
    typedef _T1&                                        _T1_reference;
    typedef typename std::remove_reference<_T2>::type&  _T2_reference;
    
    typedef const _T1&                                          _T1_const_reference;
    typedef const typename std::remove_reference<_T2>::type&    _T2_const_reference;
    
    FORCE_INLINE __epub3_compressed_pair_imp() {}
    FORCE_INLINE explicit __epub3_compressed_pair_imp(_T1_param __t1)
        : _T1(std::forward<_T1_param>(__t1)) {}
    FORCE_INLINE explicit __epub3_compressed_pair_imp(_T2_param __t2)
        : __second_(std::forward<_T2_param>(__t2)) {}
    FORCE_INLINE __epub3_compressed_pair_imp(_T1_param __t1, _T2_param __t2)
        : _T1(std::forward<_T1_param>(__t1)), __second_(std::forward<_T2_param>(__t2)) {}
    
    FORCE_INLINE
    __epub3_compressed_pair_imp(const __epub3_compressed_pair_imp& __p)
        _NOEXCEPT_(std::is_nothrow_copy_constructible<_T1>::value &&
                   std::is_nothrow_copy_constructible<_T2>::value)
        : _T1(__p.first()), __second_(__p.second()) {}
    
    FORCE_INLINE
    __epub3_compressed_pair_imp& operator=(const __epub3_compressed_pair_imp& __p)
        _NOEXCEPT_(std::is_nothrow_copy_assignable<_T1>::value &&
                   std::is_nothrow_copy_assignable<_T2>::value)
    {
        _T1::operator=(__p.first());
        __second_ = __p.second();
        return *this;
    }
    
    FORCE_INLINE
    __epub3_compressed_pair_imp(__epub3_compressed_pair_imp&& __p)
        _NOEXCEPT_(std::is_nothrow_move_constructible<_T1>::value &&
                   std::is_nothrow_move_constructible<_T2>::value)
        : _T1(std::move(__p.first())), __second_(std::forward<_T2>(__p.second())) {}
    
    FORCE_INLINE
    __epub3_compressed_pair_imp& operator=(__epub3_compressed_pair_imp&& __p)
        _NOEXCEPT_(std::is_nothrow_move_assignable<_T1>::value &&
                   std::is_nothrow_move_assignable<_T2>::value)
    {
        _T1::operator=(std::move(__p.first()));
        __second_ = std::forward<_T2>(__p.second());
        return *this;
    }
    
#if EPUB_COMPILER_SUPPORTS(CXX_VARIADIC_TEMPLATES)
    template <class... _Args1, class... _Args2, size_t... _I1, size_t... _I2>
    FORCE_INLINE
    __epub3_compressed_pair_imp(std::piecewise_construct_t __pc,
                                std::tuple<_Args1...> __first_args,
                                std::tuple<_Args2...> __second_args,
                                __tuple_indices<_I1...>,
                                __tuple_indices<_I2...>)
        : _T1(std::forward<_Args1>(std::get<_I1>(__first_args))...),
          __second_(std::forward<_Args2>(std::get<_I2>(__second_args))...)
    {}
#endif
    
    FORCE_INLINE _T1_reference          first() _NOEXCEPT           { return *this; }
    FORCE_INLINE _T1_const_reference    first() const _NOEXCEPT     { return *this; }
    
    FORCE_INLINE _T2_reference          second() _NOEXCEPT          { return __second_; }
    FORCE_INLINE _T2_const_reference    second() const _NOEXCEPT    { return __second_; }
    
    FORCE_INLINE void swap(__epub3_compressed_pair_imp& __x)
    _NOEXCEPT_(__is_nothrow_swappable<_T1>::value &&
               __is_nothrow_swappable<_T2>::value)
    {
        using std::swap;
        swap(__second_, __x.__second_);
    }
};

// implementation where only second member is empty & subclassable
template <class _T1, class _T2>
class __epub3_compressed_pair_imp<_T1, _T2, 2>
    : private _T2
{
private:
    _T1 __first_;
    
public:
    typedef _T1 _T1_param;
    typedef _T2 _T2_param;
    
    typedef typename std::remove_reference<_T1>::type&  _T1_reference;
    typedef _T2&                                        _T2_reference;
    
    typedef const typename std::remove_reference<_T1>::type&    _T1_const_reference;
    typedef const _T2&                                          _T2_const_reference;
    
    FORCE_INLINE __epub3_compressed_pair_imp() {}
    FORCE_INLINE explicit __epub3_compressed_pair_imp(_T1_param __t1)
        : __first_(std::forward<_T1_param>(__t1)) {}
    FORCE_INLINE explicit __epub3_compressed_pair_imp(_T2_param __t2)
        : _T2(std::forward<_T2_param>(__t2)) {}
    FORCE_INLINE __epub3_compressed_pair_imp(_T1_param __t1, _T2_param __t2)
        : _T2(std::forward<_T2_param>(__t2)), __first_(std::forward<_T1_param>(__t1)) {}
    
    FORCE_INLINE
    __epub3_compressed_pair_imp(const __epub3_compressed_pair_imp& __p)
        _NOEXCEPT_(std::is_nothrow_copy_constructible<_T1>::value &&
                   std::is_nothrow_copy_constructible<_T2>::value)
        : _T2(__p.second()), __first_(__p.first()) {}
    
    FORCE_INLINE
    __epub3_compressed_pair_imp& operator=(const __epub3_compressed_pair_imp& __p)
        _NOEXCEPT_(std::is_nothrow_copy_assignable<_T1>::value &&
                   std::is_nothrow_copy_assignable<_T2>::value)
    {
        __first_ = __p.first();
        _T2::operator=(__p.second());
        return *this;
    }
    
    FORCE_INLINE
    __epub3_compressed_pair_imp(__epub3_compressed_pair_imp&& __p)
        _NOEXCEPT_(std::is_nothrow_move_constructible<_T1>::value &&
                   std::is_nothrow_move_constructible<_T2>::value)
        : _T2(std::move(__p.second())), __first_(std::forward<_T1>(__p.first())) {}
    
    FORCE_INLINE
    __epub3_compressed_pair_imp& operator=(__epub3_compressed_pair_imp&& __p)
        _NOEXCEPT_(std::is_nothrow_move_assignable<_T1>::value &&
                   std::is_nothrow_move_assignable<_T2>::value)
    {
        __first_ = std::forward<_T1>(__p.first());
        _T2::operator=(std::move(__p.second()));
        return *this;
    }
    
#if EPUB_COMPILER_SUPPORTS(CXX_VARIADIC_TEMPLATES)
    template <class... _Args1, class... _Args2, size_t... _I1, size_t... _I2>
    FORCE_INLINE
    __epub3_compressed_pair_imp(std::piecewise_construct_t __pc,
                                std::tuple<_Args1...> __first_args,
                                std::tuple<_Args2...> __second_args,
                                __tuple_indices<_I1...>,
                                __tuple_indices<_I2...>)
        : _T2(std::forward<_Args2>(std::get<_I2>(__second_args))...),
          __first_(std::forward<_Args1>(std::get<_I1>(__first_args))...)
    {}
#endif
    
    FORCE_INLINE _T1_reference          first() _NOEXCEPT           { return __first_; }
    FORCE_INLINE _T1_const_reference    first() const _NOEXCEPT     { return __first_; }
    
    FORCE_INLINE _T2_reference          second() _NOEXCEPT          { return *this; }
    FORCE_INLINE _T2_const_reference    second() const _NOEXCEPT    { return *this; }
    
    FORCE_INLINE void swap(__epub3_compressed_pair_imp& __x)
    _NOEXCEPT_(__is_nothrow_swappable<_T1>::value &&
               __is_nothrow_swappable<_T2>::value)
    {
        using std::swap;
        swap(__first_, __x.__first);
    }
    
};

// implementation for two empty L& subclassable members
template <class _T1, class _T2>
class __epub3_compressed_pair_imp<_T1, _T2, 3>
    : private _T1, private _T2
{
public:
    typedef _T1 _T1_param;
    typedef _T2 _T2_param;
    
    typedef _T1& _T1_reference;
    typedef _T2& _T2_reference;
    
    typedef const _T1& _T1_const_reference;
    typedef const _T2& _T2_const_reference;
    
    FORCE_INLINE __epub3_compressed_pair_imp() {}
    FORCE_INLINE explicit __epub3_compressed_pair_imp(_T1_param __t1)
        : _T1(std::forward<_T1_param>(__t1)) {}
    FORCE_INLINE explicit __epub3_compressed_pair_imp(_T2_param __t2)
        : _T2(std::forward<_T2_param>(__t2)) {}
    FORCE_INLINE __epub3_compressed_pair_imp(_T1_param __t1, _T2_param __t2)
        : _T1(std::forward<_T1_param>(__t1)), _T2(std::forward<_T2_param>(__t2)) {}
    
    FORCE_INLINE
    __epub3_compressed_pair_imp(const __epub3_compressed_pair_imp& __p)
        _NOEXCEPT_(std::is_nothrow_copy_constructible<_T1>::value &&
                   std::is_nothrow_copy_constructible<_T2>::value)
        : _T1(__p.first()), _T2(__p.second()) {}
    
    FORCE_INLINE
    __epub3_compressed_pair_imp& operator=(const __epub3_compressed_pair_imp& __p)
        _NOEXCEPT_(std::is_nothrow_copy_assignable<_T1>::value &&
                   std::is_nothrow_copy_assignable<_T2>::value)
    {
        _T1::operator=(__p.first());
        _T2::operator=(__p.second());
        return *this;
    }
    
    FORCE_INLINE
    __epub3_compressed_pair_imp(__epub3_compressed_pair_imp&& __p)
        _NOEXCEPT_(std::is_nothrow_move_constructible<_T1>::value &&
                   std::is_nothrow_move_constructible<_T2>::value)
        : _T1(std::move(__p.first())), _T2(std::move(__p.second())) {}
    
    FORCE_INLINE
    __epub3_compressed_pair_imp& operator=(__epub3_compressed_pair_imp&& __p)
        _NOEXCEPT_(std::is_nothrow_move_assignable<_T1>::value &&
                   std::is_nothrow_move_assignable<_T2>::value)
    {
        _T1::operator=(std::move(__p.first()));
        _T2::operator=(std::move(__p.second()));
        return *this;
    }
    
#if EPUB_COMPILER_SUPPORTS(CXX_VARIADIC_TEMPLATES)
    template <class... _Args1, class... _Args2, size_t... _I1, size_t... _I2>
    FORCE_INLINE
    __epub3_compressed_pair_imp(std::piecewise_construct_t __pc,
                                std::tuple<_Args1...> __first_args,
                                std::tuple<_Args2...> __second_args,
                                __tuple_indices<_I1...>,
                                __tuple_indices<_I2...>)
        : _T1(std::forward<_Args1>(std::get<_I1>(__first_args))...),
          _T2(std::forward<_Args2>(std::get<_I2>(__second_args))...)
    {}
#endif
    
    FORCE_INLINE _T1_reference          first() _NOEXCEPT           { return *this; }
    FORCE_INLINE _T1_const_reference    first() const _NOEXCEPT     { return *this; }
    
    FORCE_INLINE _T2_reference          second() _NOEXCEPT          { return *this; }
    FORCE_INLINE _T2_const_reference    second() const _NOEXCEPT    { return *this; }
    
    FORCE_INLINE void swap(__epub3_compressed_pair_imp& __x)
    _NOEXCEPT_(__is_nothrow_swappable<_T1>::value &&
               __is_nothrow_swappable<_T2>::value)
    {
    }
    
};

template <class _T1, class _T2>
class __compressed_pair
    : private __epub3_compressed_pair_imp<_T1, _T2>
{
    typedef __epub3_compressed_pair_imp<_T1, _T2> base;
    
public:
    typedef typename base::_T1_param _T1_param;
    typedef typename base::_T2_param _T2_param;
    
    typedef typename base::_T1_reference _T1_reference;
    typedef typename base::_T2_reference _T2_reference;
    
    typedef typename base::_T1_const_reference _T1_const_reference;
    typedef typename base::_T2_const_reference _T2_const_reference;
    
    FORCE_INLINE __compressed_pair() {}
    FORCE_INLINE explicit __compressed_pair(_T1_param __t1)
        : base(std::forward<_T1_param>(__t1)) {}
    FORCE_INLINE explicit __compressed_pair(_T2_param __t2)
        : base(std::forward<_T2_param>(__t2)) {}
    FORCE_INLINE __compressed_pair(_T1_param __t1, _T2_param __t2)
        : base(std::forward<_T1_param>(__t1), std::forward<_T2_param>(__t2)) {}
    
    FORCE_INLINE
    __compressed_pair(const __compressed_pair& __p)
    _NOEXCEPT_(std::is_nothrow_copy_constructible<_T1>::value &&
               std::is_nothrow_copy_constructible<_T2>::value)
        : base(__p) {}
    
    FORCE_INLINE
    __compressed_pair& operator=(const __compressed_pair& __p)
        _NOEXCEPT_(std::is_nothrow_copy_assignable<_T1>::value &&
                   std::is_nothrow_copy_assignable<_T2>::value)
    {
        base::operator=(__p);
        return *this;
    }
    
    FORCE_INLINE
    __compressed_pair(__compressed_pair&& __p)
        _NOEXCEPT_(std::is_nothrow_move_constructible<_T1>::value &&
                   std::is_nothrow_move_constructible<_T2>::value)
        : base(std::move(__p)) {}
    
    FORCE_INLINE
    __compressed_pair& operator=(__compressed_pair&& __p)
        _NOEXCEPT_(std::is_nothrow_move_assignable<_T1>::value &&
                   std::is_nothrow_move_assignable<_T2>::value)
    {
        base::operator=(std::move(__p));
    }
    
#if EPUB_COMPILER_SUPPORTS(CXX_VARIADIC_TEMPLATES)
    template <class... _Args1, class... _Args2>
    FORCE_INLINE
    __compressed_pair(std::piecewise_construct_t __pc, std::tuple<_Args1...> __first_args,
                                                       std::tuple<_Args2...> __second_args)
        : base(__pc, std::move(__first_args), std::move(__second_args),
               typename __make_tuple_indices<sizeof...(_Args1)>::type(),
               typename __make_tuple_indices<sizeof...(_Args2)>::type())
        {}
#endif
    
    FORCE_INLINE _T1_reference          first() _NOEXCEPT           { return base::first(); }
    FORCE_INLINE _T1_const_reference    first() const _NOEXCEPT     { return base::first(); }
    
    FORCE_INLINE _T2_reference          second() _NOEXCEPT          { return base::second(); }
    FORCE_INLINE _T2_const_reference    second() const _NOEXCEPT    { return base::second(); }
    
    FORCE_INLINE void swap(__compressed_pair& __x)
        _NOEXCEPT_(__is_nothrow_swappable<_T1>::value &&
                   __is_nothrow_swappable<_T2>::value)
    {
        base::swap(__x);
    }
    
};

template <class _T1, class _T2>
FORCE_INLINE
void
swap(__compressed_pair<_T1, _T2>& __x, __compressed_pair<_T1, _T2>& __y)
    _NOEXCEPT_(__is_nothrow_swappable<_T1>::value &&
               __is_nothrow_swappable<_T2>::value)
{
    __x.swap(__y);
}

EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__compressed_pair__) */
