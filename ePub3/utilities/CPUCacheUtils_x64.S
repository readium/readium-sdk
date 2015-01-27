//
//  CPUCacheUtils_x64.s
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

#if defined(__x86_64__)

    .text
    .align 4, 0x00

/* void epub_sys_cache_invalidate(void* start, size_t len) */

    .globl  _epub_sys_cache_invalidate
_epub_sys_cache_invalidate:
    // this is a NOP on Intel processors, since the intent is to make data executable
    // and Intel L1Is are coherent with L1D.
    ret

/* void epub_sys_cache_flush(void* start, size_t len) */

    .globl  _epub_sys_cache_flush
_epub_sys_cache_flush:
    testq   %rsi,%rsi       // len == 0 ?
    jz      2f              // yes, goto exit

    mfence      // ensure prior stores complete before we flush to RAM
    clflush -1(%rdi,%rsi)   // ensure last cache line is flushed

1:
    clflush (%rdi)          // flush a line
    addq    $64,%rdi        // move to next line
    subq    $64,%rsi        // drop length
    ja      1b              // repeat while len > 0

    mfence                  // memory barrier to ensure our flushes precede later stores

2:
    ret

#endif