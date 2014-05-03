//
//  pointer_type.h
//  ePub3
//
//  Created by Jim Dovey on 2013-08-27.
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

#ifndef __ePub3__pointer_type__
#define __ePub3__pointer_type__

#include <ePub3/epub3.h>
#include <memory>
#include <type_traits>

EPUB3_BEGIN_NAMESPACE

/**
 A friendly shared-pointer-compatibility superclass.
 
 This class implements a simple forwarding New() method which will construct its core
 object type via std::make_shared<>(), forwarding all arguments. On compilers with no
 support for variadic macros (VC++10, I'm looking at you) it can handle up to five
 arguments to this constructor/factory function. It provides handy type definitions
 for both a standard non-const, non-volatile shared_ptr type and a const version, thus
 reducing boilerplate while enforcing type-safety and well-known rules.
 
 It also provides simple helpers for the somewhat fiddly shared_from_this(), which
 often requires explicit calling as
 `std::enable_shared_from_this<SomeType>::shared_from_this()` where plain
 `shared_from_this()` would be preferred. For this reason, we provide the Ptr() and
 ConstPtr() methods. Thus, for `class SomeType : public PointerType<SomeType>` we
 can use:
 
     SomeType obj = ...
     std::shared_ptr<SomeType> ptr = obj.Ptr();
     std::shared_ptr<const SomeType> constPtr = obj.ConstPtr();
 
 We also provide casting methods, allowing for easy and brief casting of a shared_ptr
 to a PointerType-derived object to be cast to a shared_ptr to a related type. Given:
 
     class BaseType { ... };
     class SubType : public BaseType, public PointerType<SubType> { ... };
 
 You can freely cast between the shared pointer types defined by both
 `PointerType<SubType>` and the (inferred) `PointerType<BaseType>`, like so:
 
     typedef PointerType<BaseType>::Pointer BaseTypePtr;
     typedef PointerType<SubType>::Pointer  SubTypePtr;
     
     SubTypePtr obj = SubType::New(...);
     BaseTypePtr base = obj->CastPtr<BaseType>();
 
 Note that the `CastPtr()` methods use SFINAE, so the compiler will report a template
 argument deduction error if you use, say, `obj->CastPtr<SomeUnrelatedType>()`.
 */
template <class _Tp>
class PointerType : public std::enable_shared_from_this<_Tp>
{
public:
    typedef std::enable_shared_from_this<_Tp>   _Base;
    typedef typename std::remove_cv<_Tp>::type  Type;
    typedef std::shared_ptr<Type>               Pointer;
    typedef std::shared_ptr<const Type>         ConstPointer;
    
protected:
    PointerType() : _Base() {}
    PointerType(const PointerType& __o) : _Base(__o) {}
    
public:
    virtual ~PointerType() {}
    
#if EPUB_COMPILER_SUPPORTS(CXX_VARIADIC_TEMPLATES)
    template <typename... _Args>
    FORCE_INLINE
    static Pointer New(_Args&& ...__args) {
        // a thought: can we do std::make_shared<decltype(*this)> type of thing ?
        return std::make_shared<_Tp>(std::forward<_Args>(__args)...);
    }
    
    template <class _Sub, typename... _Args>
    FORCE_INLINE
    static
    typename std::enable_if
    <
        std::is_base_of<_Tp, _Sub>::value,
        typename PointerType<_Sub>::Pointer
    >::type
    New(_Args&& ...__args) {
        return std::make_shared<_Sub>(std::forward<_Args>(__args)...);
    }
#else
    template <class, _Sub, typename _Arg1>
    FORCE_INLINE
    static
    typename std::enable_if
    <
        std::is_base_of<_Tp, _Sub>::value,
        typename PointerType<_Sub>::Pointer
    >::type
    New(_Arg1 __arg1) {
        return std::make_shared<_Sub>(std::forward<_Arg1>(__arg1));
    }
    template <class _Sub, typename _Arg1, typename _Arg2>
    FORCE_INLINE
    static
    typename std::enable_if
    <
        std::is_base_of<_Tp, _Sub>::value,
        typename PointerType<_Sub>::Pointer
    >::type
    New(_Arg1 __arg1, _Arg2 __arg2) {
        return std::make_shared<_Sub>(std::forward<_Arg1, _Arg2>(__arg1, __arg2));
    }
    template <class _Sub, typename _Arg1, typename _Arg2, typename _Arg3>
    FORCE_INLINE
    static
    typename std::enable_if
    <
        std::is_base_of<_Tp, _Sub>::value,
        typename PointerType<_Sub>::Pointer
    >::type
    New(_Arg1 __arg1, _Arg2 __arg2, _Arg3 __arg3) {
        return std::make_shared<_Sub>(std::forward<_Arg1, _Arg2, _Arg3>(__arg1, __arg2, __arg3));
    }
    template <class _Sub, typename _Arg1, typename _Arg2, typename _Arg3, typename _Arg4>
    FORCE_INLINE
    static
    typename std::enable_if
    <
        std::is_base_of<_Tp, _Sub>::value,
        typename PointerType<_Sub>::Pointer
    >::type
    New(_Arg1 __arg1, _Arg2 __arg2, _Arg3 __arg3, _Arg4 __arg4) {
        return std::make_shared<_Sub>(std::forward<_Arg1, _Arg2, _Arg3, _Arg4>(__arg1, __arg2, __arg3, __arg4));
    }
    template <class _Sub, typename _Arg1, typename _Arg2, typename _Arg3, typename _Arg4, typename _Arg5>
    FORCE_INLINE
    static
    typename std::enable_if
    <
        std::is_base_of<_Tp, _Sub>::value,
        typename PointerType<_Sub>::Pointer
    >::type
    New(_Arg1 __arg1, _Arg2 __arg2, _Arg3 __arg3, _Arg4 __arg4, _Arg5 __arg5) {
        return std::make_shared<_Sub>(std::forward<_Arg1, _Arg2, _Arg3, _Arg4, _Arg5>(__arg1, __arg2, __arg3, __arg4, __arg5));
    }
    
    template <typename _Arg1>
    static Pointer New(_Arg1 __arg1) {
        return std::make_shared<_Tp>(std::forward<_Arg1>(__arg1));
    }
    template <typename _Arg1, typename _Arg2>
    static Pointer New(_Arg1 __arg1, _Arg2 __arg2) {
        return std::make_shared<_Tp>(std::forward<_Arg1, _Arg2>(__arg1, __arg2));
    }
    template <typename _Arg1, typename _Arg2, typename _Arg3>
    static Pointer New(_Arg1 __arg1, _Arg2 __arg2, _Arg3 __arg3) {
        return std::make_shared<_Tp>(std::forward<_Arg1, _Arg2, _Arg3>(__arg1, __arg2, __arg3));
    }
    template <typename _Arg1, typename _Arg2, typename _Arg3, typename _Arg4>
    static Pointer New(_Arg1 __arg1, _Arg2 __arg2, _Arg3 __arg3, _Arg4 __arg4) {
        return std::make_shared<_Tp>(std::forward<_Arg1, _Arg2, _Arg3, _Arg4>(__arg1, __arg2, __arg3, __arg4));
    }
    template <typename _Arg1, typename _Arg2, typename _Arg3, typename _Arg4, typename _Arg5>
    static Pointer New(_Arg1 __arg1, _Arg2 __arg2, _Arg3 __arg3, _Arg4 __arg4, _Arg5 __arg5) {
        return std::make_shared<_Tp>(std::forward<_Arg1, _Arg2, _Arg3, _Arg4, _Arg5>(__arg1, __arg2, __arg3, __arg4, __arg5));
    }
#endif
    
    Pointer Ptr() _NOEXCEPT { return _Base::shared_from_this(); }
    ConstPointer Ptr() const _NOEXCEPT { return _Base::shared_from_this(); }
    ConstPointer ConstPtr() const _NOEXCEPT { return _Base::shared_from_this(); }

    template <class _Yp>
    inline
    typename PointerType<_Yp>::Pointer
    CastPtr() _NOEXCEPT {
        return std::dynamic_pointer_cast<_Yp>(_Base::shared_from_this());
    }
    template <class _Yp>
    typename PointerType<typename std::add_const<_Yp>::type>::Pointer
    CastPtr() const _NOEXCEPT {
        return std::dynamic_pointer_cast<const _Yp>(_Base::shared_from_this());
    }

    template <class _Yp>
    static inline
    Pointer CastFrom(typename PointerType<_Yp>::Pointer __from) _NOEXCEPT {
        return std::dynamic_pointer_cast<Type>(__from);
    }

};

EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__pointer_type__) */
