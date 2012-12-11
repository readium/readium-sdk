//
//  package.cpp
//  ePub3
//
//  Created by Jim Dovey on 2012-11-28.
//  Copyright (c) 2012 Kobo Inc. All rights reserved.
//

#include "package.h"
#include "metadata.h"
#include "archive.h"
#include "archive_xml.h"
#include "xpath_wrangler.h"
#include <libxml/xpathInternals.h>

static const char * OPFNamespace = "http://www.idpf.org/2007/opf";
static const char * DCNamespace = "http://purl.org/dc/elements/1.1/";

EPUB3_BEGIN_NAMESPACE

Package::Package(Archive* archive, const std::string& path, const std::string& type) : _archive(archive), _opf(nullptr), _type(type)
{
    if ( _archive == nullptr )
        throw std::invalid_argument("Path does not point to a recognised archive file: " + path);
    
    // TODO: Initialize lazily? Doing so would make initialization faster, but require
    // PackageLocations() to become non-const, like Packages().
    ArchiveXmlReader reader(_archive->ReaderAtPath(path));
    _opf = reader.xmlReadDocument(path.c_str(), "utf-8", XML_PARSE_RECOVER|XML_PARSE_NOENT|XML_PARSE_DTDATTR);
    if ( _opf == nullptr )
        throw std::invalid_argument(std::string(__PRETTY_FUNCTION__) + ": No OPF file at " + path);
    
    if ( Unpack() )
        throw std::invalid_argument(std::string(__PRETTY_FUNCTION__) + ": Not a valid OPF file at " + path);
}
Package::Package(Package&& o) : _archive(o._archive), _opf(o._opf), _type(std::move(o._type)), _metadata(std::move(o._metadata)), _manifest(std::move(o._manifest)), _spine(std::move(o._spine))
{
    o._archive = nullptr;
    o._opf = nullptr;
}
Package::~Package()
{
    // our Container owns the archive
    if ( _opf != nullptr )
        xmlFreeDoc(_opf);
}
std::string Package::Version() const
{
    return _getProp(xmlDocGetRootElement(_opf), "version");
}
const SpineItem* Package::SpineItemAt(size_t idx) const
{
    const SpineItem * item = _spine;
    for ( size_t i = 0; i < idx && item != nullptr; i++ )
    {
        item = item->Next();
    }
    return item;
}
const ManifestItem* Package::ManifestItemWithID(const std::string &ident) const
{
    auto found = _manifest.find(ident);
    if ( found == _manifest.end() )
        return nullptr;
    
    return found->second;
}
bool Package::Unpack()
{
    // very basic sanity check
    xmlNodePtr root = xmlDocGetRootElement(_opf);
    std::string rootName(reinterpret_cast<const char*>(root->name));
    std::transform(rootName.begin(), rootName.end(), rootName.begin(), ::tolower);
    
    if ( rootName != "package" )
        return false;       // not an OPF file, innit?
    
    XPathWrangler::NamespaceList nsList = {
        { "opf", OPFNamespace },
        { "dc", DCNamespace }
    };
    XPathWrangler xpath(_opf, nsList);
    
    // simple things: manifest and spine items
    xmlNodeSetPtr manifestNodes = nullptr;
    xmlNodeSetPtr spineNodes = nullptr;
    
    try
    {
        manifestNodes = xpath.Nodes("/opf:package/opf:manifest/opf:item");
        spineNodes = xpath.Nodes("/opf:package/opf:spine/opf:itemref");
        
        if ( manifestNodes == nullptr || spineNodes == nullptr )
            throw false;   // looks invalid, or at least unusable, to me
        
        for ( int i = 0; i < manifestNodes->nodeNr; i++ )
        {
            ManifestItem *p = new ManifestItem(manifestNodes->nodeTab[i], this);
            _manifest[p->Identifier()] = p;
        }
        
        SpineItem * cur = nullptr;
        for ( int i = 0; i < spineNodes->nodeNr; i++ )
        {
            SpineItem *next = new SpineItem(spineNodes->nodeTab[i], this);
            if ( cur != nullptr )
                cur->SetNextItem(next);
            else
                _spine = cur = next;
        }
    }
    catch (...)
    {
        if ( manifestNodes != nullptr )
            xmlXPathFreeNodeSet(manifestNodes);
        if ( spineNodes != nullptr )
            xmlXPathFreeNodeSet(spineNodes);
        return false;
    }
    
    xmlXPathFreeNodeSet(manifestNodes);
    xmlXPathFreeNodeSet(spineNodes);
    
    // now the metadata, which is slightly more involved due to extensions
    xmlNodeSetPtr metadataNodes = nullptr;
    xmlNodeSetPtr refineNodes = xmlXPathNodeSetCreate(nullptr);
    
    try
    {
        metadataNodes = xpath.Nodes("/opf:package/opf:metadata/*");
        if ( metadataNodes == nullptr )
            throw false;
        
        for ( int i = 0; i < metadataNodes->nodeNr; i++ )
        {
            xmlNodePtr node = metadataNodes->nodeTab[i];
            class Metadata *p = nullptr;
            
            if ( node->ns != nullptr && xmlStrcmp(node->ns->href, BAD_CAST DCNamespace) == 0 )
            {
                // definitely a main node
                p = new class Metadata(node);
            }
            else if ( xmlGetProp(node, BAD_CAST "refines") == nullptr )
            {
                // not refining anything, so it's a main node
                p = new class Metadata(node);
            }
            else
            {
                // by elimination it's refining something-- we'll process it later when we know we've got all the main nodes in there
                xmlXPathNodeSetAdd(refineNodes, node);
            }
            
            if ( p != nullptr )
                _metadata[p->Identifier()] = p;
        }
        
        for ( int i = 0; i < refineNodes->nodeNr; i++ )
        {
            xmlNodePtr node = refineNodes->nodeTab[i];
            std::string ident = _getProp(node, "refines");
            if ( ident.empty() )
                continue;
            
            auto found = _metadata.find(ident);
            if ( found == _metadata.end() )
                continue;
            
            found->second->AddExtension(node);
        }
    }
    catch (...)
    {
        if ( metadataNodes != nullptr )
            xmlXPathFreeNodeSet(metadataNodes);
        if ( refineNodes != nullptr )
            xmlXPathFreeNodeSet(refineNodes);
        return false;
    }
    
    xmlXPathFreeNodeSet(metadataNodes);
    xmlXPathFreeNodeSet(refineNodes);
    
    return true;
}

EPUB3_END_NAMESPACE
