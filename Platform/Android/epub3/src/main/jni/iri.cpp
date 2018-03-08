//
//  iri.cpp
//  ePub3
//
//  Created by Pedro Reis Colaco (txtr) on 2013-08-19.
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


#include <ePub3/utilities/iri.h>
#include <string>
#include <memory>

#include "jni/jni.h"

#include "epub3.h"
#include "helpers.h"

#ifdef __cplusplus
extern "C" {
#endif


/*
 * Internal constants
 **************************************************/

static const char *java_class_IRI_name = "org/readium/sdk/android/IRI";

static const char *java_method_IRI_createIRIempty_name = "createIRIempty";
static const char *java_method_IRI_createIRIempty_sign = "()Lorg/readium/sdk/android/IRI;";
static const char *java_method_IRI_createIRIstring_name = "createIRIstring";
static const char *java_method_IRI_createIRIstring_sign = "(Ljava/lang/String;)Lorg/readium/sdk/android/IRI;";
static const char *java_method_IRI_createIRIurn_name = "createIRIurn";
static const char *java_method_IRI_createIRIurn_sign = "(Ljava/lang/String;Ljava/lang/String;)Lorg/readium/sdk/android/IRI;";
static const char *java_method_IRI_createIRIurl_name = "createIRIurl";
static const char *java_method_IRI_createIRIurl_sign = "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Lorg/readium/sdk/android/IRI;";

/*
 * Internal variables
 **************************************************/

/*
 * Cached classes, methods and fields IDs.
 */

static jni::Class java_class_IRI;

static jni::StaticMethod<jobject> java_method_IRI_createIRIempty;
static jni::StaticMethod<jobject> java_method_IRI_createIRIstring;
static jni::StaticMethod<jobject> java_method_IRI_createIRIurn;
static jni::StaticMethod<jobject> java_method_IRI_createIRIurl;

/*
 * Exported functions
 **************************************************/

/**
 * Initialize the cached java elements for IRI class
 */
int onLoad_cacheJavaElements_iri(JNIEnv *env) {
	// Cache IRI class
	jclass java_class_IRI_ = NULL;
	INIT_CLASS_RETVAL(java_class_IRI_, java_class_IRI_name, ONLOAD_ERROR);
	java_class_IRI = jni::Class(env, java_class_IRI_);

	// Cache IRI class methods
	java_method_IRI_createIRIempty = jni::StaticMethod<jobject>(env, java_class_IRI,
			java_method_IRI_createIRIempty_name, java_method_IRI_createIRIempty_sign);
	java_method_IRI_createIRIstring = jni::StaticMethod<jobject>(env, java_class_IRI,
			java_method_IRI_createIRIstring_name, java_method_IRI_createIRIstring_sign);
	java_method_IRI_createIRIurn = jni::StaticMethod<jobject>(env, java_class_IRI,
			java_method_IRI_createIRIurn_name, java_method_IRI_createIRIurn_sign);
	java_method_IRI_createIRIurl = jni::StaticMethod<jobject>(env, java_class_IRI,
			java_method_IRI_createIRIurl_name, java_method_IRI_createIRIurl_sign);

	// Return JNI_VERSION for OK, if not one of the lines above already returned ONLOAD_ERROR
	return JNI_VERSION;
}

/**
 * Calls the java createIRI_empty method of IRI class
 */
jobject javaIRI_createIRIempty(JNIEnv *env) {
	jobject iri = java_method_IRI_createIRIempty(env);

	return iri;
}

/**
 * Calls the java createIRI_string method of IRI class
 */
jobject javaIRI_createIRIstring(JNIEnv *env, jstring iriStr) {
	jobject iri = java_method_IRI_createIRIstring(env, iriStr);

	return iri;
}

/**
 * Calls the java createIRI_urn method of IRI class
 */
jobject javaIRI_createIRIurn(JNIEnv *env, jstring nameID, jstring namespacedString) {
	jobject iri = java_method_IRI_createIRIurn(env, nameID, namespacedString);

	return iri;
}

/**
 * Calls the java createIRI_url method of IRI class
 */
jobject javaIRI_createIRIurl(JNIEnv *env, jstring scheme, jstring host, jstring path, jstring query, jstring fragment) {
	jobject iri = java_method_IRI_createIRIurl(env, scheme, host, path, query, fragment);

	return iri;
}


/*
 * JNI functions
 **************************************************/

/*
 * Package: org.readium.sdk.android
 * Class: IRI
 */

/*
 * Class:     org_readium_sdk_android_IRI
 * Method:    createNativeIRIempty
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_org_readium_sdk_android_IRI_createNativeIRIempty
		(JNIEnv *env, jobject thiz) {
	// Create the IRI
	std::shared_ptr<ePub3::IRI> iri(new ePub3::IRI());

	// Store in pointer pool before sending to Java
	jni::Pointer ptr(iri, POINTER_GPS("IRI"));

	// Return pointer
	return ptr.getId();
}

/*
 * Class:     org_readium_sdk_android_IRI
 * Method:    createNativeIRIstring
 * Signature: (Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_org_readium_sdk_android_IRI_createNativeIRIstring
		(JNIEnv *env, jobject thiz, jstring iriStr) {
	// Get the argument strings
	std::string iriStr_str = jni::StringUTF(env, iriStr);

	// Create the IRI
	std::shared_ptr<ePub3::IRI> iri(new ePub3::IRI(iriStr_str));

	// Store in pointer pool before sending to Java
	jni::Pointer ptr(iri, POINTER_GPS("IRI"));

	// Return pointer
	return ptr.getId();
}

/*
 * Class:     org_readium_sdk_android_IRI
 * Method:    createNativeIRIurn
 * Signature: (Ljava/lang/String;Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_org_readium_sdk_android_IRI_createNativeIRIurn
		(JNIEnv *env, jobject thiz, jstring nameID, jstring namespacedString) {
	// Get the argument strings
	std::string nameID_str = jni::StringUTF(env, nameID);
	std::string namespacedString_str = jni::StringUTF(env, namespacedString);

	// Create the IRI
	std::shared_ptr<ePub3::IRI> iri(new ePub3::IRI(nameID_str, namespacedString_str));

	// Store in pointer pool before sending to Java
	jni::Pointer ptr(iri, POINTER_GPS("IRI"));

	// Return pointer
	return ptr.getId();
}

/*
 * Class:     org_readium_sdk_android_IRI
 * Method:    createNativeIRIurl
 * Signature: (Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_org_readium_sdk_android_IRI_createNativeIRIurl
		(JNIEnv *env, jobject thiz, jstring scheme, jstring host, jstring path, jstring query, jstring fragment) {
	// Get the argument strings
	std::string scheme_str = jni::StringUTF(env, scheme);
	std::string host_str = jni::StringUTF(env, host);
	std::string path_str = jni::StringUTF(env, path);
	std::string query_str = jni::StringUTF(env, query);
	std::string fragment_str = jni::StringUTF(env, fragment);

	// Create the IRI
	std::shared_ptr<ePub3::IRI> iri(new ePub3::IRI(scheme_str, host_str, path_str, query_str, fragment_str));

	// Store in pointer pool before sending to Java
	jni::Pointer ptr(iri, POINTER_GPS("IRI"));

	// Return pointer
	return ptr.getId();
}

/*
 * Class:     org_readium_sdk_android_IRI
 * Method:    isURN
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_org_readium_sdk_android_IRI_isURN
		(JNIEnv *env, jobject thiz) {
	// Get the native pointer
	std::shared_ptr<ePub3::IRI> iri(std::static_pointer_cast<ePub3::IRI>(getNativePtr(env, thiz)));

	// Return result
	return iri->IsURN() ? JNI_TRUE : JNI_FALSE;
}

/*
 * Class:     org_readium_sdk_android_IRI
 * Method:    isRelative
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_org_readium_sdk_android_IRI_isRelative
		(JNIEnv *env, jobject thiz) {
	// Get the native pointer
	std::shared_ptr<ePub3::IRI> iri(std::static_pointer_cast<ePub3::IRI>(getNativePtr(env, thiz)));

	// Return result
	return iri->IsRelative() ? JNI_TRUE : JNI_FALSE;
}

/*
 * Class:     org_readium_sdk_android_IRI
 * Method:    isEmpty
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_org_readium_sdk_android_IRI_isEmpty
		(JNIEnv *env, jobject thiz) {
	// Get the native pointer
	std::shared_ptr<ePub3::IRI> iri(std::static_pointer_cast<ePub3::IRI>(getNativePtr(env, thiz)));

	// Return result
	return iri->IsEmpty() ? JNI_TRUE : JNI_FALSE;
}

/*
 * Class:     org_readium_sdk_android_IRI
 * Method:    getScheme
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_IRI_getScheme
		(JNIEnv *env, jobject thiz) {
	// Get the native pointer
	std::shared_ptr<ePub3::IRI> iri(std::static_pointer_cast<ePub3::IRI>(getNativePtr(env, thiz)));

	// Return result
	std::string res = iri->Scheme().stl_str();
	return jni::StringUTF(env, res);
}

/*
 * Class:     org_readium_sdk_android_IRI
 * Method:    getNameID
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_IRI_getNameID
		(JNIEnv *env, jobject thiz) {
	// Get the native pointer
	std::shared_ptr<ePub3::IRI> iri(std::static_pointer_cast<ePub3::IRI>(getNativePtr(env, thiz)));

	// Get the result
	std::string res;
	try {
		res = iri->NameID().stl_str();
	}
	catch(std::invalid_argument e) {
		jni::Exception::throwToJava(env, jni::Exception::Generic::IllegalStateException , e.what());
		return NULL;
	}

	// Return result
	return jni::StringUTF(env, res);
}

/*
 * Class:     org_readium_sdk_android_IRI
 * Method:    getHost
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_IRI_getHost
		(JNIEnv *env, jobject thiz) {
	// Get the native pointer
	std::shared_ptr<ePub3::IRI> iri(std::static_pointer_cast<ePub3::IRI>(getNativePtr(env, thiz)));

	// Return result
	std::string res = iri->Host().stl_str();
	return jni::StringUTF(env, res);
}

/*
 * Class:     org_readium_sdk_android_IRI
 * Method:    getCredentialsUID
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_IRI_getCredentialsUID
		(JNIEnv *env, jobject thiz) {
	// Get the native pointer
	std::shared_ptr<ePub3::IRI> iri(std::static_pointer_cast<ePub3::IRI>(getNativePtr(env, thiz)));

	// Return result
	ePub3::IRI::IRICredentials cred = iri->Credentials();
	std::string res = cred.first.stl_str();
	return jni::StringUTF(env, res);
}

/*
 * Class:     org_readium_sdk_android_IRI
 * Method:    getCredentialsPWD
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_IRI_getCredentialsPWD
		(JNIEnv *env, jobject thiz) {
	// Get the native pointer
	std::shared_ptr<ePub3::IRI> iri(std::static_pointer_cast<ePub3::IRI>(getNativePtr(env, thiz)));

	// Return result
	ePub3::IRI::IRICredentials cred = iri->Credentials();
	std::string res = cred.second.stl_str();
	return jni::StringUTF(env, res);
}

/*
 * Class:     org_readium_sdk_android_IRI
 * Method:    getNamespacedString
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_IRI_getNamespacedString
		(JNIEnv *env, jobject thiz) {
	// Get the native pointer
	std::shared_ptr<ePub3::IRI> iri(std::static_pointer_cast<ePub3::IRI>(getNativePtr(env, thiz)));

	// Get the result
	std::string res;
	try {
		res = iri->NamespacedString().stl_str();
	}
	catch(std::invalid_argument e) {
		jni::Exception::throwToJava(env, jni::Exception::Generic::IllegalStateException , e.what());
		return NULL;
	}

	// Return result
	return jni::StringUTF(env, res);
}

/*
 * Class:     org_readium_sdk_android_IRI
 * Method:    getPort
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_org_readium_sdk_android_IRI_getPort
		(JNIEnv *env, jobject thiz) {
	// Get the native pointer
	std::shared_ptr<ePub3::IRI> iri(std::static_pointer_cast<ePub3::IRI>(getNativePtr(env, thiz)));

	// Return result
	return (jint) iri->Port();
}

/*
 * Class:     org_readium_sdk_android_IRI
 * Method:    getPath
 * Signature: (Z)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_IRI_getPath
		(JNIEnv *env, jobject thiz, jboolean) {
	// Get the native pointer
	std::shared_ptr<ePub3::IRI> iri(std::static_pointer_cast<ePub3::IRI>(getNativePtr(env, thiz)));

	// Return result
	std::string res = iri->Path().stl_str();
	return jni::StringUTF(env, res);
}

/*
 * Class:     org_readium_sdk_android_IRI
 * Method:    getQuery
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_IRI_getQuery
		(JNIEnv *env, jobject thiz) {
	// Get the native pointer
	std::shared_ptr<ePub3::IRI> iri(std::static_pointer_cast<ePub3::IRI>(getNativePtr(env, thiz)));

	// Return result
	std::string res = iri->Query().stl_str();
	return jni::StringUTF(env, res);
}

/*
 * Class:     org_readium_sdk_android_IRI
 * Method:    getFragment
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_IRI_getFragment
		(JNIEnv *env, jobject thiz) {
	// Get the native pointer
	std::shared_ptr<ePub3::IRI> iri(std::static_pointer_cast<ePub3::IRI>(getNativePtr(env, thiz)));

	// Return result
	std::string res = iri->Fragment().stl_str();
	return jni::StringUTF(env, res);
}

/*
 * Class:     org_readium_sdk_android_IRI
 * Method:    getLastPathComponent
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_IRI_getLastPathComponent
		(JNIEnv *env, jobject thiz) {
	// Get the native pointer
	std::shared_ptr<ePub3::IRI> iri(std::static_pointer_cast<ePub3::IRI>(getNativePtr(env, thiz)));

	// Return result
	std::string res = iri->LastPathComponent().stl_str();
	return jni::StringUTF(env, res);
}

/*
 * Class:     org_readium_sdk_android_IRI
 * Method:    getContentFragmentIdentifier
 * Signature: ()Lorg/readium/sdk/android/CFI;
 */
JNIEXPORT jobject JNICALL Java_org_readium_sdk_android_IRI_getContentFragmentIdentifier
		(JNIEnv *env, jobject thiz) {
	//TODO: Implement this when CFI is available
}

/*
 * Class:     org_readium_sdk_android_IRI
 * Method:    setScheme
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_org_readium_sdk_android_IRI_setScheme
		(JNIEnv *env, jobject thiz, jstring scheme) {
	// Get the native pointer
	std::shared_ptr<ePub3::IRI> iri(std::static_pointer_cast<ePub3::IRI>(getNativePtr(env, thiz)));

	// Set it
	std::string val = jni::StringUTF(env, scheme);
	iri->SetScheme(val);
}

/*
 * Class:     org_readium_sdk_android_IRI
 * Method:    setHost
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_org_readium_sdk_android_IRI_setHost
		(JNIEnv *env, jobject thiz, jstring host) {
	// Get the native pointer
	std::shared_ptr<ePub3::IRI> iri(std::static_pointer_cast<ePub3::IRI>(getNativePtr(env, thiz)));

	// Set it
	std::string val = jni::StringUTF(env, host);
	iri->SetHost(val);
}

/*
 * Class:     org_readium_sdk_android_IRI
 * Method:    setCredentials
 * Signature: (Ljava/lang/String;Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_org_readium_sdk_android_IRI_setCredentials
		(JNIEnv *env, jobject thiz, jstring user, jstring pass) {
	// Get the native pointer
	std::shared_ptr<ePub3::IRI> iri(std::static_pointer_cast<ePub3::IRI>(getNativePtr(env, thiz)));

	// Set them
	std::string uval = jni::StringUTF(env, user);
	std::string pval = jni::StringUTF(env, pass);
	iri->SetCredentials(uval, pval);
}

/*
 * Class:     org_readium_sdk_android_IRI
 * Method:    addPathComponent
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_org_readium_sdk_android_IRI_addPathComponent
		(JNIEnv *env, jobject thiz, jstring component) {
	// Get the native pointer
	std::shared_ptr<ePub3::IRI> iri(std::static_pointer_cast<ePub3::IRI>(getNativePtr(env, thiz)));

	// Set it
	std::string val = jni::StringUTF(env, component);
	iri->AddPathComponent(val);
}

/*
 * Class:     org_readium_sdk_android_IRI
 * Method:    setQuery
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_org_readium_sdk_android_IRI_setQuery
		(JNIEnv *env, jobject thiz, jstring query) {
	// Get the native pointer
	std::shared_ptr<ePub3::IRI> iri(std::static_pointer_cast<ePub3::IRI>(getNativePtr(env, thiz)));

	// Set it
	std::string val = jni::StringUTF(env, query);
	iri->SetQuery(val);
}

/*
 * Class:     org_readium_sdk_android_IRI
 * Method:    setFragment
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_org_readium_sdk_android_IRI_setFragment
		(JNIEnv *env, jobject thiz, jstring fragment) {
	// Get the native pointer
	std::shared_ptr<ePub3::IRI> iri(std::static_pointer_cast<ePub3::IRI>(getNativePtr(env, thiz)));

	// Set it
	std::string val = jni::StringUTF(env, fragment);
	iri->SetFragment(val);
}

/*
 * Class:     org_readium_sdk_android_IRI
 * Method:    setContentFragmentIdentifier
 * Signature: (Lorg/readium/sdk/android/CFI;)V
 */
JNIEXPORT void JNICALL Java_org_readium_sdk_android_IRI_setContentFragmentIdentifier
		(JNIEnv *env, jobject thiz, jobject cfi) {
	//TODO: Implement this when CFI is available
}

/*
 * Class:     org_readium_sdk_android_IRI
 * Method:    toIRIString
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_IRI_toIRIString
		(JNIEnv *env, jobject thiz) {
	// Get the native pointer
	std::shared_ptr<ePub3::IRI> iri(std::static_pointer_cast<ePub3::IRI>(getNativePtr(env, thiz)));

	// Return result
	std::string res = iri->IRIString().stl_str();
	return jni::StringUTF(env, res);
}

/*
 * Class:     org_readium_sdk_android_IRI
 * Method:    toURIString
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_IRI_toURIString
		(JNIEnv *env, jobject thiz) {
	// Get the native pointer
	std::shared_ptr<ePub3::IRI> iri(std::static_pointer_cast<ePub3::IRI>(getNativePtr(env, thiz)));

	// Return result
	std::string res = iri->URIString().stl_str();
	return jni::StringUTF(env, res);
}

/*
 * Class:     org_readium_sdk_android_IRI
 * Method:    URLEncodeComponent
 * Signature: (Ljava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_IRI_URLEncodeComponent
		(JNIEnv *env, jclass clazz, jstring str) {
	// Get the parameter
	std::string val = jni::StringUTF(env, str);

	// Return result
	std::string res = ePub3::IRI::URLEncodeComponent(val).stl_str();
	return jni::StringUTF(env, res);
}

/*
 * Class:     org_readium_sdk_android_IRI
 * Method:    PercentEncodeUCS
 * Signature: (Ljava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_IRI_PercentEncodeUCS
		(JNIEnv *env, jclass clazz, jstring str) {
	// Get the parameter
	std::string val = jni::StringUTF(env, str);

	// Return result
	std::string res = ePub3::IRI::PercentEncodeUCS(val).stl_str();
	return jni::StringUTF(env, res);
}

/*
 * Class:     org_readium_sdk_android_IRI
 * Method:    IDNEncodeHostname
 * Signature: (Ljava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_IRI_IDNEncodeHostname
		(JNIEnv *env, jclass clazz, jstring host) {
	// Get the parameter
	std::string val = jni::StringUTF(env, host);

	// Return result
	std::string res = ePub3::IRI::IDNEncodeHostname(val).stl_str();
	return jni::StringUTF(env, res);
}


#ifdef __cplusplus
}
#endif
