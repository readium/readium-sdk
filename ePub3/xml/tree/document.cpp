//
//  document.cpp
//  ePub3
//
//  Created by Jim Dovey on 2012-11-20.
//  Copyright (c) 2012 Kobo Inc. All rights reserved.
//

#include "document.h"
#include "element.h"
#include "dtd.h"
#include <libxml/xinclude.h>

typedef std::map<ePub3::xml::Node*, xmlElementType> NodeMap;

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
        const NodeMap::iterator pos = nmap.find(reinterpret_cast<ePub3::xml::Node*>(node->_private));
        if ( pos != nmap.end() )
        {
            if ( pos->second == node->type )
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
    
    for ( xmlAttrPtr attr = node->properties; attr != nullptr; attr = attr->next ) {
        find_wrappers(reinterpret_cast<xmlNodePtr>(attr), nmap);
    }
}

EPUB3_XML_BEGIN_NAMESPACE

Document::Document(const string & version) : Document(xmlNewDoc(version.utf8()))
{
}
Document::Document(xmlDocPtr doc) : Node(reinterpret_cast<xmlNodePtr>(doc))
{
    if ( _xml == nullptr )
        throw InternalError("Failed to create new document");
    // ensure the right polymorphic type ptr is installed
    _xml->_private = this;
}
Document::Document(Element * rootElement) : Document()
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
DTD * Document::InternalSubset() const
{
    return Wrapped<DTD, _xmlDtd>(xmlGetIntSubset(const_cast<xmlDocPtr>(xml())));
}
void Document::SetInternalSubset(const string &name, const string &externalID, const string &systemID)
{
    xmlDtd * dtd = xmlCreateIntSubset(xml(), name.utf8(), externalID.utf8(), systemID.utf8());
    if ( dtd != nullptr && dtd->_private == nullptr )
        (void) Wrapped<DTD, _xmlDtd>(dtd);
}
Element * Document::Root()
{
    return Wrapped<Element, _xmlNode>(xmlDocGetRootElement(xml()));
}
const Element * Document::Root() const
{
    return Wrapped<Element, _xmlNode>(xmlDocGetRootElement(const_cast<xmlDocPtr>(xml())));
}
Element * Document::SetRoot(const string &name, const string &nsUri, const string &nsPrefix)
{
    Element * newRoot = new Element(name, this, nsUri, nsPrefix);
    Element * result = SetRoot(newRoot);
    if ( result != newRoot )
        delete newRoot;
    return newRoot;
}
Element * Document::SetRoot(const Node *nodeToCopy, bool recursive)
{
    xmlNodePtr theCopy = xmlDocCopyNode(const_cast<xmlNodePtr>(nodeToCopy->xml()), xml(), (recursive ? 1 : 0));
    if ( theCopy == nullptr )
        throw InternalError("Failed to copy new root node.");
    
    xmlNodePtr oldRoot = xmlDocSetRootElement(xml(), theCopy);
    if ( oldRoot != nullptr )
        xmlFreeNode(oldRoot);       // the glue will delete any associated C++ object
    return Root();
}
Element * Document::SetRoot(Element * element)
{
    xmlNodePtr xmlRoot = (element == nullptr ? nullptr : element->xml());
    xmlNodePtr oldRoot = xmlDocSetRootElement(xml(), xmlRoot);
    if ( oldRoot != nullptr )
        xmlFreeNode(oldRoot);
    return Root();
}
Node * Document::AddNode(Node *commentOrPINode, bool beforeRoot)
{
    if ( commentOrPINode->Type() != NodeType::Comment && commentOrPINode->Type() != NodeType::ProcessingInstruction )
        throw std::invalid_argument(std::string(__PRETTY_FUNCTION__) + ": argument must be a Comment or Processing Instruction");
    
    Element * root = Root();
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
Node * Document::AddComment(const string &comment, bool beforeRoot)
{
    return AddNode(Wrapped<Node, _xmlNode>(xmlNewDocComment(xml(), comment.utf8())), beforeRoot);
}
Node * Document::AddProcessingInstruction(const ePub3::xml::string &name, const ePub3::xml::string &content, bool beforeRoot)
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
    for ( auto item : nmap )
    {
        delete item.first;
    }
    
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

EPUB3_XML_END_NAMESPACE
