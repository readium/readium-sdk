//
//  node.h
//  ePub3
//
//  Created by Jim Dovey on 2012-11-19.
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//  
//  This program is distributed in the hope that it will be useful, but WITHOUT ANY 
//  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
//  
//  Licensed under Gnu Affero General Public License Version 3 (provided, notwithstanding this notice, 
//  Readium Foundation reserves the right to license this material under a different separate license, 
//  and if you have done so, the terms of that separate license control and the following references 
//  to GPL do not apply).
//  
//  This program is free software: you can redistribute it and/or modify it under the terms of the GNU 
//  Affero General Public License as published by the Free Software Foundation, either version 3 of 
//  the License, or (at your option) any later version. You should have received a copy of the GNU 
//  Affero General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef __ePub3_xml_node__
#define __ePub3_xml_node__

#include <ePub3/xml/base.h>
#include <ePub3/xml/ns.h>
#include <ePub3/xml/xmlstring.h>
#include <ePub3/xml/node_traits.h>

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
typedef std::vector<std::shared_ptr<Node>> NodeSet;

typedef std::map<string, string> NamespaceMap;

/**
 @ingroup xml-utils
 */
std::string TypeString(NodeType type);

/**
 @ingroup tree
 */
class Node : public WrapperBase<Node>
{
public:
#if EPUB_USE(LIBXML2)
	typedef _xmlNode*							NativePtr;
#elif EPUB_USE(WIN_XML)
	typedef Windows::Data::Xml::Dom::IXmlNode^	NativePtr;
#endif

    typedef std::list<std::shared_ptr<Node>>	NodeList;
    
    class InvalidNodeType : public exception
    {
    public:
        InvalidNodeType(const std::string & str) : exception(str, NULL) {}
        InvalidNodeType(const char * s) : exception(s, NULL) {}
    };
    
public:
    explicit Node(NativePtr node);
#if EPUB_ENABLE(XML_BUILDER)
    Node(const string & name, NodeType type,
         const string & content = string(),
         const Namespace & ns = xml::Namespace());
#endif
    Node(Node && o);
    virtual ~Node();
    
    ///////////////////////////////////////////////////////
    // Properties
    
    string Name() const;
#if EPUB_ENABLE(XML_BUILDER)
    void SetName(const string & name);
#endif
    
	string Content() const;
#if EPUB_ENABLE(XML_BUILDER)
    void SetContent(const string & content);
#endif

	string AttributeValue(const string& name, const string& namespaceURI) const;
    
	std::shared_ptr<Namespace> Namespace() const;
#if EPUB_ENABLE(XML_BUILDER)
    void SetNamespace(const class Namespace * ns);
#endif
    
    string Language() const;
#if EPUB_ENABLE(XML_BUILDER)
    void SetLanguage(const string & language);
#endif
    
	bool PreserveSpace() const;
#if EPUB_ENABLE(XML_BUILDER)
    void SetPreserveSpace(bool preserve);
#endif
    
	string BaseURL() const;
#if EPUB_ENABLE(XML_BUILDER)
    void SetBaseURL(const string & baseURL);
#endif
    
    NamespaceList NamespacesInScope() const;
    NodeType Type() const;
    bool IsTextNode() const { return Type() == NodeType::Text; }
    bool IsElementNode() const { return Type() == NodeType::Element; }

#if EPUB_USE(LIBXML2)
    int Index() const;
    int Line() const;
#endif
    
    NativePtr xml() { return _xml; }
    const NativePtr xml() const { return _xml; }
    
    ///////////////////////////////////////////////////////
    // Values
    
    string XMLString() const;
    
    string StringValue() const;
    int IntValue() const;
    double DoubleValue() const;
    bool BoolValue() const;
    
    operator const unsigned char * () const { return XMLString().utf8(); }
    operator string () const { return StringValue(); }
    operator int () const { return IntValue(); }
    operator double () const { return DoubleValue(); }
    operator bool () const { return BoolValue(); }
    
    ///////////////////////////////////////////////////////
    // Hierarchy
    
    std::shared_ptr<Document> Document();
    std::shared_ptr<const class Document> Document() const;
    
    std::shared_ptr<Element> Parent();
	std::shared_ptr<Node> NextSibling();
	std::shared_ptr<Node> NextElementSibling();
	std::shared_ptr<Node> PreviousSibling();
	std::shared_ptr<Node> FirstChild(const string & filterByName = string());
	std::shared_ptr<Node> FirstElementChild();
    NodeList Children(const string & filterByName = string());
    
	std::shared_ptr<const Element> Parent() const;
	std::shared_ptr<const Node> NextSibling() const;
	std::shared_ptr<const Node> NextElementSibling() const;
	std::shared_ptr<const Node> PreviousSibling() const;
	std::shared_ptr<const Node> FirstChild(const string & filterByName = string()) const;
	std::shared_ptr<const Node> FirstElementChild() const;
    const NodeList Children(const string & filterByName = string()) const;
#if EPUB_ENABLE(XML_BUILDER)
    std::shared_ptr<Element> AddChild(const string & name, const string & prefix = string());
	void AddChild(std::shared_ptr<Node> child);
    
    std::shared_ptr<Element> InsertAfter(const string & name, const string & prefix = string());
	void InsertAfter(std::shared_ptr<Node> child);
    
    std::shared_ptr<Element> InsertBefore(const string & name, const string & prefix = string());
	void InsertBefore(std::shared_ptr<Node> child);
    
	std::shared_ptr<Node> CopyIn(std::shared_ptr<const Node> nodeToCopy, bool recursive = true);
    
    void Detach();
#endif
    ///////////////////////////////////////////////////////
    // XPaths
    
    string Path() const;
    
    // these are simple wrappers for the XPathEvaluator class's functionality
    NodeSet FindByXPath(const string & xpath) const;
    NodeSet FindByXPath(const string & xpath, const NamespaceMap & namespaces) const;

	///////////////////////////////////////////////////////
	// Wrapper Factory

#if EPUB_USE(LIBXML2)
    static void Wrap(_xmlNode* xml);
    static void Unwrap(_xmlNode * xml);
#elif EPUB_USE(WIN_XML)
	static std::shared_ptr<Node> NewNode(NativePtr ptr);
#endif
    
    virtual
    void release() OVERRIDE
        { _xml = nullptr; }

protected:
    NativePtr _xml;
#if EPUB_ENABLE(XML_BUILDER)
    NativePtr createChild(const string & name, const string & prefix) const;
#endif
#if EPUB_USE(LIBXML2)
    void rebind(NativePtr newNode);
#endif
    
    friend class Document;
    friend class Element;
    friend class Attribute;
    
};

EPUB3_XML_END_NAMESPACE

#endif /* defined(__ePub3_xml_node__) */
