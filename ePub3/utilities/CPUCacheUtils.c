//
//  CPUCacheUtils.c
//  ePub3
//
//  Created by Jim Dovey on 2013-08-26.
//  Copyright (c) 2013 The Readium Foundation and contributors. All rights reserved.
//

#include "CPUCacheUtils.h"

int epub_sys_cache_control(int operation, void* start, size_t len)
{
    switch ( operation )
    {
        case kSyncCaches:
            epub_sys_cache_invalidate(start, len);
            return 0;
        case kFlushCaches:
            epub_sys_cache_flush(start, len);
            return 0;
        default:
            break;
    }
    return -1;
}