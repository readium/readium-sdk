//
//  resource_stream.h
//  ePub3
//
//  Created by Yonathan Teitelbaum (Mantano) on 2013-09-09.
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


#include <jni.h>
#include <ePub3/utilities/byte_stream.h>
#include "jni/jni.h"

#ifndef RESOURCE_STREAM
#define RESOURCE_STREAM
	class ResourceStream {

	private:
		std::unique_ptr<ePub3::ByteStream> _ptr;

	public:
		/**
		 * Constructor capture the unique_ptr.
		 */
		ResourceStream(std::unique_ptr<ePub3::ByteStream> &ptr) : _ptr(std::move(ptr)) {
		}

		/**
		 * Release unique_ptr.
		 */
		~ResourceStream();

		/**
		 * Getter.
		 */
		ePub3::ByteStream* getPtr();

	};
#endif

#ifdef __cplusplus
extern "C" {
#endif


/*
 * Exported functions
 **************************************************/

/**
 * Initialize the cached java elements for ResourceInputStream class
 */
int onLoad_cacheJavaElements_ResourceInputStream(JNIEnv *env);

/*
 * Calls the java createResourceInputStream method of ResourceInputStream class
 */
jobject javaResourceInputStream_createResourceInputStream(JNIEnv *env, long readerPtr, int length);

/*
 * JNI functions
 **************************************************/

JNIEXPORT jobject JNICALL Java_org_readium_sdk_android_util_ResourceInputStream_nativeSkip
		(JNIEnv* env, jobject thiz, jlong nativePtr, jint byteCount);

JNIEXPORT jobject JNICALL Java_org_readium_sdk_android_util_ResourceInputStream_nativeGetBytes
		(JNIEnv* env, jobject thiz, jlong nativePtr, jint dataLength);

JNIEXPORT  JNICALL void Java_org_readium_sdk_android_util_ResourceInputStream_nativeReleasePtr
		(JNIEnv* env, jobject thiz, jlong nativePtr);
/*
 * Package: org.readium.sdk.android
 * Class: ResourceInputStream
 */


#ifdef __cplusplus
}
#endif
