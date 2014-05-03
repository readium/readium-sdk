// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
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

#pragma once

#include <windows.h>

namespace ThreadEmulation
{
	#ifndef TLS_OUT_OF_INDEXES
	# define TLS_OUT_OF_INDEXES ((DWORD)0xffffffff)
	#endif

    #ifndef CREATE_SUSPENDED
    #define CREATE_SUSPENDED 0x00000004
    #endif

    HANDLE WINAPI CreateThread(_In_opt_ LPSECURITY_ATTRIBUTES unusedThreadAttributes, _In_ SIZE_T unusedStackSize, _In_ LPTHREAD_START_ROUTINE lpStartAddress, _In_opt_ LPVOID lpParameter, _In_ DWORD dwCreationFlags, _Out_opt_ LPDWORD unusedThreadId);
    DWORD WINAPI ResumeThread(_In_ HANDLE hThread);
    BOOL WINAPI SetThreadPriority(_In_ HANDLE hThread, _In_ int nPriority);
    
    VOID WINAPI Sleep(_In_ DWORD dwMilliseconds);

    DWORD WINAPI TlsAlloc();
    BOOL WINAPI TlsFree(_In_ DWORD dwTlsIndex);
    LPVOID WINAPI TlsGetValue(_In_ DWORD dwTlsIndex);
    BOOL WINAPI TlsSetValue(_In_ DWORD dwTlsIndex, _In_opt_ LPVOID lpTlsValue);
    
    void WINAPI TlsShutdown();
}
