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
#include <ePub3/xml/xpath.h>
#include <ePub3/xml/document.h>

EPUB3_BEGIN_NAMESPACE

XPathWrangler::XPathWrangler(xml::Document* doc, const NamespaceList& namespaces) : _doc(new xml::Document(doc->xml())), _namespaces(namespaces)
{
}
XPathWrangler::XPathWrangler(const XPathWrangler& o) : _doc(new xml::Document(o._doc->xml())), _namespaces(o._namespaces)
{
}
XPathWrangler::XPathWrangler(XPathWrangler&& o) : _doc(o._doc)
{
    o._doc = nullptr;
}
XPathWrangler::~XPathWrangler()
{
	if (_doc != nullptr)
		delete _doc;
}
XPathWrangler::StringList XPathWrangler::Strings(const string& xpath, xml::Node* node)
{
    StringList strings;
    
	xml::XPathEvaluator eval(xml::string(xpath.c_str()), _doc);
	xml::XPathEvaluator::ObjectType type;

	if ( eval.Evaluate((node ? node : _doc), &type) )
    {
        switch ( type )
        {
			case xml::XPathEvaluator::ObjectType::String:
                // a single string
                strings.emplace_back(eval.StringResult());
                break;
			case xml::XPathEvaluator::ObjectType::NodeSet:
			{
				xml::NodeSet nodes(eval.NodeSetResult());

				// a list of strings (I hope)
				for (xml::Node* node : nodes)
				{
					strings.emplace_back(node->StringValue());
				}
				break;
			}
            default:
                break;
        }
    }
    
    return strings;
}
xml::NodeSet XPathWrangler::Nodes(const string& xpath, xml::Node* node)
{
	xml::NodeSet result;

    xml::XPathEvaluator eval(xml::string(xpath.c_str()), _doc);
	xml::XPathEvaluator::ObjectType type;
    if ( eval.Evaluate((node ? node : _doc), &type) )
    {
        if ( type == xml::XPathEvaluator::ObjectType::NodeSet )
        {
			result = eval.NodeSetResult();
        }
    }
    
    return result;
}
void XPathWrangler::RegisterNamespaces(const NamespaceList &namespaces)
{
    for ( auto item : namespaces )
    {
		_namespaces[item.first] = item.second;
    }
}
void XPathWrangler::NameDefaultNamespace(const string& name)
{
	xml::NamespaceList allNS = _doc->NamespacesInScope();
	for (xml::Namespace* ns : allNS)
	{
		if (ns->Prefix().empty())
		{
			_namespaces[""] = ns->URI();
		}
	}
}

EPUB3_END_NAMESPACE
