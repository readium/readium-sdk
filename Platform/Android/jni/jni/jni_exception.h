//
//  jni_exception.h
//  ePub3
//
//  Created by Pedro Reis Colaco (txtr) on 2013-08-09.
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


#ifndef _JNI_JNIEXCEPTION_H_
#define _JNI_JNIEXCEPTION_H_


#include <exception>
#include <string>

#include "jni_log.h"

#include <jni.h>


namespace jni {


/**
 * Exception class defines exceptions for use in the JNI routines.
 * This is a std::exception with user defined message.
 */
class Exception : public std::exception {

protected:
	/**
	 * Exception message.
	 */
	std::string _msg;

public:
	/**
	 * Default constructor with optional string message.
	 */
	Exception(const std::string &msg = "Unknown exception") : _msg(msg) { }

	/**
	 * Constructor with char* message.
	 */
	Exception(const char *msg) : _msg(msg) { }

	/**
	 * Destructor.
	 */
	virtual ~Exception() throw() { }

	/**
	 * Override of what() from std::exception to return the user
	 * defined exception message.
	 */
	virtual const char *what() const throw() { return _msg.c_str(); }

	/**
	 * Throws an Exception to Java.
	 */
	static void throwToJava(JNIEnv *env, const char *exceptionClass, const char *message) {
		if(exceptionClass == NULL || message == NULL) {
			throw Exception("Need a valid exception class and message to throw exception to Java");
		}
		// Find the exception class
		// Do not use Class here due to circular dependencies
		// because Class also needs Exception.
		jclass clazz =  env->FindClass(exceptionClass);
		// Check that we found a class
		if(clazz != NULL) {
			env->ThrowNew(clazz, message);
		} else {
			LOGE("throwToJava(): couldn't get java exception class '%s' to throw '%s'", exceptionClass, message);
		}
	}

	/*
	 * Exception classes definitions
	 */

	struct IO {
		static constexpr const char *CharConversionException				= "java/io/CharConversionException";
		static constexpr const char *EOFException							= "java/io/EOFException";
		static constexpr const char *FileNotFoundException					= "java/io/FileNotFoundException";
		static constexpr const char *IOException							= "java/io/IOException";
		static constexpr const char *InterruptedIOException					= "java/io/InterruptedIOException";
		static constexpr const char *InvalidClassException					= "java/io/InvalidClassException";
		static constexpr const char *InvalidObjectException					= "java/io/InvalidObjectException";
		static constexpr const char *UTFDataFormatException					= "java/io/UTFDataFormatException";
		static constexpr const char *UnsupportedEncodingException			= "java/io/UnsupportedEncodingException";
	};

	struct Generic {
		static constexpr const char *ArithmeticException					= "java/lang/ArithmeticException";
		static constexpr const char *ArrayIndexOutOfBoundsException			= "java/lang/ArrayIndexOutOfBoundsException";
		static constexpr const char *ArrayStoreException					= "java/lang/ArrayStoreException";
		static constexpr const char *ClassCastException						= "java/lang/ClassCastException";
		static constexpr const char *ClassNotFoundException					= "java/lang/ClassNotFoundException";
		static constexpr const char *Exception								= "java/lang/Exception";
		static constexpr const char *IllegalAccessException					= "java/lang/IllegalAccessException";
		static constexpr const char *IllegalArgumentException				= "java/lang/IllegalArgumentException";
		static constexpr const char *IllegalMonitorStateException			= "java/lang/IllegalMonitorStateException";
		static constexpr const char *IllegalStateException					= "java/lang/IllegalStateException";
		static constexpr const char *IllegalThreadStateException			= "java/lang/IllegalThreadStateException";
		static constexpr const char *IndexOutOfBoundsException				= "java/lang/IndexOutOfBoundsException";
		static constexpr const char *InstantiationException					= "java/lang/InstantiationException";
		static constexpr const char *InterruptedException					= "java/lang/InterruptedException";
		static constexpr const char *NegativeArraySizeException				= "java/lang/NegativeArraySizeException";
		static constexpr const char *NoSuchFieldException					= "java/lang/NoSuchFieldException";
		static constexpr const char *NoSuchMethodException					= "java/lang/NoSuchMethodException";
		static constexpr const char *NullPointerException					= "java/lang/NullPointerException";
		static constexpr const char *NumberFormatException					= "java/lang/NumberFormatException";
		static constexpr const char *RuntimeException						= "java/lang/RuntimeException";
		static constexpr const char *SecurityException						= "java/lang/SecurityException";
		static constexpr const char *StringIndexOutOfBoundsException		= "java/lang/StringIndexOutOfBoundsException";
		static constexpr const char *TimeoutException						= "java/util/concurrent/TimeoutException";
		static constexpr const char *UnknownFormatConversionException		= "java/util/UnknownFormatConversionException";
		static constexpr const char *UnsupportedOperationException			= "java/lang/UnsupportedOperationException";
	};

	struct Net {
		static constexpr const char *ConnectException						= "java/net/ConnectException";
		static constexpr const char *MalformedURLException					= "java/net/MalformedURLException";
		static constexpr const char *ProtocolException						= "java/net/ProtocolException";
		static constexpr const char *SocketException						= "java/net/SocketException";
		static constexpr const char *SocketTimeoutException					= "java/net/SocketTimeoutException";
		static constexpr const char *UnknownHostException					= "java/net/UnknownHostException";
		static constexpr const char *UnknownServiceException				= "java/net/UnknownServiceException";
	};

	struct OS {
		static constexpr const char *OperationCanceledException				= "android/os/OperationCanceledException";
		static constexpr const char *RemoteException						= "android/os/RemoteException";
	};

	struct Security {
		static constexpr const char *AccessControlException					= "java/security/AccessControlException";
		static constexpr const char *InvalidKeyException					= "java/security/InvalidKeyException";
		static constexpr const char *InvalidParameterException				= "java/security/InvalidParameterException";
		static constexpr const char *ProviderException						= "java/security/ProviderException";
		static constexpr const char *SignatureException						= "java/security/SignatureException";
		static constexpr const char *UnrecoverableEntryException			= "java/security/UnrecoverableEntryException";
		static constexpr const char *UnrecoverableKeyException				= "java/security/UnrecoverableKeyException";
	};

};


} //namespace jni


#endif //ifndef _JNI_JNIEXCEPTION_H_
