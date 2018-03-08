//
//  element.h
//  ePub3
//
//  Created by Jim Dovey on 2012-11-27.
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

#ifndef __ePub3_xml_element__
#define __ePub3_xml_element__

#include <ePub3/xml/base.h>
#include <ePub3/xml/node.h>
#include <ePub3/xml/ns.h>

EPUB3_XML_BEGIN_NAMESPACE

/**
 @ingroup tree
 */
class Element : public Node
{
public:
#if EPUB_USE(LIBXML2)
	typedef xmlNodePtr								NativeElementPtr;
#elif EPUB_USE(WIN_XML)
	typedef Windows::Data::Xml::Dom::XmlElement^	NativeElementPtr;
#endif
public:
    explicit Element(const NativeElementPtr xml) : Node(xml_native_cast<NativePtr>(xml)) {}
#if EPUB_ENABLE(XML_BUILDER)
    Element(const string & name, std::shared_ptr<class Document> doc = nullptr, const string & nsUri=string(), const string & nsPrefix=string()) : Node(name, NodeType::Element, "") {
        if ( doc != nullptr && !nsUri.empty() ) {
            class Namespace ns(doc, nsPrefix, nsUri);
            SetNamespace(&ns);
        }
    }
	Element(const string & name, const class Namespace & ns = xml::Namespace()) : Node(name, NodeType::Element, "", ns) {}
#endif
    Element(Element &&o) : Node(std::move(dynamic_cast<Node&>(o))) {}
    virtual ~Element() {}
};

EPUB3_XML_END_NAMESPACE

#endif /* defined(__ePub3__element__) */
