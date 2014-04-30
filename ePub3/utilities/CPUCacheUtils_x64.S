//
//  CPUCacheUtils_x64.s
//  ePub3
//
//  Created by Jim Dovey on 2013-08-26.
//  Copyright (c) 2013 The Readium Foundation and contributors. All rights reserved.
//

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