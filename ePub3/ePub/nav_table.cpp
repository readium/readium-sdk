//
//  nav_table.cpp
//  ePub3
//
//  Created by Jim Dovey on 2012-12-11.
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

#include "nav_table.h"
#include "xpath_wrangler.h"

EPUB3_BEGIN_NAMESPACE

// upside: nice syntax for checking
// downside: operator[] always creates a new item
static std::map<string, bool> AllowedRootNodeNames = {
    { "nav", true }
};

NavigationTable::NavigationTable(xmlNodePtr node)
{
    if ( Parse(node) == false )
        throw std::invalid_argument("NavigationTable: supplied node does not appear to be a valid navigation document <nav> node");
}

NavigationTable::NavigationTable(xmlNodePtr node, const string& href)
    : _href(href)
{
    if ( Parse(node) == false )
        throw std::invalid_argument("NavigationTable: supplied node does not appear to be a valid navigation document <nav> node");
}

bool NavigationTable::Parse(xmlNodePtr node)
{
    if ( node == nullptr )
        return false;
    
    string name(node->name);
    if ( AllowedRootNodeNames.find(name) == AllowedRootNodeNames.end() )
        return false;
    
    _type = _getProp(node, "type", ePub3NamespaceURI);
    if ( _type.empty() )
        return false;
    
    XPathWrangler xpath(node->doc, {{"epub", ePub3NamespaceURI}}); // goddamn I love C++11 initializer list constructors
    xpath.NameDefaultNamespace("html");
    
    // look for optional <h2> title
    // Q: Should we fail on finding multiple <h2> tags here?
    auto strings = xpath.Strings("./html:h2[1]/text()", node);
    if ( !strings.empty() )
        _title = std::move(strings[0]);

    
    // load List Elements from a single Ordered List
    // first: confirm there's a single list
    xmlNodeSetPtr nodes = xpath.Nodes("./html:ol", node);
    if ( nodes == nullptr )
        return false;
    if ( nodes->nodeNr != 1 )
    {
        xmlXPathFreeNodeSet(nodes);
        return false;
    }

    LoadChildElements(this, nodes->nodeTab[0]);

    xmlXPathFreeNodeSet(nodes);
    
    return true;
}

void NavigationTable::LoadChildElements(NavigationElement *pElement, xmlNodePtr olNode)
{
    XPathWrangler xpath(olNode->doc, {{"epub", ePub3NamespaceURI}});
    xpath.NameDefaultNamespace("html");

    xmlNodeSetPtr liNodes = xpath.Nodes("./html:li", olNode);

    for ( size_t i = 0; i < liNodes->nodeNr; i++ )
    {
        NavigationElement* childElement = BuildNavigationPoint(liNodes->nodeTab[i]);
        if(childElement != nullptr)
        {
            pElement->AppendChild(childElement);
        }
    }

    xmlXPathFreeNodeSet(liNodes);
}

NavigationElement*  NavigationTable::BuildNavigationPoint(xmlNodePtr liNode)
{
    xmlNodePtr liChild = liNode->children;

    if(liChild == nullptr)
    {
        return nullptr;
    }

    NavigationPoint* point = new NavigationPoint();

    for ( ; liChild != nullptr; liChild = liChild->next )
    {
        if ( liChild->type != XML_ELEMENT_NODE )
            continue;

        std::string cName(reinterpret_cast<const char*>(liChild->name));

        if ( cName == "a" )
        {
            point->SetTitle(reinterpret_cast<const char*>(xmlNodeGetContent(liChild)));
            point->SetContent(_getProp(liChild, "href"));
        }
        else if( cName == "span" )
        {
            point->SetTitle(xmlNodeGetContent(liChild));
        }
        else if( cName == "ol" )
        {
            LoadChildElements(point, liChild);
            break;
        }
    }

    return point;
}



EPUB3_END_NAMESPACE
