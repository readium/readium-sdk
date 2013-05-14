//
//  property_extension.cpp
//  ePub3
//
//  Created by Jim Dovey on 2013-05-06.
//  Copyright (c) 2013 The Readium Foundation and contributors. All rights reserved.
//

#include "property_extension.h"
#include "property.h"
#include "property_holder.h"

EPUB3_BEGIN_NAMESPACE

bool PropertyExtension::ParseMetaElement(xmlNodePtr node)
{
    if ( node == nullptr )
        return false;
    if ( node->type != XML_ELEMENT_NODE )
        return false;
    if ( xmlStrcasecmp(node->name, MetaTagName) != 0 )
        return false;
    
    string property = _getProp(node, "property");
    if ( property.empty() )
        return false;
    
    _identifier = Owner()->Owner()->PropertyIRIFromString(property);
    _value = xmlNodeGetContent(node);
    _scheme = _getProp(node, "scheme");
    _language = xmlNodeGetLang(node);
    SetXMLIdentifier(_getProp(node, "id"));
    return true;
}

EPUB3_END_NAMESPACE
