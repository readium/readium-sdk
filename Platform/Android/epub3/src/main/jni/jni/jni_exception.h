//
//  jni_exception.h
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
