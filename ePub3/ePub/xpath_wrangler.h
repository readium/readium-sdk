//
//  xpath_wrangler.h
//  ePub3
//
//  Created by Jim Dovey on 2012-11-29.
//  Copyright (c) 2012 Kobo Inc. All rights reserved.
//

// this is a temporary solution which will be replaced with the XML object library soon

#ifndef __ePub3__xpath_wrangler__
#define __ePub3__xpath_wrangler__

#include "epub3.h"
#include <libxml/xpath.h>
#include <map>
#include <vector>
#include <string>

EPUB3_BEGIN_NAMESPACE

class XPathWrangler
{
public:
    typedef std::map<std::string, std::string>  NamespaceList;
    
public:
    XPathWrangler(xmlDocPtr doc, const NamespaceList & namespaces = NamespaceList());
    XPathWrangler(const XPathWrangler& o);
    XPathWrangler(XPathWrangler&& o);
    ~XPathWrangler();
    
    std::vector<std::string> Strings(const xmlChar *xpath, xmlNodePtr node=nullptr);
    std::vector<std::string> Strings(const char *xpath, xmlNodePtr node=nullptr) {
        return Strings(reinterpret_cast<const xmlChar*>(xpath), node);
    }
    
    xmlNodeSetPtr Nodes(const xmlChar *xpath, xmlNodePtr node=nullptr);
    xmlNodeSetPtr Nodes(const char *xpath, xmlNodePtr node=nullptr) {
        return Nodes(reinterpret_cast<const xmlChar*>(xpath), node);
    }
    
    void RegisterNamespaces(const NamespaceList& namespaces);
    
protected:
    xmlXPathContextPtr  _ctx;
};

EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__xpath_wrangler__) */
