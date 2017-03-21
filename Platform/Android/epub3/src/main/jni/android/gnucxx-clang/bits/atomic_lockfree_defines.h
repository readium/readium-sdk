//
//  atomic_lockfree_defines.h
//  ePub3
//
//  Created by Jim Dovey on 2013-04-16.
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
//
// This is a mish-mash of LLVM libc++ <atomic> and GNU libstdc++ <atomic>,
// put here to be included in place of GNU's <atomic> when compiling using clang.
// This will hopefully enable us to use clang's __c11_atomic* builtins by ensuring all
// the GNU-style atomic types are properly marked _Atomic as per C11.
//

#ifndef _GLIBCXX_ATOMIC_LOCK_FREE_H
#define _GLIBCXX_ATOMIC_LOCK_FREE_H 1
//
//#pragma GCC system_header
//
///**
// * @addtogroup atomics
// * @{
// */
//
///**
// * Lock-free property.
// *
// * 0 indicates that the types are never lock-free.
// * 1 indicates that the types are sometimes lock-free.
// * 2 indicates that the types are always lock-free.
// */
//
//#ifdef __GXX_EXPERIMENTAL_CXX0X__
//#define ATOMIC_CHAR_LOCK_FREE 0
//#define ATOMIC_CHAR16_T_LOCK_FREE 0
//#define ATOMIC_CHAR32_T_LOCK_FREE 0
//#define ATOMIC_WCHAR_T_LOCK_FREE 0
//#define ATOMIC_SHORT_LOCK_FREE 0
//#define ATOMIC_INT_LOCK_FREE 0
//#define ATOMIC_LONG_LOCK_FREE 0
//#define ATOMIC_LLONG_LOCK_FREE 0
//#endif
//
//// @} group atomics

#endif