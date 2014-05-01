//
//  _platform.h
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

#ifndef ePub3__platform_h
#define ePub3__platform_h

/* Once more, this is nabbed from WebKit: specifically <wtf/Platform.h> */

/* Include compiler-specific macros */
#include "_compiler.h"

/* ==== PLATFORM handles OS, operating environment, graphics API, and
 CPU. This macro will be phased out in favor of platform adaptation
 macros, policy decision macros, and top-level port definitions. ==== */
#define EPUB_PLATFORM(EPUB_FEATURE) (defined EPUB_PLATFORM_##EPUB_FEATURE  && EPUB_PLATFORM_##EPUB_FEATURE)


/* ==== Platform adaptation macros: these describe properties of the target environment. ==== */

/* CPU() - the target CPU architecture */
#define EPUB_CPU(EPUB_FEATURE) (defined EPUB_CPU_##EPUB_FEATURE  && EPUB_CPU_##EPUB_FEATURE)
/* HAVE() - specific system features (headers, functions or similar) that are present or not */
#define EPUB_HAVE(EPUB_FEATURE) (defined EPUB_HAVE_##EPUB_FEATURE  && EPUB_HAVE_##EPUB_FEATURE)
/* OS() - underlying operating system; only to be used for mandated low-level services like
 virtual memory, not to choose a GUI toolkit */
#define EPUB_OS(EPUB_FEATURE) (defined EPUB_OS_##EPUB_FEATURE  && EPUB_OS_##EPUB_FEATURE)


/* ==== Policy decision macros: these define policy choices for a particular port. ==== */

/* USE() - use a particular third-party library or optional OS service */
#define EPUB_USE(EPUB_FEATURE) (defined EPUB_USE_##EPUB_FEATURE  && EPUB_USE_##EPUB_FEATURE)
/* ENABLE() - turn on a specific feature of WebKit */
#define EPUB_ENABLE(EPUB_FEATURE) (defined EPUB_ENABLE_##EPUB_FEATURE  && EPUB_ENABLE_##EPUB_FEATURE)


/* ==== CPU() - the target CPU architecture ==== */

/* This also defines EPUB_CPU(BIG_ENDIAN) or EPUB_CPU(MIDDLE_ENDIAN) or neither, as appropriate. */

/* EPUB_CPU(ALPHA) - DEC Alpha */
#if defined(__alpha__)
#define EPUB_CPU_ALPHA 1
#endif

/* EPUB_CPU(IA64) - Itanium / IA-64 */
#if defined(__ia64__)
#define EPUB_CPU_IA64 1
/* 32-bit mode on Itanium */
#if !defined(__LP64__)
#define EPUB_CPU_IA64_32 1
#endif
#endif

/* EPUB_CPU(MIPS) - MIPS 32-bit */
/* Note: Only O32 ABI is tested, so we enable it for O32 ABI for now.  */
#if (defined(mips) || defined(__mips__) || defined(MIPS) || defined(_MIPS_)) \
&& defined(_ABIO32)
#define EPUB_CPU_MIPS 1
#if defined(__MIPSEB__)
#define EPUB_CPU_BIG_ENDIAN 1
#endif
#define EPUB_MIPS_PIC (defined __PIC__)
#define EPUB_MIPS_ARCH __mips
#define EPUB_MIPS_ISA(v) (defined EPUB_MIPS_ARCH && EPUB_MIPS_ARCH == v)
#define EPUB_MIPS_ISA_AT_LEAST(v) (defined EPUB_MIPS_ARCH && EPUB_MIPS_ARCH >= v)
#define EPUB_MIPS_ARCH_REV __mips_isa_rev
#define EPUB_MIPS_ISA_REV(v) (defined EPUB_MIPS_ARCH_REV && EPUB_MIPS_ARCH_REV == v)
#define EPUB_MIPS_DOUBLE_FLOAT (defined __mips_hard_float && !defined __mips_single_float)
#define EPUB_MIPS_FP64 (defined __mips_fpr && __mips_fpr == 64)
/* MIPS requires allocators to use aligned memory */
#define EPUB_USE_ARENA_ALLOC_ALIGNMENT_INTEGER 1
#endif /* MIPS */

/* EPUB_CPU(PPC) - PowerPC 32-bit */
#if   defined(__ppc__)     \
|| defined(__PPC__)     \
|| defined(__powerpc__) \
|| defined(__powerpc)   \
|| defined(__POWERPC__) \
|| defined(_M_PPC)      \
|| defined(__PPC)
#define EPUB_CPU_PPC 1
#define EPUB_CPU_BIG_ENDIAN 1
#endif

/* EPUB_CPU(PPC64) - PowerPC 64-bit */
#if   defined(__ppc64__) \
|| defined(__PPC64__)
#define EPUB_CPU_PPC64 1
#define EPUB_CPU_BIG_ENDIAN 1
#endif

/* EPUB_CPU(SH4) - SuperH SH-4 */
#if defined(__SH4__)
#define EPUB_CPU_SH4 1
#endif

/* EPUB_CPU(SPARC32) - SPARC 32-bit */
#if defined(__sparc) && !defined(__arch64__) || defined(__sparcv8)
#define EPUB_CPU_SPARC32 1
#define EPUB_CPU_BIG_ENDIAN 1
#endif

/* EPUB_CPU(SPARC64) - SPARC 64-bit */
#if defined(__sparc__) && defined(__arch64__) || defined (__sparcv9)
#define EPUB_CPU_SPARC64 1
#define EPUB_CPU_BIG_ENDIAN 1
#endif

/* EPUB_CPU(SPARC) - any SPARC, true for CPU(SPARC32) and CPU(SPARC64) */
#if EPUB_CPU(SPARC32) || EPUB_CPU(SPARC64)
#define EPUB_CPU_SPARC 1
#endif

/* EPUB_CPU(S390X) - S390 64-bit */
#if defined(__s390x__)
#define EPUB_CPU_S390X 1
#define EPUB_CPU_BIG_ENDIAN 1
#endif

/* EPUB_CPU(S390) - S390 32-bit */
#if defined(__s390__)
#define EPUB_CPU_S390 1
#define EPUB_CPU_BIG_ENDIAN 1
#endif

/* EPUB_CPU(X86) - i386 / x86 32-bit */
#if   defined(__i386__) \
|| defined(i386)     \
|| defined(_M_IX86)  \
|| defined(_X86_)    \
|| defined(__THW_INTEL)
#define EPUB_CPU_X86 1
#endif

/* EPUB_CPU(X86_64) - AMD64 / Intel64 / x86_64 64-bit */
#if   defined(__x86_64__) \
|| defined(_M_X64)
#define EPUB_CPU_X86_64 1
#endif

/* EPUB_CPU(ARM) - ARM, any version*/
#if   defined(arm) \
|| defined(__arm__) \
|| defined(ARM) \
|| defined(_ARM_)
#define EPUB_CPU_ARM 1

#if EPUB_COMPILER(CLANG) && defined(ANDROID)
# define __atomic_fetch_add(mem, val, typ)   __sync_fetch_and_add(mem, val)
# define __atomic_fetch_sub(mem, val, typ)   __sync_fetch_and_sub(mem, val)
# define __atomic_fetch_and(mem, val, typ)   __sync_fetch_and_and(mem, val)
# define __atomic_fetch_or(mem, val, typ)    __sync_fetch_and_or(mem, val)
# define __atomic_fetch_xor(mem, val, typ)   __sync_fetch_and_xor(mem, val)
# define __atomic_fetch_nand(mem, val, typ)  __sync_fetch_and_nand(mem, val)
# define __atomic_add_fetch(mem, val, typ)   __sync_add_and_fetch(mem, val)
# define __atomic_sub_fetch(mem, val, typ)   __sync_sub_and_fetch(mem, val)
# define __atomic_and_fetch(mem, val, typ)   __sync_and_and_fetch(mem, val)
# define __atomic_or_fetch(mem, val, typ) __sync_or_and_fetch(mem, val)
# define __atomic_xor_fetch(mem, val, typ)   __sync_xor_and_fetch(mem, val)
# define __atomic_nand_fetch(mem, val, typ)  __sync_nand_and_fetch(mem, val)
/*# define __atomic_load                      atomic_load
# define __atomic_store                     atomic_store
# define __atomic_exchange                  atomic_exchange*/
# define __atomic_load                      __sw_atomic_load
# define __atomic_load_n(a,m)               (*(a))
# define __atomic_store                     __sw_atomic_store
# define __atomic_store_n(a,m)              (*(a))
# define __atomic_exchange                  __sw_atomic_exchange
# define __atomic_exchange_n                __sw_atomic_exchange
# include <backup_atomics.h>
#endif

#if defined(__ARM_PCS_VFP)
#define EPUB_CPU_ARM_HARDFP 1
#endif

#if defined(__ARMEB__) || (EPUB_COMPILER(RVCT) && defined(__BIG_ENDIAN))
#define EPUB_CPU_BIG_ENDIAN 1

#elif !defined(__ARM_EABI__) \
&& !defined(__EABI__) \
&& !defined(__VFP_FP__) \
&& !defined(_WIN32_WCE) \
&& !defined(ANDROID)
#define EPUB_CPU_MIDDLE_ENDIAN 1

#endif

#define EPUB_ARM_ARCH_AT_LEAST(N) (EPUB_CPU(ARM) && EPUB_ARM_ARCH_VERSION >= N)

/* Set EPUB_ARM_ARCH_VERSION */
#if   defined(__ARM_ARCH_4__) \
|| defined(__ARM_ARCH_4T__) \
|| defined(__MARM_ARMV4__) \
|| defined(_ARMV4I_)
#define EPUB_ARM_ARCH_VERSION 4

#elif defined(__ARM_ARCH_5__) \
|| defined(__ARM_ARCH_5T__) \
|| defined(__MARM_ARMV5__)
#define EPUB_ARM_ARCH_VERSION 5

#elif defined(__ARM_ARCH_5E__) \
|| defined(__ARM_ARCH_5TE__) \
|| defined(__ARM_ARCH_5TEJ__)
#define EPUB_ARM_ARCH_VERSION 5
/*ARMv5TE requires allocators to use aligned memory*/
#define EPUB_USE_ARENA_ALLOC_ALIGNMENT_INTEGER 1

#elif defined(__ARM_ARCH_6__) \
|| defined(__ARM_ARCH_6J__) \
|| defined(__ARM_ARCH_6K__) \
|| defined(__ARM_ARCH_6Z__) \
|| defined(__ARM_ARCH_6ZK__) \
|| defined(__ARM_ARCH_6T2__) \
|| defined(__ARMV6__)
#define EPUB_ARM_ARCH_VERSION 6

#elif defined(__ARM_ARCH_7A__) \
|| defined(__ARM_ARCH_7R__) \
|| defined(__ARM_ARCH_7S__)
#define EPUB_ARM_ARCH_VERSION 7

/* RVCT sets _TARGET_ARCH_ARM */
#elif defined(__TARGET_ARCH_ARM)
#define EPUB_ARM_ARCH_VERSION __TARGET_ARCH_ARM

#if defined(__TARGET_ARCH_5E) \
|| defined(__TARGET_ARCH_5TE) \
|| defined(__TARGET_ARCH_5TEJ)
/*ARMv5TE requires allocators to use aligned memory*/
#define EPUB_USE_ARENA_ALLOC_ALIGNMENT_INTEGER 1
#endif

#else
#define EPUB_ARM_ARCH_VERSION 0

#endif

/* Set EPUB_THUMB_ARCH_VERSION */
#if   defined(__ARM_ARCH_4T__)
#define EPUB_THUMB_ARCH_VERSION 1

#elif defined(__ARM_ARCH_5T__) \
|| defined(__ARM_ARCH_5TE__) \
|| defined(__ARM_ARCH_5TEJ__)
#define EPUB_THUMB_ARCH_VERSION 2

#elif defined(__ARM_ARCH_6J__) \
|| defined(__ARM_ARCH_6K__) \
|| defined(__ARM_ARCH_6Z__) \
|| defined(__ARM_ARCH_6ZK__) \
|| defined(__ARM_ARCH_6M__)
#define EPUB_THUMB_ARCH_VERSION 3

#elif defined(__ARM_ARCH_6T2__) \
|| defined(__ARM_ARCH_7__) \
|| defined(__ARM_ARCH_7A__) \
|| defined(__ARM_ARCH_7M__) \
|| defined(__ARM_ARCH_7R__) \
|| defined(__ARM_ARCH_7S__)
#define EPUB_THUMB_ARCH_VERSION 4

/* RVCT sets __TARGET_ARCH_THUMB */
#elif defined(__TARGET_ARCH_THUMB)
#define EPUB_THUMB_ARCH_VERSION __TARGET_ARCH_THUMB

#else
#define EPUB_THUMB_ARCH_VERSION 0
#endif


/* EPUB_CPU(ARMV5_OR_LOWER) - ARM instruction set v5 or earlier */
/* On ARMv5 and below the natural alignment is required.
 And there are some other differences for v5 or earlier. */
#if !defined(ARMV5_OR_LOWER) && !EPUB_ARM_ARCH_AT_LEAST(6)
#define EPUB_CPU_ARMV5_OR_LOWER 1
#endif


/* EPUB_CPU(ARM_TRADITIONAL) - Thumb2 is not available, only traditional ARM (v4 or greater) */
/* EPUB_CPU(ARM_THUMB2) - Thumb2 instruction set is available */
/* Only one of these will be defined. */
#if !defined(EPUB_CPU_ARM_TRADITIONAL) && !defined(EPUB_CPU_ARM_THUMB2)
#  if defined(thumb2) || defined(__thumb2__) \
|| ((defined(__thumb) || defined(__thumb__)) && EPUB_THUMB_ARCH_VERSION == 4)
#    define EPUB_CPU_ARM_TRADITIONAL 0
#    define EPUB_CPU_ARM_THUMB2 1
#  elif EPUB_ARM_ARCH_AT_LEAST(4)
#    define EPUB_CPU_ARM_TRADITIONAL 1
#    define EPUB_CPU_ARM_THUMB2 0
#  else
#    error "Not supported ARM architecture"
#  endif
#elif EPUB_CPU(ARM_TRADITIONAL) && EPUB_CPU(ARM_THUMB2) /* Sanity Check */
#  error "Cannot use both of EPUB_CPU_ARM_TRADITIONAL and EPUB_CPU_ARM_THUMB2 platforms"
#endif /* !defined(EPUB_CPU_ARM_TRADITIONAL) && !defined(EPUB_CPU_ARM_THUMB2) */

#if defined(__ARM_NEON__) && !defined(EPUB_CPU_ARM_NEON)
#define EPUB_CPU_ARM_NEON 1
#endif

#if EPUB_CPU(ARM_NEON) && (!EPUB_COMPILER(GCC) || GCC_VERSION_AT_LEAST(4, 7, 0))
// All NEON intrinsics usage can be disabled by this macro.
#define HAVE_ARM_NEON_INTRINSICS 1
#endif

#if (defined(__VFP_FP__) && !defined(__SOFTFP__))
#define EPUB_CPU_ARM_VFP 1
#endif

#if defined(__ARM_ARCH_7S__)
#define EPUB_CPU_APPLE_ARMV7S 1
#endif

#endif /* ARM */

#if EPUB_CPU(ARM) || EPUB_CPU(MIPS) || EPUB_CPU(SH4) || EPUB_CPU(SPARC)
#define EPUB_CPU_NEEDS_ALIGNED_ACCESS 1
#endif

/* ==== OS() - underlying operating system; only to be used for mandated low-level services like
 virtual memory, not to choose a GUI toolkit ==== */

/* EPUB_OS(ANDROID) - Android */
#ifdef ANDROID
#define EPUB_OS_ANDROID 1
#endif

/* EPUB_OS(AIX) - AIX */
#ifdef _AIX
#define EPUB_OS_AIX 1
#endif

/* EPUB_OS(DARWIN) - Any Darwin-based OS, including Mac OS X and iPhone OS */
#ifdef __APPLE__
#define EPUB_OS_DARWIN 1

#include <Availability.h>
#include <AvailabilityMacros.h>
#include <TargetConditionals.h>
#endif

/* EPUB_OS(IOS) - iOS */
/* EPUB_OS(MAC_OS_X) - Mac OS X (not including iOS) */
#if EPUB_OS(DARWIN) && ((defined(TARGET_OS_EMBEDDED) && TARGET_OS_EMBEDDED) \
|| (defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE)                 \
|| (defined(TARGET_IPHONE_SIMULATOR) && TARGET_IPHONE_SIMULATOR))
#define EPUB_OS_IOS 1
#elif EPUB_OS(DARWIN) && defined(TARGET_OS_MAC) && TARGET_OS_MAC
#define EPUB_OS_MAC_OS_X 1

/* FIXME: These can be removed after sufficient time has passed since the removal of BUILDING_ON / TARGETING macros. */

#define ERROR_PLEASE_COMPARE_WITH_MAC_OS_X_VERSION_MIN_REQUIRED 0 / 0
#define ERROR_PLEASE_COMPARE_WITH_MAC_OS_X_VERSION_MAX_ALLOWED 0 / 0

#define BUILDING_ON_LEOPARD ERROR_PLEASE_COMPARE_WITH_MAC_OS_X_VERSION_MIN_REQUIRED
#define BUILDING_ON_SNOW_LEOPARD ERROR_PLEASE_COMPARE_WITH_MAC_OS_X_VERSION_MIN_REQUIRED
#define BUILDING_ON_LION ERROR_PLEASE_COMPARE_WITH_MAC_OS_X_VERSION_MIN_REQUIRED

#define TARGETING_LEOPARD ERROR_PLEASE_COMPARE_WITH_MAC_OS_X_VERSION_MAX_ALLOWED
#define TARGETING_SNOW_LEOPARD ERROR_PLEASE_COMPARE_WITH_MAC_OS_X_VERSION_MAX_ALLOWED
#define TARGETING_LION ERROR_PLEASE_COMPARE_WITH_MAC_OS_X_VERSION_MAX_ALLOWED
#endif

/* EPUB_OS(FREEBSD) - FreeBSD */
#if defined(__FreeBSD__) || defined(__DragonFly__) || defined(__FreeBSD_kernel__)
#define EPUB_OS_FREEBSD 1
#endif

/* EPUB_OS(HURD) - GNU/Hurd */
#ifdef __GNU__
#define EPUB_OS_HURD 1
#endif

/* EPUB_OS(LINUX) - Linux */
#ifdef __linux__
#define EPUB_OS_LINUX 1
#endif

/* EPUB_OS(NETBSD) - NetBSD */
#if defined(__NetBSD__)
#define EPUB_OS_NETBSD 1
#endif

/* EPUB_OS(OPENBSD) - OpenBSD */
#ifdef __OpenBSD__
#define EPUB_OS_OPENBSD 1
#endif

/* EPUB_OS(QNX) - QNX */
#if defined(__QNXNTO__)
#define EPUB_OS_QNX 1
#endif

/* EPUB_OS(SOLARIS) - Solaris */
#if defined(sun) || defined(__sun)
#define EPUB_OS_SOLARIS 1
#endif

/* EPUB_OS(WINCE) - Windows CE; note that for this platform OS(WINDOWS) is also defined */
#if defined(_WIN32_WCE)
#define EPUB_OS_WINCE 1
#endif

/* EPUB_OS(WINDOWS) - Any version of Windows */
#if defined(WIN32) || defined(_WIN32)
#define EPUB_OS_WINDOWS 1
#endif

#define EPUB_OS_WIN ERROR "USE WINDOWS WITH OS NOT WIN"
#define EPUB_OS_MAC ERROR "USE MAC_OS_X WITH OS NOT MAC"

/* EPUB_OS(UNIX) - Any Unix-like system */
#if   EPUB_OS(AIX)              \
|| EPUB_OS(ANDROID)          \
|| EPUB_OS(DARWIN)           \
|| EPUB_OS(FREEBSD)          \
|| EPUB_OS(HURD)             \
|| EPUB_OS(LINUX)            \
|| EPUB_OS(NETBSD)           \
|| EPUB_OS(OPENBSD)          \
|| EPUB_OS(QNX)              \
|| EPUB_OS(SOLARIS)          \
|| defined(unix)        \
|| defined(__unix)      \
|| defined(__unix__)
#define EPUB_OS_UNIX 1
#endif

/* EPUB_OS(BSD) - Any BSD-like system */
#if EPUB_OS(DARWIN) || EPUB_OS(FREEBSD) || EPUB_OS(NETBSD) || EPUB_OS(OPENBSD)
#define EPUB_OS_BSD 1
#endif

/* Operating environments */

/* FIXME: these are all mixes of OS, operating environment and policy choices. */
/* EPUB_PLATFORM(BLACKBERRY) */
/* EPUB_PLATFORM(MAC) */
/* EPUB_PLATFORM(WIN) */
/* EPUB_PLATFORM(ANDROID) */
#if defined(BUILDING_BLACKBERRY__)
#define EPUB_PLATFORM_BLACKBERRY 1
#elif EPUB_OS(DARWIN)
#define EPUB_PLATFORM_MAC 1
#elif EPUB_OS(WINDOWS)
#if __cplusplus_winrt
#define EPUB_PLATFORM_WINRT 1
#else
#define EPUB_PLATFORM_WIN 1
#endif
#elif EPUB_OS(ANDROID)
#define EPUB_PLATFORM_ANDROID 1
#endif

/* EPUB_PLATFORM(IOS) */
/* FIXME: this is sometimes used as an OS switch and sometimes for higher-level things */
#if (defined(TARGET_OS_EMBEDDED) && TARGET_OS_EMBEDDED) || (defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE)
#define EPUB_PLATFORM_IOS 1
#endif

/* EPUB_PLATFORM(IOS_SIMULATOR) */
#if defined(TARGET_IPHONE_SIMULATOR) && TARGET_IPHONE_SIMULATOR
#define EPUB_PLATFORM_IOS 1
#define EPUB_PLATFORM_IOS_SIMULATOR 1
#endif

/* On Windows, use QueryPerformanceCounter by default */
#if EPUB_OS(WINDOWS)
#define EPUB_USE_QUERY_PERFORMANCE_COUNTER  1
#endif

#if EPUB_OS(WINCE)
#define NOSHLWAPI      /* shlwapi.h not available on WinCe */

/* MSDN documentation says these functions are provided with uspce.lib.  But we cannot find this file. */
#define __usp10__      /* disable "usp10.h" */

#define _INC_ASSERT    /* disable "assert.h" */
#define assert(x)

#endif  /* OS(WINCE) && !PLATFORM(QT) */

#if EPUB_OS(WINCE)
#define EPUB_USE_WCHAR_UNICODE 1
#else
#define EPUB_USE_ICU_UNICODE 1
#endif

#if EPUB_PLATFORM(MAC) && !EPUB_PLATFORM(IOS)
#if __MAC_OS_X_VERSION_MIN_REQUIRED >= 1070
#define EPUB_HAVE_XPC 1
#endif
#define EPUB_HAVE_ACL 1
#define EPUB_USE_CF 1
#define EPUB_HAVE_READLINE 1
#define EPUB_HAVE_RUNLOOP_TIMER 1
#define EPUB_USE_SECURITY_FRAMEWORK 1
#endif /* EPUB_PLATFORM(MAC) && !EPUB_PLATFORM(IOS) */

#if EPUB_PLATFORM(IOS)
#define DONT_FINALIZE_ON_MAIN_THREAD 1
#endif

#if EPUB_OS(DARWIN)
#define ENABLE_PURGEABLE_MEMORY 1
#endif

#if EPUB_PLATFORM(IOS)
#define EPUB_HAVE_READLINE 1
#define EPUB_USE_CF 1
#define EPUB_USE_CFNETWORK 1
#endif /* PLATFORM(IOS) */

#if EPUB_PLATFORM(WIN) || EPUB_PLATFORM(WINRT)
#define EPUB_USE_CF 0
#endif

#if EPUB_PLATFORM(WIN) || EPUB_PLATFORM(WINRT)
#define EPUB_USE_CFNETWORK 0
#endif

#if EPUB_USE(CFNETWORK) || EPUB_PLATFORM(MAC) || EPUB_PLATFORM(IOS)
#define EPUB_USE_CFURLCACHE 1
#endif

#if !defined(EPUB_HAVE_ACCESSIBILITY)
#if EPUB_PLATFORM(IOS) || EPUB_PLATFORM(MAC) || EPUB_PLATFORM(WIN) || EPUB_PLATFORM(WINRT)
#define EPUB_HAVE_ACCESSIBILITY 1
#endif
#endif /* !defined(EPUB_HAVE_ACCESSIBILITY) */

#if EPUB_OS(UNIX)
#define EPUB_HAVE_ERRNO_H 1
#define EPUB_HAVE_MMAP 1
#define EPUB_HAVE_SIGNAL_H 1
#define EPUB_HAVE_STRINGS_H 1
#define EPUB_HAVE_SYS_PARAM_H 1
#define EPUB_HAVE_SYS_TIME_H 1
#define EPUB_USE_OS_RANDOMNESS 1
#define EPUB_USE_PTHREADS 1
#endif /* EPUB_OS(UNIX) */

#if EPUB_OS(UNIX) && !EPUB_OS(ANDROID) && !EPUB_OS(QNX)
#define EPUB_HAVE_LANGINFO_H 1
#endif

#if (EPUB_OS(FREEBSD) || EPUB_OS(OPENBSD)) && !defined(__GLIBC__)
#define EPUB_HAVE_PTHREAD_NP_H 1
#endif

#if !defined(EPUB_HAVE_VASPRINTF)
#if !EPUB_COMPILER(MSVC) && !EPUB_COMPILER(RVCT) && !EPUB_COMPILER(MINGW) && !(EPUB_COMPILER(GCC) && EPUB_OS(QNX))
#define EPUB_HAVE_VASPRINTF 1
#endif
#endif

#if !defined(EPUB_HAVE_STRNSTR)
#if EPUB_OS(DARWIN) || (EPUB_OS(FREEBSD) && !defined(__GLIBC__))
#define EPUB_HAVE_STRNSTR 1
#endif
#endif

#if defined(__GLIBC__) && GCC_VERSION_AT_LEAST(4, 8, 0)
// this is here because GCC 4.7 does NOT have emplace() in std::map,
// etc. and I need something to switch on when using it
#define EPUB_HAVE_CXX_MAP_EMPLACE 1
#endif

#if !EPUB_OS(WINDOWS) && !EPUB_OS(SOLARIS) && !EPUB_OS(ANDROID)
#define EPUB_HAVE_TM_GMTOFF 1
#define EPUB_HAVE_TM_ZONE 1
#define EPUB_HAVE_TIMEGM 1
#endif

#if EPUB_OS(DARWIN)

#define EPUB_HAVE_CXX_MAP_EMPLACE 1
#define EPUB_HAVE_MERGESORT 1
#define EPUB_HAVE_SYS_TIMEB_H 1
#define EPUB_USE_ACCELERATE 1

#if EPUB_PLATFORM(IOS) || __MAC_OS_X_VERSION_MIN_REQUIRED >= 1060

#define EPUB_HAVE_DISPATCH_H 1
#define EPUB_HAVE_MADV_FREE 1
#define EPUB_HAVE_PTHREAD_SETNAME_NP 1

#if !EPUB_PLATFORM(IOS)
#define EPUB_HAVE_HOSTED_CORE_ANIMATION 1
#define EPUB_HAVE_MADV_FREE_REUSE 1
#endif /* !EPUB_PLATFORM(IOS) */

#endif /* EPUB_PLATFORM(IOS) || __MAC_OS_X_VERSION_MIN_REQUIRED >= 1060 */

#endif /* EPUB_OS(DARWIN) */

#if EPUB_OS(WINDOWS) && !EPUB_OS(WINCE)
#define EPUB_HAVE_SYS_TIMEB_H 1
#define EPUB_HAVE_ALIGNED_MALLOC 1
#define EPUB_HAVE_ISDEBUGGERPRESENT 1
#endif

#if EPUB_OS(WINDOWS)
#define EPUB_HAVE_VIRTUALALLOC 1
#define EPUB_USE_OS_RANDOMNESS 1
#define EPUB_HAVE_CXX_MAP_EMPLACE 1
#endif

#if EPUB_COMPILER(CLANG) || (EPUB_COMPILER(GCC) && GCC_VERSION_AT_LEAST(4, 6, 0)) || (EPUB_COMPILER(MSVC) && _MSC_VER >= 1700)
#define EPUB_HAVE_STD_STRINGSTREAM 1
#else
#define EPUB_HAVE_STD_STRINGSTREAM 0
#endif

#if EPUB_OS(QNX)
#define EPUB_HAVE_MADV_FREE_REUSE 1
#define EPUB_HAVE_MADV_FREE 1
#endif

#if EPUB_PLATFORM(WINRT)
#define EPUB_HAVE_CXX_FILESYSTEM 1
#endif

/* WTF original does all the ENABLE_[FEATURE] bits here */

#endif
