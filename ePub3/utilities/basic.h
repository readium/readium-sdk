//
//  basic.h
//  ePub3
//
//  Created by Jim Dovey on 2012-12-20.
//  Copyright (c) 2012-2013 The Readium Foundation.
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

#include "../base.h"
//#include "alphanum.hpp"
#include <libxml/xmlstring.h>
#include <sstream>
#include <memory>

EPUB3_BEGIN_NAMESPACE

//////////////////////////////////////////////////////////////////////////////
// shorthand forms of C++11 pointer objects

// These template aliases allow the use of shorthand declarations for different
// pointer types, i.e. Shared<Container> means std::shared_ptr<Container>.

template <class _Tp>
using Shared = std::shared_ptr<_Tp>;

template <class _Tp>
using Auto = std::unique_ptr<_Tp>;

template <class _Tp>
using Weak = std::weak_ptr<_Tp>;

//////////////////////////////////////////////////////////////////////////////
// nicer way of constructing a C++ string from randomly-typed arguments

static inline std::stringstream& __format(std::stringstream& s) { return s; }

template <typename Arg1, typename... Args>
static inline std::stringstream& __format(std::stringstream& s, const Arg1& arg1, const Args&... args)
{
    s << arg1;
    return __format(s, args...);
}

template <typename... Args>
static inline std::string _Str(const Args&... args)
{
    std::stringstream s;
    return __format(s, args...).str();
}

/////////////////////////////////////////////////////////////////////////////
// C++11 user-defined literals

// const xmlChar * xmlString = "this is an xmlChar* string"_xml;
inline constexpr const xmlChar* operator "" _xml (char const *s, std::size_t len)
{
    return (const xmlChar*)s;
}

EPUB3_END_NAMESPACE

#endif
