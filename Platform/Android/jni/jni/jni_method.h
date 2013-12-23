//
//  jni_method.h
//  ePub3
//
//  Created by Pedro Reis Colaco (txtr) on 2013-08-20.
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


#ifndef _JNI_JNIMETHOD_H_
#define _JNI_JNIMETHOD_H_


#include <cstdarg>
#include <string>

#include "jni_types.h"
#include "jni_class.h"


namespace jni {


/*
 * Utilities for accessing Java class methods (class member functions
 * in C++ terminology).
 * In contrast to the original JNI design, we treat method ids as active
 * objects. A method id can be constructed from the class directly, given an
 * object of this class or a class name. After the method id is constructed,
 * there is no need to use the explicit forms Call<PrimitiveType>Method or
 * CallStatic<PrimitiveType>Method, since all the type information was already
 * deduced at construction time.
 * Method and StaticMethod classes are implemented using
 * MethodId and StaticMethodId (respectively), and they constitute
 * a proxy for the corresponding Java object. This allows to treat the result
 * as a function of the appropriate type (integer, float etc.).
 */

/**
 * GenericMethodId represents a structure common for both regular and static
 * method ids, and contains an environment handle and a 'jmethodID' object.
 */
class GenericMethodId {
protected:
	jmethodID _id;  	// Method ID
	std::string _name;	// Method name
	std::string _sig;	// Method signature

	/**
	 * Default empty constructor
	 */
	GenericMethodId() : _id(0), _name(""), _sig("") { }

	/**
	 * Constructor with a jmethodID
	 */
	GenericMethodId(jmethodID id, std::string name, std::string sig) : _id(id), _name(name), _sig(sig) {
		if(!IsEmpty()) {
			LOGD("GenericMethodId(): found an id for method '%s'", ((std::string)*this).c_str());
		} else {
			LOGE("GenericMethodId(): couldn't find an id for method '%s'", ((std::string)*this).c_str());
		}
	}

public:
	/**
	 * Checks if this method id is empty
	 */
	bool IsEmpty() const { return _id == 0; }

	/**
	 * Cast to std::string
	 */
	operator std::string() const {
		std::string res;
		res = _name + "[" + _sig + "]";
		return res;
	}
};

/**
 * MethodId class implements four ways to construct a method
 * using a class, an object of the class, a class name, or a Class object.
 * The template is suitable for any JNI type, and is subsequently specialized
 * (preinstantiated) for the primitive types and void.
 */
template<class JavaType>
class MethodId : public GenericMethodId {
public:
	/**
	 * Default empty constructor
	 */
	MethodId() : GenericMethodId() { }

	/**
	 * MethodId constructor: given a 'protoClass' (i.e., 'jclass', 'jobject'
	 * or 'const char *'), obtain the corresponding method id by calling
	 * GetMethodID, then pass it to the base class constructor. If the
	 * signature declaration for the requested method does not exist, the
	 * construction process fails.
	 */
	template<class T>
	MethodId(JNIEnv *env, T protoClass, const char *name, const char *sig) :
			GenericMethodId(env->GetMethodID(Class(env, protoClass), name, sig), name, sig)
	{ }

	/**
	 * Method call utility
	 */
	JavaType Call(JNIEnv *env, jobject obj, va_list args) const {
        return static_cast<JavaType>(env->CallObjectMethodV(obj, _id, args));
	}
};

/**
 * The specialization of MethodId template is defined as a macro block,
 * to facilitate its explicit instantiation for the primitive types.
 * The 'Type' parameter of the template serves an index into the lookup table
 * to retrieve the type-specific declarations.
 */

#define METHOD_ID_METHODS(Type)													\
template<> class MethodId<NATIVE_TYPE(Type)> : public GenericMethodId 			\
{																				\
public:																			\
	template<class T>															\
	MethodId(JNIEnv *env, T protoClass, const char *name, const char *sig) :	\
			GenericMethodId(env->GetMethodID(Class(env, protoClass),			\
					name, sig), name, sig) { }									\
																				\
	NATIVE_TYPE(Type) Call(JNIEnv *env, jobject obj, va_list args) const {		\
		return env->Call##Type##MethodV(obj, _id, args);						\
	}																			\
};

/**
 * Combo instantiation of MethodId specialization for all primitive types
 */

INSTANTIATE_FOR_PRIMITIVE_TYPES(METHOD_ID_METHODS)

/**
 * MethodId specialization for Void
 */

template<> class MethodId<jvoid> : public GenericMethodId
{
public:
	template<class T>
	MethodId(JNIEnv *env, T protoClass, const char *name, const char *sig) :
			GenericMethodId(env->GetMethodID(Class(env, protoClass), name, sig), name, sig)
	{ }

	void Call(JNIEnv *env, jobject obj, va_list args) const {
		env->CallVoidMethodV(obj, _id, args);
	}
};

/**
 * StaticMethodId template is identical to the MethodId template,
 * except for the call to GetStaticMethodID and CallStaticObjectMethodV.
 */
template<class JavaType>
class StaticMethodId : public GenericMethodId {
protected:
	jclass _clazz;

public:
	/**
	 * Default empty constructor
	 */
	StaticMethodId() : GenericMethodId(), _clazz(NULL) { }

	/**
	 * StaticMethodId constructor: given a 'protoClass' (i.e., 'jclass',
	 * 'jobject' or 'const char *'), obtain the corresponding static method id
	 * by calling GetStaticMethodID, then pass it to the base class constructor.
	 * If the signature declaration for the requested method does not exist, the
	 * construction process fails.
	 */
	template<class T>
	StaticMethodId(JNIEnv *env, T protoClass, const char *name, const char *sig) :
			GenericMethodId(env->GetStaticMethodID(Class(env, protoClass), name, sig), name, sig),
			_clazz(Class(env, protoClass))
	{ }

	/**
	 * Method call utility
	 */
	JavaType Call(JNIEnv *env, va_list args) const {
        return static_cast<JavaType>(env->CallStaticObjectMethodV(_clazz, _id, args));
	}
};

/**
 * Similarly to MethodId, the specialization of StaticMethodId template
 * is defined as a macro block. The Type parameter serves an index into
 * the lookup table to retrieve the type-specific declarations.
 *
 * Preprocessor output for the integer primitive type (corresponding to
 * the 'jint' native type) is given in file 'jni_preprocessor.cpp'.
 */

#define STATIC_METHOD_ID_METHODS(Type)													\
template<>																				\
class StaticMethodId<NATIVE_TYPE(Type)> : public GenericMethodId						\
{																						\
protected:																				\
	jclass _clazz;																		\
																						\
public:																					\
	template<class T>																	\
	StaticMethodId(JNIEnv *env, T protoClass, const char *name, const char *sig) :		\
			GenericMethodId(env->GetStaticMethodID(Class(env, protoClass),				\
					name, sig), name, sig), _clazz(Class(env, protoClass)) { }			\
																						\
	NATIVE_TYPE(Type) Call(JNIEnv *env, va_list args) const {							\
		return env->CallStatic##Type##MethodV(_clazz, _id, args);						\
	}																					\
};

/**
 * Combo instantiation of StaticMethodId specialization for all primitive
 * types
 */

INSTANTIATE_FOR_PRIMITIVE_TYPES(STATIC_METHOD_ID_METHODS)

/**
 * StaticMethodId specialization for void
 */

template<> class StaticMethodId<jvoid> : public GenericMethodId
{
protected:
	jclass _clazz;

public:
	template<class T>
	StaticMethodId(JNIEnv *env, T protoClass, const char *name, const char *sig) :
			GenericMethodId(env->GetMethodID(Class(env, protoClass), name, sig), name, sig),
			_clazz(Class(env, protoClass))
	{ }

	void Call(JNIEnv *env, va_list args) const {
		env->CallStaticVoidMethodV(_clazz, _id, args);
	}
};

/**
 * Method is a template parameterized with a native type ('jint',
 * 'jchar' etc.) It has a MethodId member.
 * After a Method object is constructed, it behaves as a proxy to the
 * corresponding actual Java method through a functor.
 */
template<class NativeType>
class Method {
private:
	MethodId<NativeType> _id;	// Method id

public:
	/**
	 * Default empty constructor
	 */
	Method() : _id() { }

	/**
	 * Construct a method given a method id.
	 */
	Method(MethodId<NativeType> id) : _id(id) { }
	  
	/**
	 * Construct a method given some object from which a class can be
	 * constructed, a method name and signature.
	 */
	template<class T>
	Method(JNIEnv *env, T protoClass, const char *name, const char *sig) :
			_id(env, Class(env, protoClass), name, sig) { }

	/**
	 * Call a method.
	 */
	NativeType operator ()(JNIEnv *env, jobject obj, ...) {
		va_list args;
		NativeType result;

		if (obj != NULL) {
			if(!_id.IsEmpty()) {
				// Log call
				std::string method(_id);
				LOGD("Method(): calling method '%s'", method.c_str());
				// Call
				va_start(args, obj);
				result = _id.Call(env, obj, args);
				va_end(args);
				// Check exceptions
				if(env->ExceptionCheck()) {
					LOGW("Method(): unhandled exception from method '%s' will be ignored. Description follows:", method.c_str());
					env->ExceptionDescribe();
					env->ExceptionClear();
				}
			} else {
				throw Exception("Cannot call an empty Method, maybe couldn't find the method id? Check logcat for details...");
			}
		} else {
			throw Exception("Call to method with an invalid object. Check logcat for details...");
		}

		return result;
	}
};

/**
 * StaticMethod is similar to Method.
 * It has a StaticMethodId member.
 * After a StaticMethod object is constructed, it behaves as a proxy to the
 * corresponding actual Java method through a functor.
 */
template<class NativeType>
class StaticMethod {

private:
	StaticMethodId<NativeType> _id;		// Static method id

public:
	/**
	 * Default empty constructor
	 */
	StaticMethod() : _id() { }

	/**
	 * Construct a static method given a static method id
	 */
	StaticMethod(StaticMethod<NativeType> &id) : _id(id) { }

	/**
	 * Construct a static method given some object from which a class can be
	 * constructed, a method name and signature.
	 */
	template<class T>
	StaticMethod(JNIEnv *env, T protoClass, const char *name, const char *sig) :
			_id(env, Class(env, protoClass), name, sig) { }

	/**
	 * Call a static method.
	 */
	NativeType operator ()(JNIEnv *env, ...) {
		va_list args;
		NativeType result;

		if (!_id.IsEmpty()) {
			// Log call
			std::string method(_id);
			LOGD("StaticMethod(): calling static method '%s'", method.c_str());
			// Call
			va_start(args, env);
			result = _id.Call(env, args);
			va_end(args);
			// Check exceptions
			if(env->ExceptionCheck()) {
				LOGW("Method(): unhandled exception from static method '%s' will be ignored. Description follows:", method.c_str());
				env->ExceptionDescribe();
				env->ExceptionClear();
			}
		} else {
			throw Exception("Cannot call an empty StaticMethod, maybe couldn't find the method id? Check logcat for details...");
		}

		return result;
	}
};


} //namespace jni


#endif //ifndef _JNI_JNIMETHOD_H_
