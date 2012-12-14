//
//  xpath_wrangler.cpp
//  ePub3
//
//  Created by Jim Dovey on 2012-11-29.
//  Copyright (c) 2012 Kobo Inc. All rights reserved.
//

#include "xpath_wrangler.h"
#include <libxml/xpathInternals.h>

#define XMLCHAR(cppstr) reinterpret_cast<const xmlChar *>(cppstr.c_str())

static void * _nsHashCopier(void * data, xmlChar * name)
{
    return xmlStrdup(reinterpret_cast<xmlChar*>(data));
}

EPUB3_BEGIN_NAMESPACE

XPathWrangler::XPathWrangler(xmlDocPtr doc, const NamespaceList& namespaces)
{
    _ctx = xmlXPathNewContext(doc);
    xmlXPathRegisterAllFunctions(_ctx);
    RegisterNamespaces(namespaces);
}
XPathWrangler::XPathWrangler(const XPathWrangler& o)
{
    _ctx = xmlXPathNewContext(o._ctx->doc);
    xmlXPathRegisterAllFunctions(_ctx);
    
    // copy across the namespaces
    if ( _ctx->nsHash != nullptr )
        xmlHashFree(_ctx->nsHash, reinterpret_cast<xmlHashDeallocator>(xmlFree));
    _ctx->nsHash = xmlHashCopy(o._ctx->nsHash, &_nsHashCopier);
}
XPathWrangler::XPathWrangler(XPathWrangler&& o) : _ctx(o._ctx)
{
    o._ctx = nullptr;
}
XPathWrangler::~XPathWrangler()
{
    if ( _ctx != nullptr )
    {
        xmlXPathRegisteredNsCleanup(_ctx);
        xmlXPathFreeContext(_ctx);
        _ctx = nullptr;
    }
}
std::vector<std::string> XPathWrangler::Strings(const xmlChar *xpath, xmlNodePtr node)
{
    std::vector<std::string> strings;
    _ctx->node = (node == nullptr ? xmlDocGetRootElement(_ctx->doc) : node);
    
    xmlXPathObjectPtr result = xmlXPathEvalExpression(xpath, _ctx);
    if ( result != nullptr )
    {
        switch ( result->type )
        {
            case XPATH_STRING:
                // a single string
                strings.push_back(reinterpret_cast<char*>(result->stringval));
                break;
            case XPATH_NODESET:
                // a list of strings (I hope)
                for ( int i = 0; i < result->nodesetval->nodeNr; i++ )
                {
                    std::string s(reinterpret_cast<char*>(xmlNodeGetContent(result->nodesetval->nodeTab[i])));
                    strings.push_back(s);
                }
                break;
            default:
                break;
        }
        
        xmlXPathFreeObject(result);
    }
    
    return std::move(strings);
}
xmlNodeSetPtr XPathWrangler::Nodes(const xmlChar *xpath, xmlNodePtr node)
{
    _ctx->node = (node == nullptr ? xmlDocGetRootElement(_ctx->doc) : node);
    
    xmlNodeSetPtr nodes = nullptr;
    xmlXPathObjectPtr result = xmlXPathEvalExpression(xpath, _ctx);
    if ( result != nullptr )
    {
        if ( result->type == XPATH_NODESET )
        {
            nodes = xmlXPathNodeSetCreate(nullptr);
            xmlNodeSetPtr res = result->nodesetval;
            if ( res->nodeNr > 0 )
            {
                // take over the node list pointer
                nodes->nodeMax = res->nodeMax;
                nodes->nodeNr = res->nodeNr;
                nodes->nodeTab = res->nodeTab;
                
                res->nodeMax = res->nodeNr = 0;
                res->nodeTab = nullptr;
            }
        }
        
        xmlXPathFreeObject(result);
    }
    
    return nodes;
}
void XPathWrangler::RegisterNamespaces(const NamespaceList &namespaces)
{
    for ( auto item : namespaces )
    {
        xmlXPathRegisterNs(_ctx, XMLCHAR(item.first), XMLCHAR(item.second));
    }
}
void XPathWrangler::NameDefaultNamespace(const std::string &name)
{
    xmlNsPtr defNs = xmlSearchNs(_ctx->doc, xmlDocGetRootElement(_ctx->doc), nullptr);
    if ( defNs != nullptr )
        xmlXPathRegisterNs(_ctx, reinterpret_cast<const xmlChar*>(name.c_str()), defNs->href);
}

EPUB3_END_NAMESPACE
