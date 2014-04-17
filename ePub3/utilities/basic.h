//
//  basic.h
//  ePub3
//
//  Created by Jim Dovey on 2012-12-20.
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

#ifndef ePub3_basic_h
#define ePub3_basic_h

#include <ePub3/base.h>
//#include "alphanum.hpp"
#if EPUB_USE(LIBXML2)
#include <libxml/xmlstring.h>
#else
#include <ePub3/xml/xmlstring.h>
#endif
#include <sstream>
#include <memory>
#include <vector>
#include <list>

EPUB3_BEGIN_NAMESPACE

//////////////////////////////////////////////////////////////////////////////
// shorthand forms of C++11 pointer objects

// These template aliases allow the use of shorthand declarations for different
// pointer types, i.e. Shared<Container> means
// std::shared_ptr<Container>.

using std::shared_ptr;
using std::weak_ptr;
using std::unique_ptr;

#if EPUB_COMPILER_SUPPORTS(CXX_ALIAS_TEMPLATES)

template <class _Tp>
using shared_vector = std::vector<std::shared_ptr<_Tp>>;

template <class _Tp>
using shared_list = std::list<std::shared_ptr<_Tp>>;

#else

template <class _Tp>
class shared_vector : public std::vector<std::shared_ptr<_Tp>>
{
    typedef std::vector<std::shared_ptr<_Tp>>   _Base;
    
public:
    shared_vector() _NOEXCEPT_(std::is_nothrow_default_constructible<typename _Base::allocator_type>::value) : _Base() {}
    explicit shared_vector(const typename _Base::allocator_type& __alloc) : _Base(__alloc) {}
    explicit shared_vector(typename _Base::size_type __n) : _Base(__n) {}
    shared_vector(typename _Base::size_type __n, const typename _Base::value_type& __v, const typename _Base::allocator_type& __a = typename _Base::allocator_type()) : _Base(__n, __v, __a) {}
    template <class _InputIterator>
    shared_vector(_InputIterator __f, _InputIterator __l, const typename _Base::allocator_type& __a = typename _Base::allocator_type()) : _Base(__f, __l, __a) {}
    shared_vector(const shared_vector& __o) : _Base(__o) {}
    shared_vector(shared_vector&& __o) : _Base(std::move(__o)) {}
#if EPUB_COMPILER_SUPPORTS(CXX_INITIALIZER_LISTS)
    shared_vector(std::initializer_list<typename _Base::value_type> __il) : _Base(__il) {}
    shared_vector(std::initializer_list<typename _Base::value_type> __il, const typename _Base::allocator_type& __a) : _Base(__il, __a) {}
#endif
    ~shared_vector() { _Base::~_Base(); }
    
    shared_vector& operator=(const shared_vector& __o) { _Base::operator=(__o); return *this; }
    shared_vector& operator=(shared_vector&& __o) { _Base::operator=(std::move(__o)); return *this; }
};

template <class _Tp>
class shared_list : public std::list<std::shared_ptr<_Tp>>
{
    typedef std::list<std::shared_ptr<_Tp>>     _Base;
    
public:
    shared_list() _NOEXCEPT_(std::is_nothrow_default_constructible<typename _Base::value_type>::value) : _Base() {}
    explicit shared_list(const typename _Base::allocator_type& __a) : _Base(__a) {}
    explicit shared_list(typename _Base::size_type __n) : _Base(__n) {}
    shared_list(typename _Base::size_type __n, const typename _Base::value_type& __v) : _Base(__n, __v) {}
    shared_list(typename _Base::size_type __n, const typename _Base::value_type& __v, const typename _Base::allocator_type& __a) : _Base(__n, __v, __a) {}
    template <class _Iter>
    shared_list(_Iter __f, _Iter __l) : _Base(__f, __l) {}
    template <class _Iter>
    shared_list(_Iter __f, _Iter __l, const typename _Base::allocator_type& __a) : _Base(__f, __l, __a) {}
    shared_list(const shared_list& __o) : _Base(__o) {}
    shared_list(shared_list&& __o) : _Base(std::move(__o)) {}
#if EPUB_COMPILER_SUPPORTS(CXX_INITIALIZER_LISTS)
    shared_list(std::initializer_list<typename _Base::value_type> __il) : _Base(__il) {}
    shared_list(std::initializer_list<typename _Base::value_type> __il, const typename _Base::allocator_type& __a) : _Base(__il, __a) {}
#endif
    ~shared_list() { _Base::~_Base(); }
    
    shared_list& operator=(const shared_list& __o) { _Base::operator=(__o); return *this; }
    shared_list& operator=(shared_list&& __o) { _Base::operator=(std::move(__o)); return *this; }
};
        
#endif

//////////////////////////////////////////////////////////////////////////////
// nicer way of constructing a C++ string from randomly-typed arguments

#if EPUB_COMPILER_SUPPORTS(CXX_VARIADIC_TEMPLATES)
static inline std::stringstream& __format(std::stringstream& s) { return s; }

template <typename Arg1, typename... Args>
static inline std::stringstream& __format(std::stringstream& s, const Arg1& arg1, const Args&... args)
{
    s << arg1;
    return __format(s, args...);
}

/**
 Formats a string from a list of components.
 
 This will stringify and concatenate the value of each component using a C++
 stringstream object. It is assumed that all arguments can be sent to an ostream
 object using the `<<` operator. To include spaces they must be explicitly added as
 strings, e.g. `_Str("There are ", 5, " items here.", std::endl);
 
 The implementation is essentially expanded to the following, for any number of
 arguments:
 
     std::stringstream ss;
     ss << Arg1 << Arg2 << ... << ArgN;
     return ss.str();
 */
template <typename... Args>
static inline std::string _Str(const Args&... args)
{
    std::stringstream s;
    return __format(s, args...).str();
}
#else   // !EPUB_COMPILER_SUPPORTS(CXX_VARIADIC_TEMPLATES)
template <typename _A1>
static inline std::string _Str(const _A1& a1)
{
    std::stringstream s;
    s << a1;
    return s.str();
}
template <typename _A1, typename _A2>
static inline std::string _Str(const _A1& a1, const _A2& a2)
{
    std::stringstream s;
    s << a1 << a2;
    return s.str();
}
template <typename _A1, typename _A2, typename _A3>
static inline std::string _Str(const _A1& a1, const _A2& a2, const _A3& a3)
{
    std::stringstream s;
    s << a1 << a2 << a3;
    return s.str();
}
template <typename _A1, typename _A2, typename _A3, typename _A4>
static inline std::string _Str(const _A1& a1, const _A2& a2, const _A3& a3,
                               const _A4& a4)
{
    std::stringstream s;
    s << a1 << a2 << a3 << a4;
    return s.str();
}
template <typename _A1, typename _A2, typename _A3, typename _A4, typename _A5>
static inline std::string _Str(const _A1& a1, const _A2& a2, const _A3& a3,
                               const _A4& a4, const _A5& a5)
{
    std::stringstream s;
    s << a1 << a2 << a3 << a4 << a5;
    return s.str();
}
template <typename _A1, typename _A2, typename _A3, typename _A4, typename _A5, typename _A6>
static inline std::string _Str(const _A1& a1, const _A2& a2, const _A3& a3,
                               const _A4& a4, const _A5& a5, const _A6& a6)
{
    std::stringstream s;
    s << a1 << a2 << a3 << a4 << a5 << a6;
    return s.str();
}
template <typename _A1, typename _A2, typename _A3, typename _A4, typename _A5, typename _A6,
          typename _A7>
static inline std::string _Str(const _A1& a1, const _A2& a2, const _A3& a3,
                               const _A4& a4, const _A5& a5, const _A6& a6,
                               const _A7& a7)
{
    std::stringstream s;
    s << a1 << a2 << a3 << a4 << a5 << a6 << a7;
    return s.str();
}
template <typename _A1, typename _A2, typename _A3, typename _A4, typename _A5, typename _A6,
          typename _A7, typename _A8>
static inline std::string _Str(const _A1& a1, const _A2& a2, const _A3& a3,
                               const _A4& a4, const _A5& a5, const _A6& a6,
                               const _A7& a7, const _A8& a8)
{
    std::stringstream s;
    s << a1 << a2 << a3 << a4 << a5 << a6 << a7 << a8;
    return s.str();
}
template <typename _A1, typename _A2, typename _A3, typename _A4, typename _A5, typename _A6,
          typename _A7, typename _A8, typename _A9>
static inline std::string _Str(const _A1& a1, const _A2& a2, const _A3& a3,
                               const _A4& a4, const _A5& a5, const _A6& a6,
                               const _A7& a7, const _A8& a8, const _A9& a9)
{
    std::stringstream s;
    s << a1 << a2 << a3 << a4 << a5 << a6 << a7 << a8 << a9;
    return s.str();
}
#endif  // !EPUB_COMPILER_SUPPORTS(CXX_VARIADIC_TEMPLATES)

#if EPUB_COMPILER_SUPPORTS(CXX_USER_LITERALS)
/////////////////////////////////////////////////////////////////////////////
// C++11 user-defined literals

///
/// const xmlChar * xmlString = "this is an xmlChar* string"_xml;
inline CONSTEXPR const xmlChar* operator "" _xml (char const *s, std::size_t len)
{
    return (const xmlChar*)s;
}
#endif  // EPUB_COMPILER_SUPPORTS(CXX_USER_LITERALS)

EPUB3_END_NAMESPACE

#endif
