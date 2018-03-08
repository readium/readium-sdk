//
//  jni_resource.h
//  ePub3
//
//  Created by Pedro Reis Colaco (txtr) on 2013-08-12.
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


#ifndef _JNI_JNIRESOURCE_H_
#define _JNI_JNIRESOURCE_H_


#include <string>

#include "jni_types.h"
#include "jni_resource_base.h"


namespace jni {


/*
 * Provide encapsulation of basic JNI facilities.
 */


/*
 * Auxiliary utilities for resource construction
 */

/**
 * ObjectTag<T> helps to determine whether T is a 'jobject' or not.
 * ObjectTag<T>::tag() returns object_tag() if T is a 'jobject',
 * and non_object_tag() for native types ('jint', etc.).
 */
struct object_tag { };
struct non_object_tag { };

template<class T>
struct ObjectTag {
	typedef non_object_tag tag;
};

template<> struct ObjectTag<jobject> {
	typedef object_tag tag;
};

/**
 * GetJResource class provides an interface for easy resource construction. 
 * 
 * The class has two template member functions, which can be invoked
 * the following three ways (the optional boolean parameter indicates
 * whether the requested resource is a static field of the Java class):
 *    GetJResource()(JNIEnv *, jobject, const char * [, bool])
 *    GetJResource()(JNIEnv *, jclass, const char * [, bool])
 *    GetJResource()(JNIEnv *, const char *, const char * [, bool])
 *
 * Calls to
 *    GetJResource()(JNIEnv *, jobject, const char *, false)
 *    GetJResource()(JNIEnv *, jobject, const char *)
 *  attempt to access a non-static field.
 * Calls to
 *    GetJResource()(JNIEnv *, jobject, const char *, true)
 *    GetJResource()(JNIEnv *, const char *, const char *)
 *    GetJResource()(JNIEnv *, const char *, const char *, true)
 *  attempt to access a static field.
 * Finally, a call to
 *    GetJResource()(JNIEnv *, const char *, const char *, false)
 * generates an exception.
 * 
 * Note, that since 'jclass' is a subclass of 'jobject',
 *    GetJResource()(JNIEnv *, jclass, const char *)
 * attempts to access the NON-STATIC field of 'jclass' 
 * as if it were a simple object.
 */
template<class JavaType>
class GetJResource {
	JavaType operator() (JNIEnv *env, jobject obj, const char *name,
			const object_tag &) const {
		return Field<JavaType>(env, obj, name);
	}

	JavaType operator() (JNIEnv *env, jobject obj, const char *name,
			const object_tag &, bool isStatic) const {
		if (isStatic)
			return StaticField<JavaType>(env, obj, name);
		else
			return Field<JavaType>(env, obj, name);
	}

	template<class T>
	JavaType operator() (JNIEnv *env, T protoClass, const char *name,
			const non_object_tag &) const {
		return StaticField<JavaType>(env, protoClass, name);
	}

	template<class T>
	JavaType operator() (JNIEnv *env, T protoClass, const char *name,
			const non_object_tag &, bool isStatic) const {
		if (!isStatic)
			throw Exception("Ambiguos attempt to construct a field");
		else
			return StaticField<JavaType>(env, protoClass, name);
	}

public:
	/*
	 * The following functions first detect the type of the second parameter
	 * through the call to ObjectTag<T>::tag(), and then invoke
	 * the corresponding resource allocation function.
	 */
	template<class T>
	JavaType operator() (JNIEnv *env, T arg, const char *name) const {
		return (*this)(env, arg, name, ObjectTag<T>::tag());
	}

	template<class T>
	JavaType operator() (JNIEnv *env, T arg, const char *name,
			bool isStatic) const {
		return (*this)(env, arg, name, ObjectTag<T>::tag(), isStatic);
	}
};

/**
 * Accessing String characters (Unicode)
 *
 * JResource type corresponds to 'jstring',
 * and Resource type corresponds to 'const jchar *'.
 * StringCharsSettings implements ResourceSettings, which can
 * serve a parameter to Resource template above.
 * Applications should use StringChars, which inherits from
 * Resource<StringCharsSettings>.
 */

struct StringCharsSettings {
	typedef jstring JResource;
	typedef const jchar *Resource;

	/**
	 * Obtaining string characters: GetF uses GetStringChars().
	 * To use the 'isCopy' parameter in GetStringChars(), pass it
	 * explicitly to GetF constructor.
	 */
	struct GetF {
		jboolean *_isCopy;
		GetF(jboolean *isCopy = 0) : _isCopy(isCopy) { }
		Resource operator() (JNIEnv *env, JResource jstr) const {
			return (jstr == 0) ? 0 : env->GetStringChars(jstr, _isCopy);
		}
	};

	/**
	 * Releasing string characters: ReleaseF uses ReleaseStringChars()
	 */
	struct ReleaseF {
		void operator() (JNIEnv *env, JResource jstr, Resource str) const {
			if (jstr != 0)
				env->ReleaseStringChars(jstr, str);
		}
	};
};

class StringChars : public Resource<StringCharsSettings> {
	typedef StringCharsSettings _settings;
	typedef Resource<_settings> _super;

public:
	StringChars() {}
	StringChars(JNIEnv *env, jstring jstr) : _super(env, jstr) {}
	StringChars(JNIEnv *env, jstring jstr, jboolean *isCopy) :
			_super(env, jstr, _settings::GetF(isCopy)) { }

	/**
	 * The following two constructors access the required Java
	 * resource field by calling GetJResource<jstring>(...),
	 * and call the regular resource constructor to build a handle
	 * to a C++ object (const jchar *).
	 */
	template<class T>
	StringChars(JNIEnv *env, T arg, const char *name) :
			_super(env, GetJResource<jstring>()(env, arg, name)) { }

	template<class T>
	StringChars(JNIEnv *env, T arg, const char *name, bool isStatic) :
			_super(env, GetJResource<jstring>()(env, arg, name, isStatic)) { }

	const jchar &operator[] (int i) const { return _resource[i]; }
	const int length() const { return _env->GetStringLength(_jresource); }

	operator const char *() const { return (const char *)_resource; }
};

/**
 * Accessing String characters (UTF-8)
 *
 * JResource type corresponds to 'jstring', 
 * and Resource type corresponds to 'const char *'.
 * StringUTFCharsSettings implements ResourceSettings, which can
 * serve a parameter to Resource template above.
 * Applications should use StringUTFChars, which inherits from
 * Resource<StringUTFCharsSettings> and provides asString() conversion
 * function (for convenient usage of C++ std::string instead of raw 'char *';
 * note that such conversion physically copies the characters).
 */

struct StringUTFCharsSettings {
	typedef jstring JResource;
	typedef const char *Resource;

	/**
	 * Obtaining string characters: GetF uses GetStringUTFChars()
	 * To use the 'isCopy' parameter in GetStringUTFChars(), pass it
	 * explicitly to GetF constructor.
	 */
	struct GetF {
		jboolean *_isCopy;
		GetF(jboolean *isCopy = 0) : _isCopy(isCopy) {}
		Resource operator() (JNIEnv *env, JResource jstr) const {
			return (jstr == 0) ? 0 : env->GetStringUTFChars(jstr, _isCopy);
		}
	};

	/**
	 * Releasing string characters: ReleaseF uses ReleaseStringUTFChars()
	 */
	struct ReleaseF {
		void operator() (JNIEnv *env, JResource jstr, Resource str) const {
			if (jstr != 0)
				env->ReleaseStringUTFChars(jstr, str);
		}
	};
};

class StringUTFChars : public Resource<StringUTFCharsSettings> {
	typedef StringUTFCharsSettings _settings;
	typedef Resource<_settings> _super;

public:
	StringUTFChars() { }
	StringUTFChars(JNIEnv *env, jstring jstr) : _super(env, jstr) { }
	StringUTFChars(JNIEnv *env, jstring jstr, jboolean *isCopy) :
			_super(env, jstr, _settings::GetF(isCopy)) { }

	/**
	 * The following two constructors access the required Java
	 * resource field by calling GetJResource<jstring>(...),
	 * and call the regular resource constructor to build a handle
	 * to a C++ object (const jchar *).
	 */
	template<class T>
	StringUTFChars(JNIEnv *env, T arg, const char *name) :
			_super(env, GetJResource<jstring>()(env, arg, name)) { }

	template<class T>
	StringUTFChars(JNIEnv *env, T arg, const char *name, bool isStatic) :
			_super(env, GetJResource<jstring>()(env, arg, name, isStatic)) { }

	const char &operator[] (int i) const { return _resource[i]; }
	const int length() const { return _env->GetStringUTFLength(_jresource); }

	std::string asString() const { return std::string(_resource); }
};

/**
 * Native arrays are exported from Java arrays, so that they can be used in C++.
 *
 * This code combines resource management with template and macro definitions,
 * to provide array definitions for all primitive types.
 *
 * The template requires two parameters: NativeType (array type) and
 * ArrayType (Java array type).
 * JResource type corresponds to ArrayType, and Resource type corresponds 
 * to a pointer to NativeType.
 * ArraySettings implements ResourceSettings,  which can serve
 * a parameter to Resource template above.
 * Applications should use Array, which inherits from
 * Resource<ArraySettings> and provides additional access functions.
 */

template<class NativeType>
struct ArraySettings {
	typedef typename ARRAY_TYPE_OF(NativeType) JResource;
	typedef NativeType *Resource;

	/**
	 * Attach to the array:
	 * GetF uses Get<PrimitiveType>ArrayElements (see below).
	 * To use the 'isCopy' parameter in Get<PrimitiveType>ArrayElements,
	 * pass it to GetF constructor.
	 */
	struct GetF {
		jboolean *_isCopy;
		GetF(jboolean *isCopy = 0) : _isCopy(isCopy) { }
		inline Resource operator() (JNIEnv *env, JResource array) const;
	};

	/**
	 * Detach from the array:
	 * ReleaseF uses Release<PrimitiveType>ArrayElements (see below).
	 * To use the 'mode' parameter in Release<PrimitiveType>ArrayElements,
	 * pass it to ReleaseF constructor.
	 */
	struct ReleaseF {
		jint _mode;
		ReleaseF(jint mode = 0) : _mode(mode) { }
		inline void operator() (JNIEnv *env, JResource array,
				Resource nativeArray) const;
	};
};

template<class NativeType>
class Array : public Resource<ArraySettings<NativeType> >
{
	typedef ArraySettings<NativeType> _settings;
	typedef Resource<_settings> _super;
	typedef typename ARRAY_TYPE_OF(NativeType) ArrayType;

public:
	Array() { }
	Array(JNIEnv *env, ArrayType array) : _super(env, array) { }
	Array(JNIEnv *env, ArrayType array, jboolean *isCopy) :
			_super(env, array, _settings::GetF(isCopy)) { }

	/**
	 * The following two constructors access the requested Java
	 * resource field by calling GetJResource<jstring>(...), and
	 * call the regular resource constructor to build a handle to
	 * a C++ object (NativeType *).
	 */
	template<class T>
	Array(JNIEnv *env, T arg, const char *name) :
			_super(env, GetJResource<ArrayType>()(env, arg, name)) { }

	template<class T>
	Array(JNIEnv *env, T arg, const char *name, bool isStatic) :
			_super(env, GetJResource<ArrayType>()(env, arg, name, isStatic)) { }

	void CustomRelease(int mode = 0) {
		ReleaseResource(_settings::ReleaseF(mode));
	}

	NativeType &operator[] (int i) { return this ->_resource[i]; }
	const NativeType &operator[] (int i) const { return this->_resource[i]; }

	const int size() const { return this->_env->GetArrayLength(this->_jresource); }
};

/**
 * The member functions of ArraySettings are defined as a macro block,
 * to facilitate their explicit instantiation for all primititve types.
 * The Type parameter of the templates serves as an index into
 * the lookup table to retrieve the type-specific definitions (NativeType
 * and ArrayType) for each instantiation.
 *
 * Example: the preprocessor output for the integer primitive type 
 * (corresponding to the 'jint' native type) is given in file 
 * 'jni_preprocessor.cpp'.
 */

#define ARRAY_SETTINGS(Type)															\
template<>																				\
inline NATIVE_TYPE(Type) *																\
ArraySettings<NATIVE_TYPE(Type)>::GetF::operator()	      				  				\
		(JNIEnv *env, ARRAY_TYPE(Type) array) const {									\
	return (array == 0) ? 0 : env->Get##Type##ArrayElements(array, _isCopy);			\
}																						\
																						\
template<>																				\
inline void																				\
ArraySettings<NATIVE_TYPE(Type)>::ReleaseF::operator()   				  				\
		(JNIEnv *env, ARRAY_TYPE(Type) array, NATIVE_TYPE(Type) *nativeArray) const {	\
	if (array != 0)                                                           			\
		env->Release##Type##ArrayElements(array, nativeArray, _mode);					\
}
   
/**
 * Combo instantiation of ArraySettings member functions for all primitive
 * types.
 */

INSTANTIATE_FOR_PRIMITIVE_TYPES(ARRAY_SETTINGS)
					
/**
 * Monitors
 *
 * JResource type corresponds to jobject.
 * Resource type is redundant, but for completeness is associated 'jobject'
 * too.
 * MonitorSettings implements ResourceSettings, which can
 * serve a parameter to Resource template above.
 * Applications should use Monitor, which inherits from
 * Resource<MonitorSettings>.
 */

struct MonitorSettings {
	typedef jobject JResource;
	typedef jobject Resource;

	/**
	 * Entering the monitor: GetF uses MonitorEnter()
	 */
	struct GetF {
		Resource operator() (JNIEnv *env, JResource obj) const {
			env->MonitorEnter(obj);
			return obj;
		}
	};

	/**
	 * Leaving the monitor: ReleaseF uses MonitorExit()
	 */
	struct ReleaseF {
		void operator() (JNIEnv *env, JResource obj, Resource dummy) const {
			env->MonitorExit(obj);
		}
	};
};

class Monitor : public Resource<MonitorSettings> {
public:
	Monitor() { }
	Monitor(JNIEnv *env, jobject obj) :
			Resource<MonitorSettings>(env, obj) { }
};

/**
 * Reservation of global references
 *
 * Both Resource and JResource types correspond to 'jobject'.
 * GlobalRefSettings implements ResourceSettings, which can
 * serve a parameter to Resource template above.
 * Applications should use GlobalRef, which inherits from
 * Resource<GlobalRefSettings> and provides equality comparison operator
 * (for comparing whether two proxies actually refer to the same object).
 */

template <class T>
struct GlobalRefSettings {
	typedef T JResource;
	typedef T Resource;

	/**
	 * Global reference acquisition: DefaultGetF uses NewGlobalRef
	 */
	struct GetF {
		Resource operator() (JNIEnv *env, JResource obj) const {
			return static_cast<JResource>(env->NewGlobalRef(obj));
		}
	};

	/**
	 * Releasing a global reference: DefaultReleaseF uses DeleteGlobalRef
	 */
	struct ReleaseF {
		void operator() (JNIEnv *env, JResource obj, Resource ref) const {
			env->DeleteGlobalRef(ref);
		}
	};
};

template<class T>
class GlobalRef : public Resource<GlobalRefSettings<T> > {
public:
	GlobalRef() {}
	GlobalRef(JNIEnv *env, T obj) :
			Resource<GlobalRefSettings<T> >(env, obj) { }

	/**
	 * Due to problems with Microsoft Visual C++ 6.0 compiler, the equality
	 * comparison operator is implemented as a member function.
	 * The version using
	 * friend bool operator== (const GlobalRef &x, const GlobalRef &y)
	 * passes compilation with g++, but fails for VC++.
	 */
	bool operator== (const GlobalRef<T> &x) {
		if (this->_env != x._env)
			return false;
		return (this->_env->IsSameObject(this->_resource, x._resource) != JNI_FALSE);
	}
};


} //namespace jni


#endif //ifndef _JNI_JNIRESOURCE_H_
