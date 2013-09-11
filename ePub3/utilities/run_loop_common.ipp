//
//  run_loop.cpp
//  ePub3
//
//  Created by Jim Dovey on 2013-04-08.
//  Copyright (c) 2013 The Readium Foundation and contributors. All rights reserved.
//

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
DWORD RunLoopTLSKey = TLS_OUT_OF_INDEXES;
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
        ExitProcess(0);
    }
    atexit(KillRunLoopTLSKey);
#else
    pthread_key_create(&RunLoopTLSKey, _DestroyTLSRunLoop);
#endif
}

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
