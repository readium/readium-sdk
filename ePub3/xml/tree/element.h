//
//  element.h
//  ePub3
//
//  Created by Jim Dovey on 2012-11-27.
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
