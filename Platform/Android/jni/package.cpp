//
//  package.cpp
//  ePub3
//
//  Created by Pedro Reis Colaco (txtr) on 2013-07-02.
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


#include <string>
#include <vector>
#include <typeinfo>
#include <algorithm>    // std::min

#include <ePub3/archive.h>
#include <ePub3/container.h>
#include <ePub3/nav_element.h>
#include <ePub3/nav_point.h>
#include <ePub3/nav_table.h>
#include <ePub3/property.h>
#include <ePub3/zip_archive.h>
#include <ePub3/utilities/byte_stream.h>

#include "jni/jni.h"

#include "epub3.h"
#include "helpers.h"
#include "package.h"
#include "resource_stream.h"


using namespace std;


#ifdef __cplusplus
extern "C" {
#endif


//TODO: change these to something more readable like EPUB3_PACKAGE(nativePtr)
//#define pckg    (*((shared_ptr<ePub3::Package>*)pckgPtr))
#define PCKG(pckgPtr)    (static_pointer_cast<ePub3::Package>(jni::Pointer(pckgPtr).getPtr()))
//#define contnr    (*((shared_ptr<ePub3::Container>*)contnrPtr))
#define contnr    (static_pointer_cast<ePub3::Container>(jni::Pointer(contnrPtr).getPtr()))


/*
 * Internal constants
 **************************************************/

static const char *javaPackageClassName = "org/readium/sdk/android/Package";

static const char *javaPackage_createPackageMethodName = "createPackage";
static const char *javaPackage_createPackageSignature = "(J)Lorg/readium/sdk/android/Package;";
static const int BUFFER_SIZE = 8192;


/*
 * Internal variables
 **************************************************/

/*
 * Cached methods and fields IDs.
 */

static jclass javaPackageClass = NULL;

static jmethodID createPackage_ID;


/*
 * Exported functions
 **************************************************/

/**
 * Initialize the cached java elements for Container class
 */
int onLoad_cacheJavaElements_package(JNIEnv *env) {
	// Cache Container class
	INIT_CLASS_RETVAL(javaPackageClass, javaPackageClassName, ONLOAD_ERROR);
	// Cache Container class methods
	INIT_STATIC_METHOD_ID_RETVAL(createPackage_ID, javaPackageClass, javaPackageClassName,
			javaPackage_createPackageMethodName, javaPackage_createPackageSignature, ONLOAD_ERROR);

	// Return JNI_VERSION for OK, if not one of the lines above already returned ONLOAD_ERROR
	return JNI_VERSION;
}

/**
 * Calls the java createPackage method of Package class
 */
jobject javaPackage_createPackage(JNIEnv *env, jlong nativePtr) {
	jobject container = env->CallStaticObjectMethod(javaPackageClass,
			createPackage_ID, nativePtr);

	return container;
}


/*
 * Internal functions
 **************************************************/

static char* getProperty(ePub3::Package* package, char* name, char* pref, ePub3::PropertyHolder* forObject)
{
	LOGD("getProperty(): called for name='%s' pref='%s'", name, pref);
    auto propertyName = ePub3::string(name);
    auto prefix = ePub3::string(pref);
    auto iri = package->MakePropertyIRI(propertyName, prefix);

    auto propertyList = forObject->PropertiesMatching(iri);

    if (propertyList.size() > 0) {
        auto prop = propertyList[0];
    	LOGD("getProperty(): returning '%s'", prop->Value().c_str());
        return (char *) prop->Value().c_str();
    }
	LOGD("getProperty(): returning EMPTY");
    return (char *) "";
}

static void loadChildren(JNIEnv* env, jobject jparent, shared_ptr<ePub3::NavigationElement> parent)
{
	auto children = parent->Children();
	for (auto childIt = children.begin(); childIt != children.end(); ++childIt) {
		auto instance = &*childIt;
		auto navigationElement = instance->get();
		if (ePub3::NavigationPoint *navigationPoint = dynamic_cast<ePub3::NavigationPoint*>(navigationElement)) {
			jstring title = toJstring(env, navigationPoint->Title().c_str(), false);
			jstring content = toJstring(env, navigationPoint->Content().c_str(), false);
			jobject jchild = env->CallStaticObjectMethod(javaJavaObjectsFactoryClass, createNavigationPoint_ID,
					title, content);
			env->DeleteLocalRef(title);
			env->DeleteLocalRef(content);
			env->CallStaticVoidMethod(javaJavaObjectsFactoryClass, addElementToParent_ID,
					jparent, jchild);
			loadChildren(env, jchild, *instance);
			env->DeleteLocalRef(jchild);
		}
	}
}

static jobject loadNavigationTable(JNIEnv* env, shared_ptr<class ePub3::NavigationTable> navigationTable)
{
    if (navigationTable != nullptr) {
		jstring type = toJstring(env, navigationTable->Type().c_str(), false);
		jstring title = toJstring(env, navigationTable->Title().c_str(), false);
		jstring sourceHref = toJstring(env, navigationTable->SourceHref().c_str(), false);

		jobject jnavigationTable = env->CallStaticObjectMethod(javaJavaObjectsFactoryClass, createNavigationTable_ID,
				type, title, sourceHref);

		env->DeleteLocalRef(type);
		env->DeleteLocalRef(title);
		env->DeleteLocalRef(sourceHref);

		loadChildren(env, jnavigationTable, navigationTable);

		return jnavigationTable;
    }
	return NULL;
}


/*
 * JNI functions
 **************************************************/

/*
 * Package: org.readium.sdk.android
 * Class: Package
 */


JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_Package_nativeGetTitle
		(JNIEnv* env, jobject thiz, jlong pckgPtr)
{
	char *data = (char *) PCKG(pckgPtr)->Title().c_str();
	return toJstring(env, data, false);
}
JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_Package_nativeGetSubtitle
		(JNIEnv* env, jobject thiz, jlong pckgPtr)
{
	char *data = (char *) PCKG(pckgPtr)->Subtitle().c_str();
	return toJstring(env, data);
}
JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_Package_nativeGetShortTitle
		(JNIEnv* env, jobject thiz, jlong pckgPtr)
{
	char *data = (char *) PCKG(pckgPtr)->ShortTitle().c_str();
	return toJstring(env, data);
}
JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_Package_nativeGetCollectionTitle
		(JNIEnv* env, jobject thiz, jlong pckgPtr)
{
	char *data = (char *) PCKG(pckgPtr)->CollectionTitle().c_str();
	return toJstring(env, data);
}
JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_Package_nativeGetEditionTitle
		(JNIEnv* env, jobject thiz, jlong pckgPtr)
{
	char *data = (char *) PCKG(pckgPtr)->EditionTitle().c_str();
	return toJstring(env, data);
}
JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_Package_nativeGetExpandedTitle
		(JNIEnv* env, jobject thiz, jlong pckgPtr)
{
	char *data = (char *) PCKG(pckgPtr)->ExpandedTitle().c_str();
	return toJstring(env, data);
}
JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_Package_nativeGetFullTitle
		(JNIEnv* env, jobject thiz, jlong pckgPtr)
{
	char *data = (char *) PCKG(pckgPtr)->FullTitle().c_str();
	return toJstring(env, data, false);
}
JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_Package_nativeGetUniqueID
		(JNIEnv* env, jobject thiz, jlong pckgPtr)
{
	char *data = (char *) PCKG(pckgPtr)->UniqueID().c_str();
	return toJstring(env, data, false);
}
JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_Package_nativeGetURLSafeUniqueID
		(JNIEnv* env, jobject thiz, jlong pckgPtr)
{
	char *data = (char *) PCKG(pckgPtr)->URLSafeUniqueID().c_str();
	return toJstring(env, data, false);
}
JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_Package_nativeGetPackageID
		(JNIEnv* env, jobject thiz, jlong pckgPtr)
{
	char *data = (char *) PCKG(pckgPtr)->PackageID().c_str();
	return toJstring(env, data, false);
}
JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_Package_nativeGetBasePath
		(JNIEnv* env, jobject thiz, jlong pckgPtr)
{
	char *data = (char *) PCKG(pckgPtr)->BasePath().c_str();
	return toJstring(env, data, false);
}
JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_Package_nativeGetType
		(JNIEnv* env, jobject thiz, jlong pckgPtr)
{
	char *data = (char *) PCKG(pckgPtr)->Type().c_str();
	return toJstring(env, data);
}
JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_Package_nativeGetVersion
		(JNIEnv* env, jobject thiz, jlong pckgPtr)
{
	char *data = (char *) PCKG(pckgPtr)->Version().c_str();
	return toJstring(env, data, false);
}
JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_Package_nativeGetISBN
		(JNIEnv* env, jobject thiz, jlong pckgPtr)
{
	char *data = (char *) PCKG(pckgPtr)->ISBN().c_str();
	return toJstring(env, data, false);
}
JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_Package_nativeGetLanguage
		(JNIEnv* env, jobject thiz, jlong pckgPtr)
{
	char *data = (char *) PCKG(pckgPtr)->Language().c_str();
	return toJstring(env, data);
}
JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_Package_nativeGetCopyrightOwner
		(JNIEnv* env, jobject thiz, jlong pckgPtr)
{
	char *data = (char *) PCKG(pckgPtr)->CopyrightOwner().c_str();
	return toJstring(env, data, false);
}
JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_Package_nativeGetSource
		(JNIEnv* env, jobject thiz, jlong pckgPtr)
{
	char *data = (char *) PCKG(pckgPtr)->Source().c_str();
	return toJstring(env, data);
}
JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_Package_nativeGetAuthors
		(JNIEnv* env, jobject thiz, jlong pckgPtr)
{
	char *data = (char *) PCKG(pckgPtr)->Authors().c_str();
	return toJstring(env, data, false);
}
JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_Package_nativeGetModificationDate
		(JNIEnv* env, jobject thiz, jlong pckgPtr)
{
	char *data = (char *) PCKG(pckgPtr)->ModificationDate().c_str();
	return toJstring(env, data, false);
}
JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_Package_nativeGetPageProgressionDirection
		(JNIEnv* env, jobject thiz, jlong pckgPtr)
{
	ePub3::PageProgression pageProgressionDirection = PCKG(pckgPtr)->PageProgressionDirection();
	char *direction;
	switch (pageProgressionDirection) {
	case ePub3::PageProgression::LeftToRight:
		direction = (char *) "ltr";
		break;
	case ePub3::PageProgression::RightToLeft:
		direction = (char *) "rtl";
		break;
	case ePub3::PageProgression::Default:
	default:
		direction = (char *) "";
	}
	return toJstring(env, direction);
}
JNIEXPORT jobject JNICALL Java_org_readium_sdk_android_Package_nativeGetAuthorList
		(JNIEnv* env, jobject thiz, jlong pckgPtr)
{
	jobject stringList = javaEPub3_createStringList(env);
	auto authorNames = PCKG(pckgPtr)->AuthorNames();
    for (auto author = authorNames.begin(); author != authorNames.end(); ++author) {
		char *data = (char *) author->c_str();
		jstring jauthor = toJstring(env, data, false);
		javaEPub3_addStringToList(env, stringList, jauthor);
		env->DeleteLocalRef(jauthor);
    }
	return stringList;
}
JNIEXPORT jobject JNICALL Java_org_readium_sdk_android_Package_nativeGetSubjects
		(JNIEnv* env, jobject thiz, jlong pckgPtr)
{
	jobject stringList = javaEPub3_createStringList(env);
	auto subjects = PCKG(pckgPtr)->Subjects();
    for (auto subject = subjects.begin(); subject != subjects.end(); ++subject) {
		char *data = (char *) subject->c_str();
		jstring jsubject = toJstring(env, data, false);
		javaEPub3_addStringToList(env, stringList, jsubject);
		env->DeleteLocalRef(jsubject);
    }
	return stringList;
}
JNIEXPORT jobject JNICALL Java_org_readium_sdk_android_Package_nativeGetSpineItems
		(JNIEnv* env, jobject thiz, jlong pckgPtr)
{
	jobject spineItemList = env->CallStaticObjectMethod(javaJavaObjectsFactoryClass,
			createSpineItemList_ID);

    auto spine = PCKG(pckgPtr)->FirstSpineItem();
    do {
    	jstring idRef = toJstring(env, spine->Idref().c_str());
        auto manifestItem = spine->ManifestItem();
    	jstring href = toJstring(env, manifestItem->BaseHref().c_str(), false);
    	const char* _page_spread;
    	ePub3::PageSpread spread = spine->Spread();
    	switch (spread) {
    	case ePub3::PageSpread::Left:
    		_page_spread = "page-spread-left";
    		break;
    	case ePub3::PageSpread::Right:
    		_page_spread = "page-spread-right";
    		break;
    	case ePub3::PageSpread::Center:
    		_page_spread = "page-spread-center";
    		break;
    	default:
    		_page_spread = "";
    	}
    	jstring pageSpread = toJstring(env, _page_spread);
    	const char *_renditionLayout = getProperty((&*PCKG(pckgPtr)), (char *) "layout", (char *) "rendition", (&*spine));
    	jstring renditionLayout = env->NewStringUTF(_renditionLayout);

    	jobject spineItem = env->CallStaticObjectMethod(javaJavaObjectsFactoryClass, createSpineItem_ID,
    			idRef, href, pageSpread, renditionLayout);

		env->CallStaticVoidMethod(javaJavaObjectsFactoryClass, addSpineItemToList_ID,
				spineItemList, spineItem);
		env->DeleteLocalRef(idRef);
		env->DeleteLocalRef(href);
		env->DeleteLocalRef(pageSpread);
		env->DeleteLocalRef(renditionLayout);
		env->DeleteLocalRef(spineItem);

    } while ((spine = spine->Next()) != nullptr);

	return spineItemList;
}
JNIEXPORT jobject JNICALL Java_org_readium_sdk_android_Package_nativeGetTableOfContents
		(JNIEnv* env, jobject thiz, jlong pckgPtr)
{
    auto navigationTable = PCKG(pckgPtr)->TableOfContents();
	return loadNavigationTable(env, navigationTable);
}
JNIEXPORT jobject JNICALL Java_org_readium_sdk_android_Package_nativeGetListOfFigures
		(JNIEnv* env, jobject thiz, jlong pckgPtr)
{
    auto navigationTable = PCKG(pckgPtr)->ListOfFigures();
	return loadNavigationTable(env, navigationTable);
}
JNIEXPORT jobject JNICALL Java_org_readium_sdk_android_Package_nativeGetListOfIllustrations
		(JNIEnv* env, jobject thiz, jlong pckgPtr)
{
    auto navigationTable = PCKG(pckgPtr)->ListOfIllustrations();
	return loadNavigationTable(env, navigationTable);
}
JNIEXPORT jobject JNICALL Java_org_readium_sdk_android_Package_nativeGetListOfTables
		(JNIEnv* env, jobject thiz, jlong pckgPtr)
{
    auto navigationTable = PCKG(pckgPtr)->ListOfTables();
	return loadNavigationTable(env, navigationTable);
}
JNIEXPORT jobject JNICALL Java_org_readium_sdk_android_Package_nativeGetPageList
		(JNIEnv* env, jobject thiz, jlong pckgPtr)
{
    auto navigationTable = PCKG(pckgPtr)->PageList();
	return loadNavigationTable(env, navigationTable);
}

JNIEXPORT jobject JNICALL Java_org_readium_sdk_android_Package_nativeGetManifestTable
		(JNIEnv* env, jobject thiz, jlong pckgPtr)
{
	ePub3::ManifestTable manifest = PCKG(pckgPtr)->Manifest();
	jobject manifestItemList = env->CallStaticObjectMethod(javaJavaObjectsFactoryClass,
			createManifestItemList_ID);

	for (auto i = manifest.begin(); i != manifest.end(); i++) {
		std::shared_ptr<ePub3::ManifestItem> item = i->second;

//    	LOGD("ManifestItem: href:%s, mediatype:%s", item->Href().c_str(), item->MediaType().c_str());
    	jstring href = toJstring(env, item->Href().c_str());
    	jstring mediaType = toJstring(env, item->MediaType().c_str());
    	jobject manifestItem = env->CallStaticObjectMethod(javaJavaObjectsFactoryClass, createManifestItem_ID,
    			href, mediaType);

		env->CallStaticVoidMethod(javaJavaObjectsFactoryClass, addManifestItemToList_ID,
				manifestItemList, manifestItem);
		env->DeleteLocalRef(href);
		env->DeleteLocalRef(mediaType);
		env->DeleteLocalRef(manifestItem);
	}
	return manifestItemList;
}
JNIEXPORT jint JNICALL Java_org_readium_sdk_android_Package_nativeGetArchiveInfoSize
		(JNIEnv* env, jobject thiz, jlong pckgPtr, jlong contnrPtr, jstring jrelativePath)
{
	char *relativePath = (char *) env->GetStringUTFChars(jrelativePath, NULL);
    auto path = ePub3::string(PCKG(pckgPtr)->BasePath()).append(relativePath);
	env->ReleaseStringUTFChars(jrelativePath, relativePath);
    auto archive = contnr->GetArchive();
    bool containsPath = archive->ContainsItem(path);
    if (!containsPath) {
        LOGE("Package.nativeGetArchiveInfoSize(): no archive found for path '%s'", path.c_str());
        return -1;
    }
    auto archiveInfo = archive->InfoAtPath(path);


    return (jint) archiveInfo.UncompressedSize();
}
JNIEXPORT jobject JNICALL Java_org_readium_sdk_android_Package_nativeInputStreamForRelativePath
		(JNIEnv* env, jobject thiz, jlong pckgPtr, jlong contnrPtr, jstring jrelativePath) {
	char *relativePath = (char *) env->GetStringUTFChars(jrelativePath, NULL);
    auto path = ePub3::string(PCKG(pckgPtr)->BasePath()).append(relativePath);
	env->ReleaseStringUTFChars(jrelativePath, relativePath);
    auto archive = contnr->GetArchive();
    bool containsPath = archive->ContainsItem(path);
    if (!containsPath) {
        LOGE("Package.nativeReadStreamForRelativePath(): no archive found for path '%s'", path.c_str());
        return NULL;
    }
    auto archiveInfo = archive->InfoAtPath(path);
    auto byteStream = PCKG(pckgPtr)->ReadStreamForItemAtPath(path);
    ResourceStream *stream = new ResourceStream(byteStream);

    jobject inputStream = javaResourceInputStream_createResourceInputStream(env, stream, (jint) archiveInfo.UncompressedSize());

	return inputStream;
}
JNIEXPORT jobject JNICALL Java_org_readium_sdk_android_Package_nativeReadStreamForRelativePath
		(JNIEnv* env, jobject thiz, jlong pckgPtr, jlong contnrPtr, jstring jrelativePath) {
	char *relativePath = (char *) env->GetStringUTFChars(jrelativePath, NULL);
    auto path = ePub3::string(PCKG(pckgPtr)->BasePath()).append(relativePath);
    auto archive = contnr->GetArchive();
    bool containsPath = archive->ContainsItem(path);
    if (!containsPath) {
        LOGE("Package.nativeReadStreamForRelativePath(): no archive found for path '%s'", path.c_str());
        return NULL;
    }
    auto archiveInfo = archive->InfoAtPath(path);
    auto reader = PCKG(pckgPtr)->ReaderForRelativePath( ePub3::string(relativePath));

	env->ReleaseStringUTFChars(jrelativePath, relativePath);

    if (reader == NULL) {
    	LOGE("Package.nativeReadStreamForRelativePath(): no reader found for path '%s'", path.c_str());
        return NULL;
    } else {
    	LOGD("Package.nativeReadStreamForRelativePath(): archive found for path '%s'", path.c_str());
    }

    char tmpBuffer[BUFFER_SIZE];

    //TODO start check for memory leak
    jobject jbuffer = javaEPub3_createBuffer(env, (jint) archiveInfo.UncompressedSize());

    ssize_t readBytes = reader->read(tmpBuffer, BUFFER_SIZE);
    while (readBytes > 0) {
    	jsize length = (jsize) readBytes;
    	jbyteArray jtmpBuffer = env->NewByteArray(readBytes);
        jbyte* jbyteBuffer = (jbyte*) malloc(sizeof(jchar) * length);

        for (int i = 0; i < length; i ++) {
        	jbyteBuffer[i] = (jbyte)tmpBuffer[i];
        }
    	env->SetByteArrayRegion(jtmpBuffer, 0, length, jbyteBuffer);
    	javaEPub3_appendBytesToBuffer(env, jbuffer, jtmpBuffer);

		env->DeleteLocalRef(jtmpBuffer);
    	free(jbyteBuffer);
        readBytes = reader->read(tmpBuffer, BUFFER_SIZE);
//        PRINT("After readBytes: %d", readBytes);
    }
    //TODO end check for memory leak
	return jbuffer;
}

JNIEXPORT jobject JNICALL Java_org_readium_sdk_android_Package_nativeGetProperty
		(JNIEnv* env, jobject thiz, jlong pckgPtr, jstring jpropertyName, jstring jprefix)
{
    char* propertyName = (char *) env->GetStringUTFChars(jpropertyName, NULL);
    char* prefix = (char *) env->GetStringUTFChars(jprefix, NULL);

    jstring prop = toJstring(env, getProperty((&*PCKG(pckgPtr)), propertyName, prefix, (&*PCKG(pckgPtr))));

    RELEASE_UTF8(jpropertyName, propertyName);
    RELEASE_UTF8(jprefix, prefix);

    return prop;
}


#ifdef __cplusplus
}
#endif
