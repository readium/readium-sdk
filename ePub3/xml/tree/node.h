//
//  node.h
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
