//
//  element.h
//  ePub3
//
//  Created by Jim Dovey on 2012-11-27.
//  Copyright (c) 2012 Kobo Inc. All rights reserved.
//

#ifndef __ePub3_xml_element__
#define __ePub3_xml_element__

#include "base.h"
#include "node.h"
#include "ns.h"

EPUB3_XML_BEGIN_NAMESPACE

class Element : public Node
{
public:
    explicit Element(const xmlNodePtr xml) : Node(xml) {}
    Element(const string & name, class Document * doc = nullptr, const string & nsUri=string(), const string & nsPrefix=string()) : Node(name, NodeType::Element, "") {
        if ( doc != nullptr && !nsUri.empty() ) {
            class Namespace ns(doc, nsPrefix, nsUri);
            SetNamespace(&ns);
        }
    }
    Element(const string & name, const class Namespace & ns = xml::Namespace()) : Node(name, NodeType::Element, "", ns) {}
    Element(Element &&o) : Node(dynamic_cast<Node&&>(o)) {}
    virtual ~Element() {}
};

EPUB3_XML_END_NAMESPACE

#endif /* defined(__ePub3__element__) */
