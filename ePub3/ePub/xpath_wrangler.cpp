//
//  xpath_wrangler.cpp
//  ePub3
//
//  Created by Jim Dovey on 2012-11-29.
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//  
//  Redistribution and use in source and binary forms, with or without modification, 
//  are permitted provided that the following conditions are met:
//  1. Redistributions of source code must retain the above copyright notice, this 
//  list of conditions and the following disclaimer.
//  2. Redistributions in binary form must reproduce the above copyright notice, 
//  this list of conditions and the following disclaimer in the documentation and/or 
//  other materials provided with the distribution.
//  3. Neither the name of the organization nor the names of its contributors may be 
//  used to endorse or promote products derived from this software without specific 
//  prior written permission.
//  
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
//  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
//  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
//  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
//  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
//  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
//  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
//  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE 
//  OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED 
//  OF THE POSSIBILITY OF SUCH DAMAGE.

#include "xpath_wrangler.h"
#include <ePub3/xml/xpath.h>
#include <ePub3/xml/document.h>

EPUB3_BEGIN_NAMESPACE

XPathWrangler::XPathWrangler(shared_ptr<xml::Document> doc, const NamespaceList& namespaces) : _doc(doc), _namespaces(namespaces)
{
}
XPathWrangler::XPathWrangler(const XPathWrangler& o) : _doc(o._doc), _namespaces(o._namespaces)
{
}
XPathWrangler::XPathWrangler(XPathWrangler&& o) : _doc(std::move(o._doc))
{
}
XPathWrangler::~XPathWrangler()
{
}
XPathWrangler::StringList XPathWrangler::Strings(const string& xpath, shared_ptr<xml::Node> node)
{
    StringList strings;
    
	xml::XPathEvaluator eval(xml::string(xpath.c_str()), _doc);
	xml::XPathEvaluator::ObjectType type;
    for (auto& pair : _namespaces)
    {
        eval.RegisterNamespace(pair.first.stl_str(), pair.second.stl_str());
    }

	if ( eval.Evaluate((bool(node) ? node : _doc), &type) )
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
				for (shared_ptr<xml::Node> node : nodes)
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
xml::NodeSet XPathWrangler::Nodes(const string& xpath, shared_ptr<xml::Node> node)
{
	xml::NodeSet result;

    xml::XPathEvaluator eval(xml::string(xpath.c_str()), _doc);
	for (auto& item : _namespaces)
	{
		eval.RegisterNamespace(item.first.stl_str(), item.second.stl_str());
	}
	xml::XPathEvaluator::ObjectType type;
    if ( eval.Evaluate((bool(node) ? node : _doc), &type) )
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
	for (auto ns : allNS)
	{
		if (ns->Prefix().empty())
		{
			_namespaces[""] = ns->URI();
		}
	}
}

EPUB3_END_NAMESPACE
