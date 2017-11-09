//
//  ns.cpp
//  ePub3
//
//  Created by Jim Dovey on 2012-11-19.
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

#include "ns.h"
#include <ePub3/xml/document.h>

EPUB3_XML_BEGIN_NAMESPACE

Namespace::Namespace(std::shared_ptr<Document> doc, const string &prefix, const string &uri)
{
    xmlDocPtr d = doc->xml();
    _xml = xmlNewNs(reinterpret_cast<xmlNodePtr>(d), uri.utf8(), prefix.utf8());
    bShouldxmlFreeNs = true;
    if (_xml->_private != nullptr)
        Node::Unwrap((xmlNodePtr)_xml);
    
    _xml->_private = new LibXML2Private<Namespace>(this);
}
Namespace::~Namespace()
{
    if (_xml == nullptr)
        return;
    
    LibXML2Private<Namespace>* priv = reinterpret_cast<LibXML2Private<Namespace>*>(_xml->_private);
    if (priv &&
#if ENABLE_WEAK_PTR_XML_NODE_WRAPPER
        priv->__sig == _READIUM_XML_SIGNATURE && priv->__ptr.lock() == nullptr/* && priv->__ptr.lock().get() == this*/
#else
        priv->__sig == _READIUM_XML_SIGNATURE && priv->__ptr.get() == this
#endif //ENABLE_WEAK_PTR_XML_NODE_WRAPPER
            )
    {
        delete priv;
        _xml->_private = nullptr;
    }

    if (bShouldxmlFreeNs)
        xmlFreeNs(_xml);
}

EPUB3_XML_END_NAMESPACE
