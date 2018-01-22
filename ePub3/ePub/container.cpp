//
//  container.cpp
//  ePub3
//
//  Created by Jim Dovey on 2012-11-28.
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

#include "package.h"
#include "container.h"
#include "archive.h"
#include "archive_xml.h"
#include "xpath_wrangler.h"
#include "byte_stream.h"
#include "filter_manager.h"
#include <ePub3/xml/document.h>
#include <ePub3/xml/io.h>
#include <ePub3/content_module_manager.h>

/*
#if EPUB_COMPILER(CLANG) && defined(ANDROID)
#ifdef __cplusplus
extern "C" {
#endif

int __cxa_thread_atexit(void (*func)(), void *obj,
                                   void *dso_symbol) {
  return 0;
}
#ifdef __cplusplus
}
#endif
#endif
*/

EPUB3_BEGIN_NAMESPACE

static const char * gContainerFilePath = "META-INF/container.xml";
static const char * gEncryptionFilePath = "META-INF/encryption.xml";
static const char * gAppleiBooksDisplayOptionsFilePath = "META-INF/com.apple.ibooks.display-options.xml";
static const char * gRootfilesXPath = "/ocf:container/ocf:rootfiles/ocf:rootfile";
static const char * gRootfilePathsXPath = "/ocf:container/ocf:rootfiles/ocf:rootfile/@full-path";
static const char * gVersionXPath = "/ocf:container/@version";

Container::Container() :
#if EPUB_PLATFORM(WINRT)
	NativeBridge(),
#endif
	_archive(nullptr), _ocf(nullptr), _packages(), _encryption(), _path()
{
}
Container::Container(Container&& o) :
#if EPUB_PLATFORM(WINRT)
NativeBridge(),
#endif
_archive(std::move(o._archive)), _ocf(o._ocf), _packages(std::move(o._packages)), _path(std::move(o._path))
{
    o._ocf = nullptr;
}
Container::~Container()
{
}

bool Container::Open(const string& path, bool skipLoadingPotentiallyEncryptedContent)
{
	_archive = Archive::Open(path.stl_str());
	if (_archive == nullptr)
		throw std::invalid_argument(_Str("Path does not point to a recognised archive file: '", path, "'"));
	_path = path;

	// TODO: Initialize lazily? Doing so would make initialization faster, but require
	// PackageLocations() to become non-const, like Packages().

    unique_ptr<ArchiveReader> r = _archive->ReaderAtPath(gContainerFilePath);
    if (!bool(r.get())) {
        throw std::invalid_argument(_Str("ZIP Path not recognised: '", gContainerFilePath, "'"));
    }
    
    ArchiveXmlReader reader(std::move(r));
    if (!reader) {
        throw std::invalid_argument(_Str("ZIP Path not recognised: '", gContainerFilePath, "'"));
    }

#if ENABLE_XML_READ_DOC_MEMORY

    _ocf = reader.readXml(ePub3::string(gContainerFilePath));

#else

#if EPUB_USE(LIBXML2)
    _ocf = reader.xmlReadDocument(gContainerFilePath, nullptr);
#else
    decltype(_ocf) __tmp(reader.ReadDocument(gContainerFilePath, nullptr, /*RESOLVE_EXTERNALS*/ 1));
    _ocf = __tmp;
#endif

#endif //ENABLE_XML_READ_DOC_MEMORY

	if (!((bool)_ocf))
		return false;

#if EPUB_COMPILER_SUPPORTS(CXX_INITIALIZER_LISTS)
	XPathWrangler xpath(_ocf, { { "ocf", "urn:oasis:names:tc:opendocument:xmlns:container" } });
#else
	XPathWrangler::NamespaceList __ns;
	__ns["ocf"] = OCFNamespaceURI;
	XPathWrangler xpath(_ocf, __ns);
#endif
	xml::NodeSet nodes = xpath.Nodes(gRootfilesXPath);

	if (nodes.empty())
		return false;

	LoadEncryption();

    ParseVendorMetadata();

	for (auto n : nodes)
	{
		string type = _getProp(n, "media-type");

		string path = _getProp(n, "full-path");
		if (path.empty())
			continue;

		auto pkg = std::make_shared<Package>(shared_from_this(), type);
        //Package::New(Ptr(), type);

		if (pkg->Open(path, skipLoadingPotentiallyEncryptedContent))
			_packages.push_back(pkg);
	}

	return true;
}

#if FUTURE_ENABLED
ContainerPtr Container::OpenContainer(const string &path) {
    auto future = ContentModuleManager::Instance()->LoadContentAtPath(path, launch::any);

    if (!future.valid()) { // future.__future_ == nullptr
        // There is no content module that handles this publication, so we attempt opening plain content (no encryption)
        // Possibly also: the content module returned an errored Future.
        return OpenContainerForContentModule(path);
    }

    // There is a proper registered content module to handle the encrypted EPUB
    // Wait for result (this is blocking unless the Future is "ready")
    ContainerPtr result = future.get();

    if (result == nullptr) {
        return OpenContainerForContentModule(path);
    }

    return result;
}
#else
    ContainerPtr Container::OpenContainer(const string &path) {

        ContainerPtr container = ContentModuleManager::Instance()->LoadContentAtPath(path);

        // 1: everything went well, we've got an encrypted EPUB handled by a ContentModule
        if (bool(container)) {
            return std::move(container);
        }

        // 2: no ContentModule was suitable, let's try non-encrypted loading
        container = OpenContainerForContentModule(path);
        if (bool(container)) {
            return std::move(container);
        } else {
            return nullptr;
        }

        // 3: there's always the option of a raised exception, which the caller captures to degrade gracefully
    }
#endif //FUTURE_ENABLED

#ifdef SUPPORT_ASYNC
future<ContainerPtr> Container::OpenContainerAsync(const string& path, launch policy)
{
    auto result = ContentModuleManager::Instance()->LoadContentAtPath(path, policy);
    
    // see if it's complete with a nil value
    if (result.wait_for(std::chrono::system_clock::duration(0)) == future_status::ready)
    {
        ContainerPtr container = result.get();
        if (container)
            result = make_ready_future<ContainerPtr>(std::move(container));
        else
            result = async(policy, &Container::OpenContainerForContentModule, path);
    }
    
    return result;
}
#endif /* SUPPORT_ASYNC */

#if EPUB_PLATFORM(WINRT)
ContainerPtr Container::OpenSynchronouslyForWinRT(const string& path)
{
	auto future = ContentModuleManager::Instance()->LoadContentAtPath(path, std::launch::deferred);

	// see if it's complete with a nil value
	if (future.wait_for(std::chrono::system_clock::duration(0)) == std::future_status::ready)
	{
		ContainerPtr result = future.get();
		if (bool(result))
			return result;
		else
			return OpenContainerForContentModule(path);
	}

	// deferred call, will run the operation synchronously now
	return future.get();
}
#endif

ContainerPtr Container::OpenContainerForContentModule(const string& path, bool skipLoadingPotentiallyEncryptedContent)
{
	ContainerPtr container = std::make_shared<Container>(); //Container::New();
	if (container->Open(path, skipLoadingPotentiallyEncryptedContent) == false)
		return nullptr;
	return std::move(container);
}
Container::PathList Container::PackageLocations() const
{
#if EPUB_COMPILER_SUPPORTS(CXX_INITIALIZER_LISTS)
    XPathWrangler xpath(_ocf, {{"ocf", "urn:oasis:names:tc:opendocument:xmlns:container"}});
#else
    XPathWrangler::NamespaceList __ns;
    __ns["ocf"] = OCFNamespaceURI;
    XPathWrangler xpath(_ocf, __ns);
#endif
    
    PathList output;
    for ( string& str : xpath.Strings(gRootfilePathsXPath) )
    {
        output.emplace_back(std::move(str));
    }
    
    return output;
}
shared_ptr<Package> Container::DefaultPackage() const
{
    if ( _packages.empty() )
        return nullptr;
    return _packages[0];
}
string Container::Version() const
{
#if EPUB_COMPILER_SUPPORTS(CXX_INITIALIZER_LISTS)
    XPathWrangler xpath(_ocf, {{"ocf", "urn:oasis:names:tc:opendocument:xmlns:container"}});
#else
    XPathWrangler::NamespaceList __ns;
    __ns["ocf"] = OCFNamespaceURI;
    XPathWrangler xpath(_ocf, __ns);
#endif
    
    std::vector<string> strings = xpath.Strings(gVersionXPath);
    if ( strings.empty() )
        return "1.0";       // guess
    
    return std::move(strings[0]);
}

void Container::ParseVendorMetadata()
{

    unique_ptr<ArchiveReader> pZipReader = _archive->ReaderAtPath(gAppleiBooksDisplayOptionsFilePath);
    if ( !pZipReader )
        return;

    ArchiveXmlReader reader(std::move(pZipReader));

#if ENABLE_XML_READ_DOC_MEMORY

    shared_ptr<xml::Document> docXml = reader.readXml(ePub3::string(gAppleiBooksDisplayOptionsFilePath));

#else

#if EPUB_USE(LIBXML2)
    shared_ptr<xml::Document> docXml = reader.xmlReadDocument(gAppleiBooksDisplayOptionsFilePath, nullptr);
#elif EPUB_USE(WIN_XML)
    auto docXml = reader.ReadDocument(gAppleiBooksDisplayOptionsFilePath, nullptr, 0);
#endif

#endif //ENABLE_XML_READ_DOC_MEMORY


    if ( !bool(docXml) )
        return;

#if EPUB_COMPILER_SUPPORTS(CXX_INITIALIZER_LISTS)
    XPathWrangler xpath(docXml);
#else
    XPathWrangler::NamespaceList __ns;
    XPathWrangler xpath(docXml, __ns);
#endif

    xml::NodeSet nodes = xpath.Nodes("/display_options/platform/option");
    if ( nodes.empty() )
    {
        //xml::string str(docXml->XMLString());
        //printf("%s\n", docXml->XMLString().utf8());
        return;
    }

    for ( auto node : nodes )
    {
        string name = _getProp(node, "name");
        if (name.empty())
            continue;

        if (name == "fixed-layout")
        {
            _appleIBooksDisplayOption_FixedLayout = node->Content(); // true | false
        }
        else if (name == "orientation-lock")
        {
            _appleIBooksDisplayOption_Orientation = node->Content(); // landscape-only | portrait-only | none
        }
    }
}

void Container::LoadEncryption()
{

    unique_ptr<ArchiveReader> pZipReader = _archive->ReaderAtPath(gEncryptionFilePath);
    if ( !pZipReader )
        return;

    ArchiveXmlReader reader(std::move(pZipReader));

#if ENABLE_XML_READ_DOC_MEMORY

    shared_ptr<xml::Document> enc = reader.readXml(ePub3::string(gEncryptionFilePath));

#else

#if EPUB_USE(LIBXML2)
    shared_ptr<xml::Document> enc = reader.xmlReadDocument(gEncryptionFilePath, nullptr);
#elif EPUB_USE(WIN_XML)
    auto enc = reader.ReadDocument(gEncryptionFilePath, nullptr, 0);
#endif

#endif //ENABLE_XML_READ_DOC_MEMORY

    if ( !bool(enc) )
        return;
#if EPUB_COMPILER_SUPPORTS(CXX_INITIALIZER_LISTS)
    XPathWrangler xpath(enc, {{"enc", XMLENCNamespaceURI}, {"ocf", OCFNamespaceURI}});
#else
    XPathWrangler::NamespaceList __ns;
    __ns["ocf"] = OCFNamespaceURI;
    __ns["enc"] = XMLENCNamespaceURI;
    XPathWrangler xpath(enc, __ns);
#endif
    xml::NodeSet nodes = xpath.Nodes("/ocf:encryption/enc:EncryptedData");
    if ( nodes.empty() )
    {
		xml::string str(enc->XMLString());
		printf("%s\n", enc->XMLString().utf8());
        return;     // should be a hard error?
    }
    
    for ( auto node : nodes )
    {
        auto encPtr = std::make_shared<EncryptionInfo>(shared_from_this()); //EncryptionInfo::New(Ptr());
        if ( encPtr->ParseXML(node) )
            _encryption.push_back(encPtr);
    }
}
shared_ptr<EncryptionInfo> Container::EncryptionInfoForPath(const string &path) const
{
    for ( auto item : _encryption )
    {
        if ( item->Path() == path )
            return item;
    }
    
    return nullptr;
}
bool Container::FileExistsAtPath(const string& path) const
{
	return _archive->ContainsItem(path.stl_str());
}
unique_ptr<ByteStream> Container::ReadStreamAtPath(const string &path) const
{
    return _archive->ByteStreamAtPath(path.stl_str());
}

EPUB3_END_NAMESPACE
