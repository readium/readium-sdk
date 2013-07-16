//
//  container.cpp
//  ePub3
//
//  Created by Jim Dovey on 2012-11-28.
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

#include "package.h"
#include "container.h"
#include "archive.h"
#include "archive_xml.h"
#include "xpath_wrangler.h"
#include "byte_stream.h"

EPUB3_BEGIN_NAMESPACE

static const char * gContainerFilePath = "META-INF/container.xml";
static const char * gEncryptionFilePath = "META-INF/encryption.xml";
static const char * gRootfilesXPath = "/ocf:container/ocf:rootfiles/ocf:rootfile";
static const char * gRootfilePathsXPath = "/ocf:container/ocf:rootfiles/ocf:rootfile/@full-path";
static const char * gVersionXPath = "/ocf:container/@version";

Container::Container() : _archive(nullptr), _ocf(nullptr), _packages(), _encryption()
{
}
Container::Container(Container&& o) : _archive(std::move(o._archive)), _ocf(o._ocf), _packages(std::move(o._packages))
{
    o._ocf = nullptr;
}
Container::~Container()
{
    if ( _ocf != nullptr )
        xmlFreeDoc(_ocf);
}
bool Container::Open(const string& path)
{
    ContainerPtr sharedThis(shared_from_this());
    _archive = std::move(Archive::Open(path.stl_str()));
    if ( _archive == nullptr )
        throw std::invalid_argument(_Str("Path does not point to a recognised archive file: '", path, "'"));
    
    // TODO: Initialize lazily? Doing so would make initialization faster, but require
    // PackageLocations() to become non-const, like Packages().
    ArchiveXmlReader reader(_archive->ReaderAtPath(gContainerFilePath));
    _ocf = reader.xmlReadDocument(gContainerFilePath, nullptr, XML_PARSE_RECOVER|XML_PARSE_NOENT|XML_PARSE_DTDATTR);
    if ( _ocf == nullptr )
        return false;

#if EPUB_COMPILER_SUPPORTS(CXX_INITIALIZER_LISTS)
    XPathWrangler xpath(_ocf, {{"ocf", "urn:oasis:names:tc:opendocument:xmlns:container"}});
#else
    XPathWrangler::NamespaceList __ns;
    __ns["ocf"] = OCFNamespaceURI;
    XPathWrangler xpath(_ocf, __ns);
#endif
    xmlNodeSetPtr nodes = xpath.Nodes(reinterpret_cast<const xmlChar*>(gRootfilesXPath));
    
    if ( nodes == nullptr || nodes->nodeNr == 0 )
        return false;
    
    for ( int i = 0; i < nodes->nodeNr; i++ )
    {
        xmlNodePtr n = nodes->nodeTab[i];
        
        const xmlChar * _type = xmlGetProp(n, reinterpret_cast<const xmlChar*>("media-type"));
        std::string type((_type == nullptr ? "" : reinterpret_cast<const char*>(_type)));
        
        const xmlChar * _path = xmlGetProp(n, reinterpret_cast<const xmlChar*>("full-path"));
        if ( _path == nullptr )
            continue;
        
        auto pkg = std::make_shared<Package>(sharedThis, type);
        if ( pkg->Open(_path) )
            _packages.push_back(pkg);
    }

    LoadEncryption();
    return true;
}
shared_ptr<Container> Container::OpenContainer(const string &path)
{
    ContainerPtr container = std::make_shared<Container>();
    if ( container->Open(path) == false )
        return nullptr;
    return container;
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
void Container::LoadEncryption()
{
    ContainerPtr sharedThis(shared_from_this());
    unique_ptr<ArchiveReader> pZipReader = _archive->ReaderAtPath(gEncryptionFilePath);
    if ( !pZipReader )
        return;
    
    ArchiveXmlReader reader(std::move(pZipReader));
    xmlDocPtr enc = reader.xmlReadDocument(gEncryptionFilePath, nullptr, XML_PARSE_RECOVER|XML_PARSE_NOENT|XML_PARSE_DTDATTR);
    if ( enc == nullptr )
        return;
#if EPUB_COMPILER_SUPPORTS(CXX_INITIALIZER_LISTS)
    XPathWrangler xpath(enc, {{"enc", XMLENCNamespaceURI}, {"ocf", OCFNamespaceURI}});
#else
    XPathWrangler::NamespaceList __ns;
    __ns["ocf"] = OCFNamespaceURI;
    __ns["enc"] = XMLENCNamespaceURI;
    XPathWrangler xpath(enc, __ns);
#endif
    xmlNodeSetPtr nodes = xpath.Nodes("/ocf:encryption/enc:EncryptedData");
    if ( nodes == nullptr || nodes->nodeNr == 0 )
    {
        xmlChar* mem = nullptr;
        int size = 0;
        xmlDocDumpMemory(enc, &mem, &size);
        printf("%s\n", reinterpret_cast<char*>(mem));
        xmlFree(mem);
        return;     // should be a hard error?
    }
    
    for ( int i = 0; i < nodes->nodeNr; i++ )
    {
        auto encPtr = std::make_shared<EncryptionInfo>(sharedThis);
        if ( encPtr->ParseXML(nodes->nodeTab[i]) )
            _encryption.push_back(encPtr);
    }
    
    xmlXPathFreeNodeSet(nodes);
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
unique_ptr<ByteStream> Container::ReadStreamAtPath(const string &path) const
{
    return _archive->ByteStreamAtPath(path.stl_str());
}

EPUB3_END_NAMESPACE
