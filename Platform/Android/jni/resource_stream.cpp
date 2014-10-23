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


//#include <ePub3/utilities/resource_stream.h>
#include <string>
#include <memory>
#include <algorithm>    // std::min
#include <typeinfo>
#include <ePub3/utilities/byte_stream.h>

#include "jni/jni.h"

#include "epub3.h"
#include "helpers.h"
#include <resource_stream.h>

ePub3::ByteStream* ResourceStream::getPtr() {
	ePub3::ByteStream* reader = _ptr.get();
	return reader;
}
ResourceStream::~ResourceStream() {
	ePub3::ByteStream* reader = _ptr.get();
	reader->Close();
	_ptr.release();
}

#ifdef __cplusplus
extern "C" {
#endif


/*
 * Internal constants
 **************************************************/

static const int BUFFER_SIZE = 128*1024;
static const char *java_class_ResourceInputStream_name = "org/readium/sdk/android/util/ResourceInputStream";

static const char *java_method_ResourceInputStream_createResourceInputStream_name = "createResourceInputStream";
static const char *java_method_ResourceInputStream_createResourceInputStream_sign = "(JJ)Lorg/readium/sdk/android/util/ResourceInputStream;";

/*
 * Internal variables
 **************************************************/

/*
 * Cached classes, methods and fields IDs.
 */

static jni::Class java_class_ResourceInputStream;

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

	// Cache IRI class methods
	java_method_ResourceInputStream_createResourceInputStream = jni::StaticMethod<jobject>(env, java_class_ResourceInputStream, java_method_ResourceInputStream_createResourceInputStream_name, java_method_ResourceInputStream_createResourceInputStream_sign);

	// Return JNI_VERSION for OK, if not one of the lines above already returned ONLOAD_ERROR
	return JNI_VERSION;
}

/**
 * Calls the java createResourceInputStream method of ResourceInputStream class
 */
jobject javaResourceInputStream_createResourceInputStream(JNIEnv *env, long streamPtr, int length) {

	return java_method_ResourceInputStream_createResourceInputStream(env, (jlong) streamPtr, (jlong) length);
}


/*
 * JNI functions
 **************************************************/

/*
 * Package: org.readium.sdk.android
 * Class: ResourceInputStream
 */

JNIEXPORT jobject JNICALL Java_org_readium_sdk_android_util_ResourceInputStream_nativeSkip
		(JNIEnv* env, jobject thiz, jlong nativePtr, jint byteCount) {

	ResourceStream* stream = (ResourceStream*) nativePtr;
    auto byteStream = stream->getPtr();
    ePub3::SeekableByteStream *seekableStream = dynamic_cast<ePub3::SeekableByteStream*>(byteStream);

    seekableStream->Seek(byteCount, std::ios::beg);
}

JNIEXPORT jobject JNICALL Java_org_readium_sdk_android_util_ResourceInputStream_nativeGetBytes
		(JNIEnv* env, jobject thiz, jlong nativePtr, jint dataLength) {

	ResourceStream* stream = (ResourceStream*) nativePtr;
    char tmpBuffer[BUFFER_SIZE];
    auto byteStream = stream->getPtr();

    jobject jbuffer = javaEPub3_createBuffer(env, dataLength);

	std::size_t readBytes = byteStream->ReadBytes(&tmpBuffer, std::min(dataLength, BUFFER_SIZE));
    int readData = readBytes;

    while (readData <= dataLength && readBytes > 0) {
    	jsize length = (jsize) readBytes;
    	jbyteArray jtmpBuffer = env->NewByteArray(readBytes);

        env->SetByteArrayRegion(jtmpBuffer, 0, length, tmpBuffer);
    	javaEPub3_appendBytesToBuffer(env, jbuffer, jtmpBuffer);

		env->DeleteLocalRef(jtmpBuffer);

    	if (readData <= dataLength) {
    		readBytes = byteStream->ReadBytes(&tmpBuffer, std::min(dataLength - readData, BUFFER_SIZE));
			readData += readBytes;
    	}
    }
	return jbuffer;
}

JNIEXPORT  JNICALL void Java_org_readium_sdk_android_util_ResourceInputStream_nativeReleasePtr
		(JNIEnv* env, jobject thiz, jlong nativePtr) {
	ResourceStream* stream = (ResourceStream*) nativePtr;
	delete stream;
}



#ifdef __cplusplus
}
#endif
