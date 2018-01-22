//
//  node.cpp
//  ePub3
//
//  Created by Jim Dovey on 2012-11-19.
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

#include <ePub3/xml/node.h>
#include <ePub3/xml/io.h>
#include <ePub3/xml/ns.h>
#include <ePub3/xml/xpath.h>
#include <ePub3/xml/document.h>
#include <ePub3/xml/element.h>
#include <ePub3/xml/dtd.h>
#include <string>
#include <sstream>
#include <cstdlib>

#if EPUB_PLATFORM(MAC)
#include "xml_bridge_dtrace_probes.h"
#endif

#ifndef NDEBUG
extern "C" void DebugPrintNode(std::shared_ptr<ePub3::xml::Node> aNode)
{
    auto output = xmlOutputBufferCreateFile(stderr, nullptr);
    xmlNodeDumpOutput(output, aNode->Document()->xml(), aNode->xml(), 0, 1, "UTF-8");
    xmlOutputBufferClose(output);
}
extern "C" void DebugPrintNodeConst(std::shared_ptr<const ePub3::xml::Node> aNode)
{
    auto output = xmlOutputBufferCreateFile(stderr, nullptr);
    xmlNodeDumpOutput(output, aNode->Document()->xml(), aNode->xml(), 0, 1, "UTF-8");
    xmlOutputBufferClose(output);
}

extern "C" const char* XMLDocumentString(std::shared_ptr<ePub3::xml::Document> aNode)
{
    xmlChar* s = nullptr;
    int ssize = 0;
    xmlDocDumpFormatMemory(aNode->xml(), &s, &ssize, 1);
    return reinterpret_cast<char*>(s);
}
extern "C" const char* XMLDocumentStringConst(std::shared_ptr<const ePub3::xml::Document> aNode)
{
    xmlChar* s = nullptr;
    int ssize = 0;
    xmlDocDumpFormatMemory(aNode->xml(), &s, &ssize, 1);
    return reinterpret_cast<char*>(s);
}
extern "C" const char* XMLNodeDocumentString(std::shared_ptr<const ePub3::xml::Node> aNode)
{
    xmlChar* s = nullptr;
    int ssize = 0;
    xmlDocDumpFormatMemory(aNode->Document()->xml(), &s, &ssize, 1);
    return reinterpret_cast<char*>(s);
}
extern "C" const char* XMLNodeDocumentStringConst(std::shared_ptr<const ePub3::xml::Node> aNode)
{
    xmlChar* s = nullptr;
    int ssize = 0;
    xmlDocDumpFormatMemory(aNode->Document()->xml(), &s, &ssize, 1);
    return reinterpret_cast<char*>(s);
}
#endif

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
    //_xml->_private = this;
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
    _xml->_private = new LibXML2Private<Node>(this);
}
Node::Node(Node && o) : _xml(o._xml) {
    typedef LibXML2Private<Node> _Private;
    _Private* priv = reinterpret_cast<_Private*>(_xml->_private);

#if ENABLE_WEAK_PTR_XML_NODE_WRAPPER
    priv->__ptr = std::shared_ptr<Node>(this);
#else
    priv->__ptr.reset(this);
#endif //ENABLE_WEAK_PTR_XML_NODE_WRAPPER

    o._xml = NULL;
}
Node::~Node()
{
    typedef LibXML2Private<Node> _Private;
    
    if (_xml == nullptr)
        return;
    
    _Private* priv = reinterpret_cast<_Private*>(_xml->_private);
    if ( priv->__sig != _READIUM_XML_SIGNATURE ||
#if ENABLE_WEAK_PTR_XML_NODE_WRAPPER
        (priv->__ptr.lock() != nullptr && priv->__ptr.lock().get() != this)
#else
        priv->__ptr.get() != this
#endif //ENABLE_WEAK_PTR_XML_NODE_WRAPPER
        ) {
        return;
    }

#if ENABLE_WEAK_PTR_XML_NODE_WRAPPER
    if (!bool(priv->__ptr.lock()))
    {
        _xml->_private = nullptr;
        delete priv;
    }
#endif //ENABLE_WEAK_PTR_XML_NODE_WRAPPER

    // free the underlying node if *and only if* it is detached
    if ( _xml->parent == nullptr && _xml->prev == nullptr && _xml->next == nullptr )
    {
#if !ENABLE_WEAK_PTR_XML_NODE_WRAPPER
        _xml->_private = nullptr;
        delete priv;
#endif //!ENABLE_WEAK_PTR_XML_NODE_WRAPPER
        xmlFreeNode(_xml);
    }
}

#if 0
#pragma mark - Properties
#endif

string Node::Name() const
{
    const xmlChar* ch = _xml->name;
    if (ch == nullptr)
        return string::EmptyString;
    return _xml->name;
}
void Node::SetName(const string &name)
{
    xmlNodeSetName(_xml, name.utf8());
}
string Node::Content() const
{
    const xmlChar* ch = xmlNodeGetContent(_xml);
    if (ch == nullptr)
        return string::EmptyString;

    string result(ch);
    xmlFree((void*)ch);
    return result;
}
void Node::SetContent(const string &content)
{
    xmlNodeSetContent(_xml, content.utf8());
}
std::shared_ptr<Namespace> Node::Namespace() const
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

    string result(ch);
    xmlFree((void*)ch);
    return result;
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

    string result(ch);
    xmlFree((void*)ch);
    return result;
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

string Node::AttributeValue(const string& name, const string& nsURI) const
{
    xmlChar * ch = nullptr;
    if ( !nsURI.empty() )
    {
        ch = xmlGetNsProp(_xml, name.xml_str(), nsURI.xml_str());
    }
    
    if ( ch == nullptr )
    {
        ch = xmlGetProp(_xml, name.xml_str());
    }
    
    if ( ch == nullptr )
        return string::EmptyString;
    
    string result(ch);
    xmlFree(ch);
    return result;
}
string Node::XMLString() const
{
    std::ostringstream stream;
    
    StreamOutputBuffer buf(stream);
    xmlNodeDumpOutput(buf.xmlBuffer(), _xml->doc, _xml, 0, 1, "UTF-8");
    buf.flush();
    
    return xmlString(stream.str().c_str());
}
string Node::StringValue() const
{
    const xmlChar * ch = xmlNodeGetContent(_xml);
    if ( ch == nullptr )
        return string();

    string result(ch);
    xmlFree((void*)ch);
    return result;
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

std::shared_ptr<Document> Node::Document()
{
    return Wrapped<class Document, _xmlDoc>(_xml->doc);
}
std::shared_ptr<const Document> Node::Document() const
{
    return const_cast<Node*>(this)->Document();
}
std::shared_ptr<Element> Node::Parent()
{
    return Wrapped<Element>(_xml->parent);
}
std::shared_ptr<const Element> Node::Parent() const
{
    return const_cast<Node*>(this)->Parent();
}
std::shared_ptr<Node> Node::NextSibling()
{
    if ( _xml->next == nullptr )
        return nullptr;
    return Wrapped<Node, _xmlNode>(_xml->next);
}
std::shared_ptr<const Node> Node::NextSibling() const
{
    return const_cast<Node*>(this)->NextSibling();
}
std::shared_ptr<Node> Node::NextElementSibling()
{
    return Wrapped<Node>(xmlNextElementSibling(_xml));
}
std::shared_ptr<const Node> Node::NextElementSibling() const
{
    return const_cast<Node*>(this)->NextElementSibling();
}
std::shared_ptr<Node> Node::PreviousSibling()
{
    if ( _xml->prev == nullptr )
        return nullptr;
    return Wrapped<Node, _xmlNode>(_xml->prev);
}
std::shared_ptr<const Node> Node::PreviousSibling() const
{
    return const_cast<Node*>(this)->PreviousSibling();
}
std::shared_ptr<Node> Node::FirstChild(const string & filterByName)
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
std::shared_ptr<const Node> Node::FirstChild(const string & filterByName) const
{
    return const_cast<Node*>(this)->FirstChild(filterByName);
}
std::shared_ptr<Node> Node::FirstElementChild()
{
    return Wrapped<Node>(xmlFirstElementChild(_xml));
}
std::shared_ptr<const Node> Node::FirstElementChild() const
{
    return const_cast<Node*>(this)->FirstElementChild();
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
std::shared_ptr<Element> Node::AddChild(const string &name, const string & prefix)
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
void Node::AddChild(std::shared_ptr<Node> child)
{
    xmlNodePtr newNode = xmlAddChild(_xml, child->xml());
    if ( newNode == nullptr )
        throw InternalError("Unable to add child node");
    child->rebind(newNode);
}
std::shared_ptr<Element> Node::InsertAfter(const string &name, const string & prefix)
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
void Node::InsertAfter(std::shared_ptr<Node> child)
{
    xmlNodePtr newNode = xmlAddNextSibling(xml(), child->xml());
    if ( newNode == nullptr )
        throw InternalError("Unable to add child node", xmlGetLastError());
    child->rebind(newNode);
}
std::shared_ptr<Element> Node::InsertBefore(const string &name, const string & prefix)
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
void Node::InsertBefore(std::shared_ptr<Node> child)
{
    xmlNodePtr newNode = xmlAddPrevSibling(xml(), child->xml());
    if ( newNode == nullptr )
        throw InternalError("Unable to add child node", xmlGetLastError());
    child->rebind(newNode);
}
std::shared_ptr<Node> Node::CopyIn(std::shared_ptr<const Node> nodeToCopy, bool recursive)
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
    if ( eval.Evaluate(shared_from_this(), &type) && type == XPathEvaluator::ObjectType::NodeSet )
        return eval.NodeSetResult();
    
    return NodeSet();
}

#if 0
#pragma mark - Internal Methods
#endif

void Node::Wrap(_xmlNode *aNode)
{
    void* wrapper = nullptr;
    switch ( aNode->type )
    {
        case XML_DOCUMENT_NODE:
        case XML_DOCUMENT_FRAG_NODE:
        case XML_HTML_DOCUMENT_NODE:
        {
            wrapper = new LibXML2Private<class Document>(new class Document(reinterpret_cast<xmlDocPtr>(aNode)));
            break;
        }
            
        case XML_DTD_NODE:
            wrapper = new LibXML2Private<DTD>(new DTD(reinterpret_cast<xmlDtdPtr>(aNode)));
            break;
            
        case XML_NAMESPACE_DECL:
            wrapper = new LibXML2Private<class Namespace>(new class Namespace(reinterpret_cast<xmlNsPtr>(aNode)));
            break;
            
        case XML_ATTRIBUTE_NODE:
            //wrapper = new Attribute(reinterpret_cast<xmlAttrPtr>(aNode));
            break;
            
        case XML_ELEMENT_NODE:
            wrapper = new LibXML2Private<Element>(new Element(aNode));
            break;
            
        default:
            wrapper = new LibXML2Private<Node>(new Node(aNode));
            break;
    }
    
    // The _private ptr in the xmlNodePtr is a POINTER TO a LibXML2Private
    // object. This means that the allocated object is shared, and the
    // xmlNodePtr holds a strong reference, released when the xmlNodePtr
    // is deallocated.
    aNode->_private = wrapper;
}
void Node::Unwrap(_xmlNode *aNode)
{
    typedef LibXML2Private<class Namespace> NsPrivate;
    typedef LibXML2Private<Node> NodePrivate;
    
    if (aNode->type == XML_NAMESPACE_DECL)
    {
        // _xmlNs is laid out differently -- _private is in a different place...
        xmlNsPtr __ns = reinterpret_cast<xmlNsPtr>(aNode);
        if (__ns->_private != nullptr)
        {
            NsPrivate* ptr = reinterpret_cast<NsPrivate*>(__ns->_private);
            if (ptr->__sig == _READIUM_XML_SIGNATURE)
            {
#if !ENABLE_WEAK_PTR_XML_NODE_WRAPPER
                ptr->__ptr->release();
#endif //!ENABLE_WEAK_PTR_XML_NODE_WRAPPER
                delete ptr;
            }
            __ns->_private = nullptr;
        }
    }
    else if (aNode->_private != nullptr)
    {
        NodePrivate* ptr = reinterpret_cast<NodePrivate*>(aNode->_private);
        if (ptr->__sig == _READIUM_XML_SIGNATURE)
        {
            ptr->__ptr->release();
            delete ptr;
        }
        aNode->_private = nullptr;
    }
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
    
    typedef LibXML2Private<Node> _Private;
    
    if ( _xml != nullptr )
    {
        if ( _xml->parent == nullptr && _xml->next == nullptr && _xml->prev == nullptr )
        {
            xmlNodePtr __n = _xml;
            this->release();
            xmlFreeNode(__n); // releases libxml's reference to this object
        }
    }
    
    _xml = newNode;
    
    typedef std::shared_ptr<Node> NodePtr;
    if (_xml->_private != nullptr && *((unsigned int*)(_xml->_private)) == _READIUM_XML_SIGNATURE)
    {
        // reassign the bridge ptr on the libxml node
        _Private* ptr = reinterpret_cast<_Private*>(newNode->_private);
        ptr->__ptr = shared_from_this();
    }
    else if (_xml->_private != nullptr)
    {
        auto ptr = shared_from_this();
        _xml->_private = new _Private(ptr);
    }
}

EPUB3_XML_END_NAMESPACE
