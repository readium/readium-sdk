//
//  content_handler.cpp
//  ePub3
//
//  Created by Jim Dovey on 2013-01-30.
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
