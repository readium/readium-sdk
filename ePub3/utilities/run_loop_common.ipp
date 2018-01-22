//
//  run_loop.cpp
//  ePub3
//
//  Created by Jim Dovey on 2013-04-08.
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

#if FUTURE_ENABLED

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

#endif //FUTURE_ENABLED

