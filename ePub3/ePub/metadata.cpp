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

EPUB3_BEGIN_NAMESPACE

static const xmlChar * DCMES_uri = (const xmlChar*)"http://purl.org/dc/elements/1.1/";

// C++11 FTW
static std::map<std::string, Metadata::DCType> NameToIDMap = {
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

Metadata::Metadata(xmlNodePtr node) : _node(node)
{
    if ( _node == nullptr )
        throw std::invalid_argument("NUL node pointer supplied");
    if ( !Decode() )
        throw std::domain_error("Node does not appear to be a valid metadata item");
}
Metadata::Metadata(Metadata&& o) : _node(o._node), _type(o._type), _ext(std::move(o._ext))
{
    o._node = nullptr;
}
Metadata::~Metadata()
{
}
bool Metadata::Decode()
{
    xmlNsPtr ns = _node->ns;
    if ( xmlStrcasecmp(ns->href, DCMES_uri) != 0 )
        return false;
    
    _type = NameToIDMap[reinterpret_cast<const char*>(_node->name)];
    if ( _type == DCType::Invalid )
        return false;
    
    return true;
}
std::string Metadata::Name() const
{
    return reinterpret_cast<const char*>(_node->name);
}
std::string Metadata::Identifier() const
{
    return _getProp(_node, "id");
}
std::string Metadata::Value() const
{
    return reinterpret_cast<const char*>(xmlNodeGetContent(_node));
}
std::string Metadata::Language() const
{
    const xmlChar * ch = xmlNodeGetLang(_node);
    if ( ch == nullptr )
        return "";
    return reinterpret_cast<const char*>(ch);
}
void Metadata::AddExtension(xmlNodePtr node)
{
    try { _ext.push_back(Extension(node)); }
    catch (...) { }
}

Metadata::Extension::Extension(xmlNodePtr node) : _node(node)
{
    if ( _node == nullptr )
        throw std::invalid_argument("NUL node pointer supplied");
}
Metadata::Extension::Extension(Extension&& o) : _node(o._node)
{
}
Metadata::Extension::~Extension()
{
}
std::string Metadata::Extension::Property() const
{
    return _getProp(_node, "property");
}
std::string Metadata::Extension::Scheme() const
{
    return _getProp(_node, "scheme");
}
std::string Metadata::Extension::Value() const
{
    return reinterpret_cast<const char*>(xmlNodeGetContent(_node));
}
std::string Metadata::Extension::Identifier() const
{
    return _getProp(_node, "id");
}
std::string Metadata::Extension::Language() const
{
    return reinterpret_cast<const char*>(xmlNodeGetLang(_node));
}

EPUB3_END_NAMESPACE
