//
//  CPUCacheUtils_x64.s
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
