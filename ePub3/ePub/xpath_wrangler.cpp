//
//  xpath_wrangler.cpp
//  ePub3
//
//  Created by Jim Dovey on 2012-11-29.
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

#include "xpath_wrangler.h"
#include <libxml/xpathInternals.h>

#define XMLCHAR(utfstr) utfstr.xml_str()

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
XPathWrangler::StringList XPathWrangler::Strings(const string& xpath, xmlNodePtr node)
{
    StringList strings;
    _ctx->node = (node == nullptr ? xmlDocGetRootElement(_ctx->doc) : node);
    
    xmlXPathObjectPtr result = xmlXPathEvalExpression(xpath.xml_str(), _ctx);
    if ( result != nullptr )
    {
        switch ( result->type )
        {
            case XPATH_STRING:
                // a single string
                strings.emplace_back(result->stringval);
                break;
            case XPATH_NODESET:
                if ( result->nodesetval == nullptr )
                    break;
                
                // a list of strings (I hope)
                for ( int i = 0; i < result->nodesetval->nodeNr; i++ )
                {
                    strings.emplace_back(xmlXPathCastNodeToString(result->nodesetval->nodeTab[i]));
                    //strings.emplace_back(xmlNodeGetContent(result->nodesetval->nodeTab[i]));
                }
                break;
            default:
                break;
        }
        
        xmlXPathFreeObject(result);
    }
    
    return std::move(strings);
}
xmlNodeSetPtr XPathWrangler::Nodes(const string& xpath, xmlNodePtr node)
{
    _ctx->node = (node == nullptr ? xmlDocGetRootElement(_ctx->doc) : node);
    
    xmlNodeSetPtr nodes = nullptr;
    xmlXPathObjectPtr result = xmlXPathEvalExpression(xpath.xml_str(), _ctx);
    if ( result != nullptr )
    {
        if ( result->type == XPATH_NODESET && result->nodesetval != nullptr )
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
void XPathWrangler::NameDefaultNamespace(const string& name)
{
    xmlNsPtr defNs = xmlSearchNs(_ctx->doc, xmlDocGetRootElement(_ctx->doc), nullptr);
    if ( defNs != nullptr )
        xmlXPathRegisterNs(_ctx, name.xml_str(), defNs->href);
}

EPUB3_END_NAMESPACE
