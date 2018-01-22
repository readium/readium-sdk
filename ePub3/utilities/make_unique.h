//
//  make_unique.h
//  ePub3
//
//  Created by Jim Dovey on 2013-05-28.
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//  
//  Redistribution and use in source and binary forms, with or without modification, 
//  are permitted provided that the following conditions are met:
//  1. Redistributions of source code must retain the above copyright notice, this 
//  list of conditions and the following disclaimer.
//  2. Redistributions in binary form must reproduce the above copyright notice, 
//  this list of conditions and the following disclaimer in the documentation and/or 
//  other materials provided with the distribution.
//  3. Neither the name of the organization nor the names of its contributors may be 
//  used to endorse or promote products derived from this software without specific 
//  prior written permission.
//  
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
//  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
//  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
//  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
//  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
//  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
//  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
//  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE 
//  OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED 
//  OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef ePub3_make_unique_h
#define ePub3_make_unique_h

#include <ePub3/base.h>

// from the C++14 standard

#include <cstddef>
#include <memory>
#include <type_traits>
#include <utility>

#if EPUB_PLATFORM(WINRT)
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
