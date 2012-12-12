//
//  nav_point.cpp
//  ePub3
//
//  Created by Jim Dovey on 2012-12-11.
//  Copyright (c) 2012 Kobo Inc. All rights reserved.
//

#include "nav_point.h"

EPUB3_BEGIN_NAMESPACE

NavigationPoint::NavigationPoint(xmlNodePtr node)
{
    // node should be the <a> tag
    if ( reinterpret_cast<const char*>(node->name) != std::string("a") )
        throw std::invalid_argument("Invalid NavigationPoint node");
    
    _label = reinterpret_cast<const char*>(xmlNodeGetContent(node));
    _content = _getProp(node, "href");
}

EPUB3_END_NAMESPACE
