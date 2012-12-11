//
//  ns.h
//  ePub3
//
//  Created by Jim Dovey on 2012-11-19.
//  Copyright (c) 2012 Kobo Inc. All rights reserved.
//

#ifndef __ePub3_xml_ns__
#define __ePub3_xml_ns__

#include "base.h"
#include "xmlstring.h"
#include <vector>
#include <libxml/tree.h>

EPUB3_XML_BEGIN_NAMESPACE

class Document;

class Namespace;
typedef std::vector<Namespace *> NamespaceList;

class Namespace : public WrapperBase
{
public:
    explicit Namespace(_xmlNs * ns) : _xml(ns) {}
    Namespace() : _xml(nullptr) {}
    Namespace(Document * doc, const string & prefix, const string & uri);
    virtual ~Namespace();
    
    bool IsEmpty() const { return _xml == nullptr; }
    
    Namespace * Next() const { return reinterpret_cast<Namespace*>(_xml->_private); }
    bool IsGlobal() const { return _xml->type != XML_LOCAL_NAMESPACE; }
    string URI() const { return _xml->href; }
    string Prefix() const { return _xml->prefix; }
    
    _xmlNs * xml() { return _xml; }
    const _xmlNs * xml() const { return _xml; }
    
protected:
    _xmlNs *    _xml;
    
};

EPUB3_XML_END_NAMESPACE

#endif /* defined(__ePub3_xml_ns__) */
