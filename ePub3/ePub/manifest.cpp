//
//  manifest.cpp
//  ePub3
//
//  Created by Jim Dovey on 2012-11-29.
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

#include "manifest.h"
#include "package.h"
#include "byte_stream.h"
#include "container.h"
#include "ePub3/xml/document.h"
#include REGEX_INCLUDE
#include <sstream>

EPUB3_BEGIN_NAMESPACE

#if EPUB_COMPILER_SUPPORTS(CXX_INITIALIZER_LISTS)
const ItemProperties::PropertyMap ItemProperties::PropertyLookupTable = {
    { "cover-image", ItemProperties::CoverImage },
    { "mathml", ItemProperties::ContainsMathML },
    { "nav", ItemProperties::Navigation },
    { "remote-resources", ItemProperties::HasRemoteResources },
    { "scripted", ItemProperties::HasScriptedContent },
    { "svg", ItemProperties::ContainsSVG },
    { "switch", ItemProperties::ContainsSwitch }
};
#else
const std::pair<string,ItemProperties::value_type> __pm_pairs[7] = {
    std::pair<string,ItemProperties::value_type>("cover-image", ItemProperties::CoverImage),
    std::pair<string,ItemProperties::value_type>("mathml", ItemProperties::ContainsMathML),
    std::pair<string,ItemProperties::value_type>("nav", ItemProperties::Navigation),
    std::pair<string,ItemProperties::value_type>("remote-resources", ItemProperties::HasRemoteResources),
    std::pair<string,ItemProperties::value_type>("scripted", ItemProperties::HasScriptedContent),
    std::pair<string,ItemProperties::value_type>("svg", ItemProperties::ContainsSVG),
    std::pair<string,ItemProperties::value_type>("switch", ItemProperties::ContainsSwitch)
};
const ItemProperties::PropertyMap ItemProperties::PropertyLookupTable(&__pm_pairs[0], &__pm_pairs[7]);
#endif

ItemProperties::ItemProperties(const string& attrStr) : _p(None)
{
    // I prefer the explicit syntax when I'm actually calling an implementation in an operator
    //  because it's clearer what's happening than '*this = attrStr'
    this->operator=(attrStr);
}
ItemProperties::ItemProperties(const IRI& iri) : _p(None)
{
    string attr(iri.Fragment());
    if ( attr.empty() )
        attr = iri.LastPathComponent();
    
    this->operator=(attr);
}
ItemProperties& ItemProperties::operator=(const string& attrStr)
{
    if ( attrStr.empty() )
    {
        _p = None;
        return *this;
    }
    
    // ensure the attributes string is lowercase
    string lowAttrs = attrStr.tolower();
    
    // NB: this is a C++11 raw-string literal. R"" means 'raw string', and the X(...)X bit are delimiters.
	REGEX_NS::regex re("\\w+(-\\w+)?", REGEX_NS::regex::icase);
    auto pos = REGEX_NS::sregex_iterator(lowAttrs.stl_str().begin(), lowAttrs.stl_str().end(), re);
    auto end = REGEX_NS::sregex_iterator();

    for ( ; pos != end; pos++ )
    {
        // using the entire matched range
        auto found = PropertyLookupTable.find(pos->str());
        if ( found != PropertyLookupTable.end() )
        {
            _p |= found->second;
        }
        else
        {
            printf("Property not found: %s (from %s)\n", pos->str().c_str(), attrStr.c_str());
        }
    }
    
    return *this;
}
string ItemProperties::str() const
{
    if ( _p == None )
        return "";
    
    std::vector<string> vec;
    value_type test = 1;
    
    while ( test < AllPropertiesMask )
    {
        if ( (_p & test) == test )
        {
            switch ( test )
            {
                case CoverImage:
                    vec.emplace_back("cover-image");
                    break;
                case ContainsMathML:
                    vec.emplace_back("mathml");
                    break;
                case Navigation:
                    vec.emplace_back("nav");
                    break;
                case HasRemoteResources:
                    vec.emplace_back("remote-resources");
                    break;
                case HasScriptedContent:
                    vec.emplace_back("scripted");
                    break;
                case ContainsSVG:
                    vec.emplace_back("svg");
                    break;
                case ContainsSwitch:
                    vec.emplace_back("switch");
                    break;
                default:
                    break;
            }
        }
        
        test <<= 1;
    }
    
    if ( vec.size() == 0 )
        return "";
    
    std::stringstream builder;
    builder << vec[0];
    for ( decltype(vec.size()) i = 0; i < vec.size(); i++ )
    {
        builder << ", " << vec[i];
    }
    
    return builder.str();
}

ManifestItem::ManifestItem(const shared_ptr<Package>& owner) : OwnedBy(owner), PropertyHolder(owner), _href(), _mediaType(), _mediaOverlayID(), _fallbackID(), _parsedProperties(0)
{
}
ManifestItem::ManifestItem(ManifestItem&& o) : OwnedBy(std::move(o)), PropertyHolder(std::move(o)), XMLIdentifiable(std::move(o)), _href(std::move(o._href)), _mediaType(std::move(o._mediaType)), _mediaOverlayID(std::move(o._mediaOverlayID)), _fallbackID(std::move(o._fallbackID)), _parsedProperties(std::move(o._parsedProperties))
{
}
ManifestItem::~ManifestItem()
{
}
bool ManifestItem::ParseXML(shared_ptr<xml::Node> node)
{
    SetXMLIdentifier(_getProp(node, "id"));
    if ( XMLIdentifier().empty() )
        return false;
    
    _href = _getProp(node, "href");
    if ( _href.empty() )
        return false;
    
    _mediaType = _getProp(node, "media-type");
    if ( _mediaType.empty() )
        return false;
    
    _mediaOverlayID = _getProp(node, "media-overlay");
    _fallbackID = _getProp(node, "fallback");
    _parsedProperties = ItemProperties(_getProp(node, "properties"));
    return true;
}
string ManifestItem::AbsolutePath() const
{
    return _Str(GetPackage()->BasePath(), BaseHref());
}
shared_ptr<ManifestItem> ManifestItem::MediaOverlay() const
{
    auto package = this->Owner();
    if ( !package || _mediaOverlayID.empty() )
        return nullptr;
    
    return package->ManifestItemWithID(_mediaOverlayID);
}
shared_ptr<ManifestItem> ManifestItem::Fallback() const
{
    auto package = this->Owner();
    if ( !package || _fallbackID.empty() )
        return nullptr;
    
    return package->ManifestItemWithID(_fallbackID);
}
string ManifestItem::BaseHref() const
{
    // get base part of href
    string path;
    size_t s = _href.find_first_of("#?");
    if ( s == string::npos )
        path = _href;
    else
        path = path.substr(0, s);
    return path;
}
bool ManifestItem::HasProperty(const std::vector<IRI>& properties) const
{
    for ( const IRI& iri : properties )
    {
        if ( this->ContainsProperty(iri) )
            return true;
    }
    return false;
}
EncryptionInfoPtr ManifestItem::GetEncryptionInfo() const
{
    ContainerPtr container = GetPackage()->GetContainer();
    string abs = AbsolutePath();
    if (abs.at(0) == '/')
    {
        abs = abs.substr(1, abs.length()-1);
    }
    return container->EncryptionInfoForPath(abs);
}
bool ManifestItem::CanLoadDocument() const
{
	return GetPackage()->GetContainer()->FileExistsAtPath(AbsolutePath());
}
shared_ptr<xml::Document> ManifestItem::ReferencedDocument() const
{
    // TODO: handle remote URLs
    string path(BaseHref());
    
    auto package = this->Owner();
    if ( !package )
        return nullptr;
	
    shared_ptr<xml::Document> result(nullptr);
	
#if EPUB_USE(LIBXML2)
	// With DRM, the resources are usually encrypted. We need to
	// pass them through the content filters chain before parsing
	// the XML.
    ePub3::ManifestItemPtr manifestRef = std::const_pointer_cast<ManifestItem>(Ptr());
    if (!manifestRef)
        return nullptr;
	
    shared_ptr<ByteStream> byteStream = package->GetFilterChainByteStream(manifestRef);
    if (!byteStream)
        return nullptr;
	
	void *docBuf = nullptr;
	std::size_t resbuflen = byteStream->ReadAllBytes(&docBuf);
	
    int flags = XML_PARSE_RECOVER|XML_PARSE_NOENT|XML_PARSE_DTDATTR;
	
    // In some EPUBs, UTF-8 XML/HTML files have a superfluous (erroneous?) BOM, so we either:
    // pass "utf-8" and expect InputBuffer::read_cb (in io.cpp) to skip the 3 erroneous bytes
    // (otherwise the XML parser fails),
    // or we pass NULL (in which case the parser auto-detects encoding)
    const char * encoding = nullptr;
    //const char * encoding = "utf-8";
	
	xmlDocPtr raw;
    if ( _mediaType == "text/html" ) {
        raw = htmlReadMemory((const char*)docBuf, resbuflen, path.c_str(), encoding, flags);
    } else {
        raw = xmlReadMemory((const char*)docBuf, resbuflen, path.c_str(), encoding, flags);
    }
	
	result = xml::Wrapped<xml::Document>(raw);
	
    if (docBuf)
        free(docBuf);
	
#elif EPUB_USE(WIN_XML)
	// TODO: filtering referenced document through Content Filters
	// is not yet supported on Windows
    unique_ptr<ArchiveXmlReader> reader = package->XmlReaderForRelativePath(path);
    if ( !reader )
        return nullptr;
	
    result = reader->ReadDocument(path.c_str(), "utf-8", 0);
#endif
	
    return result;
}
unique_ptr<ByteStream> ManifestItem::Reader() const
{
    auto package = GetPackage();
    if ( !package )
        return nullptr;
    
    auto container = package->GetContainer();
    if ( !container )
        return nullptr;
    
    return container->GetArchive()->ByteStreamAtPath(AbsolutePath());
}

#ifdef SUPPORT_ASYNC
unique_ptr<AsyncByteStream> ManifestItem::AsyncReader() const
{
    auto package = GetPackage();
    if ( !package )
        return nullptr;
    
    auto container = package->GetContainer();
    if ( !container )
        return nullptr;
    
    return container->GetArchive()->AsyncByteStreamAtPath(AbsolutePath());
}
#endif /* SUPPORT_ASYNC */

EPUB3_END_NAMESPACE
