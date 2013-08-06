//
//  jni_ptr.h
//  ePub3
//
//  Created by Pedro Reis Colaco (txtr) on 2013-08-02.
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


#include <memory>
#include <map>

#include <jni.h>

#include "log.h"


#ifndef _JNIPTR_JNI_H_
#define _JNIPTR_JNI_H_


namespace jni {


class PointerPool;


/**
 * Macro for Pointer name so that it keeps a record of file and
 * line where it came from.
 */
#define POINTER_GPS(name) name "[" __FILE__ ":" STRINGIZE(__LINE__) "]"

/**
 * Pointer helper class to ease the manipulation of the Pointer
 * pool. When creating an object of this class it's pointer is
 * added automatically to the pool. It also can recycle a previous
 * object by constructing it with the id. Or release it and the
 * pointer is taken out of the pool to be freed.
 */
class Pointer {

private:
	/**
	 * Generic void smart pointer
	 */
	typedef std::shared_ptr<void> sptr;

	jlong _id;
	sptr _ptr;
	std::string _name;

public:
	/**
	 * Default constructor (to nullptr).
	 */
	Pointer() : _id(0), _ptr(nullptr), _name("") { }

	/**
	 * Copy constructor.
	 */
	Pointer(const Pointer& o) : _id(o._id), _ptr(o._ptr), _name(o._name) { }

	/**
	 * Move constructor.
	 */
	Pointer(Pointer&& o) : _id(o._id), _ptr(std::move(o._ptr)), _name(std::move(o._name)) { }

	/**
	 * Construct a pointer object and add it to the pointer pool.
	 * This prevents the pointer form being deleted when passed to
	 * Java layer.
	 * The name argument is optional and should be used mainly to
	 * resolve memory leak issues. Just pass to it the macro
	 * POINTER_GPS(name) to have a complete dump of the leaks at
	 * the end of your program execution, or when you call the
	 * PointerPool::dumpLeaks() function.
	 */
	Pointer(sptr ptr, std::string name = "");

	/**
	 * Recall a pointer previously added to the pointer pool.
	 * If the id passed is not valid, this pointer will be pointing
	 * to nullptr.
	 */
	Pointer(jlong id);

	/**
	 * Nothing to be done here. This shouldn't release the pointer
	 * from the pointer pool. For that, an explicit call to the
	 * release() function needs to be done.
	 */
	~Pointer() { }

	/**
	 * Returns true if this pointer is nullptr
	 */
	bool isNull();

	/**
	 * Gets the id of this pointer.
	 */
	jlong getId();

	/**
	 * Gets a copy of the smart pointer of this pointer.
	 */
	sptr getPtr();

	/**
	 * Gets the name of the smart pointer, if any.
	 */
	std::string getName();

	/**
	 * Releases this pointer from the pointer pool.
	 */
	void release();

	/**
	 * Copy assignment operator.
	 */
	Pointer& operator=(const Pointer& o);

	/**
	 * Move assignment operator.
	 */
	Pointer& operator=(Pointer&& o);

};


/**
 * Pointer pool to hold the C/C++ smart pointers when they are
 * passed to Android Java side through JNI. This prevents them to
 * get deleted in the meanwhile and gives us control of their life
 * cycle. Every pointer added to this pool has to be removed when
 * it is not needed anymore.
 */
class PointerPool {

private:

	/**
	 * Internal static pointer pool.
	 */
	typedef std::map<jlong, Pointer> PointerPoolMap;
	static PointerPoolMap _pool;

public:

	/**
	 * Construct a pointer pool.
	 */
	PointerPool();

	/**
	 * Destruct a pointer pool. Log error if there are still some
	 * pointers left when destructed.
	 */
	~PointerPool();

	/**
	 * Adder.
	 */
	static jlong add(Pointer ptr);

	/**
	 * Getter.
	 */
	static Pointer get(jlong id);

	/**
	 * Deleters.
	 */
	static void del(jlong id);
	static void del(Pointer ptr);

	/**
	 * Dumps the pointers in the pool when needed.
	 */
	static std::string dump();

};


} //namespace JNI


#endif //ifndef _JNIPTR_JNI_H_
