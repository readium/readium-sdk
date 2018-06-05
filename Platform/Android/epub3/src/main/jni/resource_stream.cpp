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


//#include <ePub3/utilities/resource_stream.h>
#include <string>
#include <memory>
#include <algorithm>    // std::min
#include <typeinfo>
#include <ePub3/utilities/byte_stream.h>
#include <ePub3/filter.h>
#include <ePub3/filter_chain_byte_stream_range.h>
#include <ePub3/filter_chain_byte_stream.h>

#include "jni/jni.h"

#include "epub3.h"
#include "helpers.h"
#include "resource_stream.h"

ePub3::ByteStream* ResourceStream::getPtr() {
	ePub3::ByteStream* reader = _ptr.get();
	return reader;
}

std::size_t ResourceStream::getBufferSize() {
	return _bufferSize;
}

ResourceStream::~ResourceStream() {
	ePub3::ByteStream* reader = _ptr.release();
	delete reader;
}

#ifdef __cplusplus
extern "C" {
#endif


/*
 * Internal constants
 **************************************************/
static const char *java_class_ResourceInputStream_name = "org/readium/sdk/android/util/ResourceInputStream";
static const char *java_class_IOException_name = "java/io/IOException";

static const char *java_method_ResourceInputStream_createResourceInputStream_name = "createResourceInputStream";
static const char *java_method_ResourceInputStream_createResourceInputStream_sign = "(J)Lorg/readium/sdk/android/util/ResourceInputStream;";

/*
 * Internal variables
 **************************************************/

/*
 * Cached classes, methods and fields IDs.
 */

static jni::Class java_class_ResourceInputStream;

static jni::Class java_class_IOException;

static jni::StaticMethod<jobject> java_method_ResourceInputStream_createResourceInputStream;

/*
 * Exported functions
 **************************************************/

/**
 * Initialize the cached java elements for ResourceInputStream class
 */
int onLoad_cacheJavaElements_ResourceInputStream(JNIEnv *env) {

	// Cache IRI class
	jclass java_class_ResourceInputStream_ = NULL;
	INIT_CLASS_RETVAL(java_class_ResourceInputStream_, java_class_ResourceInputStream_name, ONLOAD_ERROR);
	java_class_ResourceInputStream = jni::Class(env, java_class_ResourceInputStream_);

	jclass java_class_IOException_ = NULL;
	INIT_CLASS_RETVAL(java_class_IOException_, java_class_IOException_name, ONLOAD_ERROR);
	java_class_IOException = jni::Class(env, java_class_IOException_);

	// Cache IRI class methods
	java_method_ResourceInputStream_createResourceInputStream = jni::StaticMethod<jobject>(env, java_class_ResourceInputStream, java_method_ResourceInputStream_createResourceInputStream_name, java_method_ResourceInputStream_createResourceInputStream_sign);

	// Return JNI_VERSION for OK, if not one of the lines above already returned ONLOAD_ERROR
	return JNI_VERSION;
}

/**
 * Calls the java createResourceInputStream method of ResourceInputStream class
 */
jobject javaResourceInputStream_createResourceInputStream(JNIEnv *env, long streamPtr) {

	return java_method_ResourceInputStream_createResourceInputStream(env, (jlong) streamPtr);
}

static jbyteArray GetAllBytes(JNIEnv* env, jobject thiz, jlong nativePtr) {

	LOGD("JNI --- GetAllBytes ...\n");

	ResourceStream* stream = (ResourceStream*) nativePtr;

	std::size_t tmpBufferSize = stream->getBufferSize();
	uint8_t tmpBuffer[tmpBufferSize]; // stack

	std::size_t MAX = 10 * 1024 * 1024; // MB
	jbyte * fullBuffer = new jbyte[MAX]; // heap

	ePub3::ByteStream* byteStream = stream->getPtr();

	std::size_t totalRead = 0;
	while (totalRead < MAX) {
		std::size_t bytesRead = byteStream->ReadBytes(&tmpBuffer, tmpBufferSize);

		if (bytesRead == 0) {
			break;
		}

		if ((totalRead + bytesRead) > MAX) {
			bytesRead = MAX - totalRead;
		}

		::memcpy(reinterpret_cast<uint8_t*>(fullBuffer) + totalRead, &tmpBuffer, bytesRead);

		totalRead += bytesRead;
	}

	LOGD("JNI --- GetAllBytes: %d\n", totalRead);

	jbyteArray jfullBuffer = env->NewByteArray((jsize)totalRead);
	env->SetByteArrayRegion(jfullBuffer, 0, (jsize)totalRead, fullBuffer);

	//env->DeleteLocalRef(jtmpBuffer); nope, JVM / Dalvik garbage collector

    delete [] fullBuffer;

	return jfullBuffer;
}

static jlong GetBytesX(JNIEnv* env, jobject thiz, jlong nativePtr, jlong jlen, jbyteArray jbarray) {

    jbyte* tmpBuffer = env->GetByteArrayElements(jbarray, NULL);
    jsize  dataLength = env->GetArrayLength(jbarray);

	LOGD("JNI --- GetBytes_ 1: %d - %ld\n", (std::size_t)dataLength, (long)jlen);

    if ((jsize)jlen < dataLength) {
    	dataLength = (jsize)jlen;
    }

	ResourceStream* stream = (ResourceStream*) nativePtr;

	ePub3::ByteStream* byteStream = stream->getPtr();



	ePub3::FilterChainByteStreamRange *rangeByteStream = dynamic_cast<ePub3::FilterChainByteStreamRange *>(byteStream);
	if (rangeByteStream != nullptr) {
		LOGD("JNI --- GetBytes_ FilterChainByteStreamRange\n");
	} else {
		ePub3::FilterChainByteStream *nonRangeByteStream = dynamic_cast<ePub3::FilterChainByteStream*>(byteStream);
		if (nonRangeByteStream != nullptr) {
			LOGD("JNI --- GetBytes_ FilterChainByteStream\n");
		}
		else {
			ePub3::SeekableByteStream *seekableStream = dynamic_cast<ePub3::SeekableByteStream *>(byteStream);
			if (rangeByteStream != nullptr) {
				LOGD("JNI --- GetBytes_ SeekableByteStream\n");
			} else {
				LOGD("JNI --- GetBytes_ ByteStream\n");
			}
		}
	}

	std::size_t bytesRead = byteStream->ReadBytes(reinterpret_cast<uint8_t*>(tmpBuffer), (std::size_t)dataLength);

	LOGD("JNI --- GetBytes_ 2: %d\n", bytesRead);

	std::size_t diff = (std::size_t)dataLength - bytesRead;
	if (diff > 0) {
		LOGD("JNI --- GetBytes_ 3: %d\n", diff);
	}

    env->ReleaseByteArrayElements(jbarray, tmpBuffer, 0);

    return (jlong)bytesRead;
}

static jbyteArray GetBytes(JNIEnv* env, jobject thiz, jlong nativePtr, jlong dataLength) {

	LOGD("JNI --- GetBytes 1: %ld\n", (long)dataLength);

	ResourceStream* stream = (ResourceStream*) nativePtr;

	jbyte * tmpBuffer = new jbyte[(std::size_t)dataLength]; // heap

	ePub3::ByteStream* byteStream = stream->getPtr();
	std::size_t bytesRead = byteStream->ReadBytes(reinterpret_cast<uint8_t*>(tmpBuffer), (std::size_t)dataLength);

	LOGD("JNI --- GetBytes 2: %d\n", bytesRead);

	jbyteArray jtmpBuffer = env->NewByteArray((jsize)bytesRead);
	env->SetByteArrayRegion(jtmpBuffer, 0, (jsize)bytesRead, tmpBuffer);

    delete [] tmpBuffer;

	return jtmpBuffer;
}

static jlong GetBytesRangeX(JNIEnv* env, jobject thiz, jlong nativePtr, jlong offset, jlong jlen, jbyteArray jbarray) {

    jbyte* tmpBuffer = env->GetByteArrayElements(jbarray, NULL);
    jsize  dataLength = env->GetArrayLength(jbarray);

	LOGD("JNI --- GetBytesRange_ 1: %d - %ld\n", (std::size_t)dataLength, (long)jlen);

    if ((jsize)jlen < dataLength) {
    	dataLength = (jsize)jlen;
    }

	ResourceStream* stream = (ResourceStream*) nativePtr;
	ePub3::ByteStream* byteStream = stream->getPtr();
	ePub3::FilterChainByteStreamRange *rangeByteStream = dynamic_cast<ePub3::FilterChainByteStreamRange *>(byteStream);

	std::size_t readBytes;

	if (rangeByteStream != nullptr) {
		LOGD("JNI --- GetBytesRange_ FilterChainByteStreamRange\n");
		ePub3::ByteRange range;
		range.Location((std::size_t)offset);
		range.Length((std::size_t)dataLength);
		readBytes = rangeByteStream->ReadBytes(reinterpret_cast<uint8_t*>(tmpBuffer), (std::size_t)dataLength, range);
	} else {
		ePub3::SeekableByteStream *seekableStream = dynamic_cast<ePub3::SeekableByteStream *>(byteStream);
		if (seekableStream != nullptr) {
			LOGD("JNI --- GetBytesRange_ SeekableByteStream\n");
			seekableStream->Seek((std::size_t)offset, std::ios::beg);
			readBytes = seekableStream->ReadBytes(reinterpret_cast<uint8_t*>(tmpBuffer), (std::size_t)dataLength);
		} else {
			env->ThrowNew(java_class_IOException, "Seek operation not supported for this byte stream.");
			return 0;
		}
	}

	LOGD("JNI --- GetBytesRange_ 2: %d\n", readBytes);

	std::size_t diff = (std::size_t)dataLength - readBytes;
	if (diff > 0) {
		LOGD("JNI --- GetBytesRange_ 3: %d\n", diff);
	}

    env->ReleaseByteArrayElements(jbarray, tmpBuffer, 0);

    return (jlong)readBytes;
}

static jbyteArray GetBytesRange(JNIEnv* env, jobject thiz, jlong nativePtr, jlong offset, jlong length) {

	LOGD("JNI --- GetBytesRange 1: %ld\n", (long)length);

	ResourceStream* stream = (ResourceStream*) nativePtr;
	ePub3::ByteStream* byteStream = stream->getPtr();
	ePub3::FilterChainByteStreamRange *rangeByteStream = dynamic_cast<ePub3::FilterChainByteStreamRange *>(byteStream);

	jbyte * tmpBuffer = new jbyte[(std::size_t)length];

	std::size_t readBytes;

	if (rangeByteStream != nullptr) {
		LOGD("JNI --- GetBytesRange FilterChainByteStreamRange\n");
		ePub3::ByteRange range;
		range.Location((std::size_t)offset);
		range.Length((std::size_t)length);
		readBytes = rangeByteStream->ReadBytes(reinterpret_cast<uint8_t*>(tmpBuffer), (std::size_t)length, range);
	} else {
		ePub3::SeekableByteStream *seekableStream = dynamic_cast<ePub3::SeekableByteStream *>(byteStream);
		if (seekableStream != nullptr) {
			LOGD("JNI --- GetBytesRange SeekableByteStream\n");
			seekableStream->Seek((std::size_t)offset, std::ios::beg);
			readBytes = seekableStream->ReadBytes(reinterpret_cast<uint8_t*>(tmpBuffer), (std::size_t)length);
		} else {
			env->ThrowNew(java_class_IOException, "Seek operation not supported for this byte stream.");
			return NULL;
		}
	}

	LOGD("JNI --- GetBytesRange 2: %d\n", readBytes);

	jbyteArray jtmpBuffer = env->NewByteArray((jsize)readBytes);
	env->SetByteArrayRegion(jtmpBuffer, 0, (jsize)readBytes, tmpBuffer);

    delete [] tmpBuffer;

	return jtmpBuffer;
}

/*
 * JNI functions
 **************************************************/

/*
 * Package: org.readium.sdk.android
 * Class: ResourceInputStream
 */

static void Skip(JNIEnv* env, jobject thiz, jlong nativePtr, jlong byteCount) {
	ResourceStream* stream = (ResourceStream*) nativePtr;
	auto byteStream = stream->getPtr();
	ePub3::SeekableByteStream *seekableStream = dynamic_cast<ePub3::SeekableByteStream*>(byteStream);

	if (seekableStream == nullptr) {
		// When the ByteStream is most likely a FilterChainByteStream:
		env->ThrowNew(java_class_IOException, "Skip operation is not supported for this byte stream. (it is most likely not a raw stream)");
		return;
	}

	seekableStream->Seek(byteCount, std::ios::cur);
}

JNIEXPORT void JNICALL Java_org_readium_sdk_android_util_ResourceInputStream_nativeSkip
		(JNIEnv* env, jobject thiz, jlong nativePtr, jlong byteCount) {

	Skip(env, thiz, nativePtr, byteCount);
}

JNIEXPORT void JNICALL Java_org_readium_sdk_android_util_ResourceInputStream_nativeReset
		(JNIEnv* env, jobject thiz, jlong nativePtr, jboolean ignoreMark) {

	ResourceStream* stream = (ResourceStream*) nativePtr;
	auto byteStream = stream->getPtr();
	ePub3::SeekableByteStream *seekableStream = dynamic_cast<ePub3::SeekableByteStream*>(byteStream);

	if (seekableStream == nullptr) {
		// When the ByteStream is most likely a FilterChainByteStream:
		env->ThrowNew(java_class_IOException, "Reset operation is not supported for this byte stream. (it is most likely not a raw stream)");
		return;
	}

	if (ignoreMark == JNI_TRUE) {
		seekableStream->Seek(0, std::ios::beg);
	} else {
		seekableStream->Seek(stream->markPosition, std::ios::beg);
	}
}

JNIEXPORT void JNICALL Java_org_readium_sdk_android_util_ResourceInputStream_nativeMark
		(JNIEnv* env, jobject thiz, jlong nativePtr) {

	ResourceStream* stream = (ResourceStream*) nativePtr;
	auto byteStream = stream->getPtr();
	ePub3::SeekableByteStream *seekableStream = dynamic_cast<ePub3::SeekableByteStream*>(byteStream);

	if (seekableStream == nullptr) {
		// When the ByteStream is most likely a FilterChainByteStream:
		env->ThrowNew(java_class_IOException, "Mark operation is not supported for this byte stream. (it is most likely not a raw stream)");
		return;
	}

	stream->markPosition = seekableStream->Position();
}

JNIEXPORT jlong JNICALL Java_org_readium_sdk_android_util_ResourceInputStream_nativeAvailable
		(JNIEnv* env, jobject thiz, jlong nativePtr) {

	ResourceStream* stream = (ResourceStream*) nativePtr;
	auto byteStream = stream->getPtr();
	return (jlong) byteStream->BytesAvailable();
}


JNIEXPORT jbyteArray JNICALL Java_org_readium_sdk_android_util_ResourceInputStream_nativeGetBytes
		(JNIEnv* env, jobject thiz, jlong nativePtr, jlong dataLength) {

	return GetBytes(env, thiz, nativePtr, dataLength);
}

JNIEXPORT jlong JNICALL Java_org_readium_sdk_android_util_ResourceInputStream_nativeGetBytesX
		(JNIEnv* env, jobject thiz, jlong nativePtr, jlong jlen, jbyteArray jbarray) {

	return GetBytesX(env, thiz, nativePtr, jlen, jbarray);
}

JNIEXPORT jbyteArray JNICALL Java_org_readium_sdk_android_util_ResourceInputStream_nativeGetAllBytes
		(JNIEnv* env, jobject thiz, jlong nativePtr) {

	return GetAllBytes(env, thiz, nativePtr);
}

JNIEXPORT jbyteArray JNICALL Java_org_readium_sdk_android_util_ResourceInputStream_nativeGetRangeBytes
		(JNIEnv* env, jobject thiz, jlong nativePtr, jlong offset, jlong length) {

	return GetBytesRange(env, thiz, nativePtr, offset, length);
}

JNIEXPORT jlong JNICALL Java_org_readium_sdk_android_util_ResourceInputStream_nativeGetRangeBytesX
		(JNIEnv* env, jobject thiz, jlong nativePtr, jlong offset, jlong length, jbyteArray jbarray) {

	return GetBytesRangeX(env, thiz, nativePtr, offset, length, jbarray);
}

JNIEXPORT void JNICALL Java_org_readium_sdk_android_util_ResourceInputStream_nativeClose
		(JNIEnv* env, jobject thiz, jlong nativePtr) {
	ResourceStream* stream = (ResourceStream*) nativePtr;
	delete stream;
}

#ifdef __cplusplus
}
#endif
