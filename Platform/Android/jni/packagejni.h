//
//  package.h
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
 * Signature: (JJLjava/lang/String;)Ljava/io/InputStream;
 */
JNIEXPORT jobject JNICALL Java_org_readium_sdk_android_Package_nativeInputStreamForRelativePath
		(JNIEnv* env, jobject thiz, jlong pckgPtr, jlong contnrPtr, jstring jrelativePath);
/*
 * Class:     org_readium_sdk_android_Package
 * Method:    nativeByteRangeStreamForRelativePath
 * Signature: (JJLjava/lang/String;JJ)Ljava.nio.ByteBuffer;
 */
JNIEXPORT jobject JNICALL Java_org_readium_sdk_android_Package_nativeByteRangeStreamForRelativePath
        (JNIEnv *env, jobject thiz, jlong pckgPtr, jlong contnrPtr, jstring relativePath, jlong offset, jlong length);
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
