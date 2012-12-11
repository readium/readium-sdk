//
//  container.cpp
//  ePub3
//
//  Created by Jim Dovey on 2012-11-28.
//  Copyright (c) 2012 Kobo Inc. All rights reserved.
//

#include "container.h"
#include "archive.h"
#include "package.h"
#include "archive_xml.h"
#include "xpath_wrangler.h"

EPUB3_BEGIN_NAMESPACE

static const char * gContainerFilePath = "META-INF/container.xml";
static const char * gRootfilesXPath = "ocf:container/ocf:rootfiles/ocf:rootfile";
static const char * gRootfilePathsXPath = "ocf:container/ocf:rootfiles/ocf:rootfile/@full-path";
static const char * gVersionXPath = "ocf:container/@version";

Container::Container(const std::string& path) : _archive(Archive::Open(path))
{
    if ( _archive == nullptr )
        throw std::invalid_argument("Path does not point to a recognised archive file: " + path);
    
    // TODO: Initialize lazily? Doing so would make initialization faster, but require
    // PackageLocations() to become non-const, like Packages().
    ArchiveXmlReader reader(_archive->ReaderAtPath(gContainerFilePath));
    _ocf = reader.xmlReadDocument(gContainerFilePath, "utf-8", XML_PARSE_RECOVER|XML_PARSE_NOENT|XML_PARSE_DTDATTR);
    if ( _ocf == nullptr )
        throw std::invalid_argument(std::string(__PRETTY_FUNCTION__) + ": No container.xml in " + path);
}
Container::Container(Container&& o) : _archive(o._archive), _ocf(o._ocf), _packages(std::move(o._packages))
{
    o._archive = nullptr;
    o._ocf = nullptr;
    o._packages.clear();
}
Container::~Container()
{
    if ( _archive != nullptr )
        delete _archive;
    if ( _ocf != nullptr )
        xmlFreeDoc(_ocf);
    for ( auto ptr : _packages )
    {
        delete ptr;
    }
}
Container::PathList Container::PackageLocations() const
{
    XPathWrangler::NamespaceList nsList;
    nsList["ocf"] = "urn:oasis:names:tc:opendocument:xmlns:container";
    XPathWrangler xpath(_ocf, nsList);
    
    PathList output;
    for ( std::string & str : xpath.Strings(reinterpret_cast<const xmlChar*>(gRootfilePathsXPath)) )
    {
        output.push_back(std::move(str));
    }
    
    return output;
}
const Container::PackageList& Container::Packages()
{
    if ( _packages.empty() )
    {
        XPathWrangler::NamespaceList nsList;
        nsList["ocf"] = "urn:oasis:names:tc:opendocument:xmlns:container";
        XPathWrangler xpath(_ocf, nsList);
        xmlNodeSetPtr nodes = xpath.Nodes(reinterpret_cast<const xmlChar*>(gRootfilesXPath));
        
        if ( nodes != nullptr && nodes->nodeNr > 0 )
        {
            for ( int i = 0; i < nodes->nodeNr; i++ )
            {
                xmlNodePtr n = nodes->nodeTab[i];
                
                const xmlChar * _type = xmlGetProp(n, reinterpret_cast<const xmlChar*>("media-type"));
                std::string type((_type == nullptr ? "" : reinterpret_cast<const char*>(_type)));
                
                const xmlChar * _path = xmlGetProp(n, reinterpret_cast<const xmlChar*>("full-path"));
                if ( _path == nullptr )
                    continue;
                
                std::string path(reinterpret_cast<const char*>(_path));
                _packages.push_back(new Package(_archive, path, type));
            }
        }
    }
    
    return _packages;
}
std::string Container::Version() const
{
    XPathWrangler::NamespaceList nsList;
    nsList["ocf"] = "urn:oasis:names:tc:opendocument:xmlns:container";
    XPathWrangler xpath(_ocf, nsList);
    
    std::vector<std::string> strings = xpath.Strings(reinterpret_cast<const xmlChar*>(gVersionXPath));
    if ( strings.empty() )
        return "1.0";       // guess
    
    return std::move(strings[0]);
}

EPUB3_END_NAMESPACE
