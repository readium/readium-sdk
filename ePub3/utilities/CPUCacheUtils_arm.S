//
//  CPUCacheUtils_arm.S
//  ePub3
//
//  Created by Jim Dovey on 2013-08-26.
//  Copyright (c) 2013 The Readium Foundation and contributors. All rights reserved.
//

#if __APPLE__
# define C_FN_NAME(name) _ ## name
#else
# define C_FN_NAME(name) name
#endif

#define LABEL(name)                                 \
    .globl  name                                   ;\
name:

#if defined(arm) || defined(__arm__) || defined(ARM) || defined(_ARM_) || defined(__ARM_ARCH_7A__)

    .text
    .align 2

/* seems as though these are ARM-defined opcodes which the kernel MUST implement */

/* void epub_sys_cache_invalidate(void* start, size_t len) */
LABEL(C_FN_NAME(epub_sys_cache_invalidate))
    mov r3, #0
    mov r12, #0x80000000
    swi #0x80
    bx  lr

/* void epub_sys_cache_flush(void* start, size_t len) */
LABEL(C_FN_NAME(epub_sys_cache_flush))
    mov r3, #1
    mov r12, #0x80000000
    swi #0x80
    bx  lr

#endif
