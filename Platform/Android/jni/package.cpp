//
//  package.cpp
//  ePub3
//
//  Created by Pedro Reis Colaco (txtr) on 2013-07-02.
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


#include <string>
#include <vector>
#include <typeinfo>
#include <algorithm>    // std::min

#include <ePub3/archive.h>
#include <ePub3/container.h>
#include <ePub3/media-overlays_smil_model.h>	// ePub3::MediaOverlaysSmilModel
#include <ePub3/media-overlays_smil_data.h>		// ePub3::SMILData
#include <ePub3/package.h>
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

static ePub3::string getProperty(ePub3::Package* package, char* name, char* pref, ePub3::PropertyHolder* forObject)
{
	LOGD("getProperty(): called for name='%s' pref='%s'", name, pref);
    auto propertyName = ePub3::string(name);
    auto prefix = ePub3::string(pref);
    auto iri = package->MakePropertyIRI(propertyName, prefix);

    auto propertyList = forObject->PropertiesMatching(iri);

    if (propertyList.size() > 0) {
        auto prop = propertyList[0];
        ePub3::string value(prop->Value());
    	LOGD("getProperty(): returning '%s'", value.c_str());
        return value;
    }
	LOGD("getProperty(): returning EMPTY");
    return "";
}

static void loadChildren(JNIEnv* env, jobject jparent, shared_ptr<ePub3::NavigationElement> parent)
{
	auto children = parent->Children();
	for (auto childIt = children.begin(); childIt != children.end(); ++childIt) {
		auto instance = &*childIt;
		auto navigationElement = instance->get();
		if (ePub3::NavigationPoint *navigationPoint = dynamic_cast<ePub3::NavigationPoint*>(navigationElement)) {
			jni::StringUTF title(env, (std::string&) navigationPoint->Title().stl_str());
			jstring jtitle = (jstring) title;
			jni::StringUTF content(env, (std::string&) navigationPoint->Content().stl_str());
			jstring jcontent = (jstring) content;
			jobject jchild = env->CallStaticObjectMethod(javaJavaObjectsFactoryClass, createNavigationPoint_ID,
					jtitle, jcontent);
			env->DeleteLocalRef(jtitle);
			env->DeleteLocalRef(jcontent);
			env->CallStaticVoidMethod(javaJavaObjectsFactoryClass, addElementToParent_ID,
					jparent, jchild);
			loadChildren(env, jchild, *instance);
			env->DeleteLocalRef(jchild);
		}
	}
}

static jobject loadNavigationTable(JNIEnv* env, shared_ptr<class ePub3::NavigationTable> navigationTable, char* defaultName)
{
    if (navigationTable != nullptr) {
		jni::StringUTF type(env, (std::string&) navigationTable->Type().stl_str());
		jstring jtype = (jstring) type;
		jni::StringUTF title(env, (std::string&) navigationTable->Title().stl_str());
		jstring jtitle = (jstring) title;
		jni::StringUTF sourceHref(env, (std::string&) navigationTable->SourceHref().stl_str());
		jstring jsourceHref = (jstring) sourceHref;

		jobject jnavigationTable = env->CallStaticObjectMethod(javaJavaObjectsFactoryClass, createNavigationTable_ID,
				jtype, jtitle, jsourceHref);

		env->DeleteLocalRef(jtype);
		env->DeleteLocalRef(jtitle);
		env->DeleteLocalRef(jsourceHref);

		loadChildren(env, jnavigationTable, navigationTable);

		return jnavigationTable;
    }
	jstring name = toJstring(env, defaultName, false);
	return env->CallStaticObjectMethod(javaJavaObjectsFactoryClass, createNavigationTable_ID,
			name, name, NULL);
}

static void populateJsonWithSmilParAudio(stringstream &stream, std::shared_ptr<const ePub3::SMILData::Audio> audio){
	
	stream << "{" << endl;
	stream << "\"nodeType\" : \"audio\"," << endl;
	stream << "\"src\" : \"" << audio->SrcFile() << "\"," << endl;

	double begin = (static_cast<double>(audio->ClipBeginMilliseconds())) / 1000;
	double end = (static_cast<double>(audio->ClipEndMilliseconds())) / 1000;

	stream << "\"clipBegin\" : " << begin << "," << endl;
	stream << "\"clipEnd\" : " << end << "" << endl;
	stream << "}" << endl;
}


static void populateJsonWithSmilParText(stringstream &stream, std::shared_ptr<const ePub3::SMILData::Text> text){
    
    auto srcFragmentId = text->SrcFragmentId();
    auto srcFile = text->SrcFile();

    stream << "{" << endl;
    stream << "\"nodeType\" : \"text\"," << endl;
    stream << "\"srcFile\" : \"" << srcFile << "\"," << endl;
    stream << "\"srcFragmentId\" : \"" << srcFragmentId << "\"," << endl;

    if(srcFragmentId.empty()){
    	stream << "\"src\" : \"" << srcFile << "\"" << endl;
    } else {
		stream << "\"src\" : \"" << srcFile << '#' << srcFragmentId << "\"" << endl;
    }

    stream << "}" << endl;
}

static void populateJsonWithSmilPar(stringstream &stream, std::shared_ptr<const ePub3::SMILData::Parallel> par){
	//TODO do we need this?
    //printf("CHECK SMIL DATA TREE TEXTREF FRAGID %s\n", par->_textref_fragmentID.c_str());

	if(nullptr == par){
		return;
	}

    stream << "{" << endl;
	stream << "\"epubtype\": \"" << par->Type() << "\" ," << endl;
	stream << "\"nodeType\": \"" << par->Name() << "\" ," << endl;

	stream << "\"children\" : [ " <<endl;
    if (nullptr != par->Text()){
        populateJsonWithSmilParText(stream, par->Text());
    }

    if (nullptr != par->Audio()){
    	stream << " , " << endl;
        populateJsonWithSmilParAudio(stream, par->Audio());
    }
    stream << "]" << endl;
	stream << "}" << endl;

}

static void populateJsonWithSmilSeq(stringstream &stream, std::shared_ptr<const ePub3::SMILData::Sequence> seqq){
	//TODO do we need this?
    //printf("CHECK SMIL DATA TREE TEXTREF FRAGID %s\n", seqq->_textref_fragmentID.c_str());

	if(nullptr == seqq){
        stream << "!! NULL SMIL SEQQ" << endl;
		return;
	}

    stream << "{" << endl;
    stream << "\"textref\": \"" << seqq->TextRefFile() << "\" ," << endl;
	stream << "\"epubtype\": \"" << seqq->Type() << "\" ," << endl;
	stream << "\"nodeType\": \"" << seqq->Name() << "\" ," << endl;

	stream << "\"children\": [" << endl;

	auto childrenCount = seqq->GetChildrenCount();

    for (int i = 0; i < childrenCount; ++i){

        std::shared_ptr<const ePub3::SMILData::TimeContainer> container = seqq->GetChild(i);

        if(nullptr == container){
            stream << "!! NULL SMIL CONTAINER" << endl;
        	continue;
        }

        
        //const ePub3::SMILData::Sequence *seq = dynamic_cast<const ePub3::SMILData::Sequence *>(container);
        //if (nullptr != seq){
        if (container->IsSequence()){
            auto seq = std::dynamic_pointer_cast<const ePub3::SMILData::Sequence>(container);
        	populateJsonWithSmilSeq(stream, seq);
            //continue;
        }

        //const ePub3::SMILData::Parallel *par = dynamic_cast<const ePub3::SMILData::Parallel *>(container);
        //if (nullptr != par){
        else if (container->IsParallel()){
            auto par = std::dynamic_pointer_cast<const ePub3::SMILData::Parallel>(container);
        	populateJsonWithSmilPar(stream, par);
            //continue;
        }
        else{
            stream << "!! SMIL CONTAINER TYPE???" << endl;
        }

        if(i != childrenCount-1){
        	stream << ',';
        }

    }
    stream << ']' << endl;
    stream << '}' << endl;

}

static void populateJsonWithSmilDatas(stringstream &stream, std::shared_ptr<ePub3::MediaOverlaysSmilModel> &smilDatas){
    
    if(nullptr == smilDatas){
    	return;
    }

    stream << "\"smil_models\" : [" << endl;

    int smilDataAmount = smilDatas->GetSmilCount();

    for (int i = 0; i < smilDataAmount; ++i){
        shared_ptr<ePub3::SMILData> smilData = smilDatas->GetSmil(i);

        if(nullptr == smilData){
        	continue;
        }

        stream << "{" << endl;

		//issue smilVersion
        stream << "\"smilVersion\" : \"3.0\"," << endl;

        //issue duration
        double duration = static_cast<double>(smilData->DurationMilliseconds_Metadata())/1000;
        stream << "\"duration\" : " << duration  << "," << endl;


        auto item = smilData->SmilManifestItem();

        string id;
        string href;
        if(nullptr == item){
        	id = "";
        	href = "fake.smil";
        } else{
        	id = item->Identifier().c_str();
        	href = item->Href().c_str();
        }

        stream << "\"id\" : \""<< id << "\"," << endl;
        stream << "\"href\" : \""<< href << "\"," << endl;
        
        stream << "\"spineItemId\" : \"" << smilData->XhtmlSpineItem()->Idref().c_str() << "\"," << endl;

        stream << "\"children\":[" << endl;

        auto seq = smilData->Body();
        //ePub3::SMILData::Sequence *seq = const_cast<ePub3::SMILData::Sequence *>(smilData->Body());
        populateJsonWithSmilSeq(stream, seq);
        
        stream << "]" << endl;

		stream << "}" << endl;

		//if we are not at the last index, set a comma
		if(i != smilDataAmount-1){
			stream << ",";
		}
    }

    stream << ']'; // smil_models [
}

static void populateJsonWithEscapeables(stringstream &stream, std::shared_ptr<ePub3::MediaOverlaysSmilModel> &smilDatas){
	stream << "\"escapables\" : [" << endl;

	auto amountOfEscapeables = smilDatas->GetEscapablesCount();

	for(std::vector<string>::size_type i = 0; i < amountOfEscapeables; ++i){
		stream << "\"" << smilDatas->GetEscapable(i) << "\"";
		if(i != amountOfEscapeables-1){
			stream << "," << endl;
		} else{
			stream << endl;
		}
	}

	stream << "]," << endl; //escapeables: [, the comma is there because we know that after this there will be skippables
}

static void populateJsonWithSkippables(stringstream &stream, std::shared_ptr<ePub3::MediaOverlaysSmilModel> &smilDatas){
	stream << "\"skippables\" : [" << endl;

	auto amountOfSkippables = smilDatas->GetSkippablesCount();

	for(std::vector<string>::size_type i = 0; i < amountOfSkippables; ++i){
		stream << "\"" << smilDatas->GetSkippable(i) << "\"";
		if(i != amountOfSkippables-1){
			stream << "," << endl;
		} else{
			stream << endl;
		}
	}

	stream << "]," << endl; //skippables: [, the comma is there because we know that after this there will be smil_models
}

static void populateJsonWithMediaOverlayContent(stringstream &stream, std::shared_ptr<ePub3::MediaOverlaysSmilModel> &model){
	//sanity check
	if(nullptr == model){
    	return;
    }

    double duration = static_cast<double>(model->DurationMilliseconds_Metadata())/1000;

    stream << "\"activeClass\": \"" << model->ActiveClass() << "\"," << endl;
    stream << "\"duration\": " << duration << "," << endl;
    stream << "\"narrator\": \"" << model->Narrator() << "\"," << endl;
    stream << "\"playbackActiveClass\": \"" << model->PlaybackActiveClass() << "\"," << endl;
    
    populateJsonWithEscapeables(stream, model);
    populateJsonWithSkippables(stream, model);
	populateJsonWithSmilDatas(stream, model);
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
	jni::StringUTF str(env, (std::string&) PCKG(pckgPtr)->Title().stl_str());
	return (jstring) str;
}
JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_Package_nativeGetSubtitle
		(JNIEnv* env, jobject thiz, jlong pckgPtr)
{
	jni::StringUTF str(env, (std::string&) PCKG(pckgPtr)->Subtitle().stl_str());
	return (jstring) str;
}
JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_Package_nativeGetShortTitle
		(JNIEnv* env, jobject thiz, jlong pckgPtr)
{
	jni::StringUTF str(env, (std::string&) PCKG(pckgPtr)->ShortTitle().stl_str());
	return (jstring) str;
}
JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_Package_nativeGetCollectionTitle
		(JNIEnv* env, jobject thiz, jlong pckgPtr)
{
	jni::StringUTF str(env, (std::string&) PCKG(pckgPtr)->CollectionTitle().stl_str());
	return (jstring) str;
}
JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_Package_nativeGetEditionTitle
		(JNIEnv* env, jobject thiz, jlong pckgPtr)
{
	jni::StringUTF str(env, (std::string&) PCKG(pckgPtr)->EditionTitle().stl_str());
	return (jstring) str;
}
JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_Package_nativeGetExpandedTitle
		(JNIEnv* env, jobject thiz, jlong pckgPtr)
{
	jni::StringUTF str(env, (std::string&) PCKG(pckgPtr)->ExpandedTitle().stl_str());
	return (jstring) str;
}
JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_Package_nativeGetFullTitle
		(JNIEnv* env, jobject thiz, jlong pckgPtr)
{
	jni::StringUTF str(env, (std::string&) PCKG(pckgPtr)->FullTitle().stl_str());
	return (jstring) str;
}
JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_Package_nativeGetUniqueID
		(JNIEnv* env, jobject thiz, jlong pckgPtr)
{
	jni::StringUTF str(env, (std::string&) PCKG(pckgPtr)->UniqueID().stl_str());
	return (jstring) str;
}
JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_Package_nativeGetURLSafeUniqueID
		(JNIEnv* env, jobject thiz, jlong pckgPtr)
{
	jni::StringUTF str(env, (std::string&) PCKG(pckgPtr)->URLSafeUniqueID().stl_str());
	return (jstring) str;
}
JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_Package_nativeGetPackageID
		(JNIEnv* env, jobject thiz, jlong pckgPtr)
{
	jni::StringUTF str(env, (std::string&) PCKG(pckgPtr)->PackageID().stl_str());
	return (jstring) str;
}
JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_Package_nativeGetBasePath
		(JNIEnv* env, jobject thiz, jlong pckgPtr)
{
	jni::StringUTF str(env, (std::string&) PCKG(pckgPtr)->BasePath().stl_str());
	return (jstring) str;
}
JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_Package_nativeGetType
		(JNIEnv* env, jobject thiz, jlong pckgPtr)
{
	jni::StringUTF str(env, (std::string&) PCKG(pckgPtr)->Type().stl_str());
	return (jstring) str;
}
JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_Package_nativeGetVersion
		(JNIEnv* env, jobject thiz, jlong pckgPtr)
{
	jni::StringUTF str(env, (std::string&) PCKG(pckgPtr)->Version().stl_str());
	return (jstring) str;
}
JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_Package_nativeGetISBN
		(JNIEnv* env, jobject thiz, jlong pckgPtr)
{
	jni::StringUTF str(env, (std::string&) PCKG(pckgPtr)->ISBN().stl_str());
	return (jstring) str;
}
JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_Package_nativeGetLanguage
		(JNIEnv* env, jobject thiz, jlong pckgPtr)
{
	jni::StringUTF str(env, (std::string&) PCKG(pckgPtr)->Language().stl_str());
	return (jstring) str;
}
JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_Package_nativeGetCopyrightOwner
		(JNIEnv* env, jobject thiz, jlong pckgPtr)
{
	jni::StringUTF str(env, (std::string&) PCKG(pckgPtr)->CopyrightOwner().stl_str());
	return (jstring) str;
}
JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_Package_nativeGetSource
		(JNIEnv* env, jobject thiz, jlong pckgPtr)
{
	jni::StringUTF str(env, (std::string&) PCKG(pckgPtr)->Source().stl_str());
	return (jstring) str;
}
JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_Package_nativeGetAuthors
		(JNIEnv* env, jobject thiz, jlong pckgPtr)
{
	jni::StringUTF str(env, (std::string&) PCKG(pckgPtr)->Authors().stl_str());
	return (jstring) str;
}
JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_Package_nativeGetModificationDate
		(JNIEnv* env, jobject thiz, jlong pckgPtr)
{
	jni::StringUTF str(env, (std::string&) PCKG(pckgPtr)->ModificationDate().stl_str());
	return (jstring) str;
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
    	jni::StringUTF str(env, (std::string&) author->stl_str());
		jstring jauthor = (jstring) str;
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
    	jni::StringUTF str(env, (std::string&) subject->stl_str());
		jstring jsubject = (jstring) str;
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
    	jni::StringUTF idr(env, (std::string&) spine->Idref().stl_str());
    	jstring idRef =  (jstring) idr;
    	jni::StringUTF titleUTF(env, (std::string&) spine->Title().stl_str());
    	jstring title =  (jstring) titleUTF;
        auto manifestItem = spine->ManifestItem();
    	jni::StringUTF hr(env, (std::string&) manifestItem->BaseHref().stl_str());
    	jstring href = (jstring) hr;
    	jni::StringUTF mt(env, (std::string&) manifestItem->MediaType().stl_str());
    	jstring mediaType = (jstring) mt;
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
    		break;
    	}
    	jstring pageSpread = toJstring(env, _page_spread);
    	ePub3::string _renditionLayout = getProperty((&*PCKG(pckgPtr)), (char *) "layout", (char *) "rendition", (&*spine));
    	jstring renditionLayout = env->NewStringUTF(_renditionLayout.c_str());

    	ePub3::string _media_overlay_id = manifestItem->MediaOverlayID();
    	jstring media_overlay_id = env->NewStringUTF(_media_overlay_id.c_str());

    	jobject spineItem = env->CallStaticObjectMethod(javaJavaObjectsFactoryClass, createSpineItem_ID,
    			idRef, title, href, mediaType, pageSpread, renditionLayout, media_overlay_id);

		env->CallStaticVoidMethod(javaJavaObjectsFactoryClass, addSpineItemToList_ID,
				spineItemList, spineItem);
		env->DeleteLocalRef(idRef);
		env->DeleteLocalRef(title);
		env->DeleteLocalRef(href);
		env->DeleteLocalRef(mediaType);
		env->DeleteLocalRef(pageSpread);
		env->DeleteLocalRef(renditionLayout);
		env->DeleteLocalRef(media_overlay_id);
		env->DeleteLocalRef(spineItem);

    } while ((spine = spine->Next()) != nullptr);

	return spineItemList;
}
JNIEXPORT jobject JNICALL Java_org_readium_sdk_android_Package_nativeGetTableOfContents
		(JNIEnv* env, jobject thiz, jlong pckgPtr)
{
    auto navigationTable = PCKG(pckgPtr)->TableOfContents();
	return loadNavigationTable(env, navigationTable, "toc");
}
JNIEXPORT jobject JNICALL Java_org_readium_sdk_android_Package_nativeGetListOfFigures
		(JNIEnv* env, jobject thiz, jlong pckgPtr)
{
    auto navigationTable = PCKG(pckgPtr)->ListOfFigures();
	return loadNavigationTable(env, navigationTable, "lof");
}
JNIEXPORT jobject JNICALL Java_org_readium_sdk_android_Package_nativeGetListOfIllustrations
		(JNIEnv* env, jobject thiz, jlong pckgPtr)
{
    auto navigationTable = PCKG(pckgPtr)->ListOfIllustrations();
	return loadNavigationTable(env, navigationTable, "loi");
}
JNIEXPORT jobject JNICALL Java_org_readium_sdk_android_Package_nativeGetListOfTables
		(JNIEnv* env, jobject thiz, jlong pckgPtr)
{
    auto navigationTable = PCKG(pckgPtr)->ListOfTables();
	return loadNavigationTable(env, navigationTable, "lot");
}
JNIEXPORT jobject JNICALL Java_org_readium_sdk_android_Package_nativeGetPageList
		(JNIEnv* env, jobject thiz, jlong pckgPtr)
{
    auto navigationTable = PCKG(pckgPtr)->PageList();
	return loadNavigationTable(env, navigationTable, "page-list");
}

JNIEXPORT jobject JNICALL Java_org_readium_sdk_android_Package_nativeGetManifestTable
		(JNIEnv* env, jobject thiz, jlong pckgPtr)
{
	ePub3::ManifestTable manifest = PCKG(pckgPtr)->Manifest();
	jobject manifestItemList = env->CallStaticObjectMethod(javaJavaObjectsFactoryClass,
			createManifestItemList_ID);

	for (auto i = manifest.begin(); i != manifest.end(); i++) {
		std::shared_ptr<ePub3::ManifestItem> item = i->second;

    	jni::StringUTF hr(env, (std::string&) item->Href().stl_str());
    	jstring href = (jstring) hr;
    	jni::StringUTF mt(env, (std::string&) item->MediaType().stl_str());
    	jstring mediaType = (jstring) mt;

//    	LOGD("ManifestItem: href:%s, mediatype:%s", hr.c_str(), mt.c_str());

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
	LOGI("Package.nativeInputStreamForRelativePath(): received relative path '%s'", relativePath);
	auto basePath = ePub3::string(PCKG(pckgPtr)->BasePath());
	LOGI("Package.nativeInputStreamForRelativePath(): package base path '%s'", basePath.c_str());
    auto path = basePath.append(relativePath);
	env->ReleaseStringUTFChars(jrelativePath, relativePath);
	LOGI("Package.nativeInputStreamForRelativePath(): final path '%s'", path.c_str());
    auto archive = contnr->GetArchive();
    bool containsPath = archive->ContainsItem(path);
    if (!containsPath) {
        LOGE("Package.nativeInputStreamForRelativePath(): no archive found for path '%s'", path.c_str());
        return NULL;
    }
    auto archiveInfo = archive->InfoAtPath(path);
    auto byteStream = PCKG(pckgPtr)->ReadStreamForItemAtPath(path);
    ResourceStream *stream = new ResourceStream(byteStream);

    jobject inputStream = javaResourceInputStream_createResourceInputStream(env, stream, (int) archiveInfo.UncompressedSize());

	return inputStream;
}

JNIEXPORT jobject JNICALL Java_org_readium_sdk_android_Package_nativeGetProperty
		(JNIEnv* env, jobject thiz, jlong pckgPtr, jstring jpropertyName, jstring jprefix)
{
    char* propertyName = (char *) env->GetStringUTFChars(jpropertyName, NULL);
    char* prefix = (char *) env->GetStringUTFChars(jprefix, NULL);

    ePub3::string property = getProperty((&*PCKG(pckgPtr)), propertyName, prefix, (&*PCKG(pckgPtr)));
    jstring jprop = toJstring(env, property.c_str());

    RELEASE_UTF8(jpropertyName, propertyName);
    RELEASE_UTF8(jprefix, prefix);

    return jprop;
}

JNIEXPORT jstring JNICALL Java_org_readium_sdk_android_Package_nativeGetSmilDataAsJson
		(JNIEnv* env, jobject thiz, jlong pckgPtr)
{
	stringstream stream;

	auto package = PCKG(pckgPtr);
	auto model = package->MediaOverlaysSmilModel();

	//stream << "\"media_overlay\": {" << endl;
	stream << "{" << endl;

	populateJsonWithMediaOverlayContent(stream, model);

	stream << '}'; // media_overlay

	std::string result {stream.str()};
	jni::StringUTF str(env, result);
	return (jstring) str;
}


#ifdef __cplusplus
}
#endif
