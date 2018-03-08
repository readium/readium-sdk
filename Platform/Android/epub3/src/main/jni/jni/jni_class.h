//
//  jni_class.h
//  ePub3
//
//  Created by Pedro Reis Colaco (txtr) on 2013-08-09.
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


#ifndef _JNI_JNICLASS_H_
#define _JNI_JNICLASS_H_


#include <jni.h>

#include "jni_exception.h"
#include "jni_log.h"


namespace jni {


/**
 * Class encapsulates a 'jclass' JNI object.
 * Its only purpose is to provide a variety of constructors of the form
 * Class(JNIEnv *, ProtoClass), where ProtoClass can be 'jclass',
 * 'jobject', or 'const char *' (anything from which a 'jclass' can
 * be constructed.
 * Class also has a casting operator to 'jclass' type, so that a call
 * Class(env, arg) can be used in any place where 'jclass' is required.
 */
class Class {

protected:
	/**
	 * Class handle
	 */
	jclass _clazz;
   
public:
	/**
	 * Default empty constructor
	 */
	Class() : _clazz(NULL) { }

	/**
	 * Construct a Class from full java class name like
	 * "java/lang/String".
	 */
	Class(JNIEnv *env, const char *name) : _clazz(env->FindClass(name)) {
		if (_clazz != NULL) {
			LOGD("Class(): found class '%s'", name);
		} else {
			LOGE("Class(): couldn't find class '%s'", name);
		}
	}

	/**
	 * Construct a Class from a JNI jobject.
	 */
	Class(JNIEnv *env, jobject obj) : _clazz(env->GetObjectClass(obj)) {
		if (_clazz != NULL) {
			LOGD("Class(): found class from jobject");
		} else {
			LOGE("Class(): couldn't find class from jobject");
		}
	}

protected:
	/**
	 * Check if empty and throw.
	 */
	void throwIfEmpty() const {
		if(IsEmpty()) {
			throw Exception("Using empty or invalid Class. Check logcat for details...");
		}
	}

public:

	/**
	 * Construct Class from a JNI jclass.
	 */
	Class(JNIEnv *env, jclass clazz) : _clazz(clazz) { }
	Class(jclass clazz) : _clazz(clazz) { }

	/**
	 * Casting operators to JNI jclass
	 */
	operator jclass() { throwIfEmpty(); return _clazz; }
	operator const jclass() const { throwIfEmpty(); return _clazz; }

	/**
	 * Checks if this class is empty
	 */
	bool IsEmpty() const { return _clazz == NULL; }
};


} //namespace jni


#endif //ifndef _JNI_JNICLASS_H_
