//
//  jni_string.h
//  ePub3
//
//  Created by Pedro Reis Colaco (txtr) on 2013-08-22.
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


#ifndef _JNI_JNISTRING_H_
#define _JNI_JNISTRING_H_


#include <string>

#include <jni.h>

#include "jni_resource.h"


namespace jni {


class StringBase {

protected:
	JNIEnv *_env;
	std::string _str;

public:
	/**
	 * Default empty constructor
	 */
	StringBase() : _env(NULL), _str("") { }

	/**
	 * Constructor from a const char pointer.
	 */
	StringBase(JNIEnv *env, const char *str) : _env(env), _str(str) { }

	/**
	 * Constructor from std::string.
	 */
	StringBase(JNIEnv *env, std::string& str) : _env(env), _str(str) { }

	/**
	 * Gets the internal const char pointer data of std::string.
	 * See std:string.c_str() for more information.
	 */
	const char* c_str() const { return _str.c_str(); }

	/**
	 * Gets the internal std::string of this String.
	 */
	std::string string() const { return _str; }

	/**
	 * Cast to std::string operator.
	 */
	operator std::string() const {
		return _str;
	}

	/**
	 * Cast to jstring operator.
	 */
	virtual operator jstring() const = 0;
};

class String : public StringBase {

public:
	/**
	 * Default empty constructor
	 */
	String() : StringBase(NULL, "") { }

	/**
	 * Constructor from a const char pointer.
	 */
	String(JNIEnv *env, const char *str) : StringBase(env, str) { }

	/**
	 * Constructor from std::string.
	 */
	String(JNIEnv *env, std::string& str) : StringBase(env, str) { }

	/**
	 * Constructor with a jstring.
	 * This is helpful for retrieving jstring function parameters for instance.
	 * The internal string is initialized with a copy of the jstring and the temporary
	 * JNI memory is released automatically.
	 */
	String(JNIEnv *env, jstring str) : StringBase(env, StringChars(env, str)) { }

	/**
	 * Cast to jstring operator.
	 */
	//TODO: Unicode is not tested and may have to be fine tuned.
	operator jstring() const { return _env->NewString((const jchar *)_str.c_str(), (jsize)_str.length()); }

	/**
	 * Helper static function to create an instance of String with an empty
	 * std::string.
	 */
	static String empty(JNIEnv * env) {
		return String(env, "");
	}
};

class StringUTF : public StringBase {

public:
	/**
	 * Default empty constructor
	 */
	StringUTF() : StringBase(NULL, "") { }

	/**
	 * Constructor from a const char pointer.
	 */
	StringUTF(JNIEnv *env, const char *str) : StringBase(env, str) { }

	/**
	 * Constructor from std::string.
	 */
	StringUTF(JNIEnv *env, std::string& str) : StringBase(env, str) { }

	/**
	 * Constructor with a jstring.
	 * This is helpful for retrieving jstring function parameters for instance.
	 * The internal string is initialized with a copy of the jstring and the temporary
	 * JNI memory is released automatically.
	 */
	StringUTF(JNIEnv *env, jstring str) : StringBase(env, StringUTFChars(env, str)) { }

	/**
	 * Cast to jstring operator.
	 */
	operator jstring() const { return _env->NewStringUTF(_str.c_str()); }

	/**
	 * Helper static function to create an instance of StringUTF with an empty
	 * std::string.
	 */
	static StringUTF empty(JNIEnv * env) {
		return StringUTF(env, "");
	}
};


} //namespace jni


#endif //ifndef _JNI_JNISTRING_H_
