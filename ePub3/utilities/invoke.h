//
//  invoke.h
//  ePub3
//
//  Created by Jim Dovey on 11/12/2013.
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

#ifndef ePub3_invoke_h
#define ePub3_invoke_h

#include <ePub3/base.h>
#include <type_traits>
#include <functional>

/*
 
    invoke synopsis
 
 namespace std
 {
 
 template <class F, class... Args> typename result_of<F&&(Args&&...)>::type invoke(F&& f, Args&&... args);
 template <class R, class F, class... Args> R invoke(F&& f, Args&&... args);
 
 } // namespace std
 
 */

EPUB3_BEGIN_NAMESPACE

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

// check for complete types

template <class... _Tp> struct __check_complete;

template <>
struct __check_complete<>
{
};

template <class _Hp, class _T0, class... _Tp>
struct __check_complete<_Hp, _T0, _Tp...>
    : private __check_complete<_Hp>,
      private __check_complete<_T0, _Tp...>
{
};

template <class _Hp>
struct __check_complete<_Hp, _Hp>
    : private __check_complete<_Hp>
{
};

template <class _Tp>
struct __check_complete<_Tp>
{
    static_assert(sizeof(_Tp) > 0, "Type must be complete.");
};

template <class _Tp>
struct __check_complete<_Tp&>
    : private __check_complete<_Tp>
{
};

#if EPUB_COMPILER_SUPPORTS(CXX_RVALUE_REFERENCES)
template <class _Tp>
struct __check_complete<_Tp&&>
    : private __check_complete<_Tp>
{
};
#endif

template <class _Rp, class... _Param>
struct __check_complete<_Rp (*)(_Param...)>
    : private __check_complete<_Rp>
{
};

template <class _Rp, class _Class, class... _Param>
struct __check_complete<_Rp (_Class::*)(_Param...)>
    : private __check_complete<_Class>
{
};

template <class _Rp, class _Class, class... _Param>
struct __check_complete<_Rp (_Class::*)(_Param...) const>
    : private __check_complete<_Class>
{
};

template <class _Rp, class _Class, class... _Param>
struct __check_complete<_Rp (_Class::*)(_Param...) volatile>
    : private __check_complete<_Class>
{
};

template <class _Rp, class _Class, class... _Param>
struct __check_complete<_Rp (_Class::*)(_Param...) const volatile>
    : private __check_complete<_Class>
{
};

#if EPUB_COMPILER_SUPPORTS(CXX_REFERENCE_QUALIFIED_FUNCTIONS)

template <class _Rp, class _Class, class... _Param>
struct __check_complete<_Rp (_Class::*)(_Param...) &>
    : private __check_complete<_Class>
{
};

template <class _Rp, class _Class, class... _Param>
struct __check_complete<_Rp (_Class::*)(_Param...) const&>
    : private __check_complete<_Class>
{
};

template <class _Rp, class _Class, class... _Param>
struct __check_complete<_Rp (_Class::*)(_Param...) volatile&>
    : private __check_complete<_Class>
{
};

template <class _Rp, class _Class, class... _Param>
struct __check_complete<_Rp (_Class::*)(_Param...) const volatile&>
    : private __check_complete<_Class>
{
};

template <class _Rp, class _Class, class... _Param>
struct __check_complete<_Rp (_Class::*)(_Param...) &&>
    : private __check_complete<_Class>
{
};

template <class _Rp, class _Class, class... _Param>
struct __check_complete<_Rp (_Class::*)(_Param...) const&&>
    : private __check_complete<_Class>
{
};

template <class _Rp, class _Class, class... _Param>
struct __check_complete<_Rp (_Class::*)(_Param...) volatile&&>
    : private __check_complete<_Class>
{
};

template <class _Rp, class _Class, class... _Param>
struct __check_complete<_Rp (_Class::*)(_Param...) const volatile&&>
    : private __check_complete<_Class>
{
};

#endif

template <class _Rp, class _Class>
struct __check_complete<_Rp _Class::*>
    : private __check_complete<_Class>
{
};

// decay_copy

template <class _Tp>
inline FORCE_INLINE
typename std::decay<_Tp>::type
decay_copy(_Tp&& __t)
    {
        return std::forward<_Tp>(__t);
    }

// invoke

// fallback
template <class... _Args>
__nat invoke(__any, _Args&&... __args)
    {}

template <typename _Fp, typename... _Args>
typename std::enable_if
         <
            std::is_member_pointer<typename std::decay<_Fp>::type>::value,
            typename std::result_of<_Fp&&(_Args&&...)>::type
         >::type
inline FORCE_INLINE
invoke(_Fp&& __f, _Args&&... __args)
    {
        return std::mem_fn(__f)(std::forward<_Args>(__args)...);
    }

template <typename _Fp, typename... _Args>
typename std::enable_if
         <
            !std::is_member_pointer<typename std::decay<_Fp>::type>::value,
            typename std::result_of<_Fp&&(_Args&&...)>::type
         >::type
inline FORCE_INLINE
invoke(_Fp&& __f, _Args&&... __args)
    {
        return std::forward<_Fp>(__f)(std::forward<_Args>(__args)...);
    }

template <typename _Ret, typename _Fp, typename... _Args>
inline FORCE_INLINE
_Ret
invoke(_Fp&& __f, _Args&&... __args)
    {
        return invoke(std::forward<_Fp>(__f), std::forward<_Args>(__args)...);
    }

// __invokable

template <class _Fp, class... _Args>
struct __invokable_imp
    : private __check_complete<_Fp>
{
    typedef decltype(invoke(std::declval<_Fp>(), std::declval<_Args>()...)) type;
    static const bool value = !std::is_same<type, __nat>::value;
};

template <class _Fp, class... _Args>
struct __invokable
    : public std::integral_constant<bool, __invokable_imp<_Fp, _Args...>::value>
{
};

// __invoke_of

template <bool _Invokable, class _Fp, class... _Args>
struct __invoke_of_imp  // false
{
};

template <class _Fp, class... _Args>
struct __invoke_of_imp<true, _Fp, _Args...>
{
    typedef typename __invokable_imp<_Fp, _Args...>::type type;
};

template <class _Fp, class... _Args>
struct __invoke_of
    : public __invoke_of_imp<__invokable<_Fp, _Args...>::value, _Fp, _Args...>
{
};

EPUB3_END_NAMESPACE

#endif
