//
//  package.cpp
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

#include "container.h"
#include "package.h"
#include "archive.h"
#include "archive_xml.h"
#include "xpath_wrangler.h"
#include "nav_table.h"
#include "glossary.h"
#include "iri.h"
#include "basic.h"
#include "byte_stream.h"
#include <ePub3/utilities/error_handler.h>
#include <sstream>
#include <list>
#include REGEX_INCLUDE
#include <libxml/xpathInternals.h>
#include "media-overlays_smil_utils.h"

void PrintNodeSet(xmlNodeSetPtr nodeSet)
{
    xmlBufferPtr buf = xmlBufferCreate();
    for ( int i = 0; i < nodeSet->nodeNr; i++ )
    {
        xmlNodePtr node = nodeSet->nodeTab[i];
        fprintf(stderr, "Node %02d: ", i);
        
        if ( node == nullptr )
        {
            fprintf(stderr, "[nullptr]");
        }
        else
        {
            xmlNodeDump(buf, node->doc, node, 0, 0);
            fprintf(stderr, "%s", reinterpret_cast<const char*>(xmlBufferContent(buf)));
            xmlBufferEmpty(buf);
        }
        
        fprintf(stderr, "\n");
    }
    
    xmlBufferFree(buf);
}

EPUB3_BEGIN_NAMESPACE

#if EPUB_COMPILER_SUPPORTS(CXX_USER_LITERALS)
static const xmlChar * OPFNamespace = "http://www.idpf.org/2007/opf"_xml;
static const xmlChar * DCNamespace = "http://purl.org/dc/elements/1.1/"_xml;
static const xmlChar * MediaTypeElementName = "mediaType"_xml;
#else
static const xmlChar * OPFNamespace = (const xmlChar*)"http://www.idpf.org/2007/opf";
static const xmlChar * DCNamespace = (const xmlChar*)"http://purl.org/dc/elements/1.1/";
static const xmlChar * MediaTypeElementName = (const xmlChar*)"mediaType";
#endif

bool Package::gValidateSchema = true;

PackageBase::PackageBase(const shared_ptr<Container>& owner, const string& type) : _archive(owner->GetArchive()), _opf(nullptr), _type(type)
{
    if ( !_archive )
        throw std::invalid_argument("Owner doesn't have an archive!");
}
PackageBase::PackageBase(PackageBase&& o) : _archive(o._archive), _opf(o._opf), _pathBase(std::move(o._pathBase)), _type(std::move(o._type)), _manifest(std::move(o._manifest)), _spine(std::move(o._spine))
{
    o._archive = nullptr;
    o._opf = nullptr;
}
PackageBase::~PackageBase()
{
    // our Container owns the archive
    if ( _opf != nullptr )
        xmlFreeDoc(_opf);
}
bool PackageBase::Open(const string& path)
{
    ArchiveXmlReader reader(_archive->ReaderAtPath(path.stl_str()));
    _opf = reader.xmlReadDocument(path.c_str(), nullptr, XML_PARSE_RECOVER|XML_PARSE_NOENT|XML_PARSE_DTDATTR);
    if ( _opf == nullptr )
    {
        HandleError(EPUBError::OCFInvalidRootfileURL, _Str(__PRETTY_FUNCTION__, ": No OPF file at ", path.stl_str()));
        return false;
    }
    
    size_t loc = path.rfind("/");
    if ( loc == std::string::npos )
    {
        _pathBase = '/';
    }
    else
    {
        _pathBase = path.substr(0, loc+1);
    }
    
    return true;
}
shared_ptr<SpineItem> PackageBase::SpineItemAt(size_t idx) const
{
    shared_ptr<SpineItem> item = _spine;
    for ( size_t i = 0; i < idx && item != nullptr; i++ )
    {
        item = item->Next();
    }
    return item;
}
size_t PackageBase::IndexOfSpineItemWithIDRef(const string &idref) const
{
    shared_ptr<SpineItem> item = FirstSpineItem();
    for ( size_t i = 0; item != nullptr; i++, item = item->Next() )
    {
        if ( item->Idref() == idref )
            return i;
    }
    
    return size_t(-1);
}
shared_ptr<ManifestItem> PackageBase::ManifestItemWithID(const string &ident) const
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
const shared_vector<ManifestItem> PackageBase::ManifestItemsWithProperties(PropertyIRIList properties) const
{
    shared_vector<ManifestItem> result;
    for ( auto& item : _manifest )
    {
        if ( item.second->HasProperty(properties) )
            result.push_back(item.second);
    }
    return result;
}
shared_ptr<NavigationTable> PackageBase::NavigationTable(const string &title) const
{
    auto found = _navigation.find(title);
    if ( found == _navigation.end() )
        return nullptr;
    return found->second;
}
unique_ptr<ByteStream> PackageBase::ReadStreamForItemAtPath(const string &path) const
{
    return _archive->ByteStreamAtPath(path.stl_str());
}
shared_ptr<SpineItem> PackageBase::ConfirmOrCorrectSpineItemQualifier(shared_ptr<SpineItem> pItem, CFI::Component *pComponent) const
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
            
            pItem = pItem->Next();
        }
    }
    else if ( pComponent->HasQualifier() == false )
    {
        HandleError(EPUBError::CFINonAssertedXMLID);
    }
    
    return pItem;
}
NavigationList PackageBase::NavTablesFromManifestItem(shared_ptr<PackageBase> owner, shared_ptr<ManifestItem> pItem)
{
    PackagePtr sharedPkg = std::dynamic_pointer_cast<Package>(owner);
    if ( !sharedPkg )
        return NavigationList();
    
    if ( pItem == nullptr )
        return NavigationList();
    
    xmlDocPtr doc = pItem->ReferencedDocument();
    if ( doc == nullptr )
        return NavigationList();
    
    // find each <nav> node
#if EPUB_COMPILER_SUPPORTS(CXX_INITIALIZER_LISTS)
    XPathWrangler xpath(doc, {{"epub", ePub3NamespaceURI}}); // goddamn I love C++11 initializer list constructors
#else
    XPathWrangler::NamespaceList __m;
    __m["epub"] = ePub3NamespaceURI;
    XPathWrangler xpath(doc, __m);
#endif
    xpath.NameDefaultNamespace("html");
    
    xmlNodeSetPtr nodes = xpath.Nodes("//html:nav");
    
    NavigationList tables;
    for ( int i = 0; i < nodes->nodeNr; i++ )
    {
        xmlNodePtr navNode = nodes->nodeTab[i];
        auto navTablePtr = std::make_shared<class NavigationTable>(sharedPkg, pItem->Href());
        if ( navTablePtr->ParseXML(navNode) )
            tables.push_back(navTablePtr);
    }
    
    xmlXPathFreeNodeSet(nodes);
    
    // now look for any <dl> nodes with an epub:type of "glossary"
    nodes = xpath.Nodes("//html:dl[epub:type='glossary']");
    
    return tables;
}

#if 0
#pragma mark - Package High-Level API
#endif

Package::Package(const shared_ptr<Container>& owner, const string& type) : PropertyHolder(), OwnedBy(owner), PackageBase(owner, type)
{
}
bool Package::Open(const string& path)
{
    return PackageBase::Open(path) && Unpack();
}
bool Package::_OpenForTest(xmlDocPtr doc, const string& basePath)
{
    _opf = doc;
    _pathBase = basePath;
    return Unpack();
}
bool Package::Unpack()
{
    PackagePtr sharedMe = shared_from_this();
    
    // very basic sanity check
    xmlNodePtr root = xmlDocGetRootElement(_opf);
    string rootName(reinterpret_cast<const char*>(root->name));
    rootName.tolower();
    
    if ( rootName != "package" )
    {
        HandleError(EPUBError::OPFInvalidPackageDocument);
        return false;       // not an OPF file, innit?
    }
    if ( _getProp(root, "version").empty() )
    {
        HandleError(EPUBError::OPFPackageHasNoVersion);
    }
    
    InstallPrefixesFromAttributeValue(_getProp(root, "prefix", ePub3NamespaceURI));
    
    // go through children to determine the CFI index of the <spine> tag
    static const xmlChar* kSpineName = BAD_CAST "spine";
    static const xmlChar* kManifestName = BAD_CAST "manifest";
    static const xmlChar* kMetadataName = BAD_CAST "metadata";
    _spineCFIIndex = 0;
    uint32_t idx = 0;
    xmlNodePtr child = xmlFirstElementChild(root);
    while ( child != nullptr )
    {
        idx += 2;
        if ( xmlStrEqual(child->name, kSpineName) )
        {
            _spineCFIIndex = idx;
            if ( _spineCFIIndex != 6 )
                HandleError(EPUBError::OPFSpineOutOfOrder);
        }
        else if ( xmlStrEqual(child->name, kManifestName) && idx != 4 )
        {
            HandleError(EPUBError::OPFManifestOutOfOrder);
        }
        else if ( xmlStrEqual(child->name, kMetadataName) && idx != 2 )
        {
            HandleError(EPUBError::OPFMetadataOutOfOrder);
        }
        
        child = xmlNextElementSibling(child);
    }
    
    if ( _spineCFIIndex == 0 )
    {
        HandleError(EPUBError::OPFNoSpine);
        return false;       // spineless!
    }
    
#if EPUB_COMPILER_SUPPORTS(CXX_INITIALIZER_LISTS)
    XPathWrangler xpath(_opf, {{"opf", OPFNamespace}, {"dc", DCNamespace}});
#else
    XPathWrangler::NamespaceList __m;
    __m["opf"] = OPFNamespace;
    __m["dc"] = DCNamespace;
    XPathWrangler xpath(_opf, __m);
#endif
    
    // simple things: manifest and spine items
    xmlNodeSetPtr manifestNodes = nullptr;
    xmlNodeSetPtr spineNodes = nullptr;
    
    try
    {
        manifestNodes = xpath.Nodes("/opf:package/opf:manifest/opf:item");
        spineNodes = xpath.Nodes("/opf:package/opf:spine/opf:itemref");
        
        if ( manifestNodes == nullptr )
        {
            HandleError(EPUBError::OPFNoManifestItems);
        }
        
        if ( spineNodes == nullptr )
        {
            HandleError(EPUBError::OPFNoSpineItems);
        }
        
        for ( int i = 0; i < manifestNodes->nodeNr; i++ )
        {
            auto p = std::make_shared<ManifestItem>(sharedMe);
            if ( p->ParseXML(p, manifestNodes->nodeTab[i]) )
            {
#if EPUB_HAVE(CXX_MAP_EMPLACE)
                _manifest.emplace(p->Identifier(), p);
#else
                _manifest[p->Identifier()] = p;
#endif
                StoreXMLIdentifiable(p);
            }
            else
            {
                // TODO: Need an error here
            }
        }
        
        // check fallback chains
        typedef std::map<string, bool> IdentSet;
        IdentSet idents;
        for ( auto &pair : _manifest )
        {
            ManifestItemPtr item = pair.second;
            if ( item->FallbackID().empty() )
                continue;
            
            idents[item->XMLIdentifier()] = true;
            while ( !item->FallbackID().empty() )
            {
                if ( idents[item->FallbackID()] )
                {
                    HandleError(EPUBError::OPFFallbackChainCircularReference);
                    break;
                }
                
                item = item->Fallback();
            }
            
            idents.clear();
        }
        
        SpineItemPtr cur;
        for ( int i = 0; i < spineNodes->nodeNr; i++ )
        {
            auto next = std::make_shared<SpineItem>(sharedMe);
            if ( next->ParseXML(next, spineNodes->nodeTab[i]) == false )
            {
                // TODO: need an error code here
                continue;
            }
            
            // validation of idref
            auto manifestFound = _manifest.find(next->Idref());
            if ( manifestFound == _manifest.end() )
            {
                HandleError(EPUBError::OPFInvalidSpineIdref, _Str(next->Idref(), " does not correspond to a manifest item"));
                continue;
            }
            
            // validation of spine resource type w/fallbacks
            ManifestItemPtr manifestItem = next->ManifestItem();
            bool isContentDoc = false;
            do
            {
                if ( manifestItem->MediaType() == "application/xhtml+xml" ||
                     manifestItem->MediaType() == "image/svg" )
                {
                    isContentDoc = true;
                    break;
                }
                
            } while ( (manifestItem = manifestItem->Fallback()) );
            
            if ( !isContentDoc )
                HandleError(EPUBError::OPFFallbackChainHasNoContentDocument);
            
            StoreXMLIdentifiable(next);
            
            if ( cur != nullptr )
            {
                cur->SetNextItem(next);
            }
            else
            {
                _spine = next;
            }
            
            cur = next;
        }
    }
    catch (const std::system_error& exc)
    {
        if ( manifestNodes != nullptr )
            xmlXPathFreeNodeSet(manifestNodes);
        if ( spineNodes != nullptr )
            xmlXPathFreeNodeSet(spineNodes);
        if ( exc.code().category() == epub_spec_category() )
            throw;
        return false;
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
        shared_ptr<PropertyHolder> holderPtr = std::dynamic_pointer_cast<PropertyHolder>(sharedMe);
        metadataNodes = xpath.Nodes("/opf:package/opf:metadata/*");
        if ( metadataNodes == nullptr )
            HandleError(EPUBError::OPFNoMetadata);
        
        bool foundIdentifier = false, foundTitle = false, foundLanguage = false, foundModDate = false;
        string uniqueIDRef = _getProp(root, "unique-identifier");
        if ( uniqueIDRef.empty() )
            HandleError(EPUBError::OPFPackageUniqueIDInvalid);
        
        for ( int i = 0; i < metadataNodes->nodeNr; i++ )
        {
            xmlNodePtr node = metadataNodes->nodeTab[i];
            PropertyPtr p;
            
            if ( node->ns != nullptr && xmlStrcmp(node->ns->href, BAD_CAST DCNamespace) == 0 )
            {
                // definitely a main node
                p = std::make_shared<Property>(holderPtr);
            }
            else if ( _getProp(node, "name").size() > 0 )
            {
                // it's an ePub2 item-- ignore it
                continue;
            }
            else if ( _getProp(node, "refines").empty() )
            {
                // not refining anything, so it's a main node
                p = std::make_shared<Property>(holderPtr);
            }
            else
            {
                // by elimination it's refining something-- we'll process it later when we know we've got all the main nodes in there
                xmlXPathNodeSetAdd(refineNodes, node);
            }
            
            if ( p && p->ParseMetaElement(node) )
            {
                switch ( p->Type() )
                {
                    case DCType::Identifier:
                    {
                        foundIdentifier = true;
                        if ( !uniqueIDRef.empty() && uniqueIDRef != p->XMLIdentifier() )
                            HandleError(EPUBError::OPFPackageUniqueIDInvalid);
                        break;
                    }
                    case DCType::Title:
                    {
                        foundTitle = true;
                        break;
                    }
                    case DCType::Language:
                    {
                        foundLanguage = true;
                        break;
                    }
                    case DCType::Custom:
                    {
                        if ( p->PropertyIdentifier() == MakePropertyIRI("modified", "dcterms") )
                            foundModDate = true;
                        break;
                    }
                        
                    default:
                        break;
                }
                
                AddProperty(p);
                StoreXMLIdentifiable(p);
            }
        }
        
        if ( !foundIdentifier )
            HandleError(EPUBError::OPFMissingIdentifierMetadata);
        if ( !foundTitle )
            HandleError(EPUBError::OPFMissingTitleMetadata);
        if ( !foundLanguage )
            HandleError(EPUBError::OPFMissingLanguageMetadata);
        if ( !foundModDate )
            HandleError(EPUBError::OPFMissingModificationDateMetadata);
        
        for ( int i = 0; i < refineNodes->nodeNr; i++ )
        {
            xmlNodePtr node = refineNodes->nodeTab[i];
            string ident = _getProp(node, "refines");
            if ( ident.empty() )
            {
                HandleError(EPUBError::OPFInvalidRefinementAttribute, "Empty IRI for 'refines' attribute");
                continue;
            }
            
            if ( ident[0] == '#' )
            {
                ident = ident.substr(1);
            }
            else
            {
                // validation only right now
                IRI iri(ident);
                if ( iri.IsEmpty() )
                {
                    HandleError(EPUBError::OPFInvalidRefinementAttribute, _Str("#", ident, " is not a valid IRI"));
                }
                else if ( iri.IsRelative() == false )
                {
                    HandleError(EPUBError::OPFInvalidRefinementAttribute, _Str(iri.IRIString(), " is not a relative IRI"));
                }
                continue;
            }
            
            auto found = _xmlIDLookup.find(ident);
            if ( found == _xmlIDLookup.end() )
            {
                HandleError(EPUBError::OPFInvalidRefinementTarget, _Str("#", ident, " does not reference an item in this document"));
                continue;
            }
            
            PropertyPtr prop = std::dynamic_pointer_cast<Property>(found->second);
            if ( prop )
            {
                // it's a property, so this is an extension
                PropertyExtensionPtr extPtr = std::make_shared<PropertyExtension>(prop);
                if ( extPtr->ParseMetaElement(node) )
                    prop->AddExtension(extPtr);
            }
            else
            {
                // not a property, so treat this as a plain property
                shared_ptr<PropertyHolder> ptr = std::dynamic_pointer_cast<PropertyHolder>(found->second);
                if ( ptr )
                {
                    prop = std::make_shared<Property>(ptr);
                    if ( prop->ParseMetaElement(node) )
                        ptr->AddProperty(prop);
                }
            }
        }
        
        // now look at the <spine> element for properties
        xmlNodePtr spineNode = xmlFirstElementChild(root);
        for ( uint32_t i = 2; i < _spineCFIIndex; i += 2 )
            spineNode = xmlNextElementSibling(spineNode);
        
        string value = _getProp(spineNode, "page-progression-direction");
        if ( !value.empty() )
        {
            PropertyPtr prop = std::make_shared<Property>(holderPtr);
            prop->SetPropertyIdentifier(MakePropertyIRI("page-progression-direction"));
            prop->SetValue(value);
            AddProperty(prop);
        }
    }
    catch (std::system_error& exc)
    {
        if ( metadataNodes != nullptr )
            xmlXPathFreeNodeSet(metadataNodes);
        if ( refineNodes != nullptr )
            xmlXPathFreeNodeSet(refineNodes);
        if ( exc.code().category() == epub_spec_category() )
            throw;
        return false;
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
    
    // now any content type bindings
    xmlNodeSetPtr bindingNodes = nullptr;
    
    try
    {
        bindingNodes = xpath.Nodes("/opf:package/opf:bindings/*");
        if ( bindingNodes != nullptr )
        {
            for ( int i = 0; i < bindingNodes->nodeNr; i++ )
            {
                xmlNodePtr node = bindingNodes->nodeTab[i];
                if ( xmlStrcasecmp(node->name, MediaTypeElementName) != 0 )
                    continue;
                
                ////////////////////////////////////////////////////////////
                // ePub Publications 3.0 §3.4.16: The `mediaType` Element
                
                // The media-type attribute is required.
                string mediaType = _getProp(node, "media-type");
                if ( mediaType.empty() )
                {
                    HandleError(EPUBError::OPFBindingHandlerNoMediaType);
                    throw false;
                }
                
                // Each child mediaType of a bindings element must define a unique
                // content type in its media-type attribute, and the media type
                // specified must not be a Core Media Type.
                if ( _contentHandlers[mediaType].empty() == false )
                {
                    // user shouldn't have added manual things yet, but for safety we'll look anyway
                    for ( auto ptr : _contentHandlers[mediaType] )
                    {
                        if ( typeid(*ptr) == typeid(MediaHandler) )
                        {
                            HandleError(EPUBError::OPFMultipleBindingsForMediaType);
                        }
                    }
                }
                if ( CoreMediaTypes.find(mediaType) != CoreMediaTypes.end() )
                {
                    HandleError(EPUBError::OPFCoreMediaTypeBindingEncountered);
                }
                
                // The handler attribute is required
                string handlerID = _getProp(node, "handler");
                if ( handlerID.empty() )
                {
                    HandleError(EPUBError::OPFBindingHandlerNotFound);
                }
                
                // The required handler attribute must reference the ID [XML] of an
                // item in the manifest of the default implementation for this media
                // type. The referenced item must be an XHTML Content Document.
                ManifestItemPtr handlerItem = ManifestItemWithID(handlerID);
                if ( !handlerItem )
                {
                    HandleError(EPUBError::OPFBindingHandlerNotFound);
                }
                if ( handlerItem->MediaType() != "application/xhtml+xml" )
                {
                    
                    HandleError(EPUBError::OPFBindingHandlerInvalidType, _Str("Media handlers must be XHTML content documents, but referenced item has type '", handlerItem->MediaType(), "'."));
                }
                
                // All XHTML Content Documents designated as handlers must have the
                // `scripted` property set in their manifest item's `properties`
                // attribute.
                if ( handlerItem->HasProperty(ItemProperties::HasScriptedContent) == false )
                {
                    HandleError(EPUBError::OPFBindingHandlerNotScripted);
                }
                
                // all good-- install it now
                _contentHandlers[mediaType].push_back(std::make_shared<MediaHandler>(sharedMe, mediaType, handlerItem->AbsolutePath()));
            }
        }
    }
    catch (std::exception& exc)
    {
        std::cerr << "Exception processing OPF file: " << exc.what() << std::endl;
        if ( bindingNodes != nullptr )
            xmlXPathFreeNodeSet(bindingNodes);
        throw;
    }
    catch (...)
    {
        if ( bindingNodes != nullptr )
            xmlXPathFreeNodeSet(bindingNodes);
        return false;
    }
    
    xmlXPathFreeNodeSet(bindingNodes);
    
    // now the navigation tables
    for ( auto item : _manifest )
    {
        if ( !item.second->HasProperty(ItemProperties::Navigation) )
            continue;
        
        NavigationList tables = NavTablesFromManifestItem(sharedMe, item.second);
        for ( auto table : tables )
        {
            // have to dynamic_cast these guys to get the right pointer type
            shared_ptr<class NavigationTable> navTable = std::dynamic_pointer_cast<class NavigationTable>(table);
#if EPUB_HAVE(CXX_MAP_EMPLACE)
            _navigation.emplace(navTable->Type(), navTable);
#else
            _navigation[navTable->Type()] = navTable;
#endif
        }
    }


    PackagePtr sharedPkg = std::dynamic_pointer_cast<Package>(sharedMe);
    //_mediaOverlays = std::make_shared<class MediaOverlaysSmilModel>(sharedPkg);


    // lastly, let's set the media support information
    InitMediaSupport();
    
    return true;
}
void Package::InstallPrefixesFromAttributeValue(const string& attrValue)
{
    if ( attrValue.empty() )
        return;
    
    static REGEX_NS::regex::flag_type reflags(REGEX_NS::regex::ECMAScript|REGEX_NS::regex::optimize);
    static REGEX_NS::regex re("(\\w+):\\s*(.+?)(?:\\s+|$)", reflags);
    auto pos = REGEX_NS::sregex_iterator(attrValue.stl_str().begin(), attrValue.stl_str().end(), re);
    auto end = REGEX_NS::sregex_iterator();
    
    while ( pos != end )
    {
        if ( pos->size() == 3 )     // entire match plus two captures
        {
            if ( pos->str(1) == "_" )
                HandleError(EPUBError::OPFIllegalPrefixDefinition);
            if ( PropertyHolder::ReservedVocabularies.find(pos->str(1)) != PropertyHolder::ReservedVocabularies.end() )
                HandleError(EPUBError::OPFIllegalPrefixRedeclaration);
            for ( auto& pair : PropertyHolder::ReservedVocabularies )
            {
                if ( pair.second == pos->str(2) )
                {
                    HandleError(EPUBError::OPFIllegalVocabularyIRIRedefinition);
                    break;
                }
            }
            
            RegisterPrefixIRIStem(pos->str(1), pos->str(2));
        }
        
        ++pos;
    }
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
string Package::URLSafeUniqueID() const
{
    string packageID = PackageID();
    if ( packageID.empty() )
        return string::EmptyString;
    
    string modDate = ModificationDate();
    if ( modDate.empty() )
        return packageID;
    
    // only include the first ten characters of the modification date (the date part)
    modDate = modDate.substr(0, 10);
    
    // trim the uniqueID if necessary to get the whole thing below 256 characters in length
    string::size_type maxLen = 255, totalLen = packageID.size() + 1 + modDate.size();
    if ( totalLen > maxLen )
    {
        string::size_type diff = totalLen - maxLen;
        packageID = packageID.substr(0, packageID.size() - diff);
    }
    
    return _Str(packageID, '_', modDate);
}
string Package::PackageID() const
{
#if EPUB_COMPILER_SUPPORTS(CXX_INITIALIZER_LISTS)
    XPathWrangler xpath(_opf, {{"opf", OPFNamespace}, {"dc", DCNamespace}});
#else
    XPathWrangler::NamespaceList __m;
    __m["opf"] = OPFNamespace;
    __m["dc"] = DCNamespace;
    XPathWrangler xpath(_opf, __m);
#endif
    XPathWrangler::StringList strings = xpath.Strings("//*[@id=/opf:package/@unique-identifier]/text()");
    if ( strings.empty() )
        return string::EmptyString;
    return strings[0];
}
string Package::Version() const
{
    return _getProp(xmlDocGetRootElement(_opf), "version");
}
void Package::FireLoadEvent(const IRI &url) const
{
    if ( _loadEventHandler == nullptr )
        throw std::runtime_error(_Str("No load event handler installed to load '", url.URIString(), "'"));
    
    if ( url.Path().find(_pathBase) == 0 )
    {
        _loadEventHandler(url);
        return;
    }
    
    IRI fixed(IRI::gEPUBScheme, UniqueID(), _pathBase, url.Query(), url.Fragment());
    fixed.AddPathComponent(url.Path());
    IRI::IRICredentials creds(url.Credentials());
    fixed.SetCredentials(creds.first, creds.second);
    
    _loadEventHandler(fixed);
}
shared_ptr<SpineItem> Package::SpineItemWithIDRef(const string &idref) const
{
    for ( auto item = FirstSpineItem(); item != nullptr; item = item->Next() )
    {
        if ( item->Idref() == idref )
            return item;
    }
    
    return nullptr;
}
const CFI Package::CFIForManifestItem(shared_ptr<ManifestItem> item) const
{
    CFI result;
    result._components.emplace_back(_spineCFIIndex);
    result._components.emplace_back(_Str((IndexOfSpineItemWithIDRef(item->Identifier())+1)*2, "[", item->Identifier(), "]!"));
    return result;
}
const CFI Package::CFIForSpineItem(shared_ptr<SpineItem> item) const
{
    CFI result;
    result._components.emplace_back(_spineCFIIndex);
    result._components.emplace_back(_Str((item->Index()+1)*2, "[", item->Idref(), "]!"));
    return result;
}
shared_ptr<ManifestItem> Package::ManifestItemForCFI(ePub3::CFI &cfi, CFI* pRemainingCFI) const
{
    ManifestItemPtr result;
    
    // NB: Package is a friend of CFI, so it can access the components directly
    if ( cfi._components.size() < 2 )
    {
        HandleError(EPUBError::CFITooShort, "CFI contains less than 2 nodes, so is invalid for package-based lookups.");
    }
    
    // first item directs us to the Spine: check the index against the one we know
    auto component = cfi._components[0];
    if ( component.nodeIndex != _spineCFIIndex )
    {
        HandleError(EPUBError::CFIInvalidSpineLocation, _Str("CFI first node index (spine) is ", component.nodeIndex, " but should be ", _spineCFIIndex));
        
        // fix it ?
        //component.nodeIndex = _spineCFIIndex;
        return nullptr;
    }
    
    // second component is the particular spine item
    component = cfi._components[1];
    if ( !component.IsIndirector() )
    {
        HandleError(EPUBError::CFIUnexpectedComponent, "Package-based CFI's second item must be an indirector");
        return nullptr;
    }
    
    try
    {
        if ( (component.nodeIndex & 1) == 1 )
            throw CFI::InvalidCFI("CFI spine item index is odd, which makes no sense for always-empty spine nodes.");
        SpineItemPtr item = _spine->at((component.nodeIndex>>1)-1);
        
        // check and correct any qualifiers
        item = ConfirmOrCorrectSpineItemQualifier(item, &component);
        if ( item == nullptr )
        {
            HandleError(EPUBError::CFIIndirectionTargetMissing, "CFI spine node qualifier doesn't match any spine item idref");
            return nullptr;
        }
        
        // we know it's not null, because SpineItem::at() throws an exception if out of range
        result = ManifestItemWithID(item->Idref());
        
        if ( pRemainingCFI != nullptr )
            pRemainingCFI->Assign(cfi, 2);
    }
    catch (std::out_of_range& e)
    {
        HandleError(EPUBError::CFIStepOutOfBounds, _Str("CFI references out-of-range spine item: ", e.what()));
    }
    
    return result;
}
unique_ptr<ByteStream> Package::ReadStreamForRelativePath(const string &path) const
{
    return _archive->ByteStreamAtPath(path.stl_str());
}
const string& Package::Title(bool localized) const
{
    IRI titleTypeIRI(MakePropertyIRI("title-type"));      // http://idpf.org/epub/vocab/package/#title-type
    
    // find the main one
    for ( auto& item : PropertiesMatching(titleTypeIRI) )
    {
        PropertyExtensionPtr extension = item->ExtensionWithIdentifier(titleTypeIRI);
        if ( extension == nullptr )
            continue;
        
        if ( extension->Value() == "main" )
            return (localized? item->LocalizedValue() : item->Value());
    }
    
    // no 'main title' found: just get the dc:title value
    auto items = PropertiesMatching(DCType::Title);
    if ( items.empty() )
        return string::EmptyString;
    
    if ( localized )
        return items[0]->LocalizedValue();
    
    return items[0]->Value();
}
const string& Package::Subtitle(bool localized) const
{
    IRI titleTypeIRI(MakePropertyIRI("title-type"));      // http://idpf.org/epub/vocab/package/#title-type
    
    // find the main one
    for ( auto item : PropertiesMatching(titleTypeIRI) )
    {
        PropertyExtensionPtr extension = item->ExtensionWithIdentifier(titleTypeIRI);
        if ( extension == nullptr )
            continue;
        
        if ( extension->Value() == "subtitle" )
        {
            if ( localized )
                return item->LocalizedValue();
            return item->Value();
        }
    }
    
    // no 'subtitle' found, so no subtitle
    return string::EmptyString;
}
const string& Package::ShortTitle(bool localized) const
{
    IRI titleTypeIRI(MakePropertyIRI("title-type"));      // http://idpf.org/epub/vocab/package/#title-type
    
    // find the main one
    for ( auto item : PropertiesMatching(titleTypeIRI) )
    {
        PropertyExtensionPtr extension = item->ExtensionWithIdentifier(titleTypeIRI);
        if ( extension == nullptr )
            continue;
        
        if ( extension->Value() == "short" )
        {
            if ( localized )
                return item->LocalizedValue();
            return item->Value();
        }
    }
    
    // no 'subtitle' found, so no subtitle
    return string::EmptyString;
}
const string& Package::CollectionTitle(bool localized) const
{
    IRI titleTypeIRI(MakePropertyIRI("title-type"));      // http://idpf.org/epub/vocab/package/#title-type
    
    // find the main one
    for ( auto item : PropertiesMatching(titleTypeIRI) )
    {
        PropertyExtensionPtr extension = item->ExtensionWithIdentifier(titleTypeIRI);
        if ( extension == nullptr )
            continue;
        
        if ( extension->Value() == "collection" )
        {
            if ( localized )
                return item->LocalizedValue();
            return item->Value();
        }
    }
    
    // no 'subtitle' found, so no subtitle
    return string::EmptyString;
}
const string& Package::EditionTitle(bool localized) const
{
    IRI titleTypeIRI(MakePropertyIRI("title-type"));      // http://idpf.org/epub/vocab/package/#title-type
    
    // find the main one
    for ( auto item : PropertiesMatching(titleTypeIRI) )
    {
        PropertyExtensionPtr extension = item->ExtensionWithIdentifier(titleTypeIRI);
        if ( extension == nullptr )
            continue;
        
        if ( extension->Value() == "edition" )
        {
            if ( localized )
                return item->LocalizedValue();
            return item->Value();
        }
    }
    
    // no 'subtitle' found, so no subtitle
    return string::EmptyString;
}
const string& Package::ExpandedTitle(bool localized) const
{
    IRI titleTypeIRI(MakePropertyIRI("title-type"));      // http://idpf.org/epub/vocab/package/#title-type
    
    // find the main one
    for ( auto item : PropertiesMatching(titleTypeIRI) )
    {
        PropertyExtensionPtr extension = item->ExtensionWithIdentifier(titleTypeIRI);
        if ( extension == nullptr )
            continue;
        
        if ( extension->Value() == "expanded" )
        {
            if ( localized )
                return item->LocalizedValue();
            return item->Value();
        }
    }
    
    // no 'subtitle' found, so no subtitle
    return string::EmptyString;
}
const string Package::FullTitle(bool localized) const
{
    string expanded = ExpandedTitle(localized);
    if ( !expanded.empty() )
        return expanded;
    
    auto items = PropertiesMatching(DCType::Title);
    if ( items.size() == 1 )
        return items[0]->Value();
    
    IRI displaySeqIRI(MakePropertyIRI("display-seq"));  // http://idpf.org/epub/vocab/package/#display-seq
    std::vector<string> titles(items.size());
    
    auto sequencedItems = PropertiesMatching(displaySeqIRI);
    if ( !sequencedItems.empty() )
    {
        // all these have a 1-based sequence number
        for ( auto item : sequencedItems )
        {
            PropertyExtensionPtr extension = item->ExtensionWithIdentifier(displaySeqIRI);
            size_t sz = strtoul(extension->Value().c_str(), nullptr, 10) - 1;
            titles[sz] = (localized ? item->LocalizedValue() : item->Value());
        }
    }
    else
    {
        titles.clear();
        
        // insert any non-sequenced items at the head of the list, in order
        for ( auto item : items )
        {
            titles.emplace_back((localized ? item->LocalizedValue() : item->Value()));
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
const Package::AttributionList Package::AuthorNames(bool localized) const
{
    AttributionList result;
    for ( auto item : PropertiesMatching(DCType::Creator) )
    {
        result.emplace_back((localized? item->LocalizedValue() : item->Value()));
    }
    
    if ( result.empty() )
    {
        // maybe they're using dcterms:creator instead?
        for ( auto item : PropertiesMatching(MakePropertyIRI("creator", "dcterms")) )
        {
            result.emplace_back((localized? item->LocalizedValue() : item->Value()));
        }
    }
    
    return result;
}
const Package::AttributionList Package::AttributionNames(bool localized) const
{
    AttributionList result;
    IRI fileAsIRI(MakePropertyIRI("file-as"));
    for ( auto item : PropertiesMatching(DCType::Creator) )
    {
        auto extension = item->ExtensionWithIdentifier(fileAsIRI);
        if ( extension )
            result.emplace_back(extension->Value());
        else
            result.emplace_back((localized? item->LocalizedValue() : item->Value()));
    }
    return result;
}
const string Package::Authors(bool localized) const
{
    // TODO: handle localization of the word 'and'
    AttributionList authors = AuthorNames(localized);
    if ( authors.empty() )
        return string::EmptyString;
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
const Package::AttributionList Package::ContributorNames(bool localized) const
{
    AttributionList result;
    for ( auto item : PropertiesMatching(MakePropertyIRI("contributor", "dcterms")) )
    {
        result.emplace_back((localized? item->LocalizedValue() : item->Value()));
    }
    return result;
}
const string Package::Contributors(bool localized) const
{
    // TODO: handle localization of the word 'and'
    AttributionList contributors = ContributorNames(localized);
    if ( contributors.empty() )
        return string::EmptyString;
    if ( contributors.size() == 1 )
        return contributors[0];
    else if ( contributors.size() == 2 )
        return _Str(contributors[0], " and ", contributors[1]);
    
    std::stringstream ss;
    auto pos = contributors.begin();
    auto last = pos + (contributors.size() - 1);
    while ( pos != last )
    {
        ss << *(pos++) << ", ";
    }
    
    ss << "and " << *last;
    return string(ss.str());
}
const string& Package::MediaOverlays_ActiveClass() const
{
    // See:
    // http://www.idpf.org/epub/30/spec/epub30-mediaoverlays.html#sec-package-metadata

    /*
    //PackagePtr
    std::shared_ptr<Package> sharedMe = shared_from_this();
    //std::shared_ptr<Package> sharedMe = std::enable_shared_from_this<Package>::shared_from_this();

    shared_ptr<PropertyHolder> holderPtr = std::dynamic_pointer_cast<PropertyHolder>(sharedMe);
    //PackagePtr sharedPkg = std::dynamic_pointer_cast<Package>(sharedMe);

    const string * strPtr = MediaOverlaysMetadata::GetActiveClass(holderPtr);
    return strPtr == nullptr ? string::EmptyString : *strPtr;
    */

    PropertyPtr prop = PropertyMatching("active-class", "media");
    if (prop != nullptr)
    {
        return prop->Value();
    }
    else
    {
        return string::EmptyString;
    }
}
const string& Package::MediaOverlays_PlaybackActiveClass() const
{
    // introduced in the EPUB 3.0.1 revision,
    // see:
    // https://epub-revision.googlecode.com/svn/trunk/build/301/spec/epub30-mediaoverlays.html#sec-package-metadata

    PropertyPtr prop = PropertyMatching("playback-active-class", "media");
    if (prop != nullptr)
    {
        return prop->Value();
    }
    else
    {
        return string::EmptyString;
    }
}
const string& Package::MediaOverlays_Duration() const
{
    // See:
    // http://www.idpf.org/epub/30/spec/epub30-mediaoverlays.html#sec-package-metadata

    //std::shared_ptr<ePub3::PropertyHolder> propertyHolder

    PropertyPtr prop = PropertyMatching("duration", "media");
    if (prop != nullptr)
    {
        return prop->Value();
    }
    else
    {
        return string::EmptyString;
    }
}
const string& Package::MediaOverlays_Duration(const std::shared_ptr<PropertyHolder> propertyHolder) const
{
    // See:
    // http://www.idpf.org/epub/30/spec/epub30-mediaoverlays.html#sec-package-metadata

    PropertyPtr prop = propertyHolder->PropertyMatching("duration", "media");
    if (prop != nullptr)
    {
        return prop->Value();
    }
    else
    {
        //TODO: given manifest item may be content document instead of SMIL => search for corresponding SMIL media-overlay="ID" (if any)
        return string::EmptyString;
    }
}
const string& Package::MediaOverlays_Narrator(bool localized) const
{
    // See:
    // http://www.idpf.org/epub/30/spec/epub30-mediaoverlays.html#sec-package-metadata

    PropertyPtr prop = PropertyMatching("narrator", "media");
    if (prop != nullptr)
    {
        return localized ? prop->LocalizedValue() : prop->Value();
    }
    else
    {
        return string::EmptyString;
    }
}
const string& Package::Language() const
{
    auto items = PropertiesMatching(DCType::Language);
    if ( items.empty() )
        return string::EmptyString;
    return items[0]->Value();
}
const string& Package::Source(bool localized) const
{
    auto items = PropertiesMatching(DCType::Source);
    if ( items.empty() )
        return string::EmptyString;
    return (localized? items[0]->LocalizedValue() : items[0]->Value());
}
const string& Package::CopyrightOwner(bool localized) const
{
    auto items = PropertiesMatching(DCType::Rights);
    if ( items.empty() )
        return string::EmptyString;
    return (localized? items[0]->LocalizedValue() : items[0]->Value());
}
const string& Package::ModificationDate() const
{
    auto items = PropertiesMatching(MakePropertyIRI("modified", "dcterms"));
    if ( items.empty() )
        return string::EmptyString;
    return items[0]->Value();
}
const string Package::ISBN() const
{
    for ( auto item : PropertiesMatching(DCType::Identifier) )
    {
        if ( item->ExtensionWithIdentifier(MakePropertyIRI("identifier-type")) == nullptr )
            continue;
        
        // this will be complicated...
        // TODO: Implementation of ISBN lookup
    }
    
    return string::EmptyString;
}
const Package::StringList Package::Subjects(bool localized) const
{
    StringList result;
    for ( auto item : PropertiesMatching(DCType::Subject) )
    {
        result.emplace_back((localized? item->LocalizedValue() : item->Value()));
    }
    return result;
}
PageProgression Package::PageProgressionDirection() const
{
    PropertyPtr prop = PropertyMatching("page-progression-direction");
    if ( prop )
    {
        if ( prop->Value() == "ltr" )
            return PageProgression::LeftToRight;
        else if ( prop->Value() == "rtl" )
            return PageProgression::RightToLeft;
    }
    return PageProgression::Default;
}
const Package::StringList Package::MediaTypesWithDHTMLHandlers() const
{
    StringList result;
    for ( auto pair : _contentHandlers )
    {
        for ( auto pHandler : pair.second )
        {
            if ( typeid(*pHandler) == typeid(MediaHandler) )
            {
                result.emplace_back(pair.first);
                break;
            }
        }
    }
    return result;
}
const PackageBase::ContentHandlerList Package::HandlersForMediaType(const string& mediaType) const
{
    auto found = _contentHandlers.find(mediaType);
    if ( found == _contentHandlers.end() )
        return ContentHandlerList();
    return found->second;
}
shared_ptr<MediaHandler> Package::OPFHandlerForMediaType(const string &mediaType) const
{
    auto found = _contentHandlers.find(mediaType);
    if ( found == _contentHandlers.end() )
        return nullptr;
    
    for ( auto ptr : found->second )
    {
        shared_ptr<MediaHandler> handler = std::dynamic_pointer_cast<MediaHandler>(ptr);
        if ( handler )
            return handler;
    }
    
    return nullptr;
}
const Package::StringList Package::AllMediaTypes() const
{
    std::map<string, bool>   set;
    for ( auto pair : _manifest )
    {
        set[pair.second->MediaType()] = true;
    }
    
    StringList types;
    for ( auto pair : set )
    {
        types.push_back(pair.first);
    }
    
    return types;
}
const Package::StringList Package::UnsupportedMediaTypes() const
{
    StringList types;
    for ( auto& pair : _mediaSupport )
    {
        if ( pair.second.Support() == MediaSupportInfo::SupportType::Unsupported )
        {
            types.push_back(pair.first);
        }
    }
    return types;
}
void Package::SetMediaSupport(const MediaSupportList &list)
{
    _mediaSupport = list;
}
void Package::SetMediaSupport(MediaSupportList &&list)
{
    _mediaSupport = std::move(list);
}
void Package::InitMediaSupport()
{
    PackagePtr sharedMe = std::enable_shared_from_this<Package>::shared_from_this();
    for ( auto& mediaType : AllMediaTypes() )
    {
        if ( CoreMediaTypes.find(mediaType) != CoreMediaTypes.end() )
        {
            // support for core types is required
            _mediaSupport.insert(std::make_pair(mediaType, MediaSupportInfo(sharedMe, mediaType)));
        }
        else
        {
            shared_ptr<MediaHandler> pHandler = OPFHandlerForMediaType(mediaType);
            if ( pHandler )
            {
                // supported through a handler
                _mediaSupport.insert(std::make_pair(mediaType, MediaSupportInfo(sharedMe, mediaType, MediaSupportInfo::SupportType::SupportedWithHandler)));
            }
            else
            {
                // unsupported
                _mediaSupport.insert(std::make_pair(mediaType, MediaSupportInfo(sharedMe, mediaType, false)));
            }
        }
    }
}

EPUB3_END_NAMESPACE
