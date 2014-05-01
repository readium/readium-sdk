//
//  xpath.h
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
