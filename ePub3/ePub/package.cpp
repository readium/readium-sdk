//
//  package.cpp
//  ePub3
//
//  Created by Jim Dovey on 2012-11-28.
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
#include "filter_chain.h"
#include "filter_manager.h"
#include "media-overlays_smil_model.h"
#include <ePub3/utilities/error_handler.h>
#include <sstream>
#include <list>
#include REGEX_INCLUDE
#include <ePub3/xml/document.h>
#include <ePub3/xml/element.h>

//#include "iri.h"
#include <google-url/url_canon.h>
#include <google-url/url_util.h>

EPUB3_BEGIN_NAMESPACE

#define _XML_OVERRIDE_SWITCHES (EPUB_USE(LIBXML2) && PROMISCUOUS_LIBXML_OVERRIDES == 0)

#if _XML_OVERRIDE_SWITCHES
    extern "C" {
        extern void __resetLibXMLOverrides(void);
        extern void __setupLibXML(void);
    }
#endif

void PrintNodeSet(xml::NodeSet& nodeSet)
{
	for (decltype(nodeSet.size()) i = 0; i < nodeSet.size(); i++)
	{
		auto node = nodeSet[i];
		fprintf(stderr, "Node %02lu: ", i);

		if ( !bool(node) )
		{
			fprintf(stderr, "[nullptr]");
		}
		else
		{
			fprintf(stderr, "%s", node->XMLString().utf8());
		}

		fprintf(stderr, "\n");
	}
}

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
PackageBase::PackageBase(PackageBase&& o) : _archive(o._archive), _opf(std::move(o._opf)), _pathBase(std::move(o._pathBase)), _type(std::move(o._type)), _manifestByID(std::move(o._manifestByID)), _manifestByAbsolutePath(std::move(o._manifestByAbsolutePath)), _spine(std::move(o._spine))
{
    o._archive = nullptr;
}
PackageBase::~PackageBase()
{
    // our Container owns the archive
}
bool PackageBase::Open(const string& path)
{
    ArchiveXmlReader reader(_archive->ReaderAtPath(path.stl_str()));
#if EPUB_USE(LIBXML2)
    _opf = reader.xmlReadDocument(path.c_str(), nullptr);
#elif EPUB_USE(WIN_XML)
	_opf = reader.ReadDocument(path.c_str(), nullptr, 0);
#endif
    if ( !bool(_opf) )
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
    auto found = _manifestByID.find(ident);
    if ( found == _manifestByID.end() )
        return nullptr;
    
    return found->second;
}
string PackageBase::CFISubpathForManifestItemWithID(const string &ident) const
{
    size_t sz = IndexOfSpineItemWithIDRef(ident);
    if ( sz == size_t(-1) )
        throw std::invalid_argument(_Str("Identifier '", ident, "' was not found in the spine."));
    
    return _Str(_spineCFIIndex, "/", (sz+1)*2, "[", ident, "]!");
}
const shared_vector<ManifestItem> PackageBase::ManifestItemsWithProperties(PropertyIRIList properties) const
{
    shared_vector<ManifestItem> result;
    for ( auto& item : _manifestByID )
    {
        if ( item.second->HasProperty(properties) )
            result.push_back(item.second);
    }
    return result;
}
shared_ptr<ManifestItem> PackageBase::ManifestItemAtRelativePath(const string& path) const
{
	string absPath = _pathBase + (path[0] == '/' ? path.substr(1) : path);
	
	auto found = _manifestByAbsolutePath.find(absPath);
	if (found != _manifestByAbsolutePath.end()) {
		return found->second;
	}

    // Edge case...
    // before giving up, let's check for lower/upper-case percent encoding mismatch (e.g. %2B vs. %2b)
    // (well, we're normalising to un-escaped paths)

    //if ( path.find("%") != std::string::npos ) SOMETIMES OPF MANIFEST ITEM HREF IS PERCENT-ESCAPED, BUT NOT HTML SRC !!

    url_canon::RawCanonOutputW<256> output;

    // SEE BELOW FOR A DEBUGGING BREAKPOINT / CHECK
    // note that std::string .size() is the same as
    // ePub3:string .utf8_size() defined in utfstring.h (equivalent to strlen(str.c_str()) ),
    // but not the same as ePub3:string .size() !!
    // WATCH OUT!
    url_util::DecodeURLEscapeSequences(path.c_str(), static_cast<int>(path.utf8_size()), &output);

    string path_(output.data(), output.length());

    string absPath_ = _pathBase + (path_[0] == '/' ? path_.substr(1) : path_);
    for (auto& item : _manifestByID)
    {
        string absolute = item.second->AbsolutePath();

        url_canon::RawCanonOutputW<256> output_;

//        THIS IS FOR DEBUGGING, SEE COMMENT BELOW ...
        const char * absChars = absolute.c_str();
        int absLength_STRLEN = (int)strlen(absChars);
        int absLength_SIZE = static_cast<int>(absolute.size());
        int absLength_UTF8SIZE = static_cast<int>(absolute.utf8_size());
        if (absLength_STRLEN != absLength_SIZE || absLength_STRLEN != absLength_UTF8SIZE || absLength_SIZE != absLength_UTF8SIZE)
        {
            // Place breakpoint here
            //printf("String length DIFF absLength_STRLEN:%d - absLength_SIZE:%d - absLength_UTF8SIZE:%d\n", absLength_STRLEN, absLength_SIZE, absLength_UTF8SIZE);
        }

        // note that std::string .size() is the same as
        // ePub3:string .utf8_size() defined in utfstring.h (equivalent to strlen(str.c_str()) ),
        // but not the same as ePub3:string .size() !!
        // WATCH OUT!
        url_util::DecodeURLEscapeSequences(absolute.c_str(), static_cast<int>(absolute.utf8_size()), &output_);

        string absolute_(output_.data(), output_.length());

        if (absolute_ == absPath_)
            return item.second;
    }

    // DEBUG
    //printf("MISSING ManifestItemAtRelativePath %s (%s)\n", path.c_str(), absPath.c_str());

	return nullptr;
}
shared_ptr<NavigationTable> PackageBase::NavigationTable(const string &title) const
{
    auto found = _navigation.find(title);
    if ( found == _navigation.end() )
        return nullptr;
    return found->second;
}
shared_ptr<Collection> PackageBase::CollectionWithRole(string_view role) const
{
    auto found = _collections.find(role);
    if (found == _collections.end())
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
NavigationList PackageBase::NavTablesFromManifestItem(shared_ptr<PackageBase> owner, ManifestItemPtr pItem)
{
    // have to do this one manually, as PackageBase doesn't inherit from PointerType itself
    PackagePtr sharedPkg = std::dynamic_pointer_cast<Package>(owner);
    if ( !sharedPkg )
        return NavigationList();
    
    if ( pItem == nullptr )
        return NavigationList();
    
    auto doc = pItem->ReferencedDocument();
    if ( !bool(doc) )
        return NavigationList();
    
    NavigationList navList;
	if (pItem->MediaType() != NCXContentType)
		navList = _LoadEPUB3NavTablesFromManifestItem(sharedPkg, pItem, doc);
    else
        navList = _LoadNCXNavTablesFromManifestItem(sharedPkg, pItem, doc);
    
    if (navList.empty() && pItem->Href().rfind(".ncx") == pItem->Href().size()-4)
        navList = _LoadNCXNavTablesFromManifestItem(sharedPkg, pItem, doc);
    
    return navList;
}
NavigationList PackageBase::_LoadEPUB3NavTablesFromManifestItem(PackagePtr sharedPkg, ManifestItemPtr pItem, shared_ptr<xml::Document> doc)
{
	// find each <nav> node
#if EPUB_COMPILER_SUPPORTS(CXX_INITIALIZER_LISTS)
	XPathWrangler xpath(doc, {{"epub", ePub3NamespaceURI}, {"html", XHTMLNamespaceURI}}); // goddamn I love C++11 initializer list constructors
#else
	XPathWrangler::NamespaceList __m;
	__m["epub"] = ePub3NamespaceURI;
	XPathWrangler xpath(doc, __m);
#endif
	xpath.NameDefaultNamespace("html");

	xml::NodeSet nodes = xpath.Nodes("//html:nav");

	NavigationList tables;
	for (auto navNode : nodes)
	{
		auto navTablePtr = NavigationTable::New(sharedPkg, pItem->Href());
		if (navTablePtr->ParseXML(navNode))
			tables.push_back(navTablePtr);
	}

	// now look for any <dl> nodes with an epub:type of "glossary"
	nodes = xpath.Nodes("//html:dl[epub:type='glossary']");
	for (auto node : nodes)
	{
		auto glosPtr = Glossary::New(node, sharedPkg);
		tables.push_back(glosPtr);
	}

	return tables;
}
NavigationList PackageBase::_LoadNCXNavTablesFromManifestItem(PackagePtr sharedPkg, ManifestItemPtr pItem, shared_ptr<xml::Document> doc)
{
	// find each <nav> node
#if EPUB_COMPILER_SUPPORTS(CXX_INITIALIZER_LISTS)
	XPathWrangler xpath(doc, { { "ncx", NCXNamespaceURI} }); // goddamn I love C++11 initializer list constructors
#else
	XPathWrangler::NamespaceList __m;
	__m["ncx"] = NCXNamespaceURI;
	XPathWrangler xpath(doc, __m);
#endif
	xpath.NameDefaultNamespace("ncx");

	auto titles = xpath.Strings("/ncx:ncx/ncx:docTitle/ncx:text/text()");
	string title;
	if (!titles.empty())
		title = titles[0];

	auto nodes = xpath.Nodes("/ncx:ncx/ncx:navMap");

	NavigationList tables;
	if (!nodes.empty())
	{
		auto navTablePtr = NavigationTable::New(sharedPkg, pItem->Href());
		if (navTablePtr->ParseNCXNavMap(nodes[0], title))
			tables.push_back(navTablePtr);
	}

	// now look for any <pageList> nodes
	nodes = xpath.Nodes("/ncx:ncx/ncx:pageList");
	if (!nodes.empty())
	{
		auto navTablePtr = NavigationTable::New(sharedPkg, pItem->Href());
		if (navTablePtr->ParseNCXPageList(nodes[0]))
			tables.push_back(navTablePtr);
	}

	// now any <navList> nodes
	nodes = xpath.Nodes("/ncx:ncx/ncx:navList");
	for (auto node : nodes)
	{
		auto navTablePtr = NavigationTable::New(sharedPkg, pItem->Href());
		if (navTablePtr->ParseNCXNavList(node))
			tables.push_back(navTablePtr);
	}

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
#if _XML_OVERRIDE_SWITCHES
    __setupLibXML();
#endif
    auto status = PackageBase::Open(path);
	
	if (status) {
        // Setup the content filter chain before unpacking the package
        // to filter its manifest items if needed. For example with
        // some encrypted EPUB the navigation tables must be decrypted
        // before being parsed.
		auto fm = FilterManager::Instance();
		auto fc = fm->BuildFilterChainForPackage(shared_from_this());
		SetFilterChain(fc);
		
		status = Unpack();
	}

    if (status)
    {
        ConstContainerPtr container = Owner();
        //ConstContainerPtr sharedContainer = std::dynamic_pointer_cast<Container>(owner);

        // See ReadiumJS package_document_parser.js updateMetadataWithIBookProperties()
        // https://github.com/readium/readium-js/blob/develop/epub-modules/epub/src/models/package_document_parser.js#L91
        // (vendor metadata takes precedence over OPF, to match rendering expectations with some commercial EPUBs that do not necessarily contains correct OPF metadata)

        string fxl = container->GetVendorMetadata_AppleIBooksDisplayOption_FixedLayout();
        if (fxl == "true")
        {
            this->RegisterPrefixIRIStem("rendition", "http://www.idpf.org/vocab/rendition/#");

            this->RemoveProperty("layout", "rendition");

            PropertyHolderPtr holderPtr = CastPtr<PropertyHolder>();
            PropertyPtr prop = Property::New(holderPtr);
            prop->SetPropertyIdentifier(MakePropertyIRI("layout", "rendition"));
            prop->SetValue("pre-paginated");
            AddProperty(prop);
        }

        string orientation = container->GetVendorMetadata_AppleIBooksDisplayOption_Orientation();
        bool landscape = orientation == "landscape-only";
        bool portrait = !landscape && orientation == "portrait-only";
        bool none = !landscape && !portrait && orientation == "none";
        if (landscape || portrait || none)
        {
            this->RegisterPrefixIRIStem("rendition", "http://www.idpf.org/vocab/rendition/#");
            
            this->RemoveProperty("orientation", "rendition");

            PropertyHolderPtr holderPtr = CastPtr<PropertyHolder>();
            PropertyPtr prop = Property::New(holderPtr);
            prop->SetPropertyIdentifier(MakePropertyIRI("orientation", "rendition"));
            string val = landscape?"landscape":(portrait?"portrait":"auto");
            prop->SetValue(val);
            AddProperty(prop);
        }
    }

#if _XML_OVERRIDE_SWITCHES
    __resetLibXMLOverrides();
#endif
    return status;
}
bool Package::_OpenForTest(shared_ptr<xml::Document> doc, const string& basePath)
{
#if _XML_OVERRIDE_SWITCHES
    __setupLibXML();
#endif
    _opf = doc;
    _pathBase = basePath;
    auto status = Unpack();
#if _XML_OVERRIDE_SWITCHES
    __resetLibXMLOverrides();
#endif
    return status;
}

unique_ptr<ArchiveReader> Package::ReaderForRelativePath(const string& path)       const
{
    return _archive->ReaderAtPath((_pathBase + path).stl_str());
}

bool Package::Unpack()
{
    PackagePtr sharedMe = shared_from_this();
    
    // very basic sanity check
    auto root = _opf->Root();
    string rootName(root->Name());
    rootName.tolower();
	bool isEPUB3 = true;
	string versionStr;
	int version = 0;

    
    if ( rootName != "package" )
    {
        HandleError(EPUBError::OPFInvalidPackageDocument);
        return false;       // not an OPF file, innit?
    }
	versionStr = _getProp(root, "version");
    if ( versionStr.empty() )
    {
        HandleError(EPUBError::OPFPackageHasNoVersion);
    }
	else
	{
        // GNU libstdc++ seems to not want to let us use these C++11 routines...
#ifndef _LIBCPP_VERSION
        version = (int)strtol(versionStr.c_str(), nullptr, 10);
#else
		version = std::stoi(versionStr.stl_str());
#endif

		if (version < 3)
			isEPUB3 = false;
	}

    auto val = _getProp(root, "prefix", ePub3NamespaceURI);
    InstallPrefixesFromAttributeValue(val);
    
    // go through children to determine the CFI index of the <spine> tag
    static xml::string kSpineName((const char*)"spine");
    static xml::string kManifestName((const char*)"manifest");
    static xml::string kMetadataName((const char*)"metadata");

    _spineCFIIndex = 0;
    uint32_t idx = 0;
    auto child = root->FirstElementChild();
    while ( bool(child) )
    {
        idx += 2;
        if ( child->Name() == kSpineName )
        {
            _spineCFIIndex = idx;
            if ( _spineCFIIndex != 6 )
                HandleError(EPUBError::OPFSpineOutOfOrder);
        }
        else if ( child->Name() == kManifestName && idx != 4 )
        {
            HandleError(EPUBError::OPFManifestOutOfOrder);
        }
        else if ( child->Name() == kMetadataName && idx != 2 )
        {
            HandleError(EPUBError::OPFMetadataOutOfOrder);
        }
        
		child = child->NextElementSibling();
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
    xml::NodeSet manifestNodes;
    xml::NodeSet spineNodes;
    
    try
    {
        manifestNodes = xpath.Nodes("/opf:package/opf:manifest/opf:item");
        spineNodes = xpath.Nodes("/opf:package/opf:spine/opf:itemref");
        
        if ( manifestNodes.empty() )
        {
            HandleError(EPUBError::OPFNoManifestItems);
        }
        
        if ( spineNodes.empty() )
        {
            HandleError(EPUBError::OPFNoSpineItems);
        }
        
        for ( auto node : manifestNodes )
        {
            auto p = ManifestItem::New(sharedMe);
            if ( p->ParseXML(node) )
            {
#if EPUB_HAVE(CXX_MAP_EMPLACE)
                _manifestByID.emplace(p->Identifier(), p);
                _manifestByAbsolutePath.emplace(p->AbsolutePath(), p);
#else
                _manifestByID[p->Identifier()] = p;
                _manifestByAbsolutePath[p->AbsolutePath()] = p;
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
        for ( auto &pair : _manifestByID )
        {
            ManifestItemPtr item = pair.second;
            if ( item->FallbackID().empty() )
                continue;
            
            idents[item->XMLIdentifier()] = true;
            while ( item != nullptr && !item->FallbackID().empty() )
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
        for ( auto node : spineNodes )
        {
            auto next = SpineItem::New(sharedMe);
            if ( next->ParseXML(node) == false )
            {
                // TODO: need an error code here
                continue;
            }
            
            // validation of idref
            auto manifestFound = _manifestByID.find(next->Idref());
            if ( manifestFound == _manifestByID.end() )
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
        if ( exc.code().category() == epub_spec_category() )
            throw;
        
        return false;
    }
    catch (...)
    {
        return false;
    }
    
    manifestNodes.clear();
    spineNodes.clear();
    
    // collections
    xml::NodeSet collectionNodes;
    
    try
    {
        PropertyHolderPtr holderPtr = CastPtr<PropertyHolder>();
        collectionNodes = xpath.Nodes("/opf:package/opf:collection");
        
        for (auto& node : collectionNodes)
        {
            CollectionPtr collection = Collection::New(Ptr(), nullptr);
            if (collection->ParseXML(node))
            {
#if EPUB_HAVE(CXX_MAP_EMPLACE)
                _collections.emplace(collection->Role(), collection);
#else
                _collections[collection->Role()] = collection;
#endif
            }
        }
    }
    catch (const std::system_error& exc)
    {
        if (exc.code().category() == epub_spec_category())
            throw;
        return false;
    }
    catch (...)
    {
        return false;
    }
    
    // now the metadata, which is slightly more involved due to extensions
    xml::NodeSet metadataNodes;
    xml::NodeSet refineNodes;
    
    try
    {
        PropertyHolderPtr holderPtr = CastPtr<PropertyHolder>();
        metadataNodes = xpath.Nodes("/opf:package/opf:metadata/*");
        if ( metadataNodes.empty() )
            HandleError(EPUBError::OPFNoMetadata);
        
        bool foundIdentifier = false, foundTitle = false, foundLanguage = false, foundModDate = false;
        string uniqueIDRef = _getProp(root, "unique-identifier");
        if ( uniqueIDRef.empty() )
            HandleError(EPUBError::OPFPackageUniqueIDInvalid);
        
        std::vector<string> uidRefIds = std::vector<string>();
        
        for ( auto node : metadataNodes )
        {
            PropertyPtr p;
            
			auto ns = node->Namespace();
            if ( bool(ns) && ns->URI() == xml::string(DCNamespace) )
            {
                // definitely a main node
                p = Property::New(holderPtr);
            }
            else if ( _getProp(node, "name").size() > 0 )
            {
                // It's an EPUB 2 property, we save them to allow
                // backward compatiblity by host apps.
                string name = _getProp(node, "name");
                string content = _getProp(node, "content");
#if EPUB_HAVE(CXX_MAP_EMPLACE)
	                _EPUB2Properties.emplace(name, content);
#else
	                _EPUB2Properties[name] = content;
#endif
                // it's an ePub2 item-- ignore it
                continue;
            }
            else if ( _getProp(node, "refines").empty() )
            {
                // not refining anything, so it's a main node
                p = Property::New(holderPtr);
            }
            else
            {
                // by elimination it's refining something-- we'll process it later when we know we've got all the main nodes in there
				refineNodes.push_back(node);
            }
            
            if ( p && p->ParseMetaElement(node) )
            {
                switch ( p->Type() )
                {
                    case DCType::Identifier:
                    {
                        foundIdentifier = true;
                        uidRefIds.push_back(p->XMLIdentifier());
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
        
        if ( foundIdentifier && !uniqueIDRef.empty() )
        {
            bool found = false;
            for (std::vector<string>::size_type i = 0; i < uidRefIds.size(); i++)
            {
                const string id = uidRefIds[i];
                if ( uniqueIDRef == id )
                {
                    found = true;
                    break;
                }
            }
            if (!found)
            {
                HandleError(EPUBError::OPFPackageUniqueIDInvalid);
            }
        }
        
        if ( !foundIdentifier )
            HandleError(EPUBError::OPFMissingIdentifierMetadata);
        if ( !foundTitle )
            HandleError(EPUBError::OPFMissingTitleMetadata);
        if ( !foundLanguage )
            HandleError(EPUBError::OPFMissingLanguageMetadata);
        if ( !foundModDate && isEPUB3 )
            HandleError(EPUBError::OPFMissingModificationDateMetadata);
        
        for ( auto node : refineNodes )
        {
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
                PropertyExtensionPtr extPtr = PropertyExtension::New(prop);
                if ( extPtr->ParseMetaElement(node) )
                    prop->AddExtension(extPtr);
            }
            else
            {
                // not a property, so treat this as a plain property
                PropertyHolderPtr ptr = std::dynamic_pointer_cast<PropertyHolder>(found->second);
                if ( ptr )
                {
                    prop = Property::New(ptr);
                    if ( prop->ParseMetaElement(node) )
                        ptr->AddProperty(prop);
                }
            }
        }
        
        // now look at the <spine> element for properties
		auto spineNode = root->FirstElementChild();
        for ( uint32_t i = 2; i < _spineCFIIndex; i += 2 )
            spineNode = spineNode->NextElementSibling();
        
        string value = _getProp(spineNode, "page-progression-direction");
        if ( !value.empty() )
        {
            PropertyPtr prop = Property::New(holderPtr);
            prop->SetPropertyIdentifier(MakePropertyIRI("page-progression-direction"));
            prop->SetValue(value);
            AddProperty(prop);
        }
    }
    catch (std::system_error& exc)
    {
        if ( exc.code().category() == epub_spec_category() )
            throw;
        return false;
    }
    catch (...)
    {
        return false;
    }
    
    // now any content type bindings
    xml::NodeSet bindingNodes;
    
    try
    {
        bindingNodes = xpath.Nodes("/opf:package/opf:bindings/*");
        if ( !bindingNodes.empty() )
        {
			xml::string mediaTypeElementName(MediaTypeElementName);
            for ( auto node : bindingNodes )
            {
                if ( node->Name() != mediaTypeElementName )
                    continue;
                
                ////////////////////////////////////////////////////////////
                // ePub Publications 3.0 ??3.4.16: The `mediaType` Element
                
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
                _contentHandlers[mediaType].push_back(MediaHandler::New<MediaHandler>(sharedMe, mediaType, handlerItem->AbsolutePath()));
            }
        }
    }
    catch (std::exception& exc)
    {
        std::cerr << "Exception processing OPF file: " << exc.what() << std::endl;
        throw;
    }
    catch (...)
    {
		return false;
    }
    
    // now the navigation tables
	if (isEPUB3)
	{
		// look for EPUB3 navigation document(s)
		for ( auto item : _manifestByID )
		{
			if ( !item.second->HasProperty(ItemProperties::Navigation) )
	            continue;
			
			NavigationList tables = NavTablesFromManifestItem(sharedMe, item.second);
			for ( auto& table : tables )
			{
				// have to dynamic_cast these guys to get the right pointer type
				NavigationTablePtr navTable = NavigationTable::CastFrom<NavigationElement>(table);
#if EPUB_HAVE(CXX_MAP_EMPLACE)
				_navigation.emplace(navTable->Type(), navTable);
#else
				_navigation[navTable->Type()] = navTable;
#endif
			}
		}
	}

	if (_navigation.empty() || _navigation["toc"]->Children().empty())
	{
		// look for EPUB2 NCX file
#if EPUB_COMPILER_SUPPORTS(CXX_INITIALIZER_LISTS)
		XPathWrangler xpath(_opf, { { "opf", OPFNamespace } });
#else
		XPathWrangler::NamespaceList __m;
		__m["opf"] = OPFNamespace;
		XPathWrangler xpath(_opf, __m);
#endif
		auto tocNames = xpath.Strings("/opf:package/opf:spine/@toc");

		if (tocNames.empty() && _navigation.empty())
		{
            // no NCX, and no other nav document
            // otherwise, we had a valid EPUB3 nav with empty (one-level) TOC
			HandleError(EPUBError::OPFNoNavDocument);
		}
		else
		{
            if (!tocNames.empty()) {
                try
                {
                    ManifestItemPtr tocItem = ManifestItemWithID(tocNames[0]);
                    if (!bool(tocItem))
                        throw EPUBError::OPFNoNavDocument;
                    
                    NavigationList tables = NavTablesFromManifestItem(sharedMe, tocItem);
                    for (auto& table : tables)
                    {
                        // have to dynamic_cast these guys to get the right pointer type
                        NavigationTablePtr navTable = NavigationTable::CastFrom<NavigationElement>(table);
#if EPUB_HAVE(CXX_MAP_EMPLACE)
                        _navigation.emplace(navTable->Type(), navTable);
#else
                        _navigation[navTable->Type()] = navTable;
#endif
                    }
                }
                catch (std::exception& exc)
                {
                    std::cerr << "Exception locating or processing NCX navigation document: " << exc.what() << std::endl;
                    throw;
                }
                catch (EPUBError errCode)
                {
                    // a 'break'-style mechanism here
                    // the error handler will determine if it's safe to continue
                    HandleError(errCode);
                }
                catch (...)
                {
                    HandleError(EPUBError::OPFNoNavDocument);
                }
            }
		}
	}

	// go through the TOC and copy titles to the relevant spine items for easy access
	CompileSpineItemTitles();
    
    // lastly, let's set the media support information
    InitMediaSupport();

    //std::weak_ptr<Package> weakSharedMe = sharedMe; // Not needed: smart shared pointer passed as reference, then onto OwnedBy() which maintains its own weak pointer
    _mediaOverlays = std::make_shared<class MediaOverlaysSmilModel>(sharedMe);
    _mediaOverlays->Initialize();

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
    return _getProp(_opf->Root(), "version");
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
    string absPath = _pathBase + (path[0] == '/' ? path.substr(1) : path);
    //_Str(_pathBase, path.stl_str())
    return _archive->ByteStreamAtPath(absPath);
}

#ifdef SUPPORT_ASYNC
shared_ptr<AsyncByteStream> Package::ContentStreamForItem(ManifestItemPtr manifestItem) const
{
    return _filterChain->GetFilteredOutputStreamForManifestItem(manifestItem);
}
#endif /* SUPPORT_ASYNC */

shared_ptr<ByteStream> Package::GetFilterChainByteStream(ManifestItemPtr manifestItem) const
{
	return _filterChain->GetFilterChainByteStream(manifestItem);
}

unique_ptr<ByteStream> Package::GetFilterChainByteStream(ManifestItemPtr manifestItem, SeekableByteStream *rawInput) const
{
    return _filterChain->GetFilterChainByteStream(manifestItem, rawInput);
}

shared_ptr<ByteStream> Package::GetFilterChainByteStreamRange(ManifestItemPtr manifestItem) const
{
    return _filterChain->GetFilterChainByteStreamRange(manifestItem);
}

unique_ptr<ByteStream> Package::GetFilterChainByteStreamRange(ManifestItemPtr manifestItem, SeekableByteStream *rawInput) const
{
    return _filterChain->GetFilterChainByteStreamRange(manifestItem, rawInput);
}

size_t Package::GetFilterChainSize(ManifestItemPtr manifestItem) const
{
    return _filterChain->GetFilterChainSize(manifestItem);
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
const string& Package::Language() const
{
    auto items = PropertiesMatching(DCType::Language);
    if ( items.empty() )
        return string::EmptyString;
    return items[0]->Value();
}
shared_ptr<ManifestItem> Package::CoverManifestItem() const
{
    string EPUB2CoverID = EPUB2PropertyMatching(string("cover"));
    for (auto& item : _manifestByID)
    {
        if (item.second->HasProperty(ePub3::ItemProperties::CoverImage) || item.second->Identifier() == EPUB2CoverID) {
            return item.second;
        }
    }

    return nullptr;
}
const string& Package::MediaOverlays_ActiveClass() const
{
    // See:
    // http://www.idpf.org/epub/30/spec/epub30-mediaoverlays.html#sec-package-metadata

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
const string& Package::MediaOverlays_DurationTotal() const
{
    // See:
    // http://www.idpf.org/epub/30/spec/epub30-mediaoverlays.html#sec-package-metadata

    PropertyPtr prop = PropertyMatching("duration", "media", false);
    if (prop != nullptr)
    {
        return prop->Value();
    }
    else
    {
        return string::EmptyString;
    }
}
const string& Package::MediaOverlays_DurationItem(const std::shared_ptr<ManifestItem> & manifestItem)
{
    // See:
    // http://www.idpf.org/epub/30/spec/epub30-mediaoverlays.html#sec-package-metadata

    auto iri = MakePropertyIRI("duration", "media");

    PropertyPtr prop = manifestItem->PropertyMatching(iri, false);
    if (prop == nullptr)
    {
        std::shared_ptr<ManifestItem> mediaOverlay = manifestItem->MediaOverlay();
        if (mediaOverlay != nullptr)
        {
            prop = mediaOverlay->PropertyMatching(iri, false);
        }
    }

    if (prop == nullptr)
    {
        return string::EmptyString;
    }

    return prop->Value();
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
        MediaHandlerPtr handler = ptr->CastPtr<MediaHandler>();
        if ( handler )
            return handler;
    }
    
    return nullptr;
}
const Package::StringList Package::AllMediaTypes() const
{
    std::map<string, bool>   set;
    for ( auto pair : _manifestByID )
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
        if ( pair.second->Support() == MediaSupportInfo::SupportType::Unsupported )
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
    for ( auto& mediaType : AllMediaTypes() )
    {
        if ( CoreMediaTypes.find(mediaType) != CoreMediaTypes.end() )
        {
            // support for core types is required
#if EPUB_HAVE(CXX_MAP_EMPLACE)
			_mediaSupport.emplace(mediaType, MediaSupportInfo::New(Ptr(), mediaType));
#else
            _mediaSupport.insert(std::make_pair(mediaType, MediaSupportInfo::New(Ptr(), mediaType)));
#endif
        }
        else
        {
            shared_ptr<MediaHandler> pHandler = OPFHandlerForMediaType(mediaType);
            if ( pHandler )
            {
                // supported through a handler
#if EPUB_HAVE(CXX_MAP_EMPLACE)
				_mediaSupport.emplace(mediaType, MediaSupportInfo::New(Ptr(), mediaType, MediaSupportInfo::SupportType::SupportedWithHandler));
#else
                _mediaSupport.insert(std::make_pair(mediaType, MediaSupportInfo::New(Ptr(), mediaType, MediaSupportInfo::SupportType::SupportedWithHandler)));
#endif
            }
            else
            {
                // unsupported
#if EPUB_HAVE(CXX_MAP_EMPLACE)
				_mediaSupport.emplace(mediaType, MediaSupportInfo::New(Ptr(), mediaType, false));
#else
                _mediaSupport.insert(std::make_pair(mediaType, MediaSupportInfo::New(Ptr(), mediaType, false)));
#endif
            }
        }
    }
}

string Package::EPUB2PropertyMatching(string name) const
{
    auto found = _EPUB2Properties.find(name);
    if (found != _EPUB2Properties.end()) {
        return found->second;
    }

    return string::EmptyString;
}

void Package::CompileSpineItemTitles()
{
	NavigationTablePtr toc = TableOfContents();
	if (!bool(toc))
		return;

	// optimization: we assume that TOC/spine are both in-order, so we can avoid searching a linked-list every time

	std::map<string, string> lookup;
	_CompileSpineItemTitlesInternal(toc->Children(), lookup);

	for (auto item = FirstSpineItem(); bool(item); item = item->Next())
	{
		string path = item->ManifestItem()->AbsolutePath();
		auto pos = lookup.find(path);
		if (pos != lookup.end())
			item->SetTitle(pos->second);
	}
}
void Package::_CompileSpineItemTitlesInternal(const NavigationList& navPoints, std::map<string, string>& compiled)
{
	// compile the titles to a list, accessed by absolute path
	for (auto& element : navPoints)
	{
		NavigationPointPtr pt = std::dynamic_pointer_cast<NavigationPoint>(element);
		if (bool(pt))
		{
			try
			{
				string path = pt->AbsolutePath(Ptr());
				auto pos = compiled.find(path);
				if (pos == compiled.end())
					compiled[path] = pt->Title();
			}
			catch (std::exception& cppErr)
			{
				std::cerr << "Exception: " << cppErr.what() << std::endl;
				std::cerr.flush();
#if EPUB_OS(WINDOWS)
				OutputDebugStringA(_Str("Exception: ", cppErr.what()).c_str());
#endif
			}
		}

		_CompileSpineItemTitlesInternal(element->Children(), compiled);
	}
}

EPUB3_END_NAMESPACE
