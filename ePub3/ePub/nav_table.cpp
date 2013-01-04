//
//  nav_table.cpp
//  ePub3
//
//  Created by Jim Dovey on 2012-12-11.
//  Copyright (c) 2012 Kobo Inc. All rights reserved.
//

#include "nav_table.h"
#include "xpath_wrangler.h"

// upside: nice syntax for checking
// downside: operator[] always creates a new item
static std::map<std::string, bool> AllowedRootNodeNames = {
    { "nav", true },
    { "li", true },
};

EPUB3_BEGIN_NAMESPACE

NavigationTable::NavigationTable(xmlNodePtr node)
{
    if ( Parse(node) == false )
        throw std::invalid_argument("NavigationTable: supplied node does not appear to be a valid navigation document <nav> node");
}
bool NavigationTable::Parse(xmlNodePtr node)
{
    if ( node == nullptr )
        return false;
    
    std::string name(reinterpret_cast<const char*>(node->name));
    if ( AllowedRootNodeNames[name] == false )
        return false;
    
    _type = _getProp(node, "type", ePub3NamespaceURI);
    if ( _type.empty() )
        return false;
    
    XPathWrangler xpath(node->doc, {{"epub", ePub3NamespaceURI}}); // goddamn I love C++11 initializer list constructors
    xpath.NameDefaultNamespace("html");
    
    if ( name == "nav" )
    {
        // look for optional <h2> title
        // Q: Should we fail on finding multiple <h2> tags here?
        auto strings = xpath.Strings("./html:h2[1]/text()", node);
        if ( !strings.empty() )
            _title = std::move(strings[0]);
    }
    else if ( name == "li" )
    {
        // look for optional <span> title
        // Q: As abive, should we fail on multiple title elements?
        auto strings = xpath.Strings("./html:span[1]/text()", node);
        if ( !strings.empty() )
            _title = std::move(strings[0]);
    }
    
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
    
    xmlXPathFreeNodeSet(nodes);
    nodes = xpath.Nodes("./html:ol[1]/html:li", node);
    if ( nodes == nullptr )
        return false;   // it's not explicit whether an empty <ol> is invalid, but...
    if ( nodes->nodeNr == 0 )
    {
        xmlXPathFreeNodeSet(nodes);
        return false;
    }
    
    BuildNavPoints(nodes, &_children);
    xmlXPathFreeNodeSet(nodes);
    
    return true;
}
void NavigationTable::BuildNavPoints(xmlNodeSetPtr nodes, NavigationList *navList)
{
    for ( size_t i = 0; i < nodes->nodeNr; i++ )
    {
        // should be a single <a> or optional <span> tag, followed by an optional <ol>
        xmlNodePtr liNode = nodes->nodeTab[i];
        xmlNodePtr liChild = liNode->children;
        NavigationPoint* point = nullptr;
        
        for ( ; liChild != nullptr; liChild = liChild->next )
        {
            if ( liChild->type != XML_ELEMENT_NODE )
                continue;
            
            bool builtSubTable = false;
            std::string cName(reinterpret_cast<const char*>(liChild->name));
            
            if ( cName == "a" && point == nullptr )
            {
                // create navigation points from anchor tags
                navList->push_back(new NavigationPoint(liChild));
            }
            else if ( cName == "span" )
            {
                // create a new sub-table
                if ( builtSubTable )
                {
                    // apply title to an existing item (back of the list)
                    navList->back()->SetTitle(reinterpret_cast<const char*>(xmlNodeGetContent(liChild)));
                }
                else
                {
                    // create a new table either on the input list or in the current point
                    if ( point == nullptr )
                    {
                        navList->push_back(new NavigationTable(liNode));
                    }
                    else
                    {
                        point->AppendChild(new NavigationTable(liNode));
                    }
                    
                    builtSubTable = true;
                }
            }
            else if ( cName == "ol" && !builtSubTable )
            {
                // create a new table either on the input list or in the current point
                if ( point == nullptr )
                {
                    navList->push_back(new NavigationTable(liNode));
                }
                else
                {
                    point->AppendChild(new NavigationTable(liNode));
                }
                
                builtSubTable = true;
            }
        }
        
        // all children of this <li> element have been enumerated now
    }
}

EPUB3_END_NAMESPACE
