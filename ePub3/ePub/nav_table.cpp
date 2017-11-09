//
//  nav_table.cpp
//  ePub3
//
//  Created by Jim Dovey on 2012-12-11.
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

#include "nav_table.h"
#include "xpath_wrangler.h"
#include "package.h"
#include "error_handler.h"

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

static void NCXNavLabelText(shared_ptr<xml::Node> navLabel, string& outString)
{
	auto textNode = navLabel->FirstElementChild();
	string cName = textNode->Name();
    bool enterLoop = false;
	while (bool(textNode) && cName != "text") {
        if (!enterLoop) {
            enterLoop = true;
        }
        xml::Node::Unwrap(textNode->xml());
		textNode = textNode->NextElementSibling();
	}
    if (!enterLoop) {
        xml::Node::Unwrap(textNode->xml());
    }
	if (bool(textNode))
		outString = textNode->StringValue();
	else
		outString.clear();
}
static void NCXContentHref(shared_ptr<xml::Node> content, string& outString)
{
	xml::string val = content->AttributeValue((const char*)"src", (const char*)NCXNamespaceURI);
	if (val.empty())
		val = content->AttributeValue((const char*)"src", (const char*)"");

	outString = val;
}

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
	XPathWrangler xpath(node->Document(), {{ "epub", ePub3NamespaceURI }, { "html", XHTMLNamespaceURI }}); // goddamn I love C++11 initializer list constructors
#else
    XPathWrangler::NamespaceList __ns;
    __ns["epub"] = ePub3NamespaceURI;
    XPathWrangler xpath(node->doc, __ns);
#endif
    xpath.NameDefaultNamespace("html");
    
    // look for optional <h2> title
    // Q: Should we fail on finding multiple <h2> tags here?
#if EPUB_USE(WIN_XML)
	xml::NodeSet h2nodes = xpath.Nodes("./html:h2", node);
	if (!h2nodes.empty()){
		_title = std::move(h2nodes[0]->FirstChild()->StringValue());
	}
#else
    auto strings = xpath.Strings("./html:h2[1]/text()", node);

	if ( !strings.empty() )
        _title = std::move(strings[0]);
#endif
    
    // load List Elements from a single Ordered List
    // first: confirm there's a single list
    xml::NodeSet nodes = xpath.Nodes("./html:ol", node);
    if ( nodes.empty() )
        return false;
    if ( nodes.size() != 1 ) {
        xml::Node::UnwrapNodeSet(nodes);
        return false;
    }

    LoadChildElements(Ptr(), nodes[0]);
    xml::Node::UnwrapNodeSet(nodes);
    
    return true;
}
bool NavigationTable::ParseNCXNavMap(shared_ptr<xml::Node> node, const string& title)
{
	_type = "toc";
	_title = title;

	for (auto navPoint = node->FirstElementChild(); bool(navPoint); )
	{
		string cName(navPoint->Name());
		if (cName != "navPoint")
		{
			HandleError(EPUBError::NavElementUnexpectedType, "Found a non-navPoint element inside an NCX navMap");
			continue;
		}

		LoadChildNavPoint(Ptr(), navPoint);
        xml::Node::Unwrap(navPoint->xml());
        navPoint = navPoint->NextElementSibling();
	}

	return true;
}
bool NavigationTable::ParseNCXPageList(shared_ptr<xml::Node> pNode)
{
	_type = "page-list";
	_title = "Page List";	// TODO: localization
	
	for (auto sub = pNode->FirstElementChild(); bool(sub); )
	{
		string cName(sub->Name());
		if (cName != "pageTarget")
		{
			HandleError(EPUBError::NavElementUnexpectedType, "Found a non-pageTarget element inside an NCX pageList");
			continue;
		}

		LoadChildNavPoint(Ptr(), sub);
        xml::Node::Unwrap(sub->xml());
        sub = sub->NextElementSibling();
	}

	return true;
}
bool NavigationTable::ParseNCXNavList(shared_ptr<xml::Node> pNode)
{
	

	for (auto sub = pNode->FirstElementChild(); bool(sub); )
	{
		string cName(sub->Name());
		if (cName == "navLabel")
		{
			if (_title.empty())
			{
				NCXNavLabelText(sub, _title);
				if (_title == "List of Illustrations")
					_type = "loi";
				else if (_title == "List of Tables")
					_type = "lot";
				else if (_title == "List of Figures")
					_type = "lof";
			}
			else
			{
				HandleError(EPUBError::NavListElementInvalidChild, "Multiple navLabel elements within an NCX navList");
			}

			continue;
		}

		if (cName != "navTarget")
		{
			HandleError(EPUBError::NavElementUnexpectedType, "Found a non-pageTarget element inside an NCX pageList");
			continue;
		}

		LoadChildNavPoint(Ptr(), sub);
        xml::Node::Unwrap(sub->xml());
        sub = sub->NextElementSibling();
	}

	return true;
}

void NavigationTable::LoadChildElements(shared_ptr<NavigationElement> pElement, shared_ptr<xml::Node> olNode)
{
#if EPUB_COMPILER_SUPPORTS(CXX_INITIALIZER_LISTS)
	XPathWrangler xpath(olNode->Document(), {{"epub", ePub3NamespaceURI},{"html", XHTMLNamespaceURI}});
#else
    XPathWrangler::NamespaceList __ns;
    __ns["ePub3"] = ePub3NamespaceURI;
    XPathWrangler xpath(olNode->doc, __ns);
#endif
    xpath.NameDefaultNamespace("html");

    xml::NodeSet liNodes = xpath.Nodes("./html:li", olNode);
    if ( liNodes.empty() )
    {
        // test for erroneous EPUB NavDoc (nested ol)
        xml::NodeSet olNodes = xpath.Nodes("./html:ol", olNode);
        if ( olNodes.size() > 0 )
        {
            olNode = olNodes[0];
            liNodes = xpath.Nodes("./html:li", olNode);
        }
    }

    for ( auto liNode : liNodes )
    {
        auto childElement = BuildNavigationPoint(liNode);
        if ( childElement )
        {
            pElement->AppendChild(childElement);
        }
        xml::Node::Unwrap(liNode->xml());
    }
}
void NavigationTable::LoadChildNavPoint(shared_ptr<NavigationElement> pElement, shared_ptr<xml::Node> node)
{
	auto childElement = BuildNCXNavigationPoint(node);
	if (childElement)
		pElement->AppendChild(childElement);
}

shared_ptr<NavigationElement> NavigationTable::BuildNavigationPoint(shared_ptr<xml::Node> liNode)
{
    auto elementPtr = CastPtr<NavigationElement>();
    auto liChild = liNode->FirstElementChild();

	if ( !bool(liChild) )
		return nullptr;

    auto point = std::make_shared<NavigationPoint>(elementPtr); //NavigationPoint::New(elementPtr);

    while ( bool(liChild) )
    {
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
        xml::Node::Unwrap(liChild->xml());
        liChild = liChild->NextElementSibling();
    }

    return point;
}

shared_ptr<NavigationElement> NavigationTable::BuildNCXNavigationPoint(shared_ptr<xml::Node> node)
{
	auto elementPtr = CastPtr<NavigationElement>();
	auto point = std::make_shared<NavigationPoint>(elementPtr); //NavigationPoint::New(elementPtr);

	for (auto sub = node->FirstElementChild(); bool(sub); )
	{
		string cName = sub->Name();
		string tmp;

		if (cName == "navLabel")
		{
			NCXNavLabelText(sub, tmp);
			point->SetTitle(tmp);
		}
		else if (cName == "content")
		{
			NCXContentHref(sub, tmp);
			point->SetContent(tmp);
		}
		else if (cName == "navPoint")
		{
			LoadChildNavPoint(point, sub);
		}
        xml::Node::Unwrap(sub->xml());
        sub = sub->NextElementSibling();
	}

	return point;
}

EPUB3_END_NAMESPACE
