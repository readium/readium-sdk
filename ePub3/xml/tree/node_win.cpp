//
//  node_win.cpp
//  ePub3
//
//  Created by Jim Dovey on 2013-09-24.
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

#include "node.h"
#include "../validation/ns.h"
#include "xpath.h"
#include "element.h"
#include "document.h"
#include "../validation/dtd.h"
#include <string>
#include <sstream>
#include <cstdlib>

// for IIterator
#include <collection.h>

using namespace Windows::Data::Xml::Dom;

EPUB3_XML_BEGIN_NAMESPACE

std::string TypeString(xml::NodeType type)
{
	std::string r;
#define _TYPESTR(x) case xml::NodeType::x: r = #x; break;
	switch (type)
	{
		_TYPESTR(Element);
		_TYPESTR(Attribute);
		_TYPESTR(Text);
		_TYPESTR(CDATASection);
		_TYPESTR(EntityReference);
		_TYPESTR(Entity);
		_TYPESTR(ProcessingInstruction);
		_TYPESTR(Comment);
		_TYPESTR(Document);
		_TYPESTR(DocumentType);
		_TYPESTR(DocumentFragment);
		_TYPESTR(Notation);
#if 0
		_TYPESTR(HTMLDocument);
		_TYPESTR(DTD);
		_TYPESTR(ElementDeclaration);
		_TYPESTR(AttributeDeclaration);
		_TYPESTR(EntityDeclaration);
		_TYPESTR(NamespaceDeclaration);
		_TYPESTR(XIncludeStart);
		_TYPESTR(XIncludeEnd);
#ifdef LIBXML_DOCBOOK_ENABLED
		_TYPESTR(DocbookSGMLDocument);
#endif
#endif
	default:
		break;
	}

	return r;
}

Node::Node(NativePtr xml) : _xml(xml)
{
}
#if EPUB_ENABLE(XML_BUILDER)
Node::Node(const string & name, NodeType type, const string & content, const class Namespace & ns)
{
	NativePtr newNode = nullptr;

	switch (type)
	{
	case NodeType::Element:
		// needs subclass
		break;

	case NodeType::Text:
		// needs subclass
		break;

	case NodeType::Attribute:
		// Not Applicable
		break;

	case NodeType::CDATASection:
#ifdef LIBXML_DOCBOOK_ENABLED
	case NodeType::DocbookSGMLDocument:
#endif
	case NodeType::Document:
	case NodeType::DocumentFragment:
	case NodeType::DTD:
	case NodeType::HTMLDocument:
		// need document ptr to create these
		break;

	case NodeType::Comment:
		// needs subclass
		break;

	case NodeType::ProcessingInstruction:
		newNode = xmlNewPI(name.utf8(), content.utf8());
		break;

	default:
		newNode = xmlNewNode(const_cast<_xmlNs*>(ns.xml()), name.utf8());
		break;
	}

	if (newNode == nullptr)
		throw InvalidNodeType(std::string("NodeType '") + TypeString(type) + "' is not supported");

	_xml = newNode;
	_xml->_private = this;
}
#endif
Node::Node(Node && o) : _xml(o._xml) {
	o._xml = nullptr;
}
Node::~Node()
{
}

#if 0
#pragma mark - Properties
#endif

string Node::Name() const
{
	return __winstr(_xml->LocalName);
}
#if EPUB_ENABLE(XML_BUILDER)
void Node::SetName(const string &name)
{
	xmlNodeSetName(_xml, name.utf8());
}
#endif
string Node::Content() const
{
	auto str = __winstr(_xml->InnerText);
	if (str == nullptr)
		return string();
	return str;
}
#if EPUB_ENABLE(XML_BUILDER)
void Node::SetContent(const string &content)
{
	xmlNodeSetContent(_xml, content.utf8());
}
#endif
string Node::AttributeValue(const string& name, const string& namespaceURI) const
{
	if (IsElementNode() == false)
		return string();

	IXmlElement^ element = dynamic_cast<IXmlElement^>(_xml);
	if (element == nullptr)
		return string();

	if (namespaceURI.empty())
		return element->GetAttribute(name);

	return element->GetAttributeNS(namespaceURI, name);
}
std::shared_ptr<Namespace> Node::Namespace() const
{
	switch (Type())
	{
	case NodeType::Document:
	case NodeType::DocumentType:
	case NodeType::DocumentFragment:
	case NodeType::DTD:
	case NodeType::CDATASection:
#if EPUB_USE(LIBXML2) && defined(LIBXML_DOCBOOK_ENABLED)
	case NodeType::DocbookSGMLDocument:
#endif
	case NodeType::HTMLDocument:
	case NodeType::ProcessingInstruction:
		// these types never have namespaces
		return nullptr;

	default:
		break;
	}

	return std::make_shared<class Namespace>(std::const_pointer_cast<class Document>(Document()), __winstr(_xml->Prefix), __winstr(_xml->NamespaceUri));
}
#if EPUB_ENABLE(XML_BUILDER)
void Node::SetNamespace(const class Namespace *ns)
{
	auto element = dynamic_cast<IXmlElement^>(_xml);
	if (element == nullptr)
		return;
	
	auto attr = element->GetAttributeNodeNS(XMLNSNamespace, ns->Prefix());
	if (attr != nullptr)
	{
		attr->NodeValue = ns->URI();
	}
	else if ((attr = element->GetAttributeNode(__TEXT("xmlns"))) != nullptr)
	{
		attr->NodeValue = ns->URI();
	}
	else if (ns->Prefix() != nullptr)
	{
		element->Prefix = ns->Prefix();
		string qname(__TEXT("xmlns"));
		qname.append(ns->Prefix().Data, ns->Prefix.Length);
		element->SetAttributeNS(XMLNSNamespace, qname, ns->URI());
	}
	else
	{
		element->Prefix = __TEXT("");
		element->SetAttribute(__TEXT("xmlns"), ns->URI());
	}
}
#endif
string Node::Language() const
{
	auto native = GetAttributeValueRecursiveNS(_xml, XMLNamespace, __TEXT("lang"));
	if (native == nullptr)
		return string();
	return string(native);
}
#if EPUB_ENABLE(XML_BUILDER)
void Node::SetLanguage(const string &language)
{
	auto element = dynamic_cast<IXmlElement^>(_xml);
	if (element == nullptr)
		return;

	auto attr = element->GetAttributeNodeNS(XMLNamespace, __TEXT("lang"));
	if (attr != nullptr)
	{
		if (language.empty())
			element->RemoveAttributeNode(attr);
		else
			attr->NodeValue = language;
	}
	else if (!language.empty())
	{
		element->SetAttributeNS(XMLNamespace, __TEXT("xml:lang"), language);
	}
}
#endif
bool Node::PreserveSpace() const
{
	auto native = GetAttributeValueRecursiveNS(_xml, XMLNamespace, __TEXT("space"));
	return native == __TEXT("preserve");
}
#if EPUB_ENABLE(XML_BUILDER)
void Node::SetPreserveSpace(bool preserve)
{
	auto element = dynamic_cast<IXmlElement^>(_xml);
	if (element == nullptr)
		return;

	auto attr = element->GetAttributeNodeNS(XMLNamespace, __TEXT("space"));
	if (attr != nullptr)
	{
		attr->NodeValue = (preserve ? __TEXT("preserve") : __TEXT("default"));
	}
	else
	{
		element->SetAttributeNS(XMLNamespace, __TEXT("xml:space"), (preserve ? __TEXT("preserve") : __TEXT("default"));
	}
}
#endif
string Node::BaseURL() const
{
	auto native = GetAttributeValueRecursiveNS(_xml, XMLNamespace, __TEXT("base"));
	if (native == nullptr)
		return string();
	return string(native);
}
#if EPUB_ENABLE(XML_BUILDER)
void Node::SetBaseURL(const string &baseURL)
{
	auto element = dynamic_cast<IXmlElement^>(_xml);
	if (element == nullptr)
		return;

	auto attr = element->GetAttributeNodeNS(XMLNamespace, __TEXT("base"));
	if (attr != nullptr)
	{
		if (baseURL.empty())
			element->RemoveAttributeNode(attr);
		else
			attr->NodeValue = baseURL;
	}
	else if (!baseURL.empty())
	{
		element->SetAttributeNS(XMLNamespace, __TEXT("xml:base"), baseURL);
	}
}
#endif
NamespaceList Node::NamespacesInScope() const
{
	NamespaceList result;
	auto element = dynamic_cast<IXmlElement^>(_xml);
	if (element == nullptr)
		element = dynamic_cast<IXmlElement^>(_xml->ParentNode);
	std::map<::Platform::String^, std::shared_ptr<class Namespace>> nsmap;

	while (element != nullptr)
	{
		auto prefix = __winstr(element->Prefix);
		if (prefix->IsEmpty() == false)
		{
			auto uri = __winstr(element->NamespaceUri);
			nsmap[uri] = std::make_shared<class Namespace>(std::const_pointer_cast<class Document>(Document()), prefix, uri);
		}

		element = dynamic_cast<IXmlElement^>(element->ParentNode);
	}

	for (auto& pair : nsmap)
	{
		result.push_back(pair.second);
	}

	return result;
}
NodeType Node::Type() const
{
	return NodeType(_xml->NodeType);
}
#if EPUB_USE(LIBXML2)
int Node::Index() const
{
	int idx = 1;
	xmlNodePtr node = _xml;
	while (node->prev != nullptr)
	{
		idx++;
		node = node->prev;
	}
	return idx;
}
int Node::Line() const
{
	return static_cast<int>(XML_GET_LINE(_xml));
}
#endif

#if 0
#pragma mark - Values
#endif

string Node::XMLString() const
{
	return _xml->GetXml();
}
string Node::StringValue() const
{
	return _xml->InnerText;
}
int Node::IntValue() const
{
	return ::_wtoi(StringValue().c_str());
}
double Node::DoubleValue() const
{
	return ::wcstod(StringValue().c_str(), nullptr);
}
bool Node::BoolValue() const
{
	string str(StringValue());
	if (::_wcsnicmp(str.c_str(), __TEXT("true"), min((string::size_type)4, str.length())) == 0)
		return true;
	if (::_wcsnicmp(str.c_str(), __TEXT("yes"), min((string::size_type)3, str.length())) == 0)
		return true;

	return (::_wtoi(str.c_str()) != 0);
}

#if 0
#pragma mark - Hierarchy
#endif

std::shared_ptr<Document> Node::Document()
{
	return std::make_shared<class Document>(_xml->OwnerDocument);
}
std::shared_ptr<const Document> Node::Document() const
{
	return const_cast<Node*>(this)->Document();
}
std::shared_ptr<Node> Node::NextSibling()
{
	if (_xml->NextSibling == nullptr)
		return nullptr;
	return NewNode(_xml->NextSibling);
}
std::shared_ptr<const Node> Node::NextSibling() const
{
	return const_cast<Node*>(this)->NextSibling();
}
std::shared_ptr<Node> Node::NextElementSibling()
{
	auto next = _xml->NextSibling;
	while (next != nullptr && next->NodeType != ::Windows::Data::Xml::Dom::NodeType::ElementNode)
		next = next->NextSibling;
	if (next == nullptr)
		return nullptr;
	return NewNode(next);
}
std::shared_ptr<const Node> Node::NextElementSibling() const
{
	return const_cast<Node*>(this)->NextElementSibling();
}
std::shared_ptr<Node> Node::PreviousSibling()
{
	if (_xml->PreviousSibling == nullptr)
		return nullptr;
	return NewNode(_xml->PreviousSibling);
}
std::shared_ptr<const Node> Node::PreviousSibling() const
{
	return const_cast<Node*>(this)->PreviousSibling();
}
std::shared_ptr<Node> Node::FirstChild(const string & filterByName)
{
	if (_xml->HasChildNodes() == false)
		return nullptr;

	if (filterByName.empty())
		return NewNode(_xml->FirstChild);

	auto child = _xml->FirstChild;
	do
	{
		if (filterByName == __winstr(child->LocalName)->Data())
			return NewNode(child);

	} while ((child = child->NextSibling) != nullptr);

	return nullptr;
}
std::shared_ptr<const Node> Node::FirstChild(const string & filterByName) const
{
	return const_cast<Node*>(this)->FirstChild(filterByName);
}
std::shared_ptr<Node> Node::FirstElementChild()
{
	auto child = _xml->FirstChild;
	while (child != nullptr && child->NodeType != ::Windows::Data::Xml::Dom::NodeType::ElementNode)
		child = child->NextSibling;
	if (child == nullptr)
		return nullptr;
	return NewNode(child);
}
std::shared_ptr<const Node> Node::FirstElementChild() const
{
	return const_cast<Node*>(this)->FirstElementChild();
}
Node::NodeList Node::Children(const string & filterByName)
{
	NodeList list;
	if (_xml->HasChildNodes() == false)
		return list;

	for (auto child = _xml->FirstChild; child != nullptr; child = child->NextSibling)
	{
		if (filterByName.empty() || filterByName == __winstr(child->LocalName)->Data())
			list.push_back(NewNode(child));
	}
	return list;
}
const Node::NodeList Node::Children(const string & filterByName) const
{
	return const_cast<Node*>(this)->Children(filterByName);
}
#if EPUB_ENABLE(XML_BUILDER)
static XmlElement^ NewElement(::Platform::String^ name, ::Platform::String^ prefix, Node* contextNode)
{
	XmlElement^ newNode = nullptr;

	if (prefix->IsEmpty())
	{
		newNode = contextNode->xml()->OwnerDocument->CreateElement(name);
	}
	else
	{
		NamespaceList nslist = contextNode->NamespacesInScope();
		::Platform::String^ uri = nullptr;

		for (class Namespace* ns : nslist)
		{
			if (prefix == ns->Prefix())
			{
				uri = ns->URI();
				break;
			}
		}

		::Platform::String^ qname = ::Platform::String::Concat(::Platform::String::Concat(prefix, __TEXT(":")), name);
		newNode = contextNode->xml()->OwnerDocument->CreateElementNS(uri, qname);
	}

	return newNode;
}
Element * Node::AddChild(const string &name, const string & prefix)
{
	auto child = NewElement(name, prefix, this);
	if (child == nullptr)
		return nullptr;
	
	XmlElement^ newNode = dynamic_cast<XmlElement^>(_xml->AppendChild(child));
	if (newNode == nullptr)
		return nullptr;

	return new Element(newNode);
}
void Node::AddChild(Node *child)
{
	_xml->AppendChild(child->xml());
}
Element * Node::InsertAfter(const string &name, const string & prefix)
{
	if (_xml->ParentNode == nullptr)
		return nullptr;

	auto child = NewElement(name, prefix, this);
	if (_xml->NextSibling != nullptr)
		child = _xml->ParentNode->InsertBefore(child, _xml->NextSibling);
	else
		child = _xml->ParentNode->AppendChild(child);

	if (child == nullptr)
		return nullptr;

	return new Element(newNode);
}
void Node::InsertAfter(Node *child)
{
	xmlNodePtr newNode = xmlAddNextSibling(xml(), child->xml());
	if (newNode == nullptr)
		throw InternalError("Unable to add child node", xmlGetLastError());
	child->rebind(newNode);
}
Element * Node::InsertBefore(const string &name, const string & prefix)
{
	xmlNodePtr child = createChild(name, prefix);
	xmlNodePtr newNode = xmlAddPrevSibling(xml(), child);
	if (newNode == nullptr)
	{
		xmlFreeNode(child);
		throw InternalError(std::string("Could not add child element node named '") + name.c_str() + "'", xmlGetLastError());
	}

	return Wrapped<Element, _xmlNode>(newNode);
}
void Node::InsertBefore(Node *child)
{
	xmlNodePtr newNode = xmlAddPrevSibling(xml(), child->xml());
	if (newNode == nullptr)
		throw InternalError("Unable to add child node", xmlGetLastError());
	child->rebind(newNode);
}
Node * Node::CopyIn(const Node *nodeToCopy, bool recursive)
{
	if (nodeToCopy == nullptr)
		return nullptr;

	// complains about the difference between 'const xmlNode *' and 'xmlNode const *' ...
	xmlNodePtr theCopy = xmlDocCopyNode(const_cast<_xmlNode *>(nodeToCopy->xml()), _xml->doc, recursive ? 1 : 0);
	if (theCopy == nullptr)
		throw InternalError("Unable to copy node", xmlGetLastError());

	xmlNode * added = xmlAddChild(_xml, theCopy);
	if (added == nullptr)
	{
		xmlFreeNode(theCopy);
		throw InternalError("Unable to add copied node as a new child", xmlGetLastError());
	}

	// if added != theCopy, then theCopy has already been freed
	return Wrapped<Node, _xmlNode>(added);
}
void Node::Detach()
{
	xmlUnlinkNode(_xml);
}
#endif	// EPUB_ENABLE(XML_BUILDER)
#if 0
#pragma mark - XPath Utilities
#endif

NodeSet Node::FindByXPath(const string &xpath) const
{
	auto nodeList = _xml->SelectNodes(xpath);
	if (nodeList == nullptr)
		return NodeSet();

	NodeSet result;
	auto pos = nodeList->First();
	while (pos->HasCurrent)
	{
		result.push_back(NewNode(pos->Current));
		pos->MoveNext();
	}

	return result;
}
NodeSet Node::FindByXPath(const string &xpath, const NamespaceMap &namespaces) const
{
	XPathEvaluator eval(xpath, Document());
	eval.RegisterNamespaces(namespaces);

	XPathEvaluator::ObjectType type = XPathEvaluator::ObjectType::Undefined;
	if (eval.Evaluate(shared_from_this(), &type) && type == XPathEvaluator::ObjectType::NodeSet)
		return eval.NodeSetResult();

	return NodeSet();
}
std::shared_ptr<Node> Node::NewNode(NativePtr newNode)
{
	using ::Windows::Data::Xml::Dom::NodeType;

	switch (newNode->NodeType)
	{
	case NodeType::ElementNode:
		return std::make_shared<Element>(dynamic_cast<XmlElement^>(newNode));
	
	case NodeType::DocumentNode:
		return std::make_shared<class Document>(dynamic_cast<XmlDocument^>(newNode));

	case NodeType::NotationNode:
		return std::make_shared<DTD>(dynamic_cast<DtdNotation^>(newNode));

	default:
		break;
	}

	return std::make_shared<Node>(newNode);
}

EPUB3_XML_END_NAMESPACE
