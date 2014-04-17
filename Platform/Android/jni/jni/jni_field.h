//
//  jni_field.h
//  ePub3
//
//  Created by Pedro Reis Colaco (txtr) on 2013-08-13.
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


#ifndef _JNI_JNIFIELD_H_
#define _JNI_JNIFIELD_H_


#include "jni_types.h"
#include "jni_class.h"


namespace jni {


/*
 * Utilities for accessing Java object variables (regular member variables
 * in C++ terminology) and class variables (static member variables in C++
 * terminology).
 * In contrast to the original JNI design, we treat field ids as active
 * objects. A field id can be constructed from the class directly, given an
 * object of this class or a class name. After the field id is constructed,
 * there is no need to use the explicit forms Get<PrimitiveType>Field and 
 * Set<PrimitiveType>Field, since all the type information was already
 * deduced at construction time.
 * Field and StaticField classes are implemented using
 * FieldId and StaticFieldId (respectively), and they constitute
 * a proxy for the corresponding Java object. This allows to treat them 
 * as a data field of the appropriate type (integer, float etc.).
 */

/**
 * GenericFieldId represents a structure common for both regular and static
 * field ids, and contains an environment handle and a 'jfieldID' object.
 */
class GenericFieldId {
protected:
	JNIEnv *_env;  		// Environment handle for subsequent field manipulation
	jfieldID _id;  		// Field ID
	std::string _name;	// Field name

	/**
	 * Default empty constructor
	 */
	GenericFieldId() : _env(NULL), _id(0), _name("") { }

	/**
	 * Constructor with a jfieldID
	 */
	GenericFieldId(JNIEnv *env, jfieldID id, std::string name) : _env(env), _id(id), _name(name) {
		if (!IsEmpty()) {
			LOGD("GenericFieldId(): found an id for field '%s'", _name.c_str());
		} else {
			LOGE("GenericFieldId(): couldn't find an id for field '%s'", _name.c_str());
		}
	}

public:
	/**
	 * Checks if this field id is empty
	 */
	bool IsEmpty() const { return _id == 0; }

	/**
	 * Cast to std::string
	 */
	operator std::string() const {
		std::string res;
		res = _name;
		return res;
	}
};

/**
 * FieldId class implements four ways to construct a field
 * using a class, an object of the class, a class name, or a Class object.
 * The template is suitable for any JNI type, and is subsequently specialized
 * (preinstantiated) for the primitive types.
 */
template<class JavaType>
class FieldId : public GenericFieldId {
public:
	/**
	 * Default empty constructor
	 */
	FieldId() : GenericFieldId() { }

	/**
	 * FieldId constructor: given a 'protoClass' (i.e., 'jclass', 'jobject'
	 * or 'const char *'), obtain the corresponding field id by calling
	 * GetFieldID, then pass it to the base class constructor along with
	 * the environment handle. If the signature declaration for the requested
	 * type does not exist, the construction process fails.
	 */
	template<class T>
	FieldId(JNIEnv *env, T protoClass, const char *name,
			const char *sig = SIGNATURE_OF(JavaType)) :
			GenericFieldId(env, env->GetFieldID(Class(env, protoClass), name, sig), name)
	{ }

	/**
	 * Get and Set utilities
	 */
	JavaType Get(jobject obj) const {
		return static_cast<JavaType>(_env->GetObjectField(obj, _id));
	}
	void Set(jobject obj, JavaType val) {
		_env->SetObjectField(obj, _id, val);
	}
};

/**
 * The specialization of FieldId template is defined as a macro block,
 * to facilitate its explicit instantiation for the primitive types.
 * The 'Type' parameter of the template serves an index into the lookup table
 * to retrieve the type-specific declarations.
 */

#define FIELD_ID_METHODS(Type)												\
template<> class FieldId<NATIVE_TYPE(Type)> : public GenericFieldId 		\
{																			\
public:																		\
	FieldId() : GenericFieldId() { }										\
																			\
	template<class T>														\
	FieldId(JNIEnv *env, T protoClass, const char *name) :					\
			GenericFieldId(env, env->GetFieldID(Class(env, protoClass),		\
					name, SIGNATURE(Type)), name) { }						\
																			\
	NATIVE_TYPE(Type) Get(jobject obj) const {								\
		return _env->Get##Type##Field(obj, _id);							\
	}																		\
	void Set(jobject obj, NATIVE_TYPE(Type) val) {							\
		_env->Set##Type##Field(obj, _id, val);								\
	}																		\
};

/**
 * Combo instantiation of FieldId specialization for all primitive types
 */

INSTANTIATE_FOR_PRIMITIVE_TYPES(FIELD_ID_METHODS)

/**
 * StaticFieldId template is identical to the FieldId template,
 * except for the call to GetStaticFieldID instead of GetFieldId.
 */
template<class JavaType>
class StaticFieldId : public GenericFieldId {
public:
	/**
	 * Default empty constructor
	 */
	StaticFieldId() : GenericFieldId() { }

   /**
    * StaticFieldId constructor: given a 'protoClass' (i.e., 'jclass',
    * 'jobject' or 'const char *'), obtain the corresponding static field id
    * by calling GetStaticFieldID, then pass it to the base class constructor
    * along with the environment handle. If the signature declaration for the
    * requested type does not exist, the construction process fails.
    */
	template<class T>
	StaticFieldId(JNIEnv *env, T protoClass, const char *name,
			const char *sig = SIGNATURE_OF(JavaType)) :
			GenericFieldId(env, env->GetStaticFieldID(Class(env, protoClass),
					name, sig), name) { }

	/**
	 * Get and Set utilities
	 */
	JavaType Get(jclass clazz) const {
		return static_cast<JavaType>(_env->GetStaticObjectField(clazz, _id));
	}
	void Set(jclass clazz, JavaType val) {
		_env->SetStaticObjectField(clazz, _id, val);
	}
};

/**
 * Similarly to FieldId, the specialization of StaticFieldId template
 * is defined as a macro block. The Type parameter serves an index into
 * the lookup table to retrieve the type-specific declarations.
 *
 * Preprocessor output for the integer primitive type (corresponding to
 * the 'jint' native type) is given in file 'jni_preprocessor.cpp'.
 */

#define STATIC_FIELD_ID_METHODS(Type)										\
template<>																	\
class StaticFieldId<NATIVE_TYPE(Type)> : public GenericFieldId				\
{																			\
public:																		\
	StaticFieldId() : GenericFieldId() { }									\
																			\
	template<class T>														\
	StaticFieldId(JNIEnv *env, T protoClass, const char *name) :			\
			GenericFieldId(env,												\
					env->GetStaticFieldID(Class(env, protoClass),			\
					name, SIGNATURE(Type)), name) { }						\
																			\
	NATIVE_TYPE(Type) Get(jclass clazz) const {								\
		return _env->GetStatic##Type##Field(clazz, _id);					\
	}																		\
	void Set(jclass clazz, NATIVE_TYPE(Type) val) {							\
		_env->SetStatic##Type##Field(clazz, _id, val);						\
	}																		\
};

/**
 * Combo instantiation of StaticFieldId specialization for all primitive
 * types
 */

INSTANTIATE_FOR_PRIMITIVE_TYPES(STATIC_FIELD_ID_METHODS)

/**
 * Field is a template parameterized with a native type ('jint',
 * 'jchar' etc.) It has two members: a FieldId and an object itself.
 * After a Field object is constructed, it behaves as a proxy to the
 * corresponding actual Java field.
 */
template<class NativeType>
class Field {
	typedef Field<NativeType> _self;
   
private:
	jobject _obj;				// The Java object that hosts the field
	FieldId<NativeType> _id;	// field id

public:
	/**
	 * Default empty constructor
	 */
	Field() : _obj(NULL), _id() { }

	/**
	 * Construct a field given a field id and an object.
	 */
	Field(FieldId<NativeType> id, jobject obj) : _obj(obj), _id(id) { }
	  
	/**
	 * Construct a field given an object, field name and signature.
	 */
	Field(JNIEnv *env, jobject obj, const char *name, const char *sig) :
			_obj(obj), _id(env, obj, name, sig) { }

	/**
	 * Construct a field given an object and field name (this constructor
	 * is appropriate for classes that have a TypeDeclarations structure).
	 */
	Field(JNIEnv *env, jobject obj, const char *name) :
			_obj(obj), _id(env, obj, name) { }

protected:
	/**
	 * Check if empty and throw.
	 */
	void throwIfEmpty() const {
		if(_id.IsEmpty()) {
			throw Exception("Using empty or invalid Field. Check logcat for details...");
		}
	}

public:

	/**
	 * Assignment operator
	 */
	_self &operator= (const _self &rhs) {
		throwIfEmpty();
		if (this != &rhs)
			_id.Set(_obj, rhs.Get(rhs._obj));
		return *this;
	}

	/**
	 * Assignment operator that receives a native type object
	 * (note that the operator returns a Field<NativeType> object).
	 * This operator is necessary, because NativeType cannot be simply
	 * converted to Field.
	 */
	_self &operator= (const NativeType &rhs) {
		throwIfEmpty();
		_id.Set(_obj, rhs);
		return *this;
	}

	/**
	 * Casting to NativeType
	 */
	operator NativeType() const {
		throwIfEmpty();
		return _id.Get(_obj);
	}
};

/**
 * StaticField is similar to Field.
 * It has two members: FieldId and (unlike Field) a Java class object.
 * After a StaticField object is constructed, it behaves as a proxy to the
 * corresponding actual Java field.
 */
template<class NativeType>
class StaticField {
	typedef StaticField<NativeType> _self;
   
private:
	jclass _clazz;					// The Java class that hosts the field
	StaticFieldId<NativeType> _id;	// Field id

public:
	/**
	 * Default empty constructor
	 */
	StaticField() : _clazz(NULL), _id() { }

	/**
	 * Construct a field given a field id and a class
	 */
	StaticField(StaticField<NativeType> &id, jclass clazz) :
			_clazz(clazz), _id(id) { }
	  
	/**
	 * Construct a field given some object from which a class can be
	 * constructed, a field name and signature.
	 */
	template<class T>
	StaticField(JNIEnv *env, T protoClass, const char *name,
			const char *sig) :
			_clazz(Class(env, protoClass)),
			_id(env, Class(env, protoClass), name, sig) { }

	/**
	 * Construct a field given some object from which a class can be
	 * constructed, a field name and signature (this constructor is
	 * appropriate for classes that have a TypeDeclarations structure).
	 */
	template<class T>
	StaticField(JNIEnv *env, T protoClass, const char *name) :
			_clazz(Class(env, protoClass)),
			_id(env, Class(env, protoClass), name) { }

protected:
	/**
	 * Check if empty and throw.
	 */
	void throwIfEmpty() const {
		if(_id.IsEmpty()) {
			throw Exception("Using empty or invalid StaticField. Check logcat for details...");
		}
	}

public:

	/**
	 * Assignment operator
	 */
	_self &operator= (const _self &rhs) {
		throwIfEmpty();
		if (this != &rhs)
			_id.Set(_clazz, rhs.Get(rhs._clazz));
		return *this;
	}

	/**
	 * Assignment operator that receives a native type object
	 * (note that the operator returns a Field<NativeType> object).
	 */
	_self &operator= (const NativeType &rhs) {
		throwIfEmpty();
		_id.Set(_clazz, rhs);
		return *this;
	}

	/**
	 * Casting to NativeType
	 */
	operator NativeType() const {
		throwIfEmpty();
		return _id.Get(_clazz);
	}
};


} //namespace jni


#endif //ifndef _JNI_JNIFIELD_H_
