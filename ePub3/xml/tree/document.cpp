//
//  document.cpp
//  ePub3
//
//  Created by Jim Dovey on 2012-11-20.
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

#include <ePub3/xml/document.h>
#include <ePub3/xml/element.h>
#include <ePub3/xml/dtd.h>
#include <libxml/xinclude.h>

typedef std::map<std::shared_ptr<ePub3::xml::Node>, xmlElementType> NodeMap;

void find_wrappers(xmlNodePtr node, NodeMap & nmap)
{
    if ( node == nullptr )
        return;
    
    // entity references can be cyclic, so ignore them
    // (libxml also does this)
    if ( node->type != XML_ENTITY_REF_NODE )
    {
        for ( xmlNodePtr child = node->children; child != nullptr; child = child->next )
            find_wrappers(child, nmap);
    }
    
    // local wrapper
    if ( node->_private != nullptr )
        nmap[reinterpret_cast<ePub3::xml::Node*>(node->_private)->shared_from_this()] = node->type;
    
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
    
    for ( xmlAttrPtr attr = node->properties; attr != nullptr; attr = attr->next ) {
        find_wrappers(reinterpret_cast<xmlNodePtr>(attr), nmap);
    }
}

void prune_unchanged_wrappers(xmlNodePtr node, NodeMap & nmap)
{
    if ( node == nullptr )
        return;
    
    if ( node->type != XML_ENTITY_REF_NODE )
    {
        for ( xmlNodePtr child = node->children; child != nullptr; child = child->next )
            prune_unchanged_wrappers(child, nmap);
    }
    
    if ( node->_private != nullptr )
    {
        const NodeMap::iterator pos = nmap.find(ePub3::xml::Wrapped<ePub3::xml::Node>(node));
        if ( pos != nmap.end() )
        {
            if ( pos->second == node->type )
                nmap.erase(pos);
            else
                ePub3::xml::Node::Unwrap(node);
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
    
    for ( xmlAttrPtr attr = node->properties; attr != nullptr; attr = attr->next ) {
        find_wrappers(reinterpret_cast<xmlNodePtr>(attr), nmap);
    }
}

EPUB3_XML_BEGIN_NAMESPACE

Document::Document(const string & version) : Node(reinterpret_cast<xmlNodePtr>(xmlNewDoc(version.utf8())))
{
}
Document::Document(xmlDocPtr doc) : Node(reinterpret_cast<xmlNodePtr>(doc))
{
    if ( _xml == nullptr )
        throw InternalError("Failed to create new document");
    // ensure the right polymorphic type ptr is installed
    //_xml->_private = this;
}
Document::Document(std::shared_ptr<Element> rootElement) : Node(reinterpret_cast<xmlNodePtr>(xmlNewDoc(BAD_CAST "1.0")))
{
    if ( SetRoot(rootElement) == nullptr )
        throw InternalError("Failed to set document root element");
}
Document::~Document()
{
    xmlDocPtr doc = xml();
    Unwrap(_xml);
    _xml = nullptr;
    xmlFreeDoc(doc);
}
string Document::Encoding() const
{
    return xml()->encoding;
}
std::shared_ptr<DTD> Document::InternalSubset() const
{
    return Wrapped<DTD, _xmlDtd>(xmlGetIntSubset(const_cast<xmlDocPtr>(xml())));
}
void Document::SetInternalSubset(const string &name, const string &externalID, const string &systemID)
{
    xmlDtd * dtd = xmlCreateIntSubset(xml(), name.utf8(), externalID.utf8(), systemID.utf8());
    if ( dtd != nullptr && dtd->_private == nullptr )
        (void) Wrapped<DTD, _xmlDtd>(dtd);
}
std::shared_ptr<Element> Document::Root()
{
    return Wrapped<Element, _xmlNode>(xmlDocGetRootElement(xml()));
}
std::shared_ptr<const Element> Document::Root() const
{
    return Wrapped<Element, _xmlNode>(xmlDocGetRootElement(const_cast<xmlDocPtr>(xml())));
}
std::shared_ptr<Element> Document::SetRoot(const string &name, const string &nsUri, const string &nsPrefix)
{
    auto newRoot = std::make_shared<Element>(name, std::static_pointer_cast<Document>(shared_from_this()), nsUri, nsPrefix);
    auto result = SetRoot(newRoot);
    return result;
}
std::shared_ptr<Element> Document::SetRoot(std::shared_ptr<const Node> nodeToCopy, bool recursive)
{
    xmlNodePtr theCopy = xmlDocCopyNode(const_cast<xmlNodePtr>(nodeToCopy->xml()), xml(), (recursive ? 1 : 0));
    if ( theCopy == nullptr )
        throw InternalError("Failed to copy new root node.");
    
    xmlNodePtr oldRoot = xmlDocSetRootElement(xml(), theCopy);
    if ( oldRoot != nullptr )
        xmlFreeNode(oldRoot);       // the glue will delete any associated C++ object
    return Root();
}
std::shared_ptr<Element> Document::SetRoot(std::shared_ptr<Element> element)
{
    xmlNodePtr xmlRoot = (element == nullptr ? nullptr : element->xml());
    xmlNodePtr oldRoot = xmlDocSetRootElement(xml(), xmlRoot);
    if ( oldRoot != nullptr )
        xmlFreeNode(oldRoot);
    return Root();
}
std::shared_ptr<Node> Document::AddNode(std::shared_ptr<Node> commentOrPINode, bool beforeRoot)
{
    if ( commentOrPINode->Type() != NodeType::Comment && commentOrPINode->Type() != NodeType::ProcessingInstruction )
        throw std::invalid_argument(std::string(__PRETTY_FUNCTION__) + ": argument must be a Comment or Processing Instruction");
    
    auto root = Root();
    if ( root == nullptr )
    {
        AddChild(commentOrPINode);
    }
    else if ( beforeRoot )
    {
        root->InsertBefore(commentOrPINode);
    }
    else
    {
        root->InsertAfter(commentOrPINode);
    }
    
    return commentOrPINode;
}
std::shared_ptr<Node> Document::AddComment(const string &comment, bool beforeRoot)
{
    return AddNode(Wrapped<Node, _xmlNode>(xmlNewDocComment(xml(), comment.utf8())), beforeRoot);
}
std::shared_ptr<Node> Document::AddProcessingInstruction(const ePub3::string &name, const ePub3::string &content, bool beforeRoot)
{
    return AddNode(Wrapped<Node, _xmlNode>(xmlNewDocPI(xml(), name.utf8(), content.utf8())), beforeRoot);
}
void Document::DeclareEntity(const string &name, EntityType type, const string &publicID, const string &systemID, const string &value)
{
    if ( xmlAddDocEntity(xml(), name.utf8(), static_cast<int>(type), publicID.utf8(), systemID.utf8(), value.utf8()) == nullptr )
        throw InternalError(std::string("Unable to add entity declaration for ") + name.c_str());
}
int Document::ProcessXInclude(bool generateXIncludeNodes)
{
    NodeMap nmap;
    xmlNodePtr root = xmlDocGetRootElement(xml());
    find_wrappers(root, nmap);
    
    xmlResetLastError();
    int substitutionCount = xmlXIncludeProcessTreeFlags(root, generateXIncludeNodes ? 0 : XML_PARSE_NOXINCNODE);
    
    prune_unchanged_wrappers(Node::xml(), nmap);
    
    if ( substitutionCount < 0 )
        throw InternalError("Failed to process XInclude", xmlGetLastError());
    
    return substitutionCount;
}
xmlEntityPtr Document::NamedEntity(const string &name) const
{
    return xmlGetDocEntity(const_cast<xmlDocPtr>(xml()), name.utf8());
}
string Document::ContentOfNamedEntity(const string &name) const
{
    xmlEntityPtr entity = NamedEntity(name);
    if ( entity == nullptr )
        return string();
    return entity->content;
}
void Document::WriteXML(OutputBuffer& outbuf) const
{
    outbuf.writeDocument(const_cast<xmlDocPtr>(xml()));
}
void Document::WriteXML(string& str) const
{
    std::ostringstream ss;
    StreamOutputBuffer buf(ss);
    WriteXML(buf);
    str.assign(ss.str());
}

EPUB3_XML_END_NAMESPACE
