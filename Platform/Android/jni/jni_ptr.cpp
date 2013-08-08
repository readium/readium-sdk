//
//  jni_ptr.cpp
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
#include <sstream>
#include <cstdio>


#include <jni.h>

#include "jni_ptr.h"


namespace jni {


/*
 * Pointer
 **************************************************/

/**
 * Construct a pointer object and add it to the pointer pool.
 * This prevents the pointer form being deleted when passed to
 * Java layer.
 * The name argument is optional and should be used mainly to
 * resolve memory leak issues. Just pass to it the macro
 * POINTER_GPS(name) to have a complete dump of the leaks at
 * the end of your program execution.
 */
Pointer::Pointer(sptr ptr, std::string name = "") : _id((jlong) ptr.get()), _ptr(ptr), _name(name) {
	// Prevent from adding nullptr to pointer pool
	if(ptr != nullptr) {
		PointerPool::add((Pointer)*this);
	}
}

/**
 * Recall a pointer previously added to the pointer pool.
 * If the id passed is not valid, this pointer will be pointing
 * to nullptr.
 */
Pointer::Pointer(jlong id) {
	// Get from pointer pool
	Pointer p = PointerPool::get(id);
	// Create the pointer copies
	_id = p.getId();
	_ptr = p.getPtr();
	_name = p.getName();
}

/**
 * Returns true if this pointer is nullptr.
 */
bool Pointer::isNull() {
	return (_ptr == nullptr);
}

/**
 * Returns true if this pointer is unique.
 */
bool Pointer::isUnique() {
	return _ptr.unique();
}

/**
 * Returns the current use_count of this pointer.
 */
long Pointer::useCount() {
	return _ptr.use_count();
}

/**
 * Gets the id of this pointer.
 */
jlong Pointer::getId() {
	return _id;
}

/**
 * Gets a copy of the smart pointer of this pointer.
 */
Pointer::sptr Pointer::getPtr() {
	return _ptr;
}

/**
 * Gets the name of the smart pointer, if any.
 */
std::string Pointer::getName() {
	return _name;
}

/**
 * Releases this pointer from the pointer pool.
 */
void Pointer::release() {
	PointerPool::del(_id);
}


/**
 * Copy assignment operator.
 */
Pointer& Pointer::operator=(const Pointer& o) {
	if (this != &o) {
		_id = o._id;
		_ptr = o._ptr;
		_name = o._name;
	}

	return *this;
}

/**
 * Move assignment operator.
 */
Pointer& Pointer::operator=(Pointer&& o) {
	if (this != &o) {
		_id = o._id;
		_ptr = std::move(o._ptr);
		_name = std::move(o._name);
	}

	return *this;
}


/*
 * PointerPool
 **************************************************/

/**
 * Static pointer pool. Just for enabling the leaks check.
 */
static PointerPool sPointerPool = PointerPool();

/**
 * Initialize internal static pointer pool map.
 */
PointerPool::PointerPoolMap PointerPool::_pool = PointerPoolMap();

/**
 * Construct a pointer pool.
 */
PointerPool::PointerPool() {
	LOGD("PointerPool(): pointer pool created");

	// Nothing really to do here...
}

/**
 * Destruct a pointer pool. Log error if there are still some
 * pointers left when destructed.
 */
PointerPool::~PointerPool() {
	// Nothing really to do here, just check for memory leaks and dump
	// them if needed...

	if(_pool.size() > 0) {
		std::string leaks = PointerPool::dump();
		LOGE("~PointerPool(): it seems that we leaked %s", leaks.c_str());
	} else {
		LOGD("~PointerPool(): pointer pool destroyed (no leaks)");
	}
}

/**
 * Adder.
 */
jlong PointerPool::add(Pointer ptr) {
	jlong id = ptr.getId();
	_pool[id] = ptr;

	LOGD("PointerPool::add(): added pointer %llX(%i) %s", id, ptr.useCount(), ptr.getName().c_str());
	return id;
}

/**
 * Getter.
 */
Pointer PointerPool::get(jlong id) {
	auto p = _pool.find(id);
	if(p != _pool.end()) {
		Pointer ptr(p->second);

		LOGD("PointerPool::get(): got pointer %llX(%i) %s", id, ptr.useCount(), ptr.getName().c_str());
		return ptr;
	} else {
		LOGE("PointerPool::get(): attempting to get non existing pointer from pool");
	}
	return Pointer();
}

/**
 * Deleters.
 */
void PointerPool::del(jlong id) {
	auto p = _pool.find(id);
	if(p != _pool.end()) {
		Pointer ptr(p->second);

		_pool.erase(p);

		// Verify that we are deleting a pointer that is unique
		// without any other copies alive, and raise warning otherwise
		if(ptr.isUnique()) {
			LOGD("PointerPool::del(): deleted pointer %llX(%i) %s", id, ptr.useCount(), ptr.getName().c_str());
		} else {
			LOGW("PointerPool::del(): deleted non-unique pointer %llX(%i) %s", id, ptr.useCount(), ptr.getName().c_str());
		}
	} else {
		LOGE("PointerPool::del(): attempting to remove non existing pointer from pool");
	}
}
void PointerPool::del(Pointer ptr) {
	PointerPool::del(ptr.getId());
}

/**
 * Dumps the pointers in the pool when needed.
 */
std::string PointerPool::dump() {
	std::ostringstream dump;

	if(_pool.size() > 0) {
		dump << _pool.size() << " pointers in the pool:" << std::endl;
		for(auto p = _pool.begin(); p != _pool.end(); ++p) {
			jlong id = p->first;
			Pointer& ptr = p->second;
			std::string name;
			if(!ptr.isNull()) {
				name = ptr.getName();
				if(name.empty()) {
					name = "(empty)";
				}
			} else {
				name = "(nullptr)";
			}
			char sid[100];
			sprintf(sid, "%llX", id);
			dump << "    " << sid << "(" << ptr.useCount() << ")" << " " << name << std::endl;
		}
		return dump.str();
	} else {
		return "";
	}
}

} //namespace JNI
