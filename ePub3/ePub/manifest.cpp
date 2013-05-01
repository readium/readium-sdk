//
//  manifest.cpp
//  ePub3
//
//  Created by Jim Dovey on 2012-11-29.
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

#include "manifest.h"
#include "package.h"
#include "byte_stream.h"
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
    REGEX_NS::regex re("\\w+", REGEX_NS::regex::icase);
    auto pos = REGEX_NS::sregex_iterator(lowAttrs.stl_str().begin(), lowAttrs.stl_str().end(), re);
    auto end = REGEX_NS::sregex_iterator();
    
    for ( ; pos != end; pos++ )
    {
        // using the entire matched range
        auto found = PropertyLookupTable.find(pos->str());
        if ( found != PropertyLookupTable.end() )
            _p |= found->second;
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

ManifestItem::ManifestItem(xmlNodePtr node, const class Package* package) : _owner(package)
{
    _identifier = _getProp(node, "id");
    if ( _identifier.empty() )
        throw std::invalid_argument("Manifest items must have an 'id' attribute");
    
    _href = _getProp(node, "href");
    if ( _href.empty() )
        throw std::invalid_argument("Manifest items must have a 'href' attribute");
    
    _mediaType = _getProp(node, "media-type");
    if ( _href.empty() )
        throw std::invalid_argument("Manifest items must have a 'media-type' attribute");
    
    _mediaOverlayID = _getProp(node, "media-overlay");
    _fallbackID = _getProp(node, "fallback");
    _properties = ItemProperties(_getProp(node, "properties"));
}
ManifestItem::ManifestItem(ManifestItem&& o) : _owner(o._owner), _identifier(std::move(o._identifier)), _href(std::move(o._href)), _mediaType(std::move(o._mediaType)), _mediaOverlayID(std::move(o._mediaOverlayID)), _fallbackID(std::move(o._fallbackID)), _properties(o._properties)
{
    o._owner = nullptr;
}
ManifestItem::~ManifestItem()
{
}
string ManifestItem::AbsolutePath() const
{
    return _Str(_owner->BasePath(), BaseHref());
}
const ManifestItem* ManifestItem::MediaOverlay() const
{
    if ( _owner == nullptr || _mediaOverlayID.empty() )
        return nullptr;
    
    return _owner->ManifestItemWithID(_mediaOverlayID);
}
const ManifestItem* ManifestItem::Fallback() const
{
    if ( _owner == nullptr || _fallbackID.empty() )
        return nullptr;
    
    return _owner->ManifestItemWithID(_fallbackID);
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
        string attr(iri.Fragment());
        if ( attr.empty() )
            attr = iri.LastPathComponent();
        
        if ( HasProperty(attr) )
            return true;
    }
    
    return false;
}
xmlDocPtr ManifestItem::ReferencedDocument() const
{
    // TODO: handle remote URLs
    string path(BaseHref());
    
    ArchiveXmlReader * reader = _owner->XmlReaderForRelativePath(path);
    if ( reader == nullptr )
        return nullptr;
    
    xmlDocPtr result = nullptr;
    int flags = XML_PARSE_RECOVER|XML_PARSE_NOENT|XML_PARSE_DTDATTR;
    if ( _mediaType == "text/html" )
        result = reader->htmlReadDocument(path.c_str(), "utf-8", flags);
    else
        result = reader->xmlReadDocument(path.c_str(), "utf-8", flags);
    
    return result;
}
unique_ptr<ByteStream> ManifestItem::Reader() const
{
    return _owner->ReadStreamForRelativePath(BaseHref());
}

EPUB3_END_NAMESPACE
