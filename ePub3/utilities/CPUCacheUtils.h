//
//  CPUCacheUtils.h
//  ePub3
//
//  Created by Jim Dovey on 2013-08-26.
//  Copyright (c) 2013 The Readium Foundation and contributors. All rights reserved.
//

// based on OS X <libkern/OSCacheControl.h>

#ifndef ePub3_CPUCacheUtils_h
#define ePub3_CPUCacheUtils_h

#include <stdlib.h>

// operation codes for epub_sys_cache_control()
enum
{
    kSyncCaches     = 1,        // sync RAM changes into CPU caches
    kFlushCaches    = 2,        // push CPU caches out to memory & invalidate them
};

__BEGIN_DECLS

int epub_sys_cache_control(int operation, void* start, size_t len);

// equivalent to epub_sys_cache_control(kSyncCaches, ...)
void epub_sys_cache_invalidate(void* start, size_t len);

// equivalent to epub_sys_cache_control(kFlushCaches, ...)
void epub_sys_cache_flush(void* start, size_t len);

__END_DECLS

#endif
