//
//  xpath_win.cpp
//  ePub3
//
//  Created by Jim Dovey on 2013-09-24.
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

#include "xpath.h"
#include "node.h"
#include "element.h"

#include <sstream>

using namespace ::Windows::Data::Xml::Dom;

EPUB3_XML_BEGIN_NAMESPACE

XPathEvaluator::XPathEvaluator(const string & xpath, std::shared_ptr<const class Document> document)
: _xpath(xpath), _document(document), _lastResult(nullptr)
{
}
XPathEvaluator::~XPathEvaluator()
{
	_lastResult = nullptr;
}

#if 0
#pragma mark - XPath Environment
#endif

bool XPathEvaluator::RegisterNamespace(const string &prefix, const string &uri)
{
	_namespaces[prefix] = uri;
	return true;
}
bool XPathEvaluator::RegisterNamespaces(const NamespaceMap &namespaces)
{
	for (auto item : namespaces)
	{
		_namespaces[item.first] = item.second;
	}
	return true;
}
bool XPathEvaluator::RegisterAllNamespacesForElement(std::shared_ptr<const Element> element)
{
	NamespaceList nslist = element->NamespacesInScope();
	for (Namespace* ns : nslist)
	{
		_namespaces[ns->Prefix()] = ns->URI();
	}
	return true;
}

#if 0
#pragma mark - XPath Evaluation
#endif

bool XPathEvaluator::Evaluate(std::shared_ptr<const Node> node, ObjectType * resultType)
{
	try
	{
		if (_namespaces.empty())
		{
			_lastResult = node->xml()->SelectNodes(_xpath);
		}
		else
		{
			std::wstringstream s;
			for (auto& pair : _namespaces)
			{
				if (pair.second.empty())
					continue;

				if (s.tellp() > 0)
					s << TEXT(' ');
				s << TEXT("xmlns");
				if (!pair.first.empty())
				{
					s << TEXT(':');
					s << pair.first;
				}
				s << TEXT("=\"");
				s << pair.second;
				s << TEXT('"');
			}

			string ns(s.str());
			try
			{
				_lastResult = node->xml()->SelectNodesNS(_xpath, ns);
			}
			catch (...)
			{
				_lastResult = nullptr;
			}
		}

		if (_lastResult == nullptr)
			return false;

		if (resultType != nullptr)
		{
			if (_lastResult->Size > 1)
			{
				*resultType = ObjectType::NodeSet;
			}
			else
			{
				IXmlNode^ inode = _lastResult->Item(0);
				using ::Windows::Data::Xml::Dom::NodeType;
				if ((inode != nullptr) && (inode->NodeType == NodeType::TextNode))
				{
					XmlText^ txt = dynamic_cast<XmlText^>(inode);
					string str(txt->Data);
					if (_wtoi(str.c_str()) != 0)
					{
						*resultType = ObjectType::Number;
					}
					else if (::_wcsnicmp(str.c_str(), __TEXT("true"), min((string::size_type)4, str.length())) == 0)
					{
						*resultType = ObjectType::Boolean;
					}
					else if (::_wcsnicmp(str.c_str(), __TEXT("yes"), min((string::size_type)3, str.length())) == 0)
					{
						*resultType = ObjectType::Boolean;
					}
					else
					{
						*resultType = ObjectType::String;
					}
				}
				else
				{
					*resultType = ObjectType::NodeSet;
				}
			}
		}

		return true;
	}
	catch (...)
	{
	}

	return false;
}
bool XPathEvaluator::EvaluateAsBoolean(std::shared_ptr<const Node> node)
{
	_lastResult = nullptr;

	IXmlNode^ value = nullptr;
	try
	{
		if (_namespaces.empty())
		{
			value = node->xml()->SelectSingleNode(_xpath);
		}
		else
		{
			std::wstringstream s;
			for (auto& pair : _namespaces)
			{
				if (pair.second.empty())
					continue;

				if (s.tellp() > 0)
					s << TEXT(' ');
				s << TEXT("xmlns");
				if (!pair.first.empty())
				{
					s << TEXT(':');
					s << pair.first;
				}
				s << TEXT("=\"");
				s << pair.second;
				s << TEXT('"');
			}

			string ns(s.str());
			value = node->xml()->SelectSingleNodeNS(_xpath, ns);
		}
	}
	catch (...)
	{
	}

	if (value == nullptr)
		return false;

	return true;
}
bool XPathEvaluator::BooleanResult() const
{
	if (_lastResult == nullptr)
		throw InternalError(std::string(__PRETTY_FUNCTION__) + " called when no result available");
	if (_lastResult->Size == 0)
		return false;
	IXmlNode^ node = _lastResult->Item(0);
	if (NodeType(node->NodeType) != NodeType::Text)
		return true;
	
	string str(dynamic_cast<XmlText^>(node)->Data);
	if (::_wcsnicmp(str.c_str(), __TEXT("true"), min((string::size_type)4, str.length())) == 0)
	{
		return true;
	}
	else if (::_wcsnicmp(str.c_str(), __TEXT("yes"), min((string::size_type)3, str.length())) == 0)
	{
		return true;
	}

	return (_wtoi(str.c_str()) != 0);
}
double XPathEvaluator::NumberResult() const
{
	if (_lastResult == nullptr)
		throw InternalError(std::string(__PRETTY_FUNCTION__) + " called when no result available");
	if (_lastResult->Size == 0)
		return 0.0;
	IXmlNode^ node = _lastResult->Item(0);
	if (NodeType(node->NodeType) != NodeType::Text)
		return 0.0;

	string str(dynamic_cast<XmlText^>(node)->Data);
	return (_wtof(str.c_str()) != 0);
}
string XPathEvaluator::StringResult() const
{
	if (_lastResult == nullptr)
		throw InternalError(std::string(__PRETTY_FUNCTION__) + " called when no result available");
	if (_lastResult->Size == 0)
		return string();
	IXmlNode^ node = _lastResult->Item(0);
	if (NodeType(node->NodeType) != NodeType::Text)
		return string();

	return string(dynamic_cast<XmlText^>(node)->Data);
}
NodeSet XPathEvaluator::NodeSetResult() const
{
	if (_lastResult == nullptr)
		throw InternalError(std::string(__PRETTY_FUNCTION__) + " called when no result available");

	NodeSet result;
	auto pos = _lastResult->First();
	while (pos->HasCurrent)
	{
		result.push_back(Node::NewNode(pos->Current));
		pos->MoveNext();
	}

	return result;
}

EPUB3_XML_END_NAMESPACE
