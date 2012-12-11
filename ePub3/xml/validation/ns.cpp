//
//  ns.cpp
//  ePub3
//
//  Created by Jim Dovey on 2012-11-19.
//  Copyright (c) 2012 Kobo Inc. All rights reserved.
//

#include "ns.h"
#include "document.h"

EPUB3_XML_BEGIN_NAMESPACE

Namespace::Namespace(Document * doc, const string &prefix, const string &uri)
{
    xmlDocPtr d = doc->xml();
    _xml = xmlNewGlobalNs(d, uri.utf8(), prefix.utf8());
}
Namespace::~Namespace()
{
    if ( _xml != nullptr )
        xmlFreeNs(_xml);
}

EPUB3_XML_END_NAMESPACE
