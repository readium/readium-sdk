//
//  jni_cache_dir.c
//  ePub3
//
//  Created by Jim Dovey on 2013-04-12.
//  Copyright (c) 2012-2013 The Readium Foundation and contributors.
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

#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <jni.h>

char gAndroidCacheDir[PATH_MAX] = {0};

JNIEXPORT void JNICALL Java_com_readium_CoreGlobals_setCacheDir(JNIEnv* env, jobject thiz, jstring cachePath)
{
    const char* str = (*env)->GetStringUTFChars(env, cachePath, NULL);
    strlcpy(gAndroidCacheDir, str, PATH_MAX);
    (*env)->ReleaseStringUTFChars(env, cachePath, str);
}
