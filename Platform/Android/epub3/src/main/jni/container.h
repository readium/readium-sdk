//
//  container.h
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

JNIEXPORT jobject JNICALL Java_org_readium_sdk_android_Container_nativeGetInputStream
        (JNIEnv* env, jobject obj, jlong containerPtr, jstring jRelativePath);

#ifdef __cplusplus
}
#endif


#endif //ifndef _CONTAINER_JNI_H_
