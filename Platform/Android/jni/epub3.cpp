//
//  epub3.cpp
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


#include <string.h>
#include <stdlib.h>
#include <limits.h>

#include <ePub3/archive.h>
#include <ePub3/container.h>

#include "epub3.h"
#include "log.h"
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

//TODO: Remove when JavaObjectsFactory methods passed to respective classes
static const char *javaJavaObjectsFactoryClassName = "org/readium/sdk/android/JavaObjectsFactory";

static const char *javaEPub3ClassName = "org/readium/sdk/android/EPub3";

static const char *javaEPub3_createStringListMethodName = "createStringList";
static const char *javaEPub3_createStringListSignature = "()Ljava/util/List;";

static const char *javaEPub3_addStringToListMethodName = "addStringToList";
static const char *javaEPub3_addStringToListSignature = "(Ljava/util/List;Ljava/lang/String;)V";

static const char *javaEPub3_createBufferMethodName = "createBuffer";
static const char *javaEPub3_createBufferSignature = "(I)Ljava/nio/ByteBuffer;";

static const char *javaEPub3_appendBytesToBufferMethodName = "appendBytesToBuffer";
static const char *javaEPub3_appendBytesToBufferSignature = "(Ljava/nio/ByteBuffer;[B)V";


/*
 * Exported variables
 **************************************************/

/**
 * Global variable to share the Android Cache directory with the
 * Core SDK. It is used in /ePub3/ePub/zip_archive.cpp to store
 * temporary files.
 */
char gAndroidCacheDir[PATH_MAX] = {0};

//TODO: Remove when all these when passed to respective classes
jclass javaJavaObjectsFactoryClass = NULL;
jmethodID createSpineItemList_ID;
jmethodID createSpineItem_ID;
jmethodID addSpineItemToList_ID;
jmethodID createNavigationTable_ID;
jmethodID createNavigationPoint_ID;
jmethodID addElementToParent_ID;


/*
 * Internal variables
 **************************************************/

/*
 * Cached classes, methods and fields IDs.
 */

static jclass javaEPub3Class = NULL;

static jmethodID createStringList_ID;
static jmethodID addStringToList_ID;
static jmethodID createBuffer_ID;
static jmethodID appendBytesToBuffer_ID;

//TODO: Why is this needed? Just to make the refcounting count a copy?
static shared_ptr<ePub3::Package> currentPckgPtr;
static shared_ptr<ePub3::Container> currentContainer;


/*
 * Exported functions
 **************************************************/

/**
 * Helper function to create a jstring from a native string.
 * Optionally it may free the native string if the freeNative argument
 * is true.
 */
jstring toJstring(JNIEnv *env, const char* str, bool freeNative = false) {
	if (str == NULL) {
		return NULL;
	}

    jstring jstr = env->NewStringUTF(str);
    if (freeNative) {
    	free((void *) str);
    }

	return jstr;
}

/**
 * Calls the java createStringList method of EPub3 class
 */
jobject javaEPub3_createStringList(JNIEnv *env) {
	jobject list = env->CallStaticObjectMethod(javaEPub3Class,
			createStringList_ID);

	return list;
}

/**
 * Calls the java addStringToList method of EPub3 class
 */
void javaEPub3_addStringToList(JNIEnv *env, jobject list, jstring str) {
	env->CallStaticVoidMethod(javaEPub3Class,
			addStringToList_ID, list, str);
}

/**
 * Calls the java createBuffer method of EPub3 class
 */
jobject javaEPub3_createBuffer(JNIEnv *env, jint bufferSize) {
	jobject buf = env->CallStaticObjectMethod(javaEPub3Class,
			createBuffer_ID, bufferSize);

	return buf;
}

/**
 * Calls the java createBuffer method of EPub3 class
 */
void javaEPub3_appendBytesToBuffer(JNIEnv *env, jobject buffer, jbyteArray data) {
	env->CallStaticVoidMethod(javaEPub3Class,
			appendBytesToBuffer_ID, buffer, data);
}


/*
 * Internal functions
 **************************************************/

/**
 * Initialize the cached java elements for Container class
 */
static int onLoad_cacheJavaElements_epub3(JNIEnv *env) {
	// Cache EPub3 class
	INIT_CLASS_RETVAL(javaEPub3Class, javaEPub3ClassName, ONLOAD_ERROR);
	// Cache EPub3 class methods
	INIT_STATIC_METHOD_ID_RETVAL(createStringList_ID, javaEPub3Class, javaEPub3ClassName,
			javaEPub3_createStringListMethodName, javaEPub3_createStringListSignature, ONLOAD_ERROR);
	INIT_STATIC_METHOD_ID_RETVAL(addStringToList_ID, javaEPub3Class, javaEPub3ClassName,
			javaEPub3_addStringToListMethodName, javaEPub3_addStringToListSignature, ONLOAD_ERROR);
	INIT_STATIC_METHOD_ID_RETVAL(createBuffer_ID, javaEPub3Class, javaEPub3ClassName,
			javaEPub3_createBufferMethodName, javaEPub3_createBufferSignature, ONLOAD_ERROR);
	INIT_STATIC_METHOD_ID_RETVAL(appendBytesToBuffer_ID, javaEPub3Class, javaEPub3ClassName,
			javaEPub3_appendBytesToBufferMethodName, javaEPub3_appendBytesToBufferSignature, ONLOAD_ERROR);

	// Return JNI_VERSION for OK, if not one of the lines above already returned ONLOAD_ERROR
	return JNI_VERSION;
}

/**
 * Initializes the Readium SDK.
 */
static void initializeReadiumSDK()
{
	LOGD("initializeReadiumSDK(): initializing Readium SDK...");
    ePub3::Archive::Initialize();
	LOGD("initializeReadiumSDK(): initialization of Readium SDK finished");
}


/*
 * JNI functions
 **************************************************/

/**
 * JNI OnLoad function, called when the library is loaded.
 */
JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
	// Get the JNI Environment to be able to initialize the cached the java elements
    JNIEnv* env;
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION) != JNI_OK) {
    	// This should not happen...
    	LOGE("JNI_OnLoad(): failed to get environment. VM doesn't support JNI version 1.6");
        return ONLOAD_ERROR;
    }

    // Initialize the cached java elements from epub3
    if(onLoad_cacheJavaElements_epub3(env) == ONLOAD_ERROR) {
    	LOGE("JNI_OnLoad(): failed to cache epub3 java elements");
    	return ONLOAD_ERROR;
    }

    // Initialize the cached java elements from container
    if(onLoad_cacheJavaElements_container(env) == ONLOAD_ERROR) {
    	LOGE("JNI_OnLoad(): failed to cache container java elements");
    	return ONLOAD_ERROR;
    }

    // Initialize the cached java elements from package
    if(onLoad_cacheJavaElements_package(env) == ONLOAD_ERROR) {
    	LOGE("JNI_OnLoad(): failed to cache package java elements");
    	return ONLOAD_ERROR;
    }

    // Initialize the rest of the cached java elements that are still in JavaObjectsFactory class
    // TODO: Move all these elements to each respective class and remove these lines

	INIT_CLASS_RETVAL(javaJavaObjectsFactoryClass, javaJavaObjectsFactoryClassName, ONLOAD_ERROR);

	INIT_STATIC_METHOD_ID_RETVAL(createSpineItemList_ID, javaJavaObjectsFactoryClass, javaJavaObjectsFactoryClassName,
			"createSpineItemList", "()Ljava/util/List;", ONLOAD_ERROR);
	INIT_STATIC_METHOD_ID_RETVAL(createSpineItem_ID, javaJavaObjectsFactoryClass, javaJavaObjectsFactoryClassName,
			"createSpineItem", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Lorg/readium/sdk/android/SpineItem;", ONLOAD_ERROR);
	INIT_STATIC_METHOD_ID_RETVAL(addSpineItemToList_ID, javaJavaObjectsFactoryClass, javaJavaObjectsFactoryClassName,
			"addSpineItemToList", "(Ljava/util/List;Lorg/readium/sdk/android/SpineItem;)V", ONLOAD_ERROR);

	INIT_STATIC_METHOD_ID_RETVAL(createNavigationTable_ID, javaJavaObjectsFactoryClass, javaJavaObjectsFactoryClassName,
			"createNavigationTable", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Lorg/readium/sdk/android/components/navigation/NavigationTable;", ONLOAD_ERROR);
	INIT_STATIC_METHOD_ID_RETVAL(createNavigationPoint_ID, javaJavaObjectsFactoryClass, javaJavaObjectsFactoryClassName,
			"createNavigationPoint", "(Ljava/lang/String;Ljava/lang/String;)Lorg/readium/sdk/android/components/navigation/NavigationPoint;", ONLOAD_ERROR);
	INIT_STATIC_METHOD_ID_RETVAL(addElementToParent_ID, javaJavaObjectsFactoryClass, javaJavaObjectsFactoryClassName,
			"addElementToParent", "(Lorg/readium/sdk/android/components/navigation/NavigationElement;Lorg/readium/sdk/android/components/navigation/NavigationElement;)V", ONLOAD_ERROR);

	// Return the JNI version this library wants to use
    return JNI_VERSION;
}

/**
 * JNI OnUnload function, called when the library is unloaded.
 */
JNIEXPORT void JNI_OnUnload(JavaVM* vm, void* reserved) {
	//TODO: Fill when needed
}


/*
 * Class:     org_readium_sdk_android_EPub3
 * Method:    setCachePath
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL
Java_org_readium_sdk_android_EPub3_setCachePath(JNIEnv* env, jobject thiz, jstring cachePath) {
	const char* str;
	// Get a c string from java string
	GET_UTF8_RET(str, cachePath);
	// Coppy the string to the global variable
	strlcpy(gAndroidCacheDir, str, PATH_MAX);
	// Release the c string
	RELEASE_UTF8(cachePath, str);
}

/*
 * Class:     org_readium_sdk_android_EPub3
 * Method:    openBook
 * Signature: (Ljava/lang/String;)Lorg/readium/sdk/android/Container
 */
JNIEXPORT jobject JNICALL
Java_org_readium_sdk_android_EPub3_openBook(JNIEnv* env, jobject thiz, jstring path) {
	// Initialize core ePub3 SDK
	initializeReadiumSDK();

	char *nativePath;
	GET_UTF8_RETVAL(nativePath, path, NULL);
	LOGD("EPub3.openBook(): path received is '%s'", nativePath);

	std::string spath = std::string(nativePath);
	shared_ptr<ePub3::Container> _container = ePub3::Container::OpenContainer(spath);
	LOGD("EPub3.openBook(): _container OK, version: %s\n", _container->Version().c_str());
	currentContainer = _container;	//TODO: Why is this?

	jobject jContainer = javaContainer_createContainer(env, (jlong) &currentContainer, path);

    auto packages = _container->Packages();

    for (auto packageIt = packages.begin(); packageIt != packages.end(); ++packageIt) {
    	auto package = &*(&*packageIt);
        LOGD("EPub3.openBook(): package type: %p %s\n", package, typeid(package).name());
        currentPckgPtr = *package;	//TODO: Why is this?

        javaContainer_addPackageToContainer(env, jContainer, (jint) &currentPckgPtr);
        LOGD("EPub3.openBook(): package added");
    }

    RELEASE_UTF8(path, nativePath);

	return jContainer;
}


#ifdef __cplusplus
}
#endif
