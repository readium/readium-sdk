//
//  epub3.h
//  ePub3
//
//  Created by Pedro Reis Colaco (txtr) on 2013-05-29.
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

/**
 * Global variable to share the Android Cache directory with the
 * Core SDK. It is used in /ePub3/ePub/zip_archive.cpp to store
 * temporary files.
 */
extern char gAndroidCacheDir[];

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

/*
 * Class:     org_readium_sdk_android_EPub3
 * Method:    setCachePath
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_org_readium_sdk_android_EPub3_setCachePath(JNIEnv* env, jobject thiz, jstring cachePath);

/*
 * Class:     org_readium_sdk_android_EPub3
 * Method:    isEpub3Book
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_org_readium_sdk_android_EPub3_isEpub3Book(JNIEnv* env, jobject thiz, jstring Path);

/*
 * Class:     org_readium_sdk_android_EPub3
 * Method:    openBook
 * Signature: (Ljava/lang/String;)Lorg/readium/sdk/android/Container
 */
JNIEXPORT jobject JNICALL Java_org_readium_sdk_android_EPub3_openBook(JNIEnv* env, jobject thiz, jstring path);

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
