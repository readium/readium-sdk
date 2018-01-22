//
//  package.h
//  ePub3
//
//  Created by Pedro Reis Colaco (txtr) on 2013-07-02.
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


#ifndef _PACKAGE_JNI_H_
#define _PACKAGE_JNI_H_


#ifdef __cplusplus
extern "C" {
#endif


/*
 * Exported functions
 **************************************************/

/**
 * Initialize the cached java elements for Container class
 */
int onLoad_cacheJavaElements_package(JNIEnv *env);

/**
 * Calls the java createPackage method of Package class
 */
jobject javaPackage_createPackage(JNIEnv *env, jlong nativePtr);


/*
 * JNI functions
 **************************************************/

/*
 * Package: org.readium.sdk.android
 * Class: Package
 */

/*
 * Class:     org_readium_sdk_android_Package
 * Method:    nativeGetTitle
 * Signature: (J)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_Package_nativeGetTitle(JNIEnv* env, jobject thiz, jlong pckgPtr);
/*
 * Class:     org_readium_sdk_android_Package
 * Method:    nativeGetSubtitle
 * Signature: (J)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_Package_nativeGetSubtitle(JNIEnv* env, jobject thiz, jlong pckgPtr);
/*
 * Class:     org_readium_sdk_android_Package
 * Method:    nativeGetShortTitle
 * Signature: (J)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_Package_nativeGetShortTitle(JNIEnv* env, jobject thiz, jlong pckgPtr);
/*
 * Class:     org_readium_sdk_android_Package
 * Method:    nativeGetCollectionTitle
 * Signature: (J)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_Package_nativeGetCollectionTitle(JNIEnv* env, jobject thiz, jlong pckgPtr);
/*
 * Class:     org_readium_sdk_android_Package
 * Method:    nativeGetEditionTitle
 * Signature: (J)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_Package_nativeGetEditionTitle(JNIEnv* env, jobject thiz, jlong pckgPtr);
/*
 * Class:     org_readium_sdk_android_Package
 * Method:    nativeGetExpandedTitle
 * Signature: (J)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_Package_nativeGetExpandedTitle(JNIEnv* env, jobject thiz, jlong pckgPtr);
/*
 * Class:     org_readium_sdk_android_Package
 * Method:    nativeGetFullTitle
 * Signature: (J)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_Package_nativeGetFullTitle(JNIEnv* env, jobject thiz, jlong pckgPtr);
/*
 * Class:     org_readium_sdk_android_Package
 * Method:    nativeGetUniqueID
 * Signature: (J)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_Package_nativeGetUniqueID(JNIEnv* env, jobject thiz, jlong pckgPtr);
/*
 * Class:     org_readium_sdk_android_Package
 * Method:    nativeGetURLSafeUniqueID
 * Signature: (J)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_Package_nativeGetURLSafeUniqueID(JNIEnv* env, jobject thiz, jlong pckgPtr);
/*
 * Class:     org_readium_sdk_android_Package
 * Method:    nativeGetPackageID
 * Signature: (J)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_Package_nativeGetPackageID(JNIEnv* env, jobject thiz, jlong pckgPtr);
/*
 * Class:     org_readium_sdk_android_Package
 * Method:    nativeGetType
 * Signature: (J)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_Package_nativeGetType(JNIEnv* env, jobject thiz, jlong pckgPtr);
/*
 * Class:     org_readium_sdk_android_Package
 * Method:    nativeGetVersion
 * Signature: (J)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_Package_nativeGetVersion(JNIEnv* env, jobject thiz, jlong pckgPtr);
/*
 * Class:     org_readium_sdk_android_Package
 * Method:    nativeGetISBN
 * Signature: (J)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_Package_nativeGetISBN(JNIEnv* env, jobject thiz, jlong pckgPtr);
/*
 * Class:     org_readium_sdk_android_Package
 * Method:    nativeGetLanguage
 * Signature: (J)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_Package_nativeGetLanguage(JNIEnv* env, jobject thiz, jlong pckgPtr);
/*
 * Class:     org_readium_sdk_android_Package
 * Method:    nativeGetCopyrightOwner
 * Signature: (J)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_Package_nativeGetCopyrightOwner(JNIEnv* env, jobject thiz, jlong pckgPtr);
/*
 * Class:     org_readium_sdk_android_Package
 * Method:    nativeGetSource
 * Signature: (J)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_Package_nativeGetSource(JNIEnv* env, jobject thiz, jlong pckgPtr);
/*
 * Class:     org_readium_sdk_android_Package
 * Method:    nativeGetAuthors
 * Signature: (J)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_Package_nativeGetAuthors(JNIEnv* env, jobject thiz, jlong pckgPtr);
/*
 * Class:     org_readium_sdk_android_Package
 * Method:    nativeGetModificationDate
 * Signature: (J)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_Package_nativeGetModificationDate(JNIEnv* env, jobject thiz, jlong pckgPtr);
/*
 * Class:     org_readium_sdk_android_Package
 * Method:    nativeGetSubjects
 * Signature: (J)Ljava/util/List;
 */
JNIEXPORT jobject JNICALL Java_org_readium_sdk_android_Package_nativeGetSubjects(JNIEnv* env, jobject thiz, jlong pckgPtr);
/*
 * Class:     org_readium_sdk_android_Package
 * Method:    nativeGetCoverManifestItem
 * Signature: (J)Ljava/util/ManifestItem;
 */
JNIEXPORT jobject JNICALL Java_org_readium_sdk_android_Package_nativeGetCoverManifestItem(JNIEnv* env, jobject thiz, jlong pckgPtr);
/*
 * Class:     org_readium_sdk_android_Package
 * Method:    nativeGetManifestTable
 * Signature: (J)Ljava/util/List;
 */
JNIEXPORT jobject JNICALL Java_org_readium_sdk_android_Package_nativeGetManifestTable
		(JNIEnv* env, jobject thiz, jlong pckgPtr);
/*
 * Class:     org_readium_sdk_android_Package
 * Method:    nativeGetManifestTable
 * Signature: (JJLjava/lang/String;)I;
 */
JNIEXPORT jint JNICALL Java_org_readium_sdk_android_Package_nativeGetArchiveInfoSize
		(JNIEnv* env, jobject thiz, jlong pckgPtr, jlong contnrPtr, jstring jrelativePath);
/*
 * Class:     org_readium_sdk_android_Package
 * Method:    nativeInputStreamForRelativePath
 * Signature: (JJLjava/lang/String;IZ)Ljava/io/InputStream;
 */
JNIEXPORT jobject JNICALL Java_org_readium_sdk_android_Package_nativeInputStreamForRelativePath
		(JNIEnv* env, jobject thiz, jlong pckgPtr, jlong contnrPtr, jstring jrelativePath, jint bufferSize, jboolean isRange);
/*
 * Class:     org_readium_sdk_android_Package
 * Method:    nativeRawInputStreamForRelativePath
 * Signature: (JJLjava/lang/String;I)Ljava/io/InputStream;
 */
JNIEXPORT jobject JNICALL Java_org_readium_sdk_android_Package_nativeRawInputStreamForRelativePath
		(JNIEnv* env, jobject thiz, jlong pckgPtr, jlong contnrPtr, jstring jrelativePath, jint bufferSize);
/*
 * Class:     org_readium_sdk_android_Package
 * Method:    nativeGetSmilDataAsJson
 * Signature: (J)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_Package_nativeGetSmilDataAsJson
		(JNIEnv* env, jobject thiz, jlong pckgPtr);
#ifdef __cplusplus
}
#endif


#endif //ifndef _PACKAGE_JNI_H_
