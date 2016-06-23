//
//  make_unique.h
//  ePub3
//
//  Created by Jim Dovey on 2013-05-28.
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

#ifndef ePub3_make_unique_h
#define ePub3_make_unique_h

#include <ePub3/base.h>

// from the C++14 standard

#include <cstddef>
#include <memory>
#include <type_traits>
#include <utility>

#if EPUB_STDLIB_SUPPORTS(MAKE_UNIQUE)
using std::make_unique;
#else

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

#if EPUB_COMPILER_SUPPORTS(CXX_VARIADIC_TEMPLATES)

template <class _Tp, class... _Args>
typename __unique_if<_Tp>::single_object
make_unique(_Args&&... args)
{
    return std::unique_ptr<_Tp>(new _Tp(std::forward<_Args>(args)...));
}

#else

template <class _Tp>
typename __unique_if<_Tp>::single_object
make_unique()
{
    return std::unique_ptr<_Tp>(new _Tp());
}

template <class _Tp, class _Arg1>
typename __unique_if<_Tp>::single_object
make_unique(_Arg1&& arg1)
{
    return std::unique_ptr<_Tp>(new _Tp(std::forward<_Arg1>(arg1)));
}

template <class _Tp, class _Arg1, class _Arg2>
typename __unique_if<_Tp>::single_object
make_unique(_Arg1&& arg1, _Arg2&& arg2)
{
    return std::unique_ptr<_Tp>(new _Tp(std::forward<_Arg1>(arg1), std::forward<_Arg2>(arg2)));
}

template <class _Tp, class _Arg1, class _Arg2, class _Arg3>
typename __unique_if<_Tp>::single_object
make_unique(_Arg1&& arg1, _Arg2&& arg2, _Arg3&& arg3)
{
    return std::unique_ptr<_Tp>(new _Tp(std::forward<_Arg1>(arg1), std::forward<_Arg2>(arg2),
                                        std::forward<_Arg2>(arg3)));
}

template <class _Tp, class _Arg1, class _Arg2, class _Arg3, class _Arg4>
typename __unique_if<_Tp>::single_object
make_unique(_Arg1&& arg1, _Arg2&& arg2, _Arg3&& arg3, _Arg4&& arg4)
{
    return std::unique_ptr<_Tp>(new _Tp(std::forward<_Arg1>(arg1), std::forward<_Arg2>(arg2),
                                        std::forward<_Arg3>(arg3), std::forward<_Arg4>(arg4)));
}

template <class _Tp, class _Arg1, class _Arg2, class _Arg3, class _Arg4, class _Arg5>
typename __unique_if<_Tp>::single_object
make_unique(_Arg1&& arg1, _Arg2&& arg2, _Arg3&& arg3, _Arg4&& arg4, _Arg5&& arg5)
{
    return std::unique_ptr<_Tp>(new _Tp(std::forward<_Arg1>(arg1), std::forward<_Arg2>(arg2),
                                        std::forward<_Arg3>(arg3), std::forward<_Arg4>(arg4),
                                        std::forward<_Arg5>(arg5)));
}

template <class _Tp, class _Arg1, class _Arg2, class _Arg3, class _Arg4, class _Arg5, class _Arg6>
typename __unique_if<_Tp>::single_object
make_unique(_Arg1&& arg1, _Arg2&& arg2, _Arg3&& arg3, _Arg4&& arg4, _Arg5&& arg5, _Arg6&& arg6)
{
    return std::unique_ptr<_Tp>(new _Tp(std::forward<_Arg1>(arg1), std::forward<_Arg2>(arg2),
                                        std::forward<_Arg3>(arg3), std::forward<_Arg4>(arg4),
                                        std::forward<_Arg5>(arg5), std::forward<_Arg6>(arg6)));
}

template <class _Tp, class _Arg1, class _Arg2, class _Arg3, class _Arg4, class _Arg5, class _Arg6, class _Arg7>
typename __unique_if<_Tp>::single_object
make_unique(_Arg1&& arg1, _Arg2&& arg2, _Arg3&& arg3, _Arg4&& arg4, _Arg5&& arg5, _Arg6&& arg6, _Arg7&& arg7)
{
    return std::unique_ptr<_Tp>(new _Tp(std::forward<_Arg1>(arg1), std::forward<_Arg2>(arg2),
                                        std::forward<_Arg3>(arg3), std::forward<_Arg4>(arg4),
                                        std::forward<_Arg5>(arg5), std::forward<_Arg6>(arg6),
                                        std::forward<_Arg7>(arg7)));
}

#endif

template <class _Tp>
typename __unique_if<_Tp>::unknown_bound
make_unique(size_t n)
{
    typedef typename std::remove_extent<_Tp>::type _Up;
    return std::unique_ptr<_Tp>(new _Up[n]());
}

#if EPUB_COMPILER_SUPPORTS(CXX_VARIADIC_TEMPLATES)

template <class _Tp, class... _Args>
typename __unique_if<_Tp>::known_bound
#if EPUB_COMPILER_SUPPORTS(CXX_DELETED_FUNCTIONS)
make_unique(_Args... args) = delete;
#else
make_unique(_Args... args) { static_assert(false, "Calling a deleted function"); }
#endif

#else

template <class _Tp>
typename __unique_if<_Tp>::known_bound
#if EPUB_COMPILER_SUPPORTS(CXX_DELETED_FUNCTIONS)
make_unique(...) = delete;
#else
make_unique(...) { static_assert(false, "Calling a deleted function"); }
#endif

#endif

EPUB3_END_NAMESPACE

#endif // !EPUB_PLATFORM(WINRT)

#endif
