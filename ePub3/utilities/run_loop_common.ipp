//
//  run_loop.cpp
//  ePub3
//
//  Created by Jim Dovey on 2013-04-08.
//  Copyright (c) 2013 The Readium Foundation and contributors. All rights reserved.
//

#include "run_loop.h"

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
    RunLoop* rl = reinterpret_cast<RunLoop*>(data);
    delete rl;
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

RunLoop* RunLoop::CurrentRunLoop()
{
    RunLoop* p = reinterpret_cast<RunLoop*>(TLS_GET(RunLoopTLSKey));
    if ( p == nullptr )
    {
        p = new RunLoop();
        TLS_SET(RunLoopTLSKey, reinterpret_cast<void*>(p));
    }
    return p;
}

EPUB3_END_NAMESPACE
