//
//  content_handler.cpp
//  ePub3
//
//  Created by Jim Dovey on 2013-01-30.
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

#include "content_handler.h"
#include "package.h"
#include "iri.h"

EPUB3_BEGIN_NAMESPACE

MediaHandler::MediaHandler(shared_ptr<Package>& owner, const string& mediaType, const string& handlerPath) : ContentHandler(owner, mediaType), _handlerIRI(IRI::gEPUBScheme, owner->PackageID(), handlerPath)
{
}
void MediaHandler::operator()(const string& src, const ParameterList& parameters) const
{
    if ( Owner() == nullptr )
        return;
    
    Owner()->FireLoadEvent(Target(src, parameters));
}
IRI MediaHandler::Target(const string& src, const ParameterList& parameters) const
{
    IRI result(_handlerIRI);       // this will already include any fragment, we just have to add the query
    
    std::stringstream ss;
    ss << "src=" << src;
    for ( auto& pair : parameters )
    {
        string str = _Str('&', IRI::URLEncodeComponent(pair.first), '=', IRI::URLEncodeComponent(pair.second));
        ss << str;
    }
    
    result.SetQuery(ss.str());
    return result;
}

EPUB3_END_NAMESPACE
