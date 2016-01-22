//
//  jni_utils.h
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


#ifndef _JNI_JNIUTILS_H_
#define _JNI_JNIUTILS_H_


#include "jni_types.h"


namespace jni {


/*
 * Here we define templates which facilitate using functions
 * Get<PrimitiveType>ArrayRegion and Set<PrimitiveType>ArrayRegion,
 * so that there is no need to explicitly specify the type parameter.
 * The actual type is inferred from the corresponding template parameter.
 */

/*
 * Template utilities for Get/Set<PrimitiveType>ArrayRegion
 */

template<class NativeType, class ArrayType>
inline void GetArrayRegion(JNIEnv *env, ArrayType array, jsize start,
						   jsize len, NativeType *buf);
template<class NativeType, class ArrayType>
inline void SetArrayRegion(JNIEnv *env, ArrayType array, jsize start,
						   jsize len, NativeType *buf);

/*
 * The implementation of Get/SetArrayRegion functions is given as
 * a macro block, to facilitate their explicit instantiation for
 * all primitive types.
 * Both template functions use the type parameter as an entry into the
 * lookup table, to retrieve NativeType, ArrayType and type signature.
 */

#define ARRAY_REGION_ROUTINES(Type)											 \
inline void GetArrayRegion(JNIEnv *env, ARRAY_TYPE(Type) array, jsize start, \
					jsize len, NATIVE_TYPE(Type) *buf) {					 \
    env->Get##Type##ArrayRegion(array, start, len, buf);					 \
}																			 \
																			 \
inline void SetArrayRegion(JNIEnv *env, ARRAY_TYPE(Type) array, jsize start, \
					jsize len, NATIVE_TYPE(Type) *buf) {					 \
    env->Set##Type##ArrayRegion(array, start, len, buf);					 \
}

/*
 * Explicit instantiation of the above definitions for all primitive types
 */

INSTANTIATE_FOR_PRIMITIVE_TYPES(ARRAY_REGION_ROUTINES)


} //namespace jni


#endif //ifndef _JNI_JNIUTILS_H_
