//
//  glossary.cpp
//  ePub3
//
//  Created by Jim Dovey on 2012-12-13.
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

#include "glossary.h"
#include <list>

EPUB3_BEGIN_NAMESPACE

Glossary::Glossary(xmlNodePtr node) : _ident("Glossary")
{
    if ( !Parse(node) )
        throw std::invalid_argument("Node is not a valid glossary");
}
const Glossary::Entry Glossary::Lookup(const Term &term) const
{
    auto f = _lookup.find(term.tolower());
    if ( f == _lookup.end() )
        return Entry();
    return f->second;
}
bool Glossary::AddDefinition(const Term &term, const Definition &definition)
{
    _lookup[term.tolower()] = std::make_pair(term, definition);
    return true;
}
bool Glossary::AddDefinition(const Term &term, Definition &&definition)
{
    _lookup[term.tolower()] = std::make_pair(term, definition);
    return true;
}
bool Glossary::Parse(xmlNodePtr node)
{
    // node names
    static const xmlChar* dlName = (const xmlChar*)"dl";
    static const xmlChar* dtName = (const xmlChar*)"dt";
    static const xmlChar* ddName = (const xmlChar*)"dd";
    
    if ( node == nullptr )
        return false;
    if ( xmlStrcasecmp(node->name, dlName) != 0 )
        return false;
    if ( _getProp(node, "type", ePub3NamespaceURI) != "glossary" )
        return false;
    
    // very basic for now: no links, just text content
    // NB: there can be multiple terms for a single definition
    std::list<Term> terms;
    
    for ( xmlNodePtr child = node->children; child != nullptr; child = child->next )
    {
        if ( child->type != XML_ELEMENT_NODE )
            continue;
        
        if ( xmlStrEqual(child->name, dtName) )
        {
            terms.push_back(reinterpret_cast<const char*>(xmlNodeGetContent(child)));
        }
        else if ( xmlStrEqual(child->name, ddName) )
        {
            // a definition: associates with all terms in the list
            Definition def(reinterpret_cast<const char*>(xmlNodeGetContent(child)));
            for ( auto term : terms )
            {
                _lookup[term.tolower()] = std::make_pair(term, def);
            }
            
            // now clear the terms list
            terms.clear();
        }
    }
    
    return true;
}

EPUB3_END_NAMESPACE
