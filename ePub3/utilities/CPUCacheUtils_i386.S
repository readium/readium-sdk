//
//  CPUCacheUtils_i386.s
//  ePub3
//
//  Created by Jim Dovey on 2013-08-26.
//
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

#if defined(__i386__) && !defined(__x86_64__)

    .text
    .align 4, 0x00

/* void epub_sys_cache_invalidate(void* start, size_t len) */

    .globl  epub_sys_cache_invalidate
epub_sys_cache_invalidate:
    // this is a NOP on Intel processors, since the intent is to make data executable
    // and Intel L1Is are coherent with L1D.
    ret



/* void epub_sys_cache_flush(void* start, size_t len) */

    .globl epub_sys_cache_flush
epub_sys_cache_flush:
    movl    8(%esp),%ecx        // %exc <- len
    movl    4(%esp),%edx        // %edx <- start
    testl   %ecx,%ecx           // length == 0 ?
    jz      2f                  // if length == 0 return

    mfence                      // ensure previous stores complete before we flush
    clflush -1(%edx, %ecx)      // make sure last line is flushed

1:
    clflush (%edx)              // flush one line
    addl    $64,%edx            // step to next line
    subl    $64,%ecx            // drop count by one line
    ja      1b                  // repeat while %ecx > 0
    mfence                      // mfence when done, to ensure our flushes precede later stores
2:
    ret

#endif
