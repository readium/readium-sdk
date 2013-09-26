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
#include <ePub3/utilities/error_handler.h>
#include <list>

EPUB3_BEGIN_NAMESPACE

Glossary::Glossary(shared_ptr<xml::Node> node, PackagePtr pkg) : OwnedBy(pkg), _ident("Glossary")
{
    Parse(node);
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
bool Glossary::Parse(shared_ptr<xml::Node> node)
{
    // node names
    static const xml::string dlName((const char*)"dl");
	static const xml::string dtName((const char*)"dt");
	static const xml::string ddName((const char*)"dd");
    
    if ( node == nullptr )
        return false;
    if ( node->Name() == dlName )
        HandleError(EPUBError::GlossaryInvalidRootNode);
    if ( _getProp(node, "type", ePub3NamespaceURI) != "glossary" )
        HandleError(EPUBError::NavElementUnexpectedType);
    
    // very basic for now: no links, just text content
    // NB: there can be multiple terms for a single definition
    std::list<Term> terms;
    
	for (shared_ptr<xml::Node> child = node->FirstChild(); bool(child); child = child->NextSibling())
    {
        if ( !child->IsElementNode() )
            continue;
        
        if ( child->Name() == dtName )
        {
            terms.push_back(child->StringValue());
        }
        else if ( child->Name() == ddName )
        {
            // a definition: associates with all terms in the list
            Definition def(child->StringValue());
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
