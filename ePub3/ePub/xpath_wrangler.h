//
//  xpath_wrangler.h
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

// this is a temporary solution which will be replaced with the XML object library soon

#ifndef __ePub3__xpath_wrangler__
#define __ePub3__xpath_wrangler__

#include "epub3.h"
#include "utfstring.h"
#include <libxml/xpath.h>
#include <map>
#include <vector>

EPUB3_BEGIN_NAMESPACE

class XPathWrangler
{
public:
    typedef std::map<string, string>    NamespaceList;
    typedef std::vector<string>         StringList;
    
public:
    XPathWrangler(xmlDocPtr doc, const NamespaceList & namespaces = NamespaceList());
    XPathWrangler(const XPathWrangler& o);
    XPathWrangler(XPathWrangler&& o);
    ~XPathWrangler();
    
    StringList      Strings(const string& xpath, xmlNodePtr node=nullptr);
    xmlNodeSetPtr   Nodes(const string& xpath, xmlNodePtr node=nullptr);
    
    void            RegisterNamespaces(const NamespaceList& namespaces);
    void            NameDefaultNamespace(const string& name);
    
protected:
    xmlXPathContextPtr  _ctx;
};

EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__xpath_wrangler__) */
