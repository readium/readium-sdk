//
//  content_handler.cpp
//  ePub3
//
//  Created by Jim Dovey on 2013-01-30.
//  Copyright (c) 2013 The Readium Foundation.
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

#include "content_handler.h"
#include "package.h"
#include "iri.h"

EPUB3_BEGIN_NAMESPACE

void MediaHandler::operator()(const string& src, const ParameterList& parameters) const
{
    if ( _owner == nullptr )
        return;
    
    _owner->FireLoadEvent(Target(src, parameters));
}
IRI MediaHandler::Target(const string& src, const ParameterList& parameters) const
{
    IRI result(_handlerPath);       // this will already include any fragment, we just have to add the query
    
    std::stringstream ss;
    ss << "src=" << src;
    for ( auto pair : parameters )
    {
        ss << IRI::URLEncodeComponent(pair.first) << "=" << IRI::URLEncodeComponent(pair.second);
    }
    
    result.SetQuery(ss.str());
    return result;
}

EPUB3_END_NAMESPACE
