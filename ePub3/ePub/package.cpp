//
//  package.cpp
//  ePub3
//
//  Created by Jim Dovey on 2012-11-28.
//  Copyright (c) 2012-2013 The Readium Foundation.
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
#include "metadata.h"
#include "archive.h"
#include "archive_xml.h"
#include "xpath_wrangler.h"
#include "nav_table.h"
#include "glossary.h"
#include <sstream>
#include <list>
#include <regex>
#include <libxml/xpathInternals.h>

static const char * OPFNamespace = "http://www.idpf.org/2007/opf";
static const char * DCNamespace = "http://purl.org/dc/elements/1.1/";

EPUB3_BEGIN_NAMESPACE

const Package::PropertyVocabularyMap PackageBase::gReservedVocabularies({
    { "", "http://idpf.org/epub/vocab/package/#" },
    { "dcterms", "http://purl.org/dc/terms/" },
    { "marc", "http://id.loc.gov/vocabulary/" },
    { "media", "http://www.idpf.org/epub/vocab/overlays/#" },
    { "onix", "http://www.editeur.org/ONIX/book/codelists/current.html#" },
    { "xsd", "http://www.w3.org/2001/XMLSchema#" }
});

PackageBase::PackageBase(Archive* archive, const string& path, const string& type) : _archive(archive), _opf(nullptr), _type(type), _vocabularyLookup(gReservedVocabularies)
{
    if ( _archive == nullptr )
        throw std::invalid_argument("Path does not point to a recognised archive file: " + path.stl_str());
    
    // TODO: Initialize lazily? Doing so would make initialization faster, but require
    // PackageLocations() to become non-const, like Packages().
    ArchiveXmlReader reader(_archive->ReaderAtPath(path.stl_str()));
    _opf = reader.xmlReadDocument(path.c_str(), "utf-8", XML_PARSE_RECOVER|XML_PARSE_NOENT|XML_PARSE_DTDATTR);
    if ( _opf == nullptr )
        throw std::invalid_argument(std::string(__PRETTY_FUNCTION__) + ": No OPF file at " + path.stl_str());
    
    size_t loc = path.rfind("/");
    if ( loc == std::string::npos )
    {
        _pathBase = '/';
    }
    else
    {
        _pathBase = path.substr(0, loc+1);
    }
}
PackageBase::PackageBase(PackageBase&& o) : _archive(o._archive), _opf(o._opf), _pathBase(std::move(o._pathBase)), _type(std::move(o._type)), _metadata(std::move(o._metadata)), _manifest(std::move(o._manifest)), _spine(std::move(o._spine)), _vocabularyLookup(std::move(o._vocabularyLookup))
{
    o._archive = nullptr;
    o._opf = nullptr;
}
PackageBase::~PackageBase()
{
    for ( auto item : _metadata )
    {
        delete item;
    }
    for ( auto item : _manifest )
    {
        delete item.second;
    }
    for ( auto item : _navigation )
    {
        delete item.second;
    }
    
    // our Container owns the archive
    if ( _opf != nullptr )
        xmlFreeDoc(_opf);
}
const SpineItem* PackageBase::SpineItemAt(size_t idx) const
{
    const SpineItem* item = _spine.get();
    for ( size_t i = 0; i < idx && item != nullptr; i++ )
    {
        item = item->Next();
    }
    return item;
}
size_t PackageBase::IndexOfSpineItemWithIDRef(const string &idref) const
{
    const SpineItem* item = FirstSpineItem();
    for ( size_t i = 0; item != nullptr; i++, item = item->Next() )
    {
        if ( item->Idref() == idref )
            return i;
    }
    
    return size_t(-1);
}
const ManifestItem* PackageBase::ManifestItemWithID(const string &ident) const
{
    auto found = _manifest.find(ident);
    if ( found == _manifest.end() )
        return nullptr;
    
    return found->second;
}
string PackageBase::CFISubpathForManifestItemWithID(const string &ident) const
{
    size_t sz = IndexOfSpineItemWithIDRef(ident);
    if ( sz == size_t(-1) )
        throw std::invalid_argument(_Str("Identifier '", ident, "' was not found in the spine."));
    
    return _Str(_spineCFIIndex, "/", sz*2, "[", ident, "]!");
}
const std::vector<const ManifestItem*> PackageBase::ManifestItemsWithProperties(PropertyList properties) const
{
    std::vector<const ManifestItem*> result;
    for ( auto item : _manifest )
    {
        if ( item.second->HasProperty(properties) )
            result.push_back(item.second);
    }
    return result;
}
const NavigationTable* PackageBase::NavigationTable(const string &title) const
{
    auto found = _navigation.find(title);
    if ( found == _navigation.end() )
        return nullptr;
    return found->second;
}
void PackageBase::RegisterPrefixIRIStem(const string &prefix, const string &iriStem)
{
    _vocabularyLookup[prefix] = iriStem;
}
IRI PackageBase::MakePropertyIRI(const string &reference, const string& prefix) const
{
    auto found = _vocabularyLookup.find(prefix);
    if ( found == _vocabularyLookup.end() )
        throw UnknownPrefix(_Str("Unknown prefix '", prefix, "'"));
    return IRI(found->second + reference);
}
IRI PackageBase::PropertyIRIFromAttributeValue(const string &attrValue) const
{
    static std::regex re("^(?:(.+?):)?(.+)$");
    std::smatch pieces;
    if ( std::regex_match(attrValue.stl_str(), pieces, re) == false )
        throw std::invalid_argument(_Str("Attribute '", attrValue, "' doesn't look like a property name to me"));
    
    // there are two captures, at indices 1 and 2
    return MakePropertyIRI(pieces[2], pieces[1]);
}
void PackageBase::InstallPrefixesFromAttributeValue(const ePub3::string &attrValue)
{
    if ( attrValue.empty() )
        return;
    
    static std::regex re(R"X(^(.+?): *(.+?)\s+)X");
    auto pos = std::sregex_iterator(attrValue.stl_str().begin(), attrValue.stl_str().end(), re);
    auto end = std::sregex_iterator();
    
    for ( ; pos != end; ++pos )
    {
        if ( pos->size() == 3 )     // entire match plus two captures
        {
            const std::smatch& match = *pos;
            RegisterPrefixIRIStem(match[1].str(), match[2].str());
        }
    }
}
const SpineItem* PackageBase::ConfirmOrCorrectSpineItemQualifier(const SpineItem* pItem, CFI::Component *pComponent) const
{
    if ( pComponent->HasQualifier() && pItem->Idref() != pComponent->qualifier )
    {
        // find the item with the qualifier
        pItem = _spine.get();
        uint32_t idx = 2;
        
        while ( pItem != nullptr )
        {
            if ( pItem->Idref() == pComponent->qualifier )
            {
                // found it-- correct the CFI
                pComponent->nodeIndex = idx;
                break;
            }
            
            pItem = pItem->Next();
        }
    }
    
    return pItem;
}
NavigationList PackageBase::NavTablesFromManifestItem(const ManifestItem* pItem)
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
        tables.push_back(new class NavigationTable(navNode));
    }
    
    xmlXPathFreeNodeSet(nodes);
    
    // now look for any <dl> nodes with an epub:type of "glossary"
    nodes = xpath.Nodes("//html:dl[epub:type='glossary']");
    
    return tables;
}

#if 0
#pragma mark - Package High-Level API
#endif

Package::Package(Archive* archive, const string& path, const string& type) : PackageBase(archive, path, type)
{
    if ( !Unpack() )
        throw std::invalid_argument(_Str(__PRETTY_FUNCTION__, ": Not a valid OPF file at ", path));
}
bool Package::Unpack()
{
    // very basic sanity check
    xmlNodePtr root = xmlDocGetRootElement(_opf);
    string rootName(reinterpret_cast<const char*>(root->name));
    rootName.tolower();
    
    if ( rootName != "package" )
        return false;       // not an OPF file, innit?
    
    InstallPrefixesFromAttributeValue(_getProp(root, "prefix", ePub3NamespaceURI));
    
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
            _manifest.emplace(p->Identifier(), p);
        }
        
        SpineItem* cur = nullptr;
        for ( int i = 0; i < spineNodes->nodeNr; i++ )
        {
            SpineItem* next = new SpineItem(spineNodes->nodeTab[i], this);
            if ( cur != nullptr )
            {
                cur->SetNextItem(next);
            }
            else
            {
                _spine.reset(next);
            }
            
            cur = next;
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
        
        std::map<string, class Metadata*> metadataByID;
        
        for ( int i = 0; i < metadataNodes->nodeNr; i++ )
        {
            xmlNodePtr node = metadataNodes->nodeTab[i];
            class Metadata* p = nullptr;
            
            if ( node->ns != nullptr && xmlStrcmp(node->ns->href, BAD_CAST DCNamespace) == 0 )
            {
                // definitely a main node
                p = new class Metadata(node, this);
            }
            else if ( xmlGetProp(node, BAD_CAST "refines") == nullptr )
            {
                // not refining anything, so it's a main node
                p = new class Metadata(node, this);
            }
            else
            {
                // by elimination it's refining something-- we'll process it later when we know we've got all the main nodes in there
                xmlXPathNodeSetAdd(refineNodes, node);
            }
            
            if ( p != nullptr )
            {
                _metadata.push_back(p);
                if ( !p->Identifier().empty() )
                    metadataByID[p->Identifier()] = p;
            }
        }
        
        for ( int i = 0; i < refineNodes->nodeNr; i++ )
        {
            xmlNodePtr node = refineNodes->nodeTab[i];
            string ident = _getProp(node, "refines");
            if ( ident.empty() )
                continue;
            
            if ( ident[0] == '#' )
                ident = ident.substr(1);
            
            auto found = metadataByID.find(ident);
            if ( found == metadataByID.end() )
                continue;
            
            found->second->AddExtension(node, this);
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
            _navigation.emplace(table->Title(), dynamic_cast<class NavigationTable*>(table));
        }
    }
    
    return true;
}

string Package::UniqueID() const
{
    string packageID = PackageID();
    if ( packageID.empty() )
        return string::EmptyString;
    
    string modDate = ModificationDate();
    if ( modDate.empty() )
        return packageID;
    
    return _Str(packageID, "@", modDate);
}
string Package::PackageID() const
{
    XPathWrangler xpath(_opf, {{"opf", OPFNamespace}, {"dc", DCNamespace}});
    XPathWrangler::StringList strings = xpath.Strings("//*[@id=/opf:package/@unique-identifier]/text()");
    if ( strings.empty() )
        return string::EmptyString;
    return strings[0];
}
string Package::Version() const
{
    return _getProp(xmlDocGetRootElement(_opf), "version");
}
const PackageBase::MetadataMap Package::MetadataItemsWithDCType(Metadata::DCType type) const
{
    return MetadataItemsWithProperty(IRIForDCType(type));
}
const PackageBase::MetadataMap Package::MetadataItemsWithProperty(const IRI &iri) const
{
    MetadataMap result;
    for ( auto item : _metadata )
    {
        if ( item->Property() == iri )
        {
            result.push_back(item);
        }
        else
        {
            for ( auto extension : item->Extensions() )
            {
                if ( extension->Property() == iri )
                {
                    result.push_back(item);
                    break;
                }
            }
        }
    }
    
    return result;
}
const SpineItem* Package::SpineItemWithIDRef(const string &idref) const
{
    for ( const SpineItem* item = FirstSpineItem(); item != nullptr; item = item->Next() )
    {
        if ( item->Idref() == idref )
            return item;
    }
    
    return nullptr;
}
const CFI Package::CFIForManifestItem(const ManifestItem *item) const
{
    CFI result;
    result._components.emplace_back(_spineCFIIndex);
    result._components.emplace_back(_Str(IndexOfSpineItemWithIDRef(item->Identifier())*2, "[", item->Identifier(), "]!"));
    return result;
}
const CFI Package::CFIForSpineItem(const SpineItem *item) const
{
    CFI result;
    result._components.emplace_back(_spineCFIIndex);
    result._components.emplace_back(_Str(item->Index()*2, "[", item->Idref(), "]!"));
    return result;
}
const ManifestItem* Package::ManifestItemForCFI(ePub3::CFI &cfi, CFI* pRemainingCFI) const
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
        
        if ( pRemainingCFI != nullptr )
            pRemainingCFI->Assign(cfi, 2);
    }
    catch (std::out_of_range& e)
    {
        throw CFI::InvalidCFI("CFI references out-of-range spine item");
    }
    
    return result;
}
const string Package::Title() const
{
    IRI titleTypeIRI(MakePropertyIRI("title-type"));      // http://idpf.org/epub/vocab/package/#title-type
    
    // find the main one
    for ( auto item : MetadataItemsWithProperty(titleTypeIRI) )
    {
        const Metadata::Extension* extension = item->ExtensionWithProperty(titleTypeIRI);
        if ( extension == nullptr )
            continue;
        
        if ( extension->Value() == "main" )
            return item->Value();
    }
    
    // no 'main title' found: just get the dc:title value
    const MetadataMap items = MetadataItemsWithDCType(Metadata::DCType::Title);
    if ( items.empty() )
        return string::EmptyString;
    return items[0];
}
const string Package::Subtitle() const
{
    IRI titleTypeIRI(MakePropertyIRI("title-type"));      // http://idpf.org/epub/vocab/package/#title-type
    
    // find the main one
    for ( auto item : MetadataItemsWithProperty(titleTypeIRI) )
    {
        const Metadata::Extension* extension = item->ExtensionWithProperty(titleTypeIRI);
        if ( extension == nullptr )
            continue;
        
        if ( extension->Value() == "subtitle" )
            return item->Value();
    }
    
    // no 'subtitle' found, so no subtitle
    return string::EmptyString;
}
const string Package::FullTitle() const
{
    MetadataMap items = MetadataItemsWithDCType(Metadata::DCType::Title);
    if ( items.size() == 1 )
        return items[0];
    
    IRI displaySeqIRI(MakePropertyIRI("display-seq"));  // http://idpf.org/epub/vocab/package/#display-seq
    std::vector<string> titles(items.size());
    
    MetadataMap sequencedItems = MetadataItemsWithProperty(displaySeqIRI);
    if ( !sequencedItems.empty() )
    {
        // all these have a 1-based sequence number
        for ( auto item : sequencedItems )
        {
            const Metadata::Extension* extension = item->ExtensionWithProperty(displaySeqIRI);
            size_t sz = strtoul(extension->Value().c_str(), nullptr, 10) - 1;
            titles[sz] = item->Value();
        }
    }
    else
    {
        titles.clear();
        
        // insert any non-sequenced items at the head of the list, in order
        for ( auto item : items )
        {
            titles.emplace_back(item->Value());
        }
    }
    
    // put them all together now
    auto pos = titles.begin();
    
    // TODO: this ought to be localized based on the value of Language().
    std::stringstream ss;
    ss << *(pos++) << ": " << *(pos++);
    while ( pos != titles.end() )
    {
        ss << ", " << *(pos++);
    }
    
    return string(ss.str());
}
const Package::AttributionList Package::AuthorNames() const
{
    AttributionList result;
    for ( auto item : MetadataItemsWithDCType(Metadata::DCType::Creator) )
    {
        result.emplace_back(item->Value());
    }
    return result;
}
const Package::AttributionList Package::AttributionNames() const
{
    AttributionList result;
    IRI fileAsIRI(MakePropertyIRI("file-as"));
    for ( auto item : MetadataItemsWithDCType(Metadata::DCType::Creator) )
    {
        const Metadata::Extension* extension = item->ExtensionWithProperty(fileAsIRI);
        if ( extension != nullptr )
            result.emplace_back(extension->Value());
        else
            result.emplace_back(item->Value());
    }
    return result;
}
const string Package::Authors() const
{
    // TODO: handle localization
    AttributionList authors = AuthorNames();
    if ( authors.size() == 1 )
        return authors[0];
    else if ( authors.size() == 2 )
        return _Str(authors[0], " and ", authors[1]);
    
    std::stringstream ss;
    auto pos = authors.begin();
    auto last = pos + (authors.size() - 1);
    while ( pos != last )
    {
        ss << *(pos++) << ", ";
    }
    
    ss << "and " << *last;
    return string(ss.str());
}
const string Package::Language() const
{
    MetadataMap items = MetadataItemsWithDCType(Metadata::DCType::Language);
    if ( items.empty() )
        return string::EmptyString;
    return items[0]->Value();
}
const string Package::Source() const
{
    MetadataMap items = MetadataItemsWithDCType(Metadata::DCType::Source);
    if ( items.empty() )
        return string::EmptyString;
    return items[0]->Value();
}
const string Package::CopyrightOwner() const
{
    MetadataMap items = MetadataItemsWithDCType(Metadata::DCType::Rights);
    if ( items.empty() )
        return string::EmptyString;
    return items[0]->Value();
}
const string Package::ModificationDate() const
{
    MetadataMap items = MetadataItemsWithProperty(MakePropertyIRI("modified", "dcterms"));
    if ( items.empty() )
        return string::EmptyString;
    return items[0]->Value();
}
const string Package::ISBN() const
{
    for ( auto item : MetadataItemsWithDCType(Metadata::DCType::Identifier) )
    {
        if ( item->ExtensionWithProperty(MakePropertyIRI("identifier-type")) == nullptr )
            continue;
        
        // this will be complicated...
        // TODO: Implementation of ISBN lookup
    }
    
    return string::EmptyString;
}
const Package::StringList Package::Subjects() const
{
    StringList result;
    for ( auto item : MetadataItemsWithDCType(Metadata::DCType::Subject) )
    {
        result.emplace_back(item->Value());
    }
    return result;
}

EPUB3_END_NAMESPACE
