//
//  ns.cpp
//  ePub3
//
//  Created by Jim Dovey on 2012-11-19.
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
    if (
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
