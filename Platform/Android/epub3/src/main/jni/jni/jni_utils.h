//
//  jni_utils.h
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
