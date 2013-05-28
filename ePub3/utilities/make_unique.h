//
//  make_unique.h
//  ePub3
//
//  Created by Jim Dovey on 2013-05-28.
//  Copyright (c) 2013 The Readium Foundation and contributors. All rights reserved.
//

#ifndef ePub3_make_unique_h
#define ePub3_make_unique_h

#include <ePub3/base.h>

// from the C++14 standard

#include <cstddef>
#include <memory>
#include <type_traits>
#include <utility>

EPUB3_BEGIN_NAMESPACE

template <class _Tp>
struct __unique_if
{
    typedef std::unique_ptr<_Tp> single_object;
};

template <class _Tp>
struct __unique_if<_Tp[]>
{
    typedef std::unique_ptr<_Tp> unknown_bound;
};

template <class _Tp, size_t _Size>
struct __unique_if<_Tp[_Size]>
{
    typedef void known_bound;
};

template <class _Tp, class... _Args>
typename __unique_if<_Tp>::single_object
make_unique(_Args&&... args)
{
    return std::unique_ptr<_Tp>(new _Tp(std::forward<_Args>(args)...));
}

template <class _Tp>
typename __unique_if<_Tp>::unknown_bound
make_unique(size_t n)
{
    typedef typename std::remove_extent<_Tp>::type _Up;
    return std::unique_ptr<_Tp>(new _Up[n]());
}

template <class _Tp, class... _Args>
typename __unique_if<_Tp>::known_bound
#if EPUB_COMPILER_SUPPORTS(CXX_DELETED_FUNCTIONS)
make_unique(_Args... args) = delete;
#else
make_unique(_Args... args) { static_assert(false, "Calling a deleted function"); }
#endif

EPUB3_END_NAMESPACE

#endif
