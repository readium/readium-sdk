//
//  xpath.h
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

#ifndef __ePub3_xml_xpath__
#define __ePub3_xml_xpath__

#include <ePub3/xml/base.h>
#include <ePub3/xml/node.h>
#if EPUB_USE(LIBXML2)
#include <libxml/xpath.h>
#endif
#include <functional>
#if EPUB_USE(PTHREADS)
# include <pthread.h>
#endif

EPUB3_XML_BEGIN_NAMESPACE

class Document;
class Element;

/**
 @ingroup xml-utils
 */
class XPathEvaluator : public WrapperBase<XPathEvaluator>
{
public:
#if EPUB_USE(LIBXML2)
    typedef std::function<void(xmlXPathParserContextPtr ctx, int nargs)> XPathFunction;

    enum class ObjectType : uint8_t {
        Undefined    = ::XPATH_UNDEFINED,
        NodeSet      = ::XPATH_NODESET,
        Boolean      = ::XPATH_BOOLEAN,
        Number       = ::XPATH_NUMBER,
        String       = ::XPATH_STRING,
        /*
         Point        = ::XPATH_POINT,
         Range        = ::XPATH_RANGE,
         LocationSet  = ::XPATH_LOCATIONSET,
         Users        = ::XPATH_USERS,
         XSLTTree     = ::XPATH_XSLT_TREE,
         */
    };
#elif EPUB_USE(WIN_XML)
	enum class ObjectType {
		Undefined,
		NodeSet,
		Boolean,
		Number,
		String,
	};
#endif

	typedef std::map<string, string> NamespaceMap;
    
public:
	XPathEvaluator(const string & xpath, std::shared_ptr<const Document> document);
    virtual ~XPathEvaluator();
    
    string XPath() const { return _xpath; }
	std::shared_ptr<const class Document> Document() const { return _document; }
    
    //////////////////////////////////////////////////////////////////
    // Compilation (optional)
    bool Compile();
    
    //////////////////////////////////////////////////////////////////
    // Evaluation
    
	bool Evaluate(std::shared_ptr<const Node> node, ObjectType * resultType = nullptr);
    
    // special optimized entry point when evaluating as a boolean. Returns result directly.
	bool EvaluateAsBoolean(std::shared_ptr<const Node> node);
    
    // these throw if there is no current result
    bool BooleanResult() const;
    double NumberResult() const;
    string StringResult() const;
    xml::NodeSet NodeSetResult() const;
    
    //////////////////////////////////////////////////////////////////
    // XPath Environment Setup
    
    bool RegisterNamespace(const string & prefix, const string & uri);
    bool RegisterNamespaces(const NamespaceMap & namespaces);
	bool RegisterAllNamespacesForElement(std::shared_ptr<const Element> element);
#if EPUB_USE(LIBXML2)
    bool RegisterFunction(const string & name, XPathFunction fn);
    bool RegisterFunction(const string & name, const string & namespaceURI, XPathFunction fn);
    
    bool RegisterVariable(const string & name, void * data, ObjectType type, const string & namespaceURI = string());
#endif
    
    virtual
    void release() OVERRIDE
        {}
protected:
#if EPUB_USE(LIBXML2)
    typedef std::map<string, XPathFunction>    FunctionLookup;
    
    static void _XMLFunctionWrapper(xmlXPathParserContextPtr ctx, int nargs);
    void PerformFunction(xmlXPathParserContextPtr ctx, const string & name, const string & uri, int nargs);
#endif
    string									_xpath;
	std::shared_ptr<const class Document>	_document;
#if EPUB_USE(LIBXML2)
    _xmlXPathContext *      _ctx;
    _xmlXPathCompExpr *     _compiled;
    FunctionLookup          _functions;
    
    _xmlXPathObject *       _lastResult;
#elif EPUB_USE(WIN_XML)
	::Windows::Data::Xml::Dom::XmlNodeList^	_lastResult;
	std::map<string, string>				_namespaces;
#endif
};

EPUB3_XML_END_NAMESPACE

#endif /* defined(__ePub3_xml_xpath__) */
