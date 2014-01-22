//
//  waitfortask.h
//  WinRTPortingKit
//
//  Created by Vincent Richomme on 2014-01-22.
//  Copyright (c) 2014-2013 The Readium Foundation and contributors.
//  
//  The Readium SDK is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//  
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#ifndef _BASE_OAL_
#define _BASE_OAL_

#include <stdio.h>

#define POSIX_WRT 1

#ifdef __cplusplus
extern "C" {
#endif

	int __cdecl stat_wrt(const char * _StorageFile, struct stat * _Stat);

	FILE * __cdecl fopen_wrt(const char * _Filename, const char * _Mode);

	int __cdecl fseek_wrt(FILE * _File, long _Offset, int _Origin);
	
	long __cdecl ftell_wrt(FILE * _File);
	
	size_t __cdecl fread_wrt(void * _DstBuf, size_t _ElementSize, size_t _Count, FILE * _File);
	
	int __cdecl fclose_wrt(FILE * _File);
	
	void __cdecl clearerr_wrt(FILE * _File);
	
	int __cdecl feof_wrt(_In_ FILE * _File);
	
	int __cdecl ferror_wrt(_In_ FILE * _File);

	
#ifdef __cplusplus
}
#endif


#endif // _BASE_OAL_