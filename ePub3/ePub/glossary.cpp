//
//  glossary.cpp
//  ePub3
//
//  Created by Jim Dovey on 2012-12-13.
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//  
//  Redistribution and use in source and binary forms, with or without modification, 
//  are permitted provided that the following conditions are met:
//  1. Redistributions of source code must retain the above copyright notice, this 
//  list of conditions and the following disclaimer.
//  2. Redistributions in binary form must reproduce the above copyright notice, 
//  this list of conditions and the following disclaimer in the documentation and/or 
//  other materials provided with the distribution.
//  3. Neither the name of the organization nor the names of its contributors may be 
//  used to endorse or promote products derived from this software without specific 
//  prior written permission.
//  
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
//  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
//  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
//  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
//  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
//  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
//  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
//  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE 
//  OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED 
//  OF THE POSSIBILITY OF SUCH DAMAGE.

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
