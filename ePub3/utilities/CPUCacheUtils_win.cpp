//
//  CPUCacheUtils_win.cpp
//  ePub3
//
//  Created by Jim Dovey on 2013-08-26.
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


#ifdef ENABLE_SYS_CACHE_FLUSH
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

#endif //ENABLE_SYS_CACHE_FLUSH