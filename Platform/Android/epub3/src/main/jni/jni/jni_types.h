//
//  jni_types.h
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


#ifndef _JNI_JNITYPES_H_
#define _JNI_JNITYPES_H_


#include <string>

#include <jni.h>

#include "jni_exception.h"


namespace jni {


/*
 * Lookup table for primitive types.
 * Each primitive type (PrimitiveType) has a block of declarations 
 * entitled <PrimitiveType>Declarations, which has four constituents:
 * 	- NativeType:       Java type corresponding to PrimitiveType
 *  - ArrayType:        Java array type corresponding to PrimitiveType
 *	- signature:        type signature (a string constant corresponding
 *                      to PrimitiveType)
 *	- array_signature:  array type signature (a string constant
 *				        corresponding to ArrayType)
 */

struct BooleanDeclarations {
	typedef jboolean NativeType;
	typedef jbooleanArray ArrayType;
	static const char *signature() { return "Z"; }
	static const char *array_signature() { return "[Z"; }
};

struct ByteDeclarations {
	typedef jbyte NativeType;
	typedef jbyteArray ArrayType;
	static const char *signature() { return "B"; }
	static const char *array_signature() { return "[B"; }
};

struct CharDeclarations {
	typedef jchar NativeType;
	typedef jcharArray ArrayType;
	static const char *signature() { return "C"; }
	static const char *array_signature() { return "[C"; }
};

struct ShortDeclarations {
	typedef jshort NativeType;
	typedef jshortArray ArrayType;
	static const char *signature() { return "S"; }
	static const char *array_signature() { return "[S"; }
};

struct IntDeclarations {
	typedef jint NativeType;
	typedef jintArray ArrayType;
	static const char *signature() { return "I"; }
	static const char *array_signature() { return "[I"; }
};

struct LongDeclarations {
	typedef jlong NativeType;
	typedef jlongArray ArrayType;
	static const char *signature() { return "J"; }
	static const char *array_signature() { return "[J"; }
};

struct FloatDeclarations {
	typedef jfloat NativeType;
	typedef jfloatArray ArrayType;
	static const char *signature() { return "F"; }
	static const char *array_signature() { return "[F"; }
};

struct DoubleDeclarations {
	typedef jdouble NativeType;
	typedef jdoubleArray ArrayType;
	static const char *signature() { return "D"; }
	static const char *array_signature() { return "[D"; }
};

/*
 * Macros for mapping a given PrimitiveType into
 * the corresponding NativeType, ArrayType, and signatures.
 * For example,
 *   NATIVE_TYPE(Boolean) = BooleanDeclarations::NativeType (= jboolean)
 */

#define NATIVE_TYPE(Type) 		Type##Declarations::NativeType
#define ARRAY_TYPE(Type)  		Type##Declarations::ArrayType
#define SIGNATURE(Type)   		Type##Declarations::signature()
#define ARRAY_SIGNATURE(Type)   Type##Declarations::array_signature()

/*
 * Macros for explicit instantiation of templates (realized as macros)
 * for all primitive types
 */

#define INSTANTIATE_FOR_PRIMITIVE_TYPES(BLOCK_MACRO)	\
BLOCK_MACRO(Boolean)									\
BLOCK_MACRO(Byte)										\
BLOCK_MACRO(Char)										\
BLOCK_MACRO(Short)										\
BLOCK_MACRO(Int)										\
BLOCK_MACRO(Long)										\
BLOCK_MACRO(Float)										\
BLOCK_MACRO(Double)

/*
 * Lookup table for JNI native types and native array types.
 * For example, given the 'jint' type, it should be possible to determine
 * that its signature is "I", and the corresponding array type is 'jintArray'.
 * Similarly, for 'jintArray' it should be possible to determine that its 
 * signature is "[I", and the corresponding native type is 'jint'.
 *
 * This feature is implemented using the template specialization technique.
 * The default implementation of 'TypeDeclarations' structure is empty.
 * Specialization for native types is performed using the preprocessor.
 *
 * Here is a sample preprocessor output for the 'jint' type:
 *
 * template<> struct TypeDeclarations<IntDeclarations::NativeType>
 * {
 *    typedef IntDeclarations Declarations;
 *    typedef IntDeclarations::NativeType NativeType;
 *    typedef IntDeclarations::ArrayType  ArrayType;
 *    static const char *signature() { return IntDeclarations::signature(); }
 * }; 	 
 *
 * The corresponding structure for 'jintArray' only differs in the signature 
 * function, which yields IntDeclarations::array_signature().
 *
 * A bug in Microsoft Visual C++ 6.0 prohibits nested declarations of 
 * the form TypeDeclarations<NativeType>::Declarations::ArrayType,
 * otherwise NativeType and ArrayType fields could be omitted.
 *
 */

template<class JavaType>
struct TypeDeclarations {};

#define TYPE_DECLARATIONS(Type)												\
template<> struct TypeDeclarations<NATIVE_TYPE(Type)> {						\
	typedef Type##Declarations Declarations;								\
	typedef NATIVE_TYPE(Type) NativeType;									\
	typedef ARRAY_TYPE(Type) ArrayType;										\
	static const char *signature() { return SIGNATURE(Type); }				\
};

#define ARRAY_DECLARATIONS(Type)											\
template<> struct TypeDeclarations<ARRAY_TYPE(Type)> {						\
	typedef Type##Declarations Declarations;								\
	typedef NATIVE_TYPE(Type) NativeType;									\
	typedef ARRAY_TYPE(Type) ArrayType;										\
	static const char *signature() { return ARRAY_SIGNATURE(Type); }		\
};

/*
 * Combo instantiation of TypeDeclarations specializations for all
 * primitive types
 */

INSTANTIATE_FOR_PRIMITIVE_TYPES(TYPE_DECLARATIONS)
INSTANTIATE_FOR_PRIMITIVE_TYPES(ARRAY_DECLARATIONS)

/*
 * In order to be able to provide default parameters like
 * SIGNATURE_OF(JavaType) = TypeDeclarations<JavaType>::signature()
 * (for example, see the constructor of GenericField),
 * we need to declare the corresponding structures for 'jobject' type. 
 * These declarations will cause a run-time error if 'signature()' or 
 * 'array_signature()' of 'jobject' type are actually invoked.
 */

struct ObjectDeclarations {
	typedef jobject NativeType;
	typedef jobjectArray ArrayType;
	static const char *signature() {
		throw Exception("No signature available for jobject");
	}
	static const char *array_signature() {
		throw Exception("No signature available for jobjectArray");
	}
};

TYPE_DECLARATIONS(Object)

/*
 * Declaration of Void type.
 * This is needed by Method<jvoid> for methods returning void.
 */

typedef void jvoid;

struct VoidDeclarations {
	typedef jvoid NativeType;
	typedef jvoid ArrayType;
	static const char *signature() { return "V"; }
	static const char *array_signature() { return "V"; }
};

TYPE_DECLARATIONS(Void)

/*
 * Macros for mapping any JNI type (jint, jintArray, jobject, etc.)
 * to the corresponding Declarations structure, 
 * NativeType, ArrayType, and signatures.
 * For example,
 *   SIGNATURE_OF(jintArray) = TypeDeclarations<jintArray>::signature() =
 *                             IntDeclarations::array_signature() = "[I"
 * If a type TypeDeclarations<jintArray> is not declared, a compilation
 * error will occur.
 */

#define DECLARATIONS_OF(JavaType)								\
TypeDeclarations<JavaType>::Declarations

#define NATIVE_TYPE_OF(JavaType)								\
TypeDeclarations<JavaType>::NativeType

#define ARRAY_TYPE_OF(JavaType)									\
TypeDeclarations<JavaType>::ArrayType

#define SIGNATURE_OF(JavaType)									\
TypeDeclarations<JavaType>::signature()

/*
 * Lookup tables for other JNI types may also be declared, 
 * here is an example for 'jstring'.
 */

struct StringDeclarations {
	typedef jstring NativeType;
	typedef jobject ArrayType;
	static const char *signature() { return "Ljava/lang/String;"; }
	static const char *array_signature() { return "[Ljava/lang/String;"; }
};

TYPE_DECLARATIONS(String)


} //namespace jni


#endif //ifndef _JNI_JNITYPES_H_
