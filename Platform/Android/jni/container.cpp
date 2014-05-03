//
//  container.cpp
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


#include <ePub3/archive.h>
#include <ePub3/container.h>
#include <ePub3/zip_archive.h>
#include <string>
#include <vector>
#include <typeinfo>

#include "epub3.h"
#include "helpers.h"
#include "container.h"
#include "package.h"


using namespace std;


#ifdef __cplusplus
extern "C" {
#endif


/*
 * Internal constants
 **************************************************/

static const char *javaContainerClassName = "org/readium/sdk/android/Container";

static const char *javaContainer_createContainerMethodName = "createContainer";
static const char *javaContainer_createContainerSignature = "(JLjava/lang/String;)Lorg/readium/sdk/android/Container;";

static const char *javaContainer_addPackageToContainerMethodName = "addPackageToContainer";
static const char *javaContainer_addPackageToContainerSignature = "(Lorg/readium/sdk/android/Container;Lorg/readium/sdk/android/Package;)V";

/*
 * Internal variables
 **************************************************/

/*
 * Cached classes, methods and fields IDs.
 */

static jclass javaContainerClass = NULL;

static jmethodID createContainer_ID;
static jmethodID addPackageToContainer_ID;

/*
 * Exported functions
 **************************************************/

/**
 * Initialize the cached java elements for Container class
 */
int onLoad_cacheJavaElements_container(JNIEnv *env) {
	// Cache Container class
	INIT_CLASS_RETVAL(javaContainerClass, javaContainerClassName, ONLOAD_ERROR);
	// Cache Container class methods
	INIT_STATIC_METHOD_ID_RETVAL(createContainer_ID, javaContainerClass, javaContainerClassName,
			javaContainer_createContainerMethodName, javaContainer_createContainerSignature, ONLOAD_ERROR);
	INIT_STATIC_METHOD_ID_RETVAL(addPackageToContainer_ID, javaContainerClass, javaContainerClassName,
			javaContainer_addPackageToContainerMethodName, javaContainer_addPackageToContainerSignature, ONLOAD_ERROR);

	// Return JNI_VERSION for OK, if not one of the lines above already returned ONLOAD_ERROR
	return JNI_VERSION;
}

/**
 * Calls the java createContainer method of Container class
 */
jobject javaContainer_createContainer(JNIEnv *env, jlong nativePtr, jstring path) {
	jobject container = env->CallStaticObjectMethod(javaContainerClass,
			createContainer_ID, nativePtr, path);

	return container;
}

/**
 * Creates a Package for the packagePtr native pointer supplied and
 * calls the java addPackageToContainer method of Container class
 */
void javaContainer_addPackageToContainer(JNIEnv *env, jobject container, jlong packagePtr) {
	// Create new Package based on the packagePtr pointer
	jobject package = javaPackage_createPackage(env, packagePtr);
	// Add it to the Container
    env->CallStaticVoidMethod(javaContainerClass, addPackageToContainer_ID,
    		container, package);
}


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
Java_org_readium_sdk_android_Container_nativeLoadData(JNIEnv* env, jobject thiz, jlong pckgPtr) {
	//TODO: load the container data instead of passing the path only, as in the package
}


#ifdef __cplusplus
}
#endif
