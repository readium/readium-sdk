//
//  _compiler.h
//  ePub3
//
//  Created by Jim Dovey on 2013-04-08.
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

#ifndef ePub3__compiler_h
#define ePub3__compiler_h

// shamelessly based upon the WTF (Web Types Framework?) Compiler.h in WebKit.

/* EPUB_COMPILER() - the compiler being used to build the project */
#define EPUB_COMPILER(EPUB_FEATURE) (defined EPUB_COMPILER_##EPUB_FEATURE  && EPUB_COMPILER_##EPUB_FEATURE)

/* EPUB_COMPILER_SUPPORTS() - whether the compiler being used to build the project supports the given feature. */
#define EPUB_COMPILER_SUPPORTS(EPUB_COMPILER_FEATURE) (defined EPUB_COMPILER_SUPPORTS_##EPUB_COMPILER_FEATURE  && EPUB_COMPILER_SUPPORTS_##EPUB_COMPILER_FEATURE)

/* COMPILER_QUIRK() - whether the compiler being used to build the project requires a given quirk. */
#define EPUB_COMPILER_QUIRK(_EPUB_COMPILER_QUIRK) (defined EPUB_COMPILER_QUIRK_##_EPUB_COMPILER_QUIRK  && EPUB_COMPILER_QUIRK_##_EPUB_COMPILER_QUIRK)

/* EPUB_STDLIB_SUPPORTS() - whether the standard lib being used to build the project supports the given feature. */
#define EPUB_STDLIB_SUPPORTS(EPUB_STDLIB_FEATURE) (defined EPUB_STDLIB_SUPPORTS_##EPUB_STDLIB_FEATURE  && EPUB_STDLIB_SUPPORTS_##EPUB_STDLIB_FEATURE)

/* ==== COMPILER() - the compiler being used to build the project ==== */

/* COMPILER(CLANG) - Clang  */
#if defined(__clang__)
#define EPUB_COMPILER_CLANG 1

#define CLANG_VERSION (__clang_major__ * 10000 + __clang_minor__ * 100 + __clang_patchlevel__)
#define CLANG_VERSION_AT_LEAST(major, minor, patch) (CLANG_VERSION >= (major * 10000 + minor * 100 + patch))

#ifndef __has_extension
#define __has_extension __has_feature /* Compatibility with older versions of clang */
#endif

#define EPUB_CLANG_PRAGMA(PRAGMA) _Pragma(PRAGMA)

/* C++11 Features */

/* Specific compiler features */
#define EPUB_COMPILER_SUPPORTS_CXX_VARIADIC_TEMPLATES __has_extension(cxx_variadic_templates)

/* There is a bug in clang that comes with Xcode 4.2 where AtomicStrings can't be implicitly converted to Strings
 in the presence of move constructors and/or move assignment operators. This bug has been fixed in Xcode 4.3 clang, so we
 check for both cxx_rvalue_references as well as the unrelated cxx_nonstatic_member_init feature which we know was added in 4.3 */
#define EPUB_COMPILER_SUPPORTS_CXX_RVALUE_REFERENCES __has_extension(cxx_rvalue_references) && __has_extension(cxx_nonstatic_member_init)

#define EPUB_COMPILER_SUPPORTS_CXX_NONSTATIC_MEMBER_INIT __has_extension(cxx_nonstatic_member_init)
#define EPUB_COMPILER_SUPPORTS_CXX_DELETED_FUNCTIONS __has_extension(cxx_deleted_functions)
#define EPUB_COMPILER_SUPPORTS_CXX_DEFAULTED_FUNCTIONS __has_extension(cxx_defaulted_functions)
#define EPUB_COMPILER_SUPPORTS_CXX_NULLPTR __has_feature(cxx_nullptr)
#define EPUB_COMPILER_SUPPORTS_CXX_EXPLICIT_CONVERSIONS __has_feature(cxx_explicit_conversions)
#define EPUB_COMPILER_SUPPORTS_BLOCKS __has_feature(blocks)
#define EPUB_COMPILER_SUPPORTS_C_STATIC_ASSERT __has_extension(c_static_assert)
#define EPUB_COMPILER_SUPPORTS_CXX_STATIC_ASSERT __has_extension(cxx_static_assert)
#define EPUB_COMPILER_SUPPORTS_CXX_OVERRIDE_CONTROL __has_extension(cxx_override_control)
#define EPUB_COMPILER_SUPPORTS_HAS_TRIVIAL_DESTRUCTOR __has_extension(has_trivial_destructor)
#define EPUB_COMPILER_SUPPORTS_CXX_STRONG_ENUMS __has_extension(cxx_strong_enums)
#define EPUB_COMPILER_SUPPORTS_CXX_ALIAS_TEMPLATES __has_extension(cxx_alias_templates)
#define EPUB_COMPILER_SUPPORTS_CXX_CONSTEXPR __has_extension(cxx_constexpr)
#define EPUB_COMPILER_SUPPORTS_CXX_INITIALIZER_LISTS __has_extension(cxx_generalized_initializers)
#define EPUB_COMPILER_SUPPORTS_CXX_RAW_STRING_LITERALS __has_extension(cxx_raw_string_literals)
#define EPUB_COMPILER_SUPPORTS_CXX_UNICODE_LITERALS __has_extension(cxx_unicode_literals)
#define EPUB_COMPILER_SUPPORTS_CXX_USER_LITERALS __has_extension(cxx_user_literals)
#define EPUB_COMPILER_SUPPORTS_CXX_DEFAULT_TEMPLATE_ARGS __has_extension(cxx_default_function_template_args)
#define EPUB_COMPILER_SUPPORTS_CXX_DELEGATING_CONSTRUCTORS __has_extension(cxx_delegating_constructors)
#define EPUB_COMPILER_SUPPORTS_CXX_NOEXCEPT __has_extension(cxx_noexcept)
#define EPUB_COMPILER_SUPPORTS_CXX_IMPLICIT_MOVES __has_extension(cxx_implicit_moves)
#define EPUB_COMPILER_SUPPORTS_CXX_INHERITING_CONSTRUCTORS __has_extension(cxx_inheriting_constructors)
#define EPUB_COMPILER_SUPPORTS_CXX_INLINE_NAMESPACES __has_extension(cxx_inline_namespaces)
#define EPUB_COMPILER_SUPPORTS_CXX_LOCAL_UNNAMED_TEMPLATE_ARGS __has_extension(cxx_local_type_template_args)
#define EPUB_COMPILER_SUPPORTS_CXX_REFERENCE_QUALIFIED_FUNCTIONS __has_extension(cxx_reference_qualified_functions)
#define EPUB_COMPILER_SUPPORTS_CXX_AUTO_TYPE __has_extension(cxx_auto_type)
#define EPUB_COMPILER_SUPPORTS_CXX_THREAD_LOCAL __has_extension(cxx_thread_local)
#define EPUB_COMPILER_SUPPORTS_CXX_TRAILING_RETURN __has_extension(cxx_trailing_return)
#define EPUB_COMPILER_SUPPORTS_CXX_UNRESTRICTED_UNIONS __has_extension(cxx_unrestricted_unions)
#define EPUB_COMPILER_SUPPORTS_CXX_ALIGNAS __has_extension(cxx_alignas)
#define EPUB_COMPILER_SUPPORTS_CXX_DECLTYPE __has_extension(cxx_decltype)
#define EPUB_COMPILER_SUPPORTS_CXX_ATTRIBUTES __has_extension(cxx_attributes)
#define EPUB_COMPILER_SUPPORTS_CXX_EXTERN_TEMPLATES CLANG_VERSION_AT_LEAST(3,2,0)
#define EPUB_COMPILER_SUPPORTS_CXX_FINAL_KEYWORD EPUB_COMPILER_SUPPORTS_CXX_OVERRIDE_CONTROL

/* C++14 features */

#define EPUB_COMPILER_SUPPORTS_CXX_BINARY_LITERALS __has_extension(cxx_binary_literals)
#define EPUB_COMPILER_SUPPORTS_CXX_RELAXED_CONSTEXPR __has_extension(cxx_relaxed_constexpr)
#define EPUB_COMPILER_SUPPORTS_CXX_CONTEXTUAL_CONVERSIONS __has_extension(cxx_contextual_conversions)
#define EPUB_COMPILER_SUPPORTS_CXX_DECLTYPE_AUTO __has_extension(cxx_decltype_auto)
#define EPUB_COMPILER_SUPPORTS_CXX_AGGREGATE_NSDMI __has_extension(cxx_aggregate_nsdmi)
#define EPUB_COMPILER_SUPPORTS_CXX_INIT_CAPTURE __has_extension(cxx_init_capture)
#define EPUB_COMPILER_SUPPORTS_CXX_GENERIC_LAMBDA __has_extension(cxx_generic_lambda)
#define EPUB_COMPILER_SUPPORTS_CXX_RETURN_TYPE_DEDUCTION __has_extension(cxx_return_type_deduction)
#define EPUB_COMPILER_SUPPORTS_CXX_RUNTIME_SIZED_ARRAYS __has_extension(cxx_runtime_array)
#define EPUB_COMPILER_SUPPORTS_CXX_VARIABLE_TEMPLATES __has_extension(cxx_variable_templates)

#endif

#ifndef EPUB_CLANG_PRAGMA
#define EPUB_CLANG_PRAGMA(PRAGMA)
#endif

/* EPUB_COMPILER(MSVC) - Microsoft Visual C++ */
/* EPUB_COMPILER(MSVC7_OR_LOWER) - Microsoft Visual C++ 2003 or lower*/
/* EPUB_COMPILER(MSVC9_OR_LOWER) - Microsoft Visual C++ 2008 or lower*/
#if defined(_MSC_VER)
#define EPUB_COMPILER_MSVC 1
#if _MSC_VER < 1400
#define EPUB_COMPILER_MSVC7_OR_LOWER 1
#elif _MSC_VER < 1600
#define EPUB_COMPILER_MSVC9_OR_LOWER 1
#endif

/* Specific compiler features */

#if !EPUB_COMPILER(CLANG)
# if _MSC_VER >= 1600
#  define EPUB_COMPILER_SUPPORTS_CXX_NULLPTR 1
# endif
# if _MSC_VER >= 1700               // VS2012
# if defined(WINRT)
#  define EPUB_STDLIB_SUPPORTS_MAKE_UNIQUE 1
# endif
#  define EPUB_COMPILER_SUPPORTS_CXX_RVALUE_REFERENCES 1
#  define EPUB_COMPILER_SUPPORTS_C_STATIC_ASSERT 1
#  define EPUB_COMPILER_SUPPORTS_CXX_STATIC_ASSERT 1
#  define EPUB_COMPILER_SUPPORTS_CXX_AUTO_TYPE 1
#  define EPUB_COMPILER_SUPPORTS_CXX_STRONG_ENUMS 1
# endif
# if _MSC_VER >= 1800   // VS2013
# if ! defined EPUB_STDLIB_SUPPORTS_MAKE_UNIQUE
#  define EPUB_STDLIB_SUPPORTS_MAKE_UNIQUE 1
# endif
#  define EPUB_COMPILER_SUPPORTS_CXX_DEFAULT_TEMPLATE_ARGS 1
#  define EPUB_COMPILER_SUPPORTS_CXX_DELEGATING_CONSTRUCTORS 1
#  define EPUB_COMPILER_SUPPORTS_CXX_EXPLICIT_CONVERSIONS 1
#  define EPUB_COMPILER_SUPPORTS_CXX_INITIALIZER_LISTS 1
#  define EPUB_COMPILER_SUPPORTS_CXX_RAW_STRING_LITERALS 1
#  define EPUB_COMPILER_SUPPORTS_CXX_VARIADIC_TEMPLATES 1
#  define EPUB_COMPILER_SUPPORTS_CXX_STATIC_ASSERT 1
#  define EPUB_COMPILER_SUPPORTS_CXX_TRAILING_RETURN 1
#  define EPUB_COMPILER_SUPPORTS_CXX_EXTERN_TEMPLATES 1
#  define EPUB_COMPILER_SUPPORTS_CXX_LOCAL_UNNAMED_TEMPLATE_ARGS 1
#  define EPUB_COMPILER_SUPPORTS_CXX_FINAL_KEYWORD 1
#  define EPUB_COMPILER_SUPPORTS_CXX_OVERRIDE_CONTROL 1
#  define EPUB_COMPILER_SUPPORTS_CXX_ALIAS_TEMPLATES 1
#  define EPUB_COMPILER_SUPPORTS_CXX_DELETED_FUNCTIONS 1
#  define EPUB_COMPILER_SUPPORTS_CXX_NONSTATIC_MEMBER_INIT 1
# endif
# if _MSC_VER >= 1900   // VS2015
#  define EPUB_COMPILER_SUPPORTS_CXX_DEFAULTED_FUNCTIONS 1
#  define EPUB_COMPILER_SUPPORTS_CXX_REFERENCE_QUALIFIED_FUNCTIONS 1
#  define EPUB_COMPILER_SUPPORTS_CXX_INHERITING_CONSTRUCTORS 1
#  define EPUB_COMPILER_SUPPORTS_CXX_ALIGNAS 1
#  define EPUB_COMPILER_SUPPORTS_CXX_CONSTEXPR 1
#  define EPUB_COMPILER_SUPPORTS_CXX_NOEXCEPT 1
#  define EPUB_COMPILER_SUPPORTS_CXX_DECLTYPE_AUTO 1
#  define EPUB_COMPILER_SUPPORTS_CXX_RETURN_TYPE_DEDUCTION 1
#  define EPUB_COMPILER_SUPPORTS_CXX_GENERIC_LAMBDA 1
#  define EPUB_COMPILER_SUPPORTS_CXX_RESUMABLE_FUNCTIONS 1
# endif
#endif

#if !EPUB_COMPILER(CLANG)
#define EPUB_COMPILER_SUPPORTS_CXX_OVERRIDE_CONTROL 1
#if _MSC_VER < 1700
#define EPUB_COMPILER_QUIRK_FINAL_IS_CALLED_SEALED 1
#endif
#endif

#endif

/* EPUB_COMPILER(RVCT) - ARM RealView Compilation Tools */
#if defined(__CC_ARM) || defined(__ARMCC__)
#define EPUB_COMPILER_RVCT 1
#define RVCT_VERSION_AT_LEAST(major, minor, patch, build) (__ARMCC_VERSION >= (major * 100000 + minor * 10000 + patch * 1000 + build))
#else
/* Define this for !RVCT compilers, just so we can write things like RVCT_VERSION_AT_LEAST(3, 0, 0, 0). */
#define RVCT_VERSION_AT_LEAST(major, minor, patch, build) 0
#endif

/* EPUB_COMPILER(GCCE) - GNU Compiler Collection for Embedded */
#if defined(__GCCE__)
#define EPUB_COMPILER_GCCE 1
#define GCCE_VERSION (__GCCE__ * 10000 + __GCCE_MINOR__ * 100 + __GCCE_PATCHLEVEL__)
#define GCCE_VERSION_AT_LEAST(major, minor, patch) (GCCE_VERSION >= (major * 10000 + minor * 100 + patch))
#endif

/* EPUB_COMPILER(GCC) - GNU Compiler Collection */
/* --gnu option of the RVCT compiler also defines __GNUC__ */
#if defined(__GNUC__) && !EPUB_COMPILER(RVCT)
#define EPUB_COMPILER_GCC 1
#define GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#define GCC_VERSION_AT_LEAST(major, minor, patch) (GCC_VERSION >= (major * 10000 + minor * 100 + patch))
#else
/* Define this for !GCC compilers, just so we can write things like GCC_VERSION_AT_LEAST(4, 1, 0). */
#define GCC_VERSION_AT_LEAST(major, minor, patch) 0
#endif

/* Specific compiler features */
#if EPUB_COMPILER(GCC) && !EPUB_COMPILER(CLANG) && defined(__cplusplus) && (__cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__))
# if GCC_VERSION_AT_LEAST(4,3,0)
#  define EPUB_COMPILER_SUPPORTS_CXX_RVALUE_REFERENCES 1
#  define EPUB_COMPILER_SUPPORTS_CXX_VARIADIC_TEMPLATES 1
#  define EPUB_COMPILER_SUPPORTS_CXX_STATIC_ASSERT 1
#  define EPUB_COMPILER_SUPPORTS_C_STATIC_ASSERT 1
#  define EPUB_COMPILER_SUPPORTS_CXX_DECLTYPE 1
#  define EPUB_COMPILER_SUPPORTS_CXX_DEFAULT_TEMPLATE_ARGS 1
#  define EPUB_COMPILER_SUPPORTS_CXX_EXTERN_TEMPLATES 1
# endif
# if GCC_VERSION_AT_LEAST(4,4,0)
#  define EPUB_COMPILER_SUPPORTS_CXX_INITIALIZER_LISTS 1
#  define EPUB_COMPILER_SUPPORTS_CXX_AUTO_TYPE 1
#  define EPUB_COMPILER_SUPPORTS_CXX_STRONG_ENUMS 1
#  define EPUB_COMPILER_SUPPORTS_CXX_DEFAULTED_FUNCTIONS 1
#  define EPUB_COMPILER_SUPPORTS_CXX_DELETED_FUNCTIONS 1
#  define EPUB_COMPILER_SUPPORTS_CXX_INLINE_NAMESPACES 1
# endif
# if GCC_VERSION_AT_LEAST(4,5,0)
#  define EPUB_COMPILER_SUPPORTS_CXX_EXPLICIT_CONVERSIONS 1
#  define EPUB_COMPILER_SUPPORTS_CXX_UNICODE_LITERALS 1
#  define EPUB_COMPILER_SUPPORTS_CXX_RAW_STRING_LITERALS 1
#  define EPUB_COMPILER_SUPPORTS_CXX_LOCAL_UNNAMED_TEMPLATE_ARGS 1
#  define EPUB_COMPILER_SUPPORTS_CXX_UNRESTRICTED_UNIONS 1
# endif
# if GCC_VERSION_AT_LEAST(4,6,0)
#  define EPUB_COMPILER_SUPPORTS_CXX_NULLPTR 1
#  define EPUB_COMPILER_SUPPORTS_CXX_CONSTEXPR 1
#  define EPUB_COMPILER_SUPPORTS_CXX_NOEXCEPT 1
# endif
# if GCC_VERSION_AT_LEAST(4,7,0)
#  define EPUB_COMPILER_SUPPORTS_CXX_NONSTATIC_MEMBER_INIT 1
#  define EPUB_COMPILER_SUPPORTS_CXX_ALIAS_TEMPLATES 1
#  define EPUB_COMPILER_SUPPORTS_CXX_DELEGATING_CONSTRUCTORS 1
#  define EPUB_COMPILER_SUPPORTS_CXX_USER_LITERALS 1
#  define EPUB_COMPILER_SUPPORTS_CXX_OVERRIDE_CONTROL 1
# endif
# if GCC_VERSION_AT_LEAST(4,8,0)
#  define EPUB_COMPILER_SUPPORTS_CXX_ATTRIBUTES 1
#  define EPUB_COMPILER_SUPPORTS_CXX_ALIGNAS 1
#  define EPUB_COMPILER_SUPPORTS_CXX_THREAD_LOCAL 1
#  define EPUB_COMPILER_SUPPORTS_CXX_RETURN_TYPE_DEDUCTION 1
#  define EPUB_COMPILER_SUPPORTS_CXX_INHERITING_CONSTRUCTORS 1
# endif
# if GCC_VERSION_AT_LEAST(4,8,1)
#  define EPUB_COMPILER_SUPPORTS_CXX_REFERENCE_QUALIFIED_FUNCTIONS 1
# endif
# if GCC_VERSION_AT_LEAST(4,9,0)
#  define EPUB_COMPILER_SUPPORTS_CXX_CONTEXTUAL_CONVERSIONS 1
#  define EPUB_COMPILER_SUPPORTS_CXX_BINARY_LITERALS 1
#  define EPUB_COMPILER_SUPPORTS_CXX_RUNTIME_SIZED_ARRAYS 1
#  define EPUB_COMPILER_SUPPORTS_CXX_INIT_CAPTURE 1
# endif
#endif

/* EPUB_COMPILER(MINGW) - MinGW GCC */
/* EPUB_COMPILER(MINGW64) - mingw-w64 GCC - only used as additional check to exclude mingw.org specific functions */
#if defined(__MINGW32__)
#define EPUB_COMPILER_MINGW 1
#include <_mingw.h> /* private MinGW header */
#if defined(__MINGW64_VERSION_MAJOR) /* best way to check for mingw-w64 vs mingw.org */
#define EPUB_COMPILER_MINGW64 1
#endif /* __MINGW64_VERSION_MAJOR */
#endif /* __MINGW32__ */

/* EPUB_COMPILER(INTEL) - Intel C++ Compiler */
#if defined(__INTEL_COMPILER)
#define EPUB_COMPILER_INTEL 1
#endif

/* EPUB_COMPILER(SUNCC) */
#if defined(__SUNPRO_CC) || defined(__SUNPRO_C)
#define EPUB_COMPILER_SUNCC 1
#endif

/* ==== Compiler features ==== */


/* ALWAYS_INLINE */

#ifndef ALWAYS_INLINE
#if EPUB_COMPILER(GCC) && defined(NDEBUG) && !EPUB_COMPILER(MINGW)
#define ALWAYS_INLINE inline __attribute__((__always_inline__))
#elif (EPUB_COMPILER(MSVC) || EPUB_COMPILER(RVCT)) && defined(NDEBUG)
#define ALWAYS_INLINE __forceinline
#else
#define ALWAYS_INLINE inline
#endif
#endif


/* NEVER_INLINE */

#ifndef NEVER_INLINE
#if EPUB_COMPILER(GCC)
#define NEVER_INLINE __attribute__((__noinline__))
#elif EPUB_COMPILER(RVCT)
#define NEVER_INLINE __declspec(noinline)
#else
#define NEVER_INLINE
#endif
#endif


/* UNLIKELY */

#ifndef UNLIKELY
#if EPUB_COMPILER(GCC) || (EPUB_COMPILER(RVCT) && defined(__GNUC__))
#define UNLIKELY(x) __builtin_expect((x), 0)
#else
#define UNLIKELY(x) (x)
#endif
#endif


/* LIKELY */

#ifndef LIKELY
#if EPUB_COMPILER(GCC) || (EPUB_COMPILER(RVCT) && defined(__GNUC__))
#define LIKELY(x) __builtin_expect((x), 1)
#else
#define LIKELY(x) (x)
#endif
#endif


/* NO_RETURN */


#ifndef NO_RETURN
#if EPUB_COMPILER(GCC)
#define NO_RETURN __attribute((__noreturn__))
#elif EPUB_COMPILER(MSVC) || EPUB_COMPILER(RVCT)
#define NO_RETURN __declspec(noreturn)
#else
#define NO_RETURN
#endif
#endif


/* NO_RETURN_WITH_VALUE */

#ifndef NO_RETURN_WITH_VALUE
#if !EPUB_COMPILER(MSVC)
#define NO_RETURN_WITH_VALUE NO_RETURN
#else
#define NO_RETURN_WITH_VALUE
#endif
#endif


/* WARN_UNUSED_RETURN */

#if EPUB_COMPILER(GCC)
#define WARN_UNUSED_RETURN __attribute__ ((warn_unused_result))
#else
#define WARN_UNUSED_RETURN
#endif

/* OVERRIDE and FINAL */

#if EPUB_COMPILER_SUPPORTS(CXX_OVERRIDE_CONTROL)
#define OVERRIDE override
#else
#define OVERRIDE
#endif

#if EPUB_COMPILER_SUPPORTS(CXX_FINAL_KEYWORD)
#if EPUB_COMPILER_QUIRK(FINAL_IS_CALLED_SEALED)
#define FINAL sealed
#else
#define FINAL final
#endif
#else
#define FINAL
#endif

/* REFERENCED_FROM_ASM */

#ifndef REFERENCED_FROM_ASM
#if EPUB_COMPILER(GCC)
#define REFERENCED_FROM_ASM __attribute__((used))
#else
#define REFERENCED_FROM_ASM
#endif
#endif

/* OBJC_CLASS */

#ifndef OBJC_CLASS
#ifdef __OBJC__
#define OBJC_CLASS @class
#else
#define OBJC_CLASS class
#endif
#endif

/* ABI */
#if defined(__ARM_EABI__) || defined(__EABI__)
#define EPUB_COMPILER_SUPPORTS_EABI 1
#endif

/* C++11 constexpr */
#if EPUB_COMPILER_SUPPORTS(CXX_CONSTEXPR)
#define CONSTEXPR constexpr
#else
#define CONSTEXPR
#endif

/* C++11 noexcept */
#if EPUB_COMPILER_SUPPORTS(CXX_NOEXCEPT)
#define _NOEXCEPT noexcept
#define _NOEXCEPT_(x) noexcept(x)
#else
#ifndef _MSC_VER
#define noexcept throw()
#define _NOEXCEPT throw()
#endif
#define _NOEXCEPT_(x)
#endif

#endif /* ePub3__compiler_h */
