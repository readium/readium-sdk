//
//  node.cpp
//  ePub3
//
//  Created by Jim Dovey on 2012-11-19.
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

#include <ePub3/xml/node.h>
#include <ePub3/xml/io.h>
#include <ePub3/xml/ns.h>
#include <ePub3/xml/xpath.h>
#include <ePub3/xml/document.h>
#include <string>
#include <sstream>
#include <cstdlib>

EPUB3_XML_BEGIN_NAMESPACE

std::string TypeString(NodeType type)
{
    std::string r;
#define _TYPESTR(x) case NodeType::x: r = #x; break;
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
        default:
            break;
    }
    
    return r;
}

Node::Node(_xmlNode *xml) : _xml(xml)
{
    _xml->_private = this;
}
Node::Node(const string & name, NodeType type, const string & content, const class Namespace & ns)
{
    _xmlNode * newNode = nullptr;
    
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
    
    if ( newNode == nullptr )
        throw InvalidNodeType(std::string("NodeType '") + TypeString(type) + "' is not supported");
    
    _xml = newNode;
    _xml->_private = this;
}
Node::Node(Node && o) : _xml(o._xml) {
    _xml->_private = this;
    o._xml = NULL;
}
Node::~Node()
{
    if ( _xml->_private != this )
        return;
    
    // free the underlying node if *and only if* it is detached
    if ( _xml->parent == nullptr && _xml->prev == nullptr && _xml->next == nullptr )
        xmlFreeNode(_xml);
}

#if 0
#pragma mark - Properties
#endif

string Node::Name() const
{
    return _xml->name;
}
void Node::SetName(const string &name)
{
    xmlNodeSetName(_xml, name.utf8());
}
string Node::Content() const
{
    return ( _xml->content );
}
void Node::SetContent(const string &content)
{
    xmlNodeSetContent(_xml, content.utf8());
}
Namespace * Node::Namespace() const
{
    switch ( Type() )
    {
        case NodeType::Document:
        case NodeType::DocumentType:
        case NodeType::DocumentFragment:
        case NodeType::DTD:
        case NodeType::CDATASection:
#ifdef LIBXML_DOCBOOK_ENABLED
        case NodeType::DocbookSGMLDocument:
#endif
        case NodeType::HTMLDocument:
        case NodeType::ProcessingInstruction:
            // these types never have namespaces
            return nullptr;
            
        default:
            break;
    }
    
    return Wrapped<class Namespace, _xmlNs>(_xml->ns);
}
void Node::SetNamespace(const class Namespace *ns)
{
    xmlSetNs(_xml, const_cast<_xmlNs*>(ns->xml()));
}
string Node::Language() const
{
    const xmlChar * ch = xmlNodeGetLang(_xml);
    if ( ch == nullptr )
        return string();
    return ch;
}
void Node::SetLanguage(const string &language)
{
    xmlNodeSetLang(_xml, language.utf8());
}
bool Node::PreserveSpace() const
{
    return xmlNodeGetSpacePreserve(_xml) == 1;
}
void Node::SetPreserveSpace(bool preserve)
{
    xmlNodeSetSpacePreserve(_xml, (preserve ? 1 : 0));
}
string Node::BaseURL() const
{
    const xmlChar * ch = xmlNodeGetBase(_xml->doc, _xml);
    if ( ch == nullptr )
        return string();
    return ch;
}
void Node::SetBaseURL(const string &baseURL)
{
    xmlNodeSetBase(_xml, baseURL.utf8());
}
NamespaceList Node::NamespacesInScope() const
{
    NamespaceList result;
    xmlNsPtr *pNamespaces = xmlGetNsList(_xml->doc, _xml);
    if ( pNamespaces == nullptr )
        return result;
    
    for ( int i = 0; pNamespaces[i] != nullptr; i++ )
    {
        xmlNsPtr xmlNs = pNamespaces[i];
        result.push_back(Wrapped<class Namespace, _xmlNs>(xmlNs));
    }
    
    xmlMemFree(pNamespaces);
    
    return result;
}
NodeType Node::Type() const
{
    return NodeType(_xml->type);
}
int Node::Index() const
{
    int idx = 1;
    xmlNodePtr node = _xml;
    while ( node->prev != nullptr )
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

#if 0
#pragma mark - Values
#endif

string Node::XMLString() const
{
    std::ostringstream stream;
    StreamOutputBuffer buf(stream);
    xmlNodeDumpOutput(buf, _xml->doc, _xml, 0, 0, nullptr);
    return xmlString(stream.str().c_str());
}
string Node::StringValue() const
{
    const xmlChar * content = xmlNodeGetContent(_xml);
    if ( content == nullptr )
        return string();
    return content;
}
int Node::IntValue() const
{
    return std::atoi(StringValue().c_str());
}
double Node::DoubleValue() const
{
    return std::strtod(StringValue().c_str(), nullptr);
}
bool Node::BoolValue() const
{
    string str(StringValue());
    if ( ::strncasecmp(str.c_str(), "true", std::min((string::size_type)4, str.length())) == 0 )
        return true;
    if ( ::strncasecmp(str.c_str(), "yes", std::min((string::size_type)3, str.length())) == 0 )
        return true;
    
    return (std::atoi(str.c_str()) != 0);
}

#if 0
#pragma mark - Hierarchy
#endif

Document * Node::Document()
{
    return Wrapped<class Document, _xmlDoc>(_xml->doc);
}
const Document * Node::Document() const
{
    return const_cast<Node*>(this)->Document();
}
Node * Node::NextSibling()
{
    if ( _xml->next == nullptr )
        return nullptr;
    return Wrapped<Node, _xmlNode>(_xml);
}
const Node * Node::NextSibling() const
{
    return const_cast<Node*>(this)->NextSibling();
}
Node * Node::PreviousSibling()
{
    if ( _xml->prev == nullptr )
        return nullptr;
    return Wrapped<Node, _xmlNode>(_xml->prev);
}
const Node * Node::PreviousSibling() const
{
    return const_cast<Node*>(this)->PreviousSibling();
}
Node * Node::FirstChild(const string & filterByName)
{
    xmlNodePtr child = _xml->children;
    if ( child == nullptr )
        return nullptr;
    
    if ( filterByName.empty() )
        return Wrapped<Node, _xmlNode>(child);
    
    do
    {
        if ( filterByName == child->name )
            return Wrapped<Node, _xmlNode>(child);
        
    } while ( (child = child->next) != nullptr );
    
    return nullptr;
}
const Node * Node::FirstChild(const string & filterByName) const
{
    return const_cast<Node*>(this)->FirstChild(filterByName);
}
Node::NodeList Node::Children(const string & filterByName)
{
    NodeList list;
    for ( xmlNodePtr child = _xml->children; child != nullptr; child = child->next )
    {
        if ( filterByName.empty() || filterByName == child->name )
            list.push_back(Wrapped<Node, _xmlNode>(child));
    }
    return list;
}
const Node::NodeList Node::Children(const string & filterByName) const
{
    return const_cast<Node*>(this)->Children(filterByName);
}
Element * Node::AddChild(const string &name, const string & prefix)
{
    xmlNodePtr child = createChild(name, prefix);
    xmlNodePtr newNode = xmlAddChild(_xml, child);
    if ( newNode == nullptr )
    {
        xmlFreeNode(child);
        throw InternalError(std::string("Could not add child element node named '") + name.c_str() + "'", xmlGetLastError());
    }
    
    return Wrapped<Element, _xmlNode>(newNode);
}
void Node::AddChild(Node *child)
{
    xmlNodePtr newNode = xmlAddChild(_xml, child->xml());
    if ( newNode == nullptr )
        throw InternalError("Unable to add child node");
    child->rebind(newNode);
}
Element * Node::InsertAfter(const string &name, const string & prefix)
{
    xmlNodePtr child = createChild(name, prefix);
    xmlNodePtr newNode = xmlAddNextSibling(xml(), child);
    if ( newNode == nullptr )
    {
        xmlFreeNode(child);
        throw InternalError(std::string("Could not add child element node named '") + name.c_str() + "'", xmlGetLastError());
    }
    
    return Wrapped<Element, _xmlNode>(newNode);
}
void Node::InsertAfter(Node *child)
{
    xmlNodePtr newNode = xmlAddNextSibling(xml(), child->xml());
    if ( newNode == nullptr )
        throw InternalError("Unable to add child node", xmlGetLastError());
    child->rebind(newNode);
}
Element * Node::InsertBefore(const string &name, const string & prefix)
{
    xmlNodePtr child = createChild(name, prefix);
    xmlNodePtr newNode = xmlAddPrevSibling(xml(), child);
    if ( newNode == nullptr )
    {
        xmlFreeNode(child);
        throw InternalError(std::string("Could not add child element node named '") + name.c_str() + "'", xmlGetLastError());
    }
    
    return Wrapped<Element, _xmlNode>(newNode);
}
void Node::InsertBefore(Node *child)
{
    xmlNodePtr newNode = xmlAddPrevSibling(xml(), child->xml());
    if ( newNode == nullptr )
        throw InternalError("Unable to add child node", xmlGetLastError());
    child->rebind(newNode);
}
Node * Node::CopyIn(const Node *nodeToCopy, bool recursive)
{
    if ( nodeToCopy == nullptr )
        return nullptr;
    
    // complains about the difference between 'const xmlNode *' and 'xmlNode const *' ...
    xmlNodePtr theCopy = xmlDocCopyNode(const_cast<_xmlNode *>(nodeToCopy->xml()), _xml->doc, recursive ? 1 : 0);
    if ( theCopy == nullptr )
        throw InternalError("Unable to copy node", xmlGetLastError());
    
    xmlNode * added = xmlAddChild(_xml, theCopy);
    if ( added == nullptr )
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

#if 0
#pragma mark - XPath Utilities
#endif

string Node::Path() const
{
    xmlChar * path = xmlGetNodePath(_xml);
    if ( path == nullptr )
        return string();
    string r(path);
    xmlFree(path);
    return r;
}
NodeSet Node::FindByXPath(const string &xpath) const
{
    return FindByXPath(xpath, NamespaceMap());
}
NodeSet Node::FindByXPath(const string &xpath, const NamespaceMap &namespaces) const
{
    XPathEvaluator eval(xpath, Document());
    eval.RegisterNamespaces(namespaces);
    
    XPathEvaluator::ObjectType type = XPathEvaluator::ObjectType::Undefined;
    if ( eval.Evaluate(this, &type) && type == XPathEvaluator::ObjectType::NodeSet )
        return eval.NodeSetResult();
    
    return NodeSet();
}

#if 0
#pragma mark - Internal Methods
#endif

WrapperBase * Node::Wrap(_xmlNode *aNode)
{
    WrapperBase * wrapper = nullptr;
    switch ( aNode->type )
    {
        case XML_DOCUMENT_NODE:
        case XML_DOCUMENT_FRAG_NODE:
        case XML_HTML_DOCUMENT_NODE:
            //wrapper = new class Document(reinterpret_cast<xmlDocPtr>(aNode));
            break;
            
        case XML_DTD_NODE:
            //wrapper = new DTD(reinterpret_cast<xmlDtdPtr>(aNode));
            break;
            
        case XML_NAMESPACE_DECL:
            wrapper = new class Namespace(reinterpret_cast<xmlNsPtr>(aNode));
            break;
            
        case XML_ATTRIBUTE_NODE:
            //wrapper = new Attribute(reinterpret_cast<xmlAttrPtr>(aNode));
            break;
            
        case XML_ELEMENT_NODE:
            //wrapper = new Element(aNode);
            break;
            
        default:
            wrapper = new Node(aNode);
            break;
    }
    
    aNode->_private = wrapper;
    return wrapper;
}
void Node::Unwrap(_xmlNode *aNode)
{
    /*
    const char * content = (const char *)xmlNodeGetContent(aNode);
    fprintf(stderr, "Deleting node: type %d, name %s, content %s\n", aNode->type, (aNode->name == nullptr ? "(null)" : (const char *)aNode->name), (content == nullptr ? "(null)" : content));
    */
    if ( aNode->_private == nullptr )
        return;
    if ( aNode->type == 0 )
        return;
    
    WrapperBase * obj = reinterpret_cast<WrapperBase*>(aNode->_private);
    aNode->_private = nullptr;
    delete obj;
}
xmlNodePtr Node::createChild(const string &name, const string &prefix) const
{
    xmlNs * ns = nullptr;
    if ( Type() != NodeType::Element )
        throw InternalError("Cannot add children to non-element node of type '" + TypeString(Type()) + "'");
    
    if ( prefix.empty() )
    {
        ns = xmlSearchNs(_xml->doc, _xml, nullptr);
    }
    else
    {
        // use the existing namespace if one exists
        ns = xmlSearchNs(_xml->doc, _xml, prefix.utf8());
        if ( ns == nullptr )
            throw InternalError(std::string("The namespace prefix '") + prefix.c_str() + "' is unknown");
    }
    
    return xmlNewNode(ns, name.utf8());
}
void Node::rebind(_xmlNode *newNode)
{
    if ( _xml == newNode )
        return;
    
    if ( _xml != nullptr )
    {
        if ( _xml->parent == nullptr && _xml->next == nullptr && _xml->prev == nullptr )
            xmlFreeNode(_xml);
    }
    
    _xml = newNode;
}

EPUB3_XML_END_NAMESPACE
