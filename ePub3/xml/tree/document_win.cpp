//
//  document_win.cpp
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

#include "document.h"
#include "element.h"
#include "../validation/dtd.h"

#include <collection.h>

using namespace ::Windows::Data::Xml::Dom;

#if 0
typedef std::map<ePub3::xml::Node*, ePub3::xml::NodeType> NodeMap;

void find_wrappers(xmlNodePtr node, NodeMap & nmap)
{
	if (node == nullptr)
		return;

	// entity references can be cyclic, so ignore them
	// (libxml also does this)
	if (node->type != XML_ENTITY_REF_NODE)
	{
		for (xmlNodePtr child = node->children; child != nullptr; child = child->next)
			find_wrappers(child, nmap);
	}

	// local wrapper
	if (node->_private != nullptr)
		nmap[reinterpret_cast<ePub3::xml::Node*>(node->_private)] = node->type;

	switch (node->type)
	{
	case XML_DTD_NODE:
	case XML_ATTRIBUTE_NODE:
	case XML_ELEMENT_DECL:
	case XML_ATTRIBUTE_DECL:
	case XML_ENTITY_DECL:
	case XML_DOCUMENT_NODE:
		return;
	default:
		break;
	}

	for (xmlAttrPtr attr = node->properties; attr != nullptr; attr = attr->next) {
		find_wrappers(reinterpret_cast<xmlNodePtr>(attr), nmap);
	}
}

void prune_unchanged_wrappers(xmlNodePtr node, NodeMap & nmap)
{
	if (node == nullptr)
		return;

	if (node->type != XML_ENTITY_REF_NODE)
	{
		for (xmlNodePtr child = node->children; child != nullptr; child = child->next)
			prune_unchanged_wrappers(child, nmap);
	}

	if (node->_private != nullptr)
	{
		const NodeMap::iterator pos = nmap.find(reinterpret_cast<ePub3::xml::Node*>(node->_private));
		if (pos != nmap.end())
		{
			if (pos->second == node->type)
				nmap.erase(pos);
			else
				node->_private = nullptr;
		}
	}

	switch (node->type)
	{
	case XML_DTD_NODE:
	case XML_ATTRIBUTE_NODE:
	case XML_ELEMENT_DECL:
	case XML_ATTRIBUTE_DECL:
	case XML_ENTITY_DECL:
	case XML_DOCUMENT_NODE:
		return;
	default:
		break;
	}

	for (xmlAttrPtr attr = node->properties; attr != nullptr; attr = attr->next) {
		find_wrappers(reinterpret_cast<xmlNodePtr>(attr), nmap);
	}
}
#endif

EPUB3_XML_BEGIN_NAMESPACE

#if EPUB_ENABLE(XML_BUILDER)
Document::Document(const string & version) : Node(reinterpret_cast<xmlNodePtr>(xmlNewDoc(version.utf8())))
{
}
#endif
Document::Document(NativeDocPtr doc) : Node(xml_native_cast<NativePtr>(doc))
{
	if (_xml == nullptr)
		throw InternalError("Failed to create new document");
}
#if EPUB_ENABLE(XML_BUILDER)
Document::Document(Element * rootElement) : Node(reinterpret_cast<xmlNodePtr>(xmlNewDoc(BAD_CAST "1.0")))
{
	if (SetRoot(rootElement) == nullptr)
		throw InternalError("Failed to set document root element");
}
#endif
Document::~Document()
{
}
string Document::Encoding() const
{
	return (const char*)"utf-8";
}
std::shared_ptr<DTD> Document::InternalSubset() const
{
	auto notation = dynamic_cast<DtdNotation^>(xml()->Doctype->Notations->First()->Current);
	if (notation == nullptr)
		return nullptr;
	return std::make_shared<DTD>(notation);
}
#if EPUB_ENABLE(XML_BUILDER)
void Document::SetInternalSubset(const string &name, const string &externalID, const string &systemID)
{
	xmlDtd * dtd = xmlCreateIntSubset(xml(), name.utf8(), externalID.utf8(), systemID.utf8());
	if (dtd != nullptr && dtd->_private == nullptr)
		(void)Wrapped<DTD, _xmlDtd>(dtd);
}
#endif
std::shared_ptr<Element> Document::Root()
{
	XmlElement^ element = xml()->DocumentElement;
	if (element == nullptr)
		return nullptr;
	return std::make_shared<Element>(element);
}
std::shared_ptr<const Element> Document::Root() const
{
	return const_cast<Document*>(this)->Root();
}
#if EPUB_ENABLE(XML_BUILDER)
Element * Document::SetRoot(const string &name, const string &nsUri, const string &nsPrefix)
{
	Element * newRoot = new Element(name, this, nsUri, nsPrefix);
	Element * result = SetRoot(newRoot);
	if (result != newRoot)
		delete newRoot;
	return newRoot;
}
Element * Document::SetRoot(const Node *nodeToCopy, bool recursive)
{
	xmlNodePtr theCopy = xmlDocCopyNode(const_cast<xmlNodePtr>(nodeToCopy->xml()), xml(), (recursive ? 1 : 0));
	if (theCopy == nullptr)
		throw InternalError("Failed to copy new root node.");

	xmlNodePtr oldRoot = xmlDocSetRootElement(xml(), theCopy);
	if (oldRoot != nullptr)
		xmlFreeNode(oldRoot);       // the glue will delete any associated C++ object
	return Root();
}
Element * Document::SetRoot(Element * element)
{
	xmlNodePtr xmlRoot = (element == nullptr ? nullptr : element->xml());
	xmlNodePtr oldRoot = xmlDocSetRootElement(xml(), xmlRoot);
	if (oldRoot != nullptr)
		xmlFreeNode(oldRoot);
	return Root();
}
Node * Document::AddNode(Node *commentOrPINode, bool beforeRoot)
{
	if (commentOrPINode->Type() != NodeType::Comment && commentOrPINode->Type() != NodeType::ProcessingInstruction)
		throw std::invalid_argument(std::string(__PRETTY_FUNCTION__) + ": argument must be a Comment or Processing Instruction");

	Element * root = Root();
	if (root == nullptr)
	{
		AddChild(commentOrPINode);
	}
	else if (beforeRoot)
	{
		root->InsertBefore(commentOrPINode);
	}
	else
	{
		root->InsertAfter(commentOrPINode);
	}

	return commentOrPINode;
}
Node * Document::AddComment(const string &comment, bool beforeRoot)
{
	return AddNode(Wrapped<Node, _xmlNode>(xmlNewDocComment(xml(), comment.utf8())), beforeRoot);
}
Node * Document::AddProcessingInstruction(const ePub3::string &name, const ePub3::string &content, bool beforeRoot)
{
	return AddNode(Wrapped<Node, _xmlNode>(xmlNewDocPI(xml(), name.utf8(), content.utf8())), beforeRoot);
}
void Document::DeclareEntity(const string &name, EntityType type, const string &publicID, const string &systemID, const string &value)
{
	if (xmlAddDocEntity(xml(), name.utf8(), static_cast<int>(type), publicID.utf8(), systemID.utf8(), value.utf8()) == nullptr)
		throw InternalError(std::string("Unable to add entity declaration for ") + name.c_str());
}
#endif
Document::NativeEntityPtr Document::NamedEntity(const string &name) const
{
	return dynamic_cast<NativeEntityPtr>(xml()->Doctype->Entities->GetNamedItem(name));
}
string Document::ContentOfNamedEntity(const string &name) const
{
	NativeEntityPtr entity = NamedEntity(name);
	if (entity == nullptr)
		return string();
	return string(entity->NodeValue->ToString());
}
void Document::WriteXML(string& str) const
{
	str = string(_xml->GetXml());
}

EPUB3_XML_END_NAMESPACE
