//
//  basic.h
//  ePub3
//
//  Created by Jim Dovey on 2012-12-20.
//  Copyright (c) 2012 Kobo Inc. All rights reserved.
//

#ifndef ePub3_basic_h
#define ePub3_basic_h

#include "../base.h"
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
using Unique = std::unique_ptr<_Tp>;

template <class _Tp>
using Auto = std::auto_ptr<_Tp>;

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

EPUB3_END_NAMESPACE

#endif
