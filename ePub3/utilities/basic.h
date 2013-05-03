//
//  basic.h
//  ePub3
//
//  Created by Jim Dovey on 2012-12-20.
//  Copyright (c) 2012-2013 The Readium Foundation and contributors.
//  
//  The Readium SDK is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//  
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#ifndef ePub3_basic_h
#define ePub3_basic_h

#include <ePub3/base.h>
//#include "alphanum.hpp"
#include <libxml/xmlstring.h>
#include <sstream>
#include <memory>

EPUB3_BEGIN_NAMESPACE

//////////////////////////////////////////////////////////////////////////////
// shorthand forms of C++11 pointer objects

// These template aliases allow the use of shorthand declarations for different
// pointer types, i.e. Shared<Container> means
// std::shared_ptr<Container>.

using std::shared_ptr;
using std::weak_ptr;
using std::unique_ptr;

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
