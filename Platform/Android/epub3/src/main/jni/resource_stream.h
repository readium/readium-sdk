//
//  resource_stream.h
//  ePub3
//
//  Created by Yonathan Teitelbaum (Mantano) on 2013-09-09.
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//  
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


#include <jni.h>
#include <ePub3/utilities/byte_stream.h>
#include "jni/jni.h"

#ifndef RESOURCE_STREAM
#define RESOURCE_STREAM
	static const int DEFAULT_BUFFER_SIZE = 128*1024;

	class ResourceStream {

	private:
		std::unique_ptr<ePub3::ByteStream> _ptr;
		std::size_t _bufferSize;

	public:
		/**
		 * Constructor capture the unique_ptr.
		 */
		ResourceStream(std::unique_ptr<ePub3::ByteStream> &ptr, size_t bufferSize) : _ptr(std::move(ptr)), _bufferSize(bufferSize) {
			if (bufferSize == 0) {
				_bufferSize = DEFAULT_BUFFER_SIZE;
			}
		}

		/**
		 * Release unique_ptr.
		 */
		~ResourceStream();

		/**
		 * Getter.
		 */
		ePub3::ByteStream* getPtr();

		std::size_t getBufferSize();

		std::size_t markPosition = 0;
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
jobject javaResourceInputStream_createResourceInputStream(JNIEnv *env, long readerPtr);

/*
 * JNI functions
 **************************************************/

JNIEXPORT void JNICALL Java_org_readium_sdk_android_util_ResourceInputStream_nativeSkip
		(JNIEnv* env, jobject thiz, jlong nativePtr, jlong byteCount);

JNIEXPORT void JNICALL Java_org_readium_sdk_android_util_ResourceInputStream_nativeReset
		(JNIEnv* env, jobject thiz, jlong nativePtr, jboolean ignoreMark);

JNIEXPORT void JNICALL Java_org_readium_sdk_android_util_ResourceInputStream_nativeMark
		(JNIEnv* env, jobject thiz, jlong nativePtr);

JNIEXPORT jlong JNICALL Java_org_readium_sdk_android_util_ResourceInputStream_nativeAvailable
		(JNIEnv* env, jobject thiz, jlong nativePtr);

JNIEXPORT jbyteArray JNICALL Java_org_readium_sdk_android_util_ResourceInputStream_nativeGetBytes
		(JNIEnv* env, jobject thiz, jlong nativePtr, jlong dataLength);

JNIEXPORT jlong JNICALL Java_org_readium_sdk_android_util_ResourceInputStream_nativeGetBytesX
		(JNIEnv* env, jobject thiz, jlong nativePtr, jlong dataLength, jbyteArray jbarray);

JNIEXPORT jbyteArray JNICALL Java_org_readium_sdk_android_util_ResourceInputStream_nativeGetAllBytes
		(JNIEnv* env, jobject thiz, jlong nativePtr);

JNIEXPORT jbyteArray JNICALL Java_org_readium_sdk_android_util_ResourceInputStream_nativeGetRangeBytes
		(JNIEnv* env, jobject thiz, jlong nativePtr, jlong offset, jlong length);

JNIEXPORT jlong JNICALL Java_org_readium_sdk_android_util_ResourceInputStream_nativeGetRangeBytesX
		(JNIEnv* env, jobject thiz, jlong nativePtr, jlong offset, jlong length, jbyteArray jbarray);

JNIEXPORT void JNICALL Java_org_readium_sdk_android_util_ResourceInputStream_nativeClose
		(JNIEnv* env, jobject thiz, jlong nativePtr);

/*
 * Package: org.readium.sdk.android
 * Class: ResourceInputStream
 */


#ifdef __cplusplus
}
#endif
