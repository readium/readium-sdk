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
};


} //namespace jni


#endif //ifndef _JNI_JNIEXCEPTION_H_
