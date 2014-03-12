//
//  CPUCacheUtils_win.cpp
//  ePub3
//
//  Created by Jim Dovey on 2013-08-26.
//  Copyright (c) 2013 The Readium Foundation and contributors. All rights reserved.
//

#include "CPUCacheUtils.h"
#if EPUB_CPU(X86) || EPUB_CPU(X64)
#include <emmintrin.h>
#endif

	int epub_sys_cache_control(int operation, void* start, long len)
	{
		switch (operation)
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

	void epub_sys_cache_invalidate(void* start, long len)
	{
#if EPUB_CPU(X86) || EPUB_CPU(X64)
		// a no-op on Intel
#elif EPUB_CPU(ARM)
		// no idea...
#endif
	}

	void epub_sys_cache_flush(void* start, long len)
	{
#if EPUB_CPU(X86) || EPUB_CPU(X64)
		if (start == nullptr || len <= 0)
			return;

		unsigned char* p = reinterpret_cast<unsigned char*>(start);

		// ensure all reads/write complete before the next instruction
		_mm_mfence();

		// ensure the last line is flushed
		_mm_clflush(p + (len - 1));

		// flush all cache lines. lines are 64 bytes on both architectures
		while (len > 0)
		{
			_mm_clflush(p);
			p += 64;
			len -= 64;
		}
#elif EPUB_CPU(ARM)
		// no idea...
#endif
	}
