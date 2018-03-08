//
//  CPUCacheUtils_arm.S
//  ePub3
//
//  Created by Jim Dovey on 2013-08-26.
//
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.

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
