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
#include "nav_table.h"
#include "glossary.h"
#include <sstream>
#include <list>
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
    
    size_t loc = path.find_last_of('/');
    if ( loc == std::string::npos )
    {
        _pathBase = '/';
    }
    else
    {
        _pathBase = path.substr(0, loc+1);
    }
}
Package::Package(Package&& o) : _archive(o._archive), _opf(o._opf), _pathBase(std::move(o._pathBase)), _type(std::move(o._type)), _metadata(std::move(o._metadata)), _manifest(std::move(o._manifest)), _spine(std::move(o._spine))
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
const class NavigationTable* Package::NavigationTable(const std::string &title) const
{
    auto found = _navigation.find(title);
    if ( found == _navigation.end() )
        return nullptr;
    return found->second;
}
const ManifestItem* Package::ManifestItemForCFI(ePub3::CFI &cfi, CFI* pRemainingCFI) const throw (CFI::InvalidCFI)
{
    const ManifestItem* result = nullptr;
    
    // NB: Package is a friend of CFI, so it can access the components directly
    if ( cfi._components.size() < 2 )
        throw CFI::InvalidCFI("CFI contains less than 2 nodes, so is invalid for package-based lookups.");
    
    // first item directs us to the Spine: check the index against the one we know
    auto component = cfi._components[0];
    if ( component.nodeIndex != _spineCFIIndex )
    {
        throw CFI::InvalidCFI(_Str("CFI first node index (spine) is ", component.nodeIndex, " but should be ", _spineCFIIndex));
    }
    
    // second component is the particular spine item
    component = cfi._components[1];
    if ( !component.IsIndirector() )
        throw CFI::InvalidCFI("Package-based CFI's second item must be an indirector");
    
    try
    {
        if ( (component.nodeIndex % 2) == 1 )
            throw CFI::InvalidCFI("CFI spine item index is odd, which makes no sense for always-empty spine nodes.");
        const SpineItem* item = _spine->at(component.nodeIndex/2);
        
        // check and correct any qualifiers
        item = ConfirmOrCorrectSpineItemQualifier(item, &component);
        if ( item == nullptr )
            throw CFI::InvalidCFI("CFI spine node qualifier doesn't match any spine item idref");
        
        // we know it's not null, because SpineItem::at() throws an exception if out of range
        result = ManifestItemWithID(item->Idref());
    }
    catch (std::out_of_range& e)
    {
        throw CFI::InvalidCFI("CFI references out-of-range spine item");
    }
    
    return result;
}
bool Package::Unpack()
{
    // very basic sanity check
    xmlNodePtr root = xmlDocGetRootElement(_opf);
    std::string rootName(reinterpret_cast<const char*>(root->name));
    std::transform(rootName.begin(), rootName.end(), rootName.begin(), ::tolower);
    
    if ( rootName != "package" )
        return false;       // not an OPF file, innit?
    
    // go through children to determine the CFI index of the <spine> tag
    static const xmlChar* kSpineName = BAD_CAST "spine";
    _spineCFIIndex = 0;
    xmlNodePtr child = root->children;
    while ( child != nullptr )
    {
        if ( child->type == XML_ELEMENT_NODE )
        {
            _spineCFIIndex += 2;
            if ( xmlStrEqual(child->name, kSpineName) )
                break;
        }
        
        child = child->next;
    }
    
    if ( _spineCFIIndex == 0 )
        return false;       // spineless!
    
    XPathWrangler xpath(_opf, {{"opf", OPFNamespace}, {"dc", DCNamespace}});
    
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
    
    // now the navigation tables
    for ( auto item : _manifest )
    {
        if ( !item.second->HasProperty(ItemProperties::Navigation) )
            continue;
        
        NavigationList tables = NavTablesFromManifestItem(item.second);
        for ( auto table : tables )
        {
            // have to dynamic_cast these guys to get the right pointer type
            _navigation[table->Title()] = dynamic_cast<class NavigationTable*>(table);
        }
    }
    
    return true;
}
const SpineItem* Package::ConfirmOrCorrectSpineItemQualifier(const SpineItem *pItem, CFI::Component *pComponent) const
{
    if ( pComponent->HasQualifier() && pItem->Idref() != pComponent->qualifier )
    {
        // find the item with the qualifier
        pItem = _spine;
        uint32_t idx = 2;
        
        while ( pItem != nullptr )
        {
            if ( pItem->Idref() == pComponent->qualifier )
            {
                // found it-- correct the CFI
                pComponent->nodeIndex = idx;
                break;
            }
        }
    }
    
    return pItem;
}
NavigationList Package::NavTablesFromManifestItem(const ManifestItem *pItem)
{
    if ( pItem == nullptr )
        return NavigationList();
    
    xmlDocPtr doc = pItem->ReferencedDocument();
    if ( doc == nullptr )
        return NavigationList();
    
    // find each <nav> node
    XPathWrangler xpath(doc, {{"epub", ePub3NamespaceURI}}); // goddamn I love C++11 initializer list constructors
    xpath.NameDefaultNamespace("html");
    
    xmlNodeSetPtr nodes = xpath.Nodes("//html:nav");
    
    NavigationList tables;
    for ( size_t i = 0; i < nodes->nodeNr; i++ )
    {
        xmlNodePtr navNode = nodes->nodeTab[i];
        
        auto p = new class NavigationTable(navNode);
        if ( p != nullptr )
            tables.push_back(p);
    }
    
    xmlXPathFreeNodeSet(nodes);
    
    // now look for any <dl> nodes with an epub:type of "glossary"
    nodes = xpath.Nodes("//html:dl[epub:type='glossary']");
    
    
    return tables;
}

EPUB3_END_NAMESPACE
