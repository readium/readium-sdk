//
//  container.cpp
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


#include <ePub3/archive.h>
#include <ePub3/container.h>
#include <ePub3/zip_archive.h>
#include <string>
#include <vector>
#include <typeinfo>

#include "epub3.h"
#include "helpers.h"
#include "container.h"
#include "packagejni.h"
#include "resource_stream.h"


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

JNIEXPORT jobject JNICALL Java_org_readium_sdk_android_Container_nativeGetInputStream
		(JNIEnv* env, jobject obj, jlong containerPtr, jstring jRelativePath) {
	const char *relativePath = env->GetStringUTFChars(jRelativePath, 0);
	ePub3::Container * container = (ePub3::Container *) containerPtr;
	bool containsPath = container->FileExistsAtPath(relativePath);

    if (!containsPath) {
		return NULL;
	}

	unique_ptr<ePub3::ByteStream> byteStream = container->ReadStreamAtPath(relativePath);
	env->ReleaseStringUTFChars(jRelativePath, relativePath);
	ResourceStream *stream = new ResourceStream(byteStream, 0);
	return javaResourceInputStream_createResourceInputStream(env, (long) stream);
}


#ifdef __cplusplus
}
#endif
