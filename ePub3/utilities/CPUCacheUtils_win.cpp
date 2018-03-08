//
//  CPUCacheUtils_win.cpp
//  ePub3
//
//  Created by Jim Dovey on 2013-08-26.
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
