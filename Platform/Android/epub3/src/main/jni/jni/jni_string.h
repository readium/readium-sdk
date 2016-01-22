//
//  jni_string.h
//  ePub3
//
//  Created by Pedro Reis Colaco (txtr) on 2013-08-22.
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
