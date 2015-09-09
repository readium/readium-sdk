//
//  manifest_item.h
//  ePub3
//
//  Created by Yonathan Teitelbaum (Mantano) on 2015-09-08.
//  Copyright (c) 2015 Readium Foundation and/or its licensees. All rights reserved.
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


#include <jni.h>


#ifndef _MANIFEST_ITEM_JNI_H_
#define _MANIFEST_ITEM_JNI_H_


#ifdef __cplusplus
extern "C" {
#endif

/*
 * Class:     org_readium_sdk_android_ManifestItem
 * Method:    nativeHasProperty
 * Signature: (I)Z;
 */
JNIEXPORT jboolean JNICALL Java_org_readium_sdk_android_ManifestItem_nativeHasProperty(JNIEnv* env, jobject thiz, jlong manifestItemPtr, jint flag);

#ifdef __cplusplus
}
#endif

#endif //ifndef _MANIFEST_ITEM_JNI_H_
