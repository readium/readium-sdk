//
//  CPUCacheUtils_i386.s
//  ePub3
//
//  Created by Jim Dovey on 2013-08-26.
//  Copyright (c) 2013 The Readium Foundation and contributors. All rights reserved.
//

#if defined(__i386__) && !defined(__x86_64__)

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
