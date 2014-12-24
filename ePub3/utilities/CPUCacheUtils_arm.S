//
//  CPUCacheUtils_arm.S
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