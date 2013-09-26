//
//  property_extension.cpp
//  ePub3
//
//  Created by Jim Dovey on 2013-05-06.
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

#include "property_extension.h"
#include "property.h"
#include "property_holder.h"

EPUB3_BEGIN_NAMESPACE

bool PropertyExtension::ParseMetaElement(shared_ptr<xml::Node> node)
{
    if ( node == nullptr )
        return false;
    if ( node->IsElementNode() == false )
        return false;
    if ( node->Name() != MetaTagName )
        return false;
    
    string property = _getProp(node, "property");
    if ( property.empty() )
        return false;
    
    _identifier = Owner()->Owner()->PropertyIRIFromString(property);
	_value = node->StringValue();
    _scheme = _getProp(node, "scheme");
    _language = node->Language();
    SetXMLIdentifier(_getProp(node, "id"));
    return true;
}

EPUB3_END_NAMESPACE
