//
//  iri.h
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


#include <jni.h>


#ifndef _IRI_JNI_H_
#define _IRI_JNI_H_


#ifdef __cplusplus
extern "C" {
#endif


/*
 * Exported functions
 **************************************************/

/**
 * Initialize the cached java elements for IRI class
 */
int onLoad_cacheJavaElements_iri(JNIEnv *env);

/**
 * Calls the java createIRI_empty method of IRI class
 */
jobject javaIRI_createIRIempty(JNIEnv *env);

/**
 * Calls the java createIRI_string method of IRI class
 */
jobject javaIRI_createIRIstring(JNIEnv *env, jstring iriStr);

/**
 * Calls the java createIRI_urn method of IRI class
 */
jobject javaIRI_createIRIurn(JNIEnv *env, jstring nameID, jstring namespacedString);

/**
 * Calls the java createIRI_url method of IRI class
 */
jobject javaIRI_createIRIurl(JNIEnv *env, jstring scheme, jstring host, jstring path, jstring query, jstring fragment);


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
		(JNIEnv *env, jobject thiz);

/*
 * Class:     org_readium_sdk_android_IRI
 * Method:    createNativeIRIstring
 * Signature: (Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_org_readium_sdk_android_IRI_createNativeIRIstring
		(JNIEnv *env, jobject thiz, jstring iriStr);

/*
 * Class:     org_readium_sdk_android_IRI
 * Method:    createNativeIRIurn
 * Signature: (Ljava/lang/String;Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_org_readium_sdk_android_IRI_createNativeIRIurn
		(JNIEnv *env, jobject thiz, jstring nameID, jstring namespacedString);

/*
 * Class:     org_readium_sdk_android_IRI
 * Method:    createNativeIRIurl
 * Signature: (Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_org_readium_sdk_android_IRI_createNativeIRIurl
		(JNIEnv *env, jobject thiz, jstring scheme, jstring host, jstring path, jstring query, jstring fragment);

/*
 * Class:     org_readium_sdk_android_IRI
 * Method:    isURN
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_org_readium_sdk_android_IRI_isURN
  (JNIEnv *env, jobject thiz);

/*
 * Class:     org_readium_sdk_android_IRI
 * Method:    isRelative
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_org_readium_sdk_android_IRI_isRelative
  (JNIEnv *env, jobject thiz);

/*
 * Class:     org_readium_sdk_android_IRI
 * Method:    isEmpty
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_org_readium_sdk_android_IRI_isEmpty
  (JNIEnv *env, jobject thiz);

/*
 * Class:     org_readium_sdk_android_IRI
 * Method:    getScheme
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_IRI_getScheme
  (JNIEnv *env, jobject thiz);

/*
 * Class:     org_readium_sdk_android_IRI
 * Method:    getNameID
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_IRI_getNameID
  (JNIEnv *env, jobject thiz);

/*
 * Class:     org_readium_sdk_android_IRI
 * Method:    getHost
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_IRI_getHost
  (JNIEnv *env, jobject thiz);

/*
 * Class:     org_readium_sdk_android_IRI
 * Method:    getCredentialsUID
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_IRI_getCredentialsUID
  (JNIEnv *env, jobject thiz);

/*
 * Class:     org_readium_sdk_android_IRI
 * Method:    getCredentialsPWD
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_IRI_getCredentialsPWD
  (JNIEnv *env, jobject thiz);

/*
 * Class:     org_readium_sdk_android_IRI
 * Method:    getNamespacedString
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_IRI_getNamespacedString
  (JNIEnv *env, jobject thiz);

/*
 * Class:     org_readium_sdk_android_IRI
 * Method:    getPort
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_org_readium_sdk_android_IRI_getPort
  (JNIEnv *env, jobject thiz);

/*
 * Class:     org_readium_sdk_android_IRI
 * Method:    getPath
 * Signature: (Z)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_IRI_getPath
  (JNIEnv *env, jobject thiz, jboolean);

/*
 * Class:     org_readium_sdk_android_IRI
 * Method:    getQuery
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_IRI_getQuery
  (JNIEnv *env, jobject thiz);

/*
 * Class:     org_readium_sdk_android_IRI
 * Method:    getFragment
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_IRI_getFragment
  (JNIEnv *env, jobject thiz);

/*
 * Class:     org_readium_sdk_android_IRI
 * Method:    getLastPathComponent
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_IRI_getLastPathComponent
  (JNIEnv *env, jobject thiz);

/*
 * Class:     org_readium_sdk_android_IRI
 * Method:    getContentFragmentIdentifier
 * Signature: ()Lorg/readium/sdk/android/CFI;
 */
JNIEXPORT jobject JNICALL Java_org_readium_sdk_android_IRI_getContentFragmentIdentifier
  (JNIEnv *env, jobject thiz);

/*
 * Class:     org_readium_sdk_android_IRI
 * Method:    setScheme
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_org_readium_sdk_android_IRI_setScheme
  (JNIEnv *env, jobject thiz, jstring scheme);

/*
 * Class:     org_readium_sdk_android_IRI
 * Method:    setHost
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_org_readium_sdk_android_IRI_setHost
  (JNIEnv *env, jobject thiz, jstring host);

/*
 * Class:     org_readium_sdk_android_IRI
 * Method:    setCredentials
 * Signature: (Ljava/lang/String;Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_org_readium_sdk_android_IRI_setCredentials
  (JNIEnv *env, jobject thiz, jstring user, jstring pass);

/*
 * Class:     org_readium_sdk_android_IRI
 * Method:    addPathComponent
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_org_readium_sdk_android_IRI_addPathComponent
  (JNIEnv *env, jobject thiz, jstring component);

/*
 * Class:     org_readium_sdk_android_IRI
 * Method:    setQuery
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_org_readium_sdk_android_IRI_setQuery
  (JNIEnv *env, jobject thiz, jstring query);

/*
 * Class:     org_readium_sdk_android_IRI
 * Method:    setFragment
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_org_readium_sdk_android_IRI_setFragment
  (JNIEnv *env, jobject thiz, jstring fragment);

/*
 * Class:     org_readium_sdk_android_IRI
 * Method:    setContentFragmentIdentifier
 * Signature: (Lorg/readium/sdk/android/CFI;)V
 */
JNIEXPORT void JNICALL Java_org_readium_sdk_android_IRI_setContentFragmentIdentifier
  (JNIEnv *env, jobject thiz, jobject cfi);

/*
 * Class:     org_readium_sdk_android_IRI
 * Method:    toIRIString
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_IRI_toIRIString
  (JNIEnv *env, jobject thiz);

/*
 * Class:     org_readium_sdk_android_IRI
 * Method:    toURIString
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_IRI_toURIString
  (JNIEnv *env, jobject thiz);

/*
 * Class:     org_readium_sdk_android_IRI
 * Method:    URLEncodeComponent
 * Signature: (Ljava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_IRI_URLEncodeComponent
  (JNIEnv *, jclass, jstring str);

/*
 * Class:     org_readium_sdk_android_IRI
 * Method:    PercentEncodeUCS
 * Signature: (Ljava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_IRI_PercentEncodeUCS
  (JNIEnv *, jclass, jstring str);

/*
 * Class:     org_readium_sdk_android_IRI
 * Method:    IDNEncodeHostname
 * Signature: (Ljava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_IRI_IDNEncodeHostname
  (JNIEnv *, jclass, jstring host);


#ifdef __cplusplus
}
#endif


#endif //ifndef _CONTAINER_JNI_H_
