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
#include "package.h"

EPUB3_BEGIN_NAMESPACE

// upside: nice syntax for checking
// downside: operator[] always creates a new item
#if EPUB_COMPILER_SUPPORTS(CXX_INITIALIZER_LISTS)
static std::map<string, bool> AllowedRootNodeNames = {
    { "nav", true }
};
#else
typedef std::pair<string, bool> __name_pair_t;
static __name_pair_t __name_pairs[1] = {
    __name_pair_t("nav", true)
};
static std::map<string, bool> AllowedRootNodeNames(&__name_pairs[0], &__name_pairs[1]);
#endif

NavigationTable::NavigationTable(shared_ptr<Package>& owner, const string& sourceHref)
    : OwnedBy(owner), _type(), _title(), _sourceHref(sourceHref)
{
}
bool NavigationTable::ParseXML(shared_ptr<xml::Node> node)
{
    if ( node == nullptr )
        return false;
    
    string name(node->Name());
    if ( AllowedRootNodeNames.find(name) == AllowedRootNodeNames.end() )
        return false;
    
    _type = _getProp(node, "type", ePub3NamespaceURI);
    if ( _type.empty() )
        return false;

#if EPUB_COMPILER_SUPPORTS(CXX_INITIALIZER_LISTS)
    XPathWrangler xpath(node->Document(), {{"epub", ePub3NamespaceURI}}); // goddamn I love C++11 initializer list constructors
#else
    XPathWrangler::NamespaceList __ns;
    __ns["epub"] = ePub3NamespaceURI;
    XPathWrangler xpath(node->doc, __ns);
#endif
    xpath.NameDefaultNamespace("html");
    
    // look for optional <h2> title
    // Q: Should we fail on finding multiple <h2> tags here?
    auto strings = xpath.Strings("./html:h2[1]/text()", node);
    if ( !strings.empty() )
        _title = std::move(strings[0]);
    
    // load List Elements from a single Ordered List
    // first: confirm there's a single list
    xml::NodeSet nodes = xpath.Nodes("./html:ol", node);
    if ( nodes.empty() )
        return false;
    if ( nodes.size() != 1 )
        return false;

    LoadChildElements(Ptr(), nodes[0]);
    
    return true;
}

void NavigationTable::LoadChildElements(shared_ptr<NavigationElement> pElement, shared_ptr<xml::Node> olNode)
{
#if EPUB_COMPILER_SUPPORTS(CXX_INITIALIZER_LISTS)
    XPathWrangler xpath(olNode->Document(), {{"epub", ePub3NamespaceURI}});
#else
    XPathWrangler::NamespaceList __ns;
    __ns["ePub3"] = ePub3NamespaceURI;
    XPathWrangler xpath(olNode->doc, __ns);
#endif
    xpath.NameDefaultNamespace("html");

    xml::NodeSet liNodes = xpath.Nodes("./html:li", olNode);

    for ( auto liNode : liNodes )
    {
        auto childElement = BuildNavigationPoint(liNode);
        if ( childElement )
        {
            pElement->AppendChild(childElement);
        }
    }
}

shared_ptr<NavigationElement> NavigationTable::BuildNavigationPoint(shared_ptr<xml::Node> liNode)
{
    auto elementPtr = CastPtr<NavigationElement>();
    auto liChild = liNode->FirstChild();

	if ( !bool(liChild) )
		return nullptr;

    auto point = NavigationPoint::New(elementPtr);

    for ( ; bool(liChild); liChild = liChild->NextSibling() )
    {
        if ( !liChild->IsElementNode() )
            continue;

        string cName(liChild->Name());

        if ( cName == "a" )
        {
            point->SetTitle(liChild->StringValue());
            point->SetContent(_getProp(liChild, "href"));
        }
        else if( cName == "span" )
        {
            point->SetTitle(liChild->StringValue());
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
