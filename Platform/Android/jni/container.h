//
//  container.h
//  ePub3
//
//  Created by Pedro Reis Colaco (txtr) on 2013-07-02.
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


#include <jni.h>


#ifndef _CONTAINER_JNI_H_
#define _CONTAINER_JNI_H_


#ifdef __cplusplus
extern "C" {
#endif


/*
 * Exported functions
 **************************************************/

/**
 * Initialize the cached java elements for Container class
 */
int onLoad_cacheJavaElements_container(JNIEnv *env);

/**
 * Calls the java createContainer method of Container class
 */
jobject javaContainer_createContainer(JNIEnv *env, jlong nativePtr, jstring path);

/**
 * Creates a Package for the packagePtr native pointer supplied and
 * calls the java addPackageToContainer method of Container class
 */
void javaContainer_addPackageToContainer(JNIEnv *env, jobject container, jlong packagePtr);

/*
 * JNI functions
 **************************************************/

/*
 * Package: org.readium.sdk.android
 * Class: Container
 */

/*
 * Class:     org_readium_sdk_android_Container
 * Method:    nativeLoadData
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT void JNICALL
Java_org_readium_sdk_android_Container_nativeLoadData(JNIEnv* env, jobject thiz, jlong pckgPtr);


#ifdef __cplusplus
}
#endif


#endif //ifndef _CONTAINER_JNI_H_
