//
//  node.h
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

#ifndef __ePub3_xml_node__
#define __ePub3_xml_node__

#include "base.h"
#include "ns.h"
#include "utfstring.h"
#include <libxml/xpath.h>
#include <string>
#include <list>
#include <map>
#include <vector>

EPUB3_XML_BEGIN_NAMESPACE

class TextNode;
class Element;
class Attribute;
class Namespace;

class Node;
typedef std::vector<Node*> NodeSet;

enum class NodeType : uint8_t {
    Element                         = ::XML_ELEMENT_NODE,
    Attribute                       = ::XML_ATTRIBUTE_NODE,
    Text                            = ::XML_TEXT_NODE,
    CDATASection                    = ::XML_CDATA_SECTION_NODE,
    EntityReference                 = ::XML_ENTITY_REF_NODE,
    Entity                          = ::XML_ENTITY_NODE,
    ProcessingInstruction           = ::XML_PI_NODE,
    Comment                         = ::XML_COMMENT_NODE,
    Document                        = ::XML_DOCUMENT_NODE,
    DocumentType                    = ::XML_DOCUMENT_TYPE_NODE,
    DocumentFragment                = ::XML_DOCUMENT_FRAG_NODE,
    Notation                        = ::XML_NOTATION_NODE,
    HTMLDocument                    = ::XML_HTML_DOCUMENT_NODE,
    DTD                             = ::XML_DTD_NODE,
    ElementDeclaration              = ::XML_ELEMENT_DECL,
    AttributeDeclaration            = ::XML_ATTRIBUTE_DECL,
    EntityDeclaration               = ::XML_ENTITY_DECL,
    NamespaceDeclaration            = ::XML_NAMESPACE_DECL,
    XIncludeStart                   = ::XML_XINCLUDE_START,
    XIncludeEnd                     = ::XML_XINCLUDE_END,
#ifdef LIBXML_DOCB_ENABLED
    DocbookSGMLDocument             = ::XML_DOCB_DOCUMENT_NODE,
#endif
};

std::string TypeString(NodeType type);

class Node : public WrapperBase
{
public:
    typedef std::list<Node*>                    NodeList;
    
    class InvalidNodeType : public exception
    {
    public:
        InvalidNodeType(const std::string & str) : exception(str, NULL) {}
        InvalidNodeType(const char * s) : exception(s, NULL) {}
    };
    
public:
    explicit Node(_xmlNode * node);
    Node(const string & name, NodeType type,
         const string & content = string(),
         const Namespace & ns = xml::Namespace());
    Node(Node && o);
    virtual ~Node();
    
    ///////////////////////////////////////////////////////
    // Properties
    
    string Name() const;
    void SetName(const string & name);
    
    string Content() const;
    void SetContent(const string & content);
    
    Namespace * Namespace() const;
    void SetNamespace(const class Namespace * ns);
    
    string Language() const;
    void SetLanguage(const string & language);
    
    bool PreserveSpace() const;
    void SetPreserveSpace(bool preserve);
    
    string BaseURL() const;
    void SetBaseURL(const string & baseURL);
    
    NamespaceList NamespacesInScope() const;
    NodeType Type() const;
    bool IsTextNode() const { return Type() == NodeType::Text; }
    bool IsElementNode() const { return Type() == NodeType::Element; }
    
    int Index() const;
    
    int Line() const;
    
    _xmlNode * xml() { return _xml; }
    const _xmlNode * xml() const { return _xml; }
    
    ///////////////////////////////////////////////////////
    // Values
    
    string XMLString() const;
    
    string StringValue() const;
    int IntValue() const;
    double DoubleValue() const;
    bool BoolValue() const;
    
    operator const xmlChar * () const { return XMLString().utf8(); }
    operator string () const { return StringValue(); }
    operator int () const { return IntValue(); }
    operator double () const { return DoubleValue(); }
    operator bool () const { return BoolValue(); }
    
    ///////////////////////////////////////////////////////
    // Hierarchy
    
    Document * Document();
    const class Document * Document() const;
    
    Element * Parent();
    Node * NextSibling();
    Node * PreviousSibling();
    Node * FirstChild(const string & filterByName = string());
    NodeList Children(const string & filterByName = string());
    
    const Element * Parent() const;
    const Node * NextSibling() const;
    const Node * PreviousSibling() const;
    const Node * FirstChild(const string & filterByName = string()) const;
    const NodeList Children(const string & filterByName = string()) const;
    
    Element * AddChild(const string & name, const string & prefix = string());
    void AddChild(Node * child);
    
    Element * InsertAfter(const string & name, const string & prefix = string());
    void InsertAfter(Node * child);
    
    Element * InsertBefore(const string & name, const string & prefix = string());
    void InsertBefore(Node * child);
    
    Node * CopyIn(const Node * nodeToCopy, bool recursive = true);
    
    void Detach();
    
    ///////////////////////////////////////////////////////
    // XPaths
    
    string Path() const;
    
    // these are simple wrappers for the XPathEvaluator class's functionality
    NodeSet FindByXPath(const string & xpath) const;
    NodeSet FindByXPath(const string & xpath, const NamespaceMap & namespaces) const;
    
    ///////////////////////////////////////////////////////
    // Wrapper Factory
    
    static WrapperBase * Wrap(_xmlNode * xml);
    static void Unwrap(_xmlNode * xml);
    
protected:
    _xmlNode *  _xml;
    
    xmlNodePtr createChild(const string & name, const string & prefix) const;
    void rebind(_xmlNode * newNode);
    
    friend class Document;
    friend class Element;
    friend class Attribute;
    
};

// specialization for the polymorphic Node class
template <>
inline Node * Wrapped<Node, _xmlNode>(xmlNode * n)
{
    if ( n == nullptr ) return nullptr;
    if ( n->_private == nullptr ) return reinterpret_cast<Node*>(n->_private);
    
    // Node::Wrap() instantiates the correct WrapperBase subclass
    return dynamic_cast<Node*>(Node::Wrap(n));
}

EPUB3_XML_END_NAMESPACE

#endif /* defined(__ePub3_xml_node__) */
