//
//  metadata.cpp
//  ePub3
//
//  Created by Jim Dovey on 2012-12-04.
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

#include "metadata.h"
#include "package.h"

EPUB3_BEGIN_NAMESPACE

static const xmlChar * DCMES_uri = (const xmlChar*)"http://purl.org/dc/elements/1.1/";
static const xmlChar * MetaTagName = (const xmlChar*)"meta";

// C++11 FTW
std::map<string, Metadata::DCType> Metadata::NameToIDMap = {
    {"meta", Metadata::DCType::Custom},
    {"identifier", Metadata::DCType::Identifier},
    {"title", Metadata::DCType::Title},
    {"language", Metadata::DCType::Language},
    {"contributor", Metadata::DCType::Contributor},
    {"coverage", Metadata::DCType::Coverage},
    {"creator", Metadata::DCType::Creator},
    {"date", Metadata::DCType::Date},
    {"description", Metadata::DCType::Description},
    {"format", Metadata::DCType::Format},
    {"publisher", Metadata::DCType::Publisher},
    {"relation", Metadata::DCType::Relation},
    {"rights", Metadata::DCType::Rights},
    {"source", Metadata::DCType::Source},
    {"subject", Metadata::DCType::Subject},
    {"type", Metadata::DCType::Type}
};

static std::map<Metadata::DCType, string> IDToNameMap = {
    {Metadata::DCType::Identifier, "identifier"},
    {Metadata::DCType::Title, "title"},
    {Metadata::DCType::Language, "language"},
    {Metadata::DCType::Contributor, "contributor"},
    {Metadata::DCType::Coverage, "coverage"},
    {Metadata::DCType::Creator, "creator"},
    {Metadata::DCType::Date, "date"},
    {Metadata::DCType::Description, "description"},
    {Metadata::DCType::Format, "format"},
    {Metadata::DCType::Publisher, "publisher"},
    {Metadata::DCType::Relation, "relation"},
    {Metadata::DCType::Rights, "rights"},
    {Metadata::DCType::Source, "source"},
    {Metadata::DCType::Subject, "subject"},
    {Metadata::DCType::Type, "type"}
};

Metadata::Metadata(xmlNodePtr node, const Package* owner) : _node(node), _type(DCType::Invalid), _extensions(), _property()
{
    if ( _node == nullptr )
        throw std::invalid_argument("NUL node pointer supplied");
    if ( !Decode(owner) )
        throw std::domain_error("Node does not appear to be a valid metadata item");
}
Metadata::Metadata(Metadata&& o) : _node(o._node), _type(o._type), _extensions(std::move(o._extensions)), _property(std::move(o._property))
{
    o._node = nullptr;
}
Metadata::~Metadata()
{
    for ( auto item : _extensions )
    {
        delete item;
    }
}
bool Metadata::Decode(const Package* owner)
{
    xmlNsPtr ns = _node->ns;
    if ( ns != nullptr && xmlStrcasecmp(ns->href, DCMES_uri) == 0 )
    {
        _type = NameToIDMap[_node->name];
        if ( _type == DCType::Invalid )
            return false;
        
        // special property IRI, not actually in the spec, but useful for comparisons and printouts
        _property = IRI(string(DCMES_uri) + _node->name);
    }
    else if ( xmlStrcasecmp(_node->name, MetaTagName) == 0 )
    {
        _type = DCType::Custom;
        string property = _getProp(_node, "property");
        if ( property.empty() )
            return false;
        
        _property = owner->PropertyIRIFromAttributeValue(property);
    }
    
    return true;
}
string Metadata::Identifier() const
{
    return _getProp(_node, "id");
}
string Metadata::Value() const
{
    return xmlNodeGetContent(_node);
}
string Metadata::Language() const
{
    const xmlChar * ch = xmlNodeGetLang(_node);
    if ( ch == nullptr )
        return string::EmptyString;
    return ch;
}
void Metadata::AddExtension(xmlNodePtr node, const Package* owner)
{
    try { _extensions.push_back(new Extension(node, owner)); }
    catch (std::exception& e) { fprintf(stderr, "ARGH: %s\n", e.what()); }
}
const Metadata::Extension* Metadata::ExtensionWithProperty(const IRI &property) const
{
    for ( auto extension : _extensions )
    {
        if ( extension->Property() == property )
            return extension;
    }
    return nullptr;
}
const IRI Metadata::IRIForDCType(DCType type)
{
    auto found = IDToNameMap.find(type);
    if ( found == IDToNameMap.end() )
        return IRI();
    return IRI(DCMES_uri + found->second);
}
const Metadata::ValueMap Metadata::DebugValues() const
{
    ValueMap values({{Property().IRIString(), Value()}});
    for ( const Extension* extension : _extensions )
    {
        values.emplace_back(_Str("  ", extension->Property().IRIString()), extension->Value());
    }
    return values;
}

Metadata::Extension::Extension(xmlNodePtr node, const Package* owner) : _node(node)
{
    if ( _node == nullptr )
        throw std::invalid_argument("NUL node pointer supplied");
    
    string property = _getProp(_node, "property");
    if ( !property.empty() )
        _property = owner->PropertyIRIFromAttributeValue(property);
}
Metadata::Extension::Extension(Extension&& o) : _node(o._node), _property(std::move(o._property))
{
}
Metadata::Extension::~Extension()
{
}
string Metadata::Extension::Scheme() const
{
    return _getProp(_node, "scheme");
}
string Metadata::Extension::Value() const
{
    return reinterpret_cast<const char*>(xmlNodeGetContent(_node));
}
string Metadata::Extension::Identifier() const
{
    return _getProp(_node, "id");
}
string Metadata::Extension::Language() const
{
    return reinterpret_cast<const char*>(xmlNodeGetLang(_node));
}

EPUB3_END_NAMESPACE
