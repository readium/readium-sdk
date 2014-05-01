//
//  log.h
//  ePub3
//
//  Created by Pedro Reis Colaco (txtr) on 2013-07-02.
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


#ifndef _LOG_JNI_H_
#define _LOG_JNI_H_


// Enable logging? 1=true/0=false
#define LOG_ENABLED 1


// If log enabled?
#if LOG_ENABLED == 1

#include <android/log.h>

// Make a string from value
#define STRINGIZE2(x) #x
#define STRINGIZE(x) STRINGIZE2(x)


// Android log tag and macros
#define  LOG_TAG    "libepub3 [" __FILE__ ":" STRINGIZE(__LINE__) "]"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define  LOGW(...)  __android_log_print(ANDROID_LOG_WARN,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

#else

// Make a string from value
#define STRINGIZE2(x) #x
#define STRINGIZE(x) STRINGIZE2(x)
// Dummy tag and macros
#define  LOG_TAG    ""
#define  LOGI(...)  ((void)0)
#define  LOGD(...)  ((void)0)
#define  LOGW(...)  ((void)0)
#define  LOGE(...)  ((void)0)

#endif // If log enabled?


#endif //ifndef _LOG_JNI_H_
