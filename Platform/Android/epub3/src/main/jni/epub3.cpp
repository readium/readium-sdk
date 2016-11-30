//
//  epub3.cpp
//  ePub3
//
//  Created by Pedro Reis Colaco (txtr) on 2013-05-29.
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


#include <string.h>
#include <stdlib.h>
#include <limits.h>

#include <ePub3/archive.h>
#include <ePub3/container.h>
#include <ePub3/initialization.h>
#include <ePub3/utilities/error_handler.h>

#include "jni/jni.h"

#include "epub3.h"
#include "helpers.h"
#include "container.h"
#include "packagejni.h"
#include "iri.h"
#include "resource_stream.h"


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

static const char *javaEPub3_handleSdkErrorMethodName = "handleSdkError";
static const char *javaEPub3_handleSdkErrorSignature = "(Ljava/lang/String;Z)Z";


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
jmethodID createManifestItemList_ID;
jmethodID createManifestItem_ID;
jmethodID addManifestItemToList_ID;


/*
 * Internal variables
 **************************************************/

/*
 * Cached classes, methods and fields IDs.
 */

static jclass javaEPub3Class = NULL;
static jobject contentFiltersRegistrationHandler = NULL;

static jmethodID createStringList_ID;
static jmethodID addStringToList_ID;
static jmethodID createBuffer_ID;
static jmethodID appendBytesToBuffer_ID;
static jmethodID handleSdkError_ID;
static jmethodID contentFiltersRegistrationHandler_Run_ID;


/*
 * Exported functions
 **************************************************/

/**
 * Helper function to get the __nativePtr from the Java object
 * and translate it to a smart pointer on result.
 */
std::shared_ptr<void> getNativePtr(JNIEnv *env, jobject thiz) {
	// Get the native pointer id
	jlong id = jni::Field<jlong>(env, thiz, "__nativePtr");

	// Get the smart pointer
	std::shared_ptr<void> res(jni::Pointer(id).getPtr());

	// Return result
	return res;
}

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
 * Calls the java appendBytesToBuffer method of EPub3 class
 */
void javaEPub3_appendBytesToBuffer(JNIEnv *env, jobject buffer, jbyteArray data) {
	env->CallStaticVoidMethod(javaEPub3Class,
			appendBytesToBuffer_ID, buffer, data);
}

jboolean javaEPub3_handleSdkError(JNIEnv *env, jstring message, jboolean isSevereEpubError) {
	jboolean b = env->CallStaticBooleanMethod(javaEPub3Class, handleSdkError_ID, message, isSevereEpubError);
	return b;
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
	INIT_STATIC_METHOD_ID_RETVAL(handleSdkError_ID, javaEPub3Class, javaEPub3ClassName,
			javaEPub3_handleSdkErrorMethodName, javaEPub3_handleSdkErrorSignature, ONLOAD_ERROR);
	// Return JNI_VERSION for OK, if not one of the lines above already returned ONLOAD_ERROR
	return JNI_VERSION;
}

// needed only for the LauncherErrorHandler() C++ callback,
// set by initializeReadiumSDK()
static JNIEnv* m_env = nullptr;

static bool LauncherErrorHandler(const ePub3::error_details& err)
{
    const char * msg = err.message();

	bool isSevereEpubError = (err.is_spec_error()
			&& (err.severity() == ePub3::ViolationSeverity::Critical
			|| err.severity() == ePub3::ViolationSeverity::Major));

    LOGD("READIUM SDK ERROR HANDLER (%s): %s\n", isSevereEpubError ? "warning" : "info", msg);

    jstring jmessage = m_env->NewStringUTF(msg);
    jboolean b = javaEPub3_handleSdkError(m_env, jmessage, (jboolean)isSevereEpubError);
    m_env->DeleteLocalRef(jmessage);
    return (bool)b;
    
    // never throws an exception
    //return ePub3::DefaultErrorHandler(err);
}

/**
 * Initializes the Readium SDK.
 */
static void initializeReadiumSDK(JNIEnv* env)
{
    m_env = env;

	LOGD("initializeReadiumSDK(): initializing Readium SDK...");

    ePub3::ErrorHandlerFn launcherErrorHandler = LauncherErrorHandler;
    ePub3::SetErrorHandler(launcherErrorHandler);

    ePub3::InitializeSdk();
    ePub3::PopulateFilterManager();

    if (contentFiltersRegistrationHandler != NULL) {
        LOGD("initializeReadiumSDK(): calling content filters registration handler %p", contentFiltersRegistrationHandler);
        env->CallVoidMethod(contentFiltersRegistrationHandler, contentFiltersRegistrationHandler_Run_ID);
    }

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

    // Initialize the cached java elements from package
    if(onLoad_cacheJavaElements_iri(env) == ONLOAD_ERROR) {
    	LOGE("JNI_OnLoad(): failed to cache IRI java elements");
    	return ONLOAD_ERROR;
    }

    // Initialize the cached java elements from package
    if(onLoad_cacheJavaElements_ResourceInputStream(env) == ONLOAD_ERROR) {
    	LOGE("JNI_OnLoad(): failed to cache ResourceInputStream java elements");
    	return ONLOAD_ERROR;
    }

    // Initialize the rest of the cached java elements that are still in JavaObjectsFactory class
    // TODO: Move all these elements to each respective class and remove these lines

	INIT_CLASS_RETVAL(javaJavaObjectsFactoryClass, javaJavaObjectsFactoryClassName, ONLOAD_ERROR);

	INIT_STATIC_METHOD_ID_RETVAL(createSpineItemList_ID, javaJavaObjectsFactoryClass, javaJavaObjectsFactoryClassName,
			"createSpineItemList", "()Ljava/util/List;", ONLOAD_ERROR);
	INIT_STATIC_METHOD_ID_RETVAL(createSpineItem_ID, javaJavaObjectsFactoryClass, javaJavaObjectsFactoryClassName,
			"createSpineItem", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;ZLjava/lang/String;)Lorg/readium/sdk/android/SpineItem;", ONLOAD_ERROR);
	INIT_STATIC_METHOD_ID_RETVAL(addSpineItemToList_ID, javaJavaObjectsFactoryClass, javaJavaObjectsFactoryClassName,
			"addSpineItemToList", "(Ljava/util/List;Lorg/readium/sdk/android/SpineItem;)V", ONLOAD_ERROR);

	INIT_STATIC_METHOD_ID_RETVAL(createNavigationTable_ID, javaJavaObjectsFactoryClass, javaJavaObjectsFactoryClassName,
			"createNavigationTable", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Lorg/readium/sdk/android/components/navigation/NavigationTable;", ONLOAD_ERROR);
	INIT_STATIC_METHOD_ID_RETVAL(createNavigationPoint_ID, javaJavaObjectsFactoryClass, javaJavaObjectsFactoryClassName,
			"createNavigationPoint", "(Ljava/lang/String;Ljava/lang/String;)Lorg/readium/sdk/android/components/navigation/NavigationPoint;", ONLOAD_ERROR);
	INIT_STATIC_METHOD_ID_RETVAL(addElementToParent_ID, javaJavaObjectsFactoryClass, javaJavaObjectsFactoryClassName,
			"addElementToParent", "(Lorg/readium/sdk/android/components/navigation/NavigationElement;Lorg/readium/sdk/android/components/navigation/NavigationElement;)V", ONLOAD_ERROR);

	INIT_STATIC_METHOD_ID_RETVAL(createManifestItemList_ID, javaJavaObjectsFactoryClass, javaJavaObjectsFactoryClassName,
			"createManifestItemList", "()Ljava/util/List;", ONLOAD_ERROR);
	INIT_STATIC_METHOD_ID_RETVAL(createManifestItem_ID, javaJavaObjectsFactoryClass, javaJavaObjectsFactoryClassName,
			"createManifestItem", "(Ljava/lang/String;Ljava/lang/String;)Lorg/readium/sdk/android/ManifestItem;", ONLOAD_ERROR);
	INIT_STATIC_METHOD_ID_RETVAL(addManifestItemToList_ID, javaJavaObjectsFactoryClass, javaJavaObjectsFactoryClassName,
			"addManifestItemToList", "(Ljava/util/List;Lorg/readium/sdk/android/ManifestItem;)V", ONLOAD_ERROR);

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
 * Method:    setContentFiltersRegistrationHandler
 * Signature: (Ljava/lang/Runnable;)V
 */
JNIEXPORT void JNICALL
Java_org_readium_sdk_android_EPub3_setContentFiltersRegistrationHandler(JNIEnv* env, jobject thiz, jobject handler) {

    LOGD("EPub3.setContentFiltersRegistrationHandler(): received handler object %p", handler);
    if (handler != NULL) {
        /**
         * Save a global reference to the handler object and attempt to find a void-returning
         * parameterless method on it called 'run'.
         */

        jobject hg = env->NewGlobalRef(handler);
        jclass rc = env->GetObjectClass(hg);
        jmethodID m = env->GetMethodID(rc, "run", "()V");
        
        if (m == NULL) {
            LOGE("EPub3.setContentFiltersRegistrationHandler(): could not find 'run' method on handler class");
            env->DeleteGlobalRef (hg);
            return;
        }

        LOGD("EPub3.setContentFiltersRegistrationHandler(): saved object %p, method %p", hg, m);
        contentFiltersRegistrationHandler = hg;
        contentFiltersRegistrationHandler_Run_ID = m;
    }
}

/*
 * Class:     org_readium_sdk_android_EPub3
 * Method:    isEpub3Book
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_org_readium_sdk_android_EPub3_isEpub3Book(JNIEnv* env, jobject thiz, jstring path) {
	// Initialize core ePub3 SDK
	initializeReadiumSDK(env);

	std::string _path = jni::StringUTF(env, path);
	LOGD("EPub3.isEpub3Book(): path received is '%s'", _path.c_str());

    shared_ptr<ePub3::Container> _container = nullptr;
    try {
        _container = ePub3::Container::OpenContainer(_path);

        shared_ptr<ePub3::Package> _package = nullptr;
        try {
        	_package = _container->DefaultPackage();

        	if(_package != nullptr) {
        		ePub3::string versionStr;
        		int version = 0;

        		versionStr = _package->Version();
        	    if(versionStr.empty()) {
                	LOGE("EPub3.isEpub3Book(): couldn't get package version");
        	    } else {
        	        // GNU libstdc++ seems to not want to let us use these C++11 routines...
#ifndef _LIBCPP_VERSION
        	        version = (int)strtol(versionStr.c_str(), nullptr, 10);
#else
        			version = std::stoi(versionStr.stl_str());
#endif

        			if(version >= 3) {
        				LOGD("EPub3.isEpub3Book(): returning true");
        				return JNI_TRUE;
        			}
        		}

        	}
        }
        catch(const std::invalid_argument& ex) {
        	LOGE("EPub3.isEpub3Book(): failed to open package: %s\n", ex.what());
        }
    }
    catch (const std::invalid_argument& ex) {
    	LOGE("EPub3.isEpub3Book(): failed to open container: %s\n", ex.what());
    }

	LOGD("EPub3.isEpub3Book(): returning false");
	return JNI_FALSE;
}

/*
 * Class:     org_readium_sdk_android_EPub3
 * Method:    openBook
 * Signature: (Ljava/lang/String;)Lorg/readium/sdk/android/Container
 */
JNIEXPORT jobject JNICALL
Java_org_readium_sdk_android_EPub3_openBook(JNIEnv* env, jobject thiz, jstring path, jstring password) {
	// Initialize core ePub3 SDK
	initializeReadiumSDK(env);

	char *nativePath;
	GET_UTF8_RETVAL(nativePath, path, NULL);
	LOGD("EPub3.openBook(): path received is '%s'", nativePath);

	std::string spath = std::string(nativePath);

    char *nativePassword;
    GET_UTF8_RETVAL(nativePassword, password, NULL);
    LOGD("EPub3.openBook(): password received is '%s'", nativePassword);

    std::string spassword = std::string(nativePassword);
    
    shared_ptr<ePub3::Container> _container = nullptr;
    try {
        _container = ePub3::Container::OpenContainer(spath, spassword);
    }
    catch (const std::invalid_argument& ex) {
    	LOGD("OpenContainer() EXCEPTION: %s\n", ex.what());
        return nullptr;
    }
    
    LOGD("EPub3.openBook(): _container OK, version: %s\n", _container->Version().c_str());

	// Save container before sending it to Java
	jni::Pointer container(_container, POINTER_GPS("container"));

	jobject jContainer = javaContainer_createContainer(env, container.getId(), path);

    auto packages = _container->Packages();

    for (auto packageIt = packages.begin(); packageIt != packages.end(); ++packageIt) {
    	auto _package = &*(&*packageIt);
        LOGD("EPub3.openBook(): package type: %p %s\n", _package, typeid(_package).name());

    	// Save package before sending it to Java
    	jni::Pointer package(*_package, POINTER_GPS("package"));

        javaContainer_addPackageToContainer(env, jContainer, package.getId());
        LOGD("EPub3.openBook(): package added");
    }

	//TODO: Just for testing dump
	//std::string dump = jni::PointerPool::dump();
	//LOGD("openBook(): pointer pool dump: %s", dump.c_str());

    RELEASE_UTF8(path, nativePath);

	return jContainer;
}

JNIEXPORT jbyteArray JNICALL
Java_org_readium_sdk_android_EPub3_readFileAtPath(JNIEnv* env, jobject thiz, jstring bookPath, jstring password, jstring filePath) {
    initializeReadiumSDK(env);

    char *nativeBookPath;
    GET_UTF8_RETVAL(nativeBookPath, bookPath, NULL);

    std::string spath = std::string(nativeBookPath);

    LOGD("EPub3.readFileAtPath(): book path received is '%s'", nativeBookPath);

    char *nativePassword;
    GET_UTF8_RETVAL(nativePassword, password, NULL);
    LOGD("EPub3.readFileAtPath(): password received is '%s'", nativePassword);

    std::string spassword = std::string(nativePassword);

    char *nativeFilePath;
    GET_UTF8_RETVAL(nativeFilePath, filePath, NULL);

    std::string sFilepath = std::string(nativeFilePath);

    LOGD("EPub3.readFileAtPath(): book path received is '%s'", nativeFilePath);

    shared_ptr<ePub3::Container> _container = nullptr;
    try {
        _container = ePub3::Container::OpenContainer(spath, spassword);
    }
    catch (const std::invalid_argument& ex) {
        LOGD("OpenContainer() EXCEPTION: %s\n", ex.what());
    }

    jbyteArray result;

    LOGD("OpenContainer() FileExistsAtPath: %s\n", _container->FileExistsAtPath(nativeFilePath) ?"true":"false");

    if (_container->FileExistsAtPath(nativeFilePath)) {
        std::vector<char> data = _container->ExtractFileAtPath(nativeFilePath);
        result = env->NewByteArray(data.size());
        env->SetByteArrayRegion(result, 0, data.size(), (const jbyte*)data.data());
    }

    return result;
}

/*
 * Class:     org_readium_sdk_android_EPub3
 * Method:    initialize
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_org_readium_sdk_android_EPub3_initialize(JNIEnv* env, jobject thiz) {
	// Initialize core ePub3 SDK
	initializeReadiumSDK(env);
}

/*
 * Class:     org_readium_sdk_android_EPub3
 * Method:    releaseNativePointer
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_org_readium_sdk_android_EPub3_releaseNativePointer(JNIEnv* env, jobject thiz, jlong ptr) {
	jni::PointerPool::del(ptr);
}


#ifdef __cplusplus
}
#endif
