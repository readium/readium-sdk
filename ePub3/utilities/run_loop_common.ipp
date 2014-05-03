//
//  run_loop.cpp
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

#include "run_loop.h"

#if EPUB_COMPILER_SUPPORTS(CXX_THREAD_LOCAL)

ePub3::RunLoopPtr ePub3::RunLoop::CurrentRunLoop()
{
    static thread_local RunLoopPtr __myRunLoopPtr;
    if ( !bool(__myRunLoopPtr) )
    {
        __myRunLoopPtr.reset(new RunLoop);
    }
    
    return __myRunLoopPtr;
}

#else

#if EPUB_OS(WINDOWS)
# include <windows.h>
# include <stdio.h>
#if EPUB_PLATFORM(WINRT)
# include "ThreadEmulation.h"
using namespace ThreadEmulation;
#endif
# define __DestructorFn     static
# define TLS_GET(key)       TlsGetValue(key)
# define TLS_SET(key, data) TlsSetValue(key, data)
#else
# include <pthread.h>
# define __DestructorFn     __attribute__((destructor)) static
# define TLS_GET(key)       pthread_getspecific(key)
# define TLS_SET(key, data) pthread_setspecific(key, data)
#endif

EPUB3_BEGIN_NAMESPACE
#if EPUB_OS(WINDOWS) 
# if !EPUB_PLATFORM(WINRT)
# ifndef TLS_OUT_OF_INDEXES
# define TLS_OUT_OF_INDEXES ((DWORD)0xffffffff)
# endif
DWORD RunLoopTLSKey = TLS_OUT_OF_INDEXES;
# endif
#else
static pthread_key_t RunLoopTLSKey;
#endif

#if !EPUB_OS(WINDOWS)
static void _DestroyTLSRunLoop(void* data)
{
    RunLoopPtr* p = reinterpret_cast<RunLoopPtr*>(data);
    delete p;
}
#endif

# if !EPUB_PLATFORM(WINRT)
static void KillRunLoopTLSKey()
{
#if EPUB_OS(WINDOWS)
    if ( RunLoopTLSKey != TLS_OUT_OF_INDEXES )
        TlsFree(RunLoopTLSKey);
#else
    pthread_key_delete(RunLoopTLSKey);
#endif
}

INITIALIZER(InitRunLoopTLSKey)
{
#if EPUB_OS(WINDOWS)
    RunLoopTLSKey = TlsAlloc();
    if ( RunLoopTLSKey == TLS_OUT_OF_INDEXES )
    {
        fprintf(stderr, "No TLS Indexes for RunLoop!\n");
        std::terminate();
    }
    atexit(KillRunLoopTLSKey);
#else
    pthread_key_create(&RunLoopTLSKey, _DestroyTLSRunLoop);
#endif
}
#endif

RunLoopPtr RunLoop::CurrentRunLoop()
{
    RunLoopPtr* p = reinterpret_cast<RunLoopPtr*>(TLS_GET(RunLoopTLSKey));
    if ( p == nullptr )
    {
        p = new std::shared_ptr<RunLoop>(new RunLoop());
        TLS_SET(RunLoopTLSKey, reinterpret_cast<void*>(p));
    }
    return *p;
}

EPUB3_END_NAMESPACE

#endif      // !EPUB_COMPILER_SUPPORTS(CXX_THREAD_LOCAL)
