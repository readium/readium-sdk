//
//  CPUCacheUtils_arm.S
//  ePub3
//
//  Created by Jim Dovey on 2013-08-26.
//  Copyright (c) 2013 The Readium Foundation and contributors. All rights reserved.
//

#if __arm__

    .text
    .align 2

#if 0

/* void epub_sys_cache_invalidate(void* start, size_t len) */
    .globl  _epub_sys_cache_invalidate
_epub_sys_cache_invalidate:
    // r0 <- start, r1 <- len
    push    {r3, lr}
    cmp     r2,#0
    be      2f

    // memory fence -- ensure prior stores complete before we sync
    mcr     p15, 0, r3, c7, c10, 4      // data synchronization barrier
1:
    // flush a single cache line
    mcr     p15, 0, r0, c7, c5, 1
    // update line ptr
    add     r0,#32
    sub     r1,#32
    ba      1b

    // another fence upon completion
    mcr     p15, 0, r3, c7, c10, 4

2:
    pop     {r3, pc}    // return

/* void epub_sys_cache_flush(void* start, size_t len) */

    .globl  _epub_sys_cache_flush
_epub_sys_cache_flush:
    // r0 <- start, r1 <- len
    push    {r3, lr}
    cmp     r2,#0x0
    be      2f

    // ensure prior stores complete before we flush
    mcr     p15, 0, r3, c7, c10, 5      // data memory barrier

1:
    // flush a single cache line
    mcr     p15, 0, r0, c7, c6, 1
    // update line ptr
    add     r0,#32
    sub     r1,#32
    ba      1b

    // another fence
    mcr     p15, 0, r3, c7, c10, 5

2:
    pop     {r3, pc}    // return

#else

// seems as though these are ARM-defined opcodes which the kernel MUST implement

/* void epub_sys_cache_invalidate(void* start, size_t len) */

    .globl  _epub_sys_cache_invalidate
_epub_sys_cache_invalidate:
    mov r3, #0
    mov r12, #0x80000000
    swi #0x80
    bx  lr

/* void epub_sys_cache_flush(void* start, size_t len) */

    .globl  _epub_sys_cache_flush
_epub_sys_cache_flush:
    mov r3, #1
    mov r12, #0x80000000
    swi #0x80
    bx  lr

#endif

#endif