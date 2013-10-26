//
//  ns.cpp
//  ePub3
//
//  Created by Jim Dovey on 2012-11-19.
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

#include "ns.h"
#include <ePub3/xml/document.h>

EPUB3_XML_BEGIN_NAMESPACE

Namespace::Namespace(std::shared_ptr<Document> doc, const string &prefix, const string &uri)
{
    xmlDocPtr d = doc->xml();
    _xml = xmlNewNs(reinterpret_cast<xmlNodePtr>(d), uri.utf8(), prefix.utf8());
    _xml->_private = this;
}
Namespace::~Namespace()
{
    _xml->_private = nullptr;
    if ( _xml != nullptr )
        xmlFreeNs(_xml);
}

EPUB3_XML_END_NAMESPACE
