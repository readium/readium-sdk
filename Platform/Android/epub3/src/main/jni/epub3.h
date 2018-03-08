//
//  epub3.h
//  ePub3
//
//  Created by Pedro Reis Colaco (txtr) on 2013-05-29.
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


#include <memory>

#include <jni.h>


#ifndef _EPUB3_JNI_H_
#define _EPUB3_JNI_H_




#ifdef __cplusplus
extern "C" {
#endif

/*
 * Constants
 **************************************************/

/**
 * OnLoad error value
 */
#define ONLOAD_ERROR	-1

/**
 * JNI version to use
 */
#define JNI_VERSION		JNI_VERSION_1_6

/*
 * Exported variables
 **************************************************/

#if ENABLE_ZIP_ARCHIVE_WRITER

/**
 * Global variable to share the Android Cache directory with the
 * Core SDK. It is used in /ePub3/ePub/zip_archive.cpp to store
 * temporary files.
 */
extern char gAndroidCacheDir[];

#endif //ENABLE_ZIP_ARCHIVE_WRITER

//TODO: Remove when all these when passed to respective classes
extern jclass javaJavaObjectsFactoryClass;
extern jmethodID createSpineItemList_ID;
extern jmethodID createSpineItem_ID;
extern jmethodID addSpineItemToList_ID;
extern jmethodID createNavigationTable_ID;
extern jmethodID createNavigationPoint_ID;
extern jmethodID addElementToParent_ID;
extern jmethodID createManifestItemList_ID;
extern jmethodID createManifestItem_ID;
extern jmethodID addManifestItemToList_ID;


/*
 * Exported functions
 **************************************************/

/**
 * Helper function to get the __nativePtr from the Java object
 * and translate it to a smart pointer on result.
 */
std::shared_ptr<void> getNativePtr(JNIEnv *env, jobject thiz);

/**
 * Helper function to create a jstring from a native string.
 * Optionally it may free the native string if the freeNative argument
 * is true.
 */
jstring toJstring(JNIEnv *env, const char* str, bool freeNative = false);

/**
 * Calls the java createStringList method of EPub3 class
 */
jobject javaEPub3_createStringList(JNIEnv *env);

/**
 * Calls the java addStringToList method of EPub3 class
 */
void javaEPub3_addStringToList(JNIEnv *env, jobject list, jstring str);

/**
 * Calls the java createBuffer method of EPub3 class
 */
jobject javaEPub3_createBuffer(JNIEnv *env, jint bufferSize);

/**
 * Calls the java createBuffer method of EPub3 class
 */
void javaEPub3_appendBytesToBuffer(JNIEnv *env, jobject buffer, jbyteArray data);

jboolean javaEPub3_handleSdkError(JNIEnv *env, jstring message, jboolean isSevereEpubError);

/*
 * JNI functions
 **************************************************/

/**
 * JNI OnLoad function, called when the library is loaded.
 */
JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved);

/**
 * JNI OnUnload function, called when the library is unloaded.
 */
JNIEXPORT void JNI_OnUnload(JavaVM* vm, void* reserved);

/*
 * Package: org.readium.sdk.android
 * Class: EPub3
 */

#if ENABLE_ZIP_ARCHIVE_WRITER

/*
 * Class:     org_readium_sdk_android_EPub3
 * Method:    setCachePath
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_org_readium_sdk_android_EPub3_setCachePath(JNIEnv* env, jobject thiz, jstring cachePath);

#endif //ENABLE_ZIP_ARCHIVE_WRITER

/*
 * Class:     org_readium_sdk_android_EPub3
 * Method:    setContentFiltersRegistrationHandler
 * Signature: (Ljava/lang/Runnable;)V
 */
JNIEXPORT void JNICALL Java_org_readium_sdk_android_EPub3_setContentFiltersRegistrationHandler(JNIEnv* env, jobject thiz, jobject handler);

/*
 * Class:     org_readium_sdk_android_EPub3
 * Method:    isEpub3Book
 * Signature: (Ljava/lang/String;)Z
 */
//JNIEXPORT jboolean JNICALL Java_org_readium_sdk_android_EPub3_isEpub3Book(JNIEnv* env, jobject thiz, jstring Path);

/*
 * Class:     org_readium_sdk_android_EPub3
 * Method:    openBook
 * Signature: (Ljava/lang/String;)Lorg/readium/sdk/android/Container
 */
JNIEXPORT jobject JNICALL Java_org_readium_sdk_android_EPub3_openBook(JNIEnv* env, jobject thiz, jstring path);

JNIEXPORT jobject JNICALL Java_org_readium_sdk_android_EPub3_openBookPlain(JNIEnv* env, jobject thiz, jstring path);

/*
 * Class:     org_readium_sdk_android_EPub3
 * Method:    initialize
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_org_readium_sdk_android_EPub3_initialize(JNIEnv* env, jobject thiz);

/*
 * Class:     org_readium_sdk_android_EPub3
 * Method:    releaseNativePointer
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_org_readium_sdk_android_EPub3_releaseNativePointer(JNIEnv* env, jobject thiz, jlong ptr);

#ifdef __cplusplus
}
#endif

#endif //ifndef _EPUB3_JNI_H_
