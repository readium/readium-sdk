//
//  jni_resource_base.h
//  ePub3
//
//  Created by Pedro Reis Colaco (txtr) on 2013-08-12.
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


#ifndef _JNI_JNIRESOURCEBASE_H_
#define _JNI_JNIRESOURCEBASE_H_


#include "jni_types.h"


namespace jni {


/*
 * Resource management for JNI is provided using the C++ "construction as
 * resource acquisition" idiom: resources are allocated in the constructor, 
 * and are released in the destructor of dedicated auxiliary objects.
 * This idiom is implemented using the Proxy pattern, with functionality 
 * similar to that of 'auto_ptr'.
 */

/**
 * Resource template: general resource management
 *
 * The template parameter must provide the following four definitions:
 * - JResource type: the original Java resource type (e.g., 'jintArray')
 * - Resource type: the corresponding exported resource (e.g., 'jint *')
 * - GetF:           a functional object, which provides resource allocation 
 *                   of the form
 *                     Resource GetF::operator()(JNIEnv *, JResource)
 * - ReleaseF:       a functional object, which provides resource deallocation 
 *                   of the form
 *                     void ReleaseF::operator()(JNIEnv *, JResource, Resource)
 *
 * The 'GetF' and 'ReleaseF' functional objects supply default allocation and 
 * deallocation facilities. In general, the JNI allows customized resource 
 * management (i.e., the 'isCopy' parameter for allocation and the 'mode' 
 * parameter for deallocation). This behaviors could be achieved by supplying
 * a user-defined functional object as an additional parameter to the
 * constructor (in case of allocation), or to function 'ReleaseResource'
 * (to be used prior to destruction for explicit customized deallocation).
 *
 * Resource functionality:
 * - Constructors, which use functional objects of the form
 *     Resource GetF::operator()(JNIEnv *, JResource)
 * - Destructor/ReleaseResource, which use functional objects of the form
 *     void ReleaseF::operator()(JNIEnv *, JResource, Resource)
 * - smart pointer functionality: op=, casting operator, get()/release()
 */
 
template<class ResourceSettings>
class Resource {
	typedef Resource<ResourceSettings> _self;
	typedef typename ResourceSettings::JResource JResource;
	typedef typename ResourceSettings::Resource ResourceHandle;
	typedef typename ResourceSettings::GetF DefaultGetF;
	typedef typename ResourceSettings::ReleaseF DefaultReleaseF;

private:
	bool _owns;		      			// true if the current object owns the resource
   
protected:
	JNIEnv *_env;					// Java environment handle
	JResource _jresource;			// Java resource handle
	ResourceHandle _resource;		// exported resource handle

public:

	/**
	 * Default constructor (to allow arrays of resources)
	 */
	Resource() : _env(0), _jresource(0), _resource(0), _owns(0) { }

	/**
	 * Constructors use a functional object of the form
	 *   Resource GetF::operator()(JNIEnv *, JResource)
	 * for resource allocation (by default, DefaultGetF())
	 */
	Resource(JNIEnv *env, JResource jresource) :
			_env(env), _owns(true), _jresource(jresource) {
		_resource = DefaultGetF()(_env, _jresource);
	}

	template<class GetF>
	Resource(JNIEnv *env, JResource jresource, GetF &getF) :
			_env(env), _owns(true), _jresource(jresource) {
		_resource = getF(_env, _jresource);
	}

	template<class GetF>
	Resource(JNIEnv *env, JResource jresource, const GetF &getF) :
			_env(env), _owns(true), _jresource(jresource) {
		_resource = getF(_env, _jresource);
	}

	/**
	 * Copy constructor
	 */
	Resource(const _self &x) :
			_env(x._env), _owns(x._owns),
			_jresource(x._jresource), _resource(x.release()) { }

	/**
	 * Assignment operator
	 */
	_self &operator= (const _self &x) {
		if (&x != this) {
			if (_owns)
				ReleaseResource();

			_env = x._env;
			_owns = x._owns;
			_jresource = x._jresource;
			_resource = x.release();
		}

		return *this;
	}

	/**
	 * Destructor: calls the default ReleaseResource()
	 */
	~Resource() {
		ReleaseResource();
	}

	/**
	 * ReleaseResource methods use a functional object of the form
	 *   void ReleaseF::operator()(JNIEnv *, JResource, Resource)
	 * for resource deallocation (by default, DefaultReleaseF())
	 */
	void ReleaseResource() {
		if (_owns) {
			DefaultReleaseF()(_env, _jresource, release());
		}
	}

	template<class ReleaseF>
	void ReleaseResource(ReleaseF &releaseF) {
		if (_owns) {
			releaseF(_env, _jresource, release());
		}
	}

	template<class ReleaseF>
	void ReleaseResource(const ReleaseF &releaseF) {
		if (_owns) {
			releaseF(_env, _jresource, release());
		}
	}

	/**
	 * Casting operators
	 */
	operator Resource() { return get(); }
	operator const Resource() const { return get(); }
	operator ResourceHandle() const { return _resource; }

	/**
	 * Function get() returns the resource
	 */
	Resource &get() { return _resource; }
	const Resource &get() const { return _resource; }

	/**
	 * function release() clears the resource handle,
	 * and returns the resource themselves
	 */
	ResourceHandle release() {
		_owns = false;
		ResourceHandle tmp = _resource;
		_resource = 0;
		return tmp;
	}
};


} //namespace jni


#endif //ifndef _JNI_JNIRESOURCEBASE_H_
