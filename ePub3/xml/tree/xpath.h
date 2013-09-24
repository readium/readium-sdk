//
//  xpath.h
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

#ifndef __ePub3_xml_xpath__
#define __ePub3_xml_xpath__

#include "../utilities/base.h"
#include "node.h"
//#include <libxml/xpath.h>
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
class XPathEvaluator
#if EPUB_USE(LIBXML2)
	: public WrapperBase
#endif
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
    XPathEvaluator(const string & xpath, const Document * document);
    virtual ~XPathEvaluator();
    
    string XPath() const { return _xpath; }
    const class Document * Document() const { return _document; }
    
    //////////////////////////////////////////////////////////////////
    // Evaluation
    
    bool Evaluate(const Node * node, ObjectType * resultType = nullptr);
    
    // special optimized entry point when evaluating as a boolean. Returns result directly.
    bool EvaluateAsBoolean(const Node * node);
    
    // these throw if there is no current result
    bool BooleanResult() const;
    double NumberResult() const;
    string StringResult() const;
    xml::NodeSet NodeSetResult() const;
    
    //////////////////////////////////////////////////////////////////
    // XPath Environment Setup
    
    bool RegisterNamespace(const string & prefix, const string & uri);
    bool RegisterNamespaces(const NamespaceMap & namespaces);
    bool RegisterAllNamespacesForElement(const Element * element);
#if EPUB_USE(LIBXML2)
    bool RegisterFunction(const string & name, XPathFunction fn);
    bool RegisterFunction(const string & name, const string & namespaceURI, XPathFunction fn);
    
    bool RegisterVariable(const string & name, void * data, ObjectType type, const string & namespaceURI = string());
#endif
protected:
#if EPUB_USE(LIBXML2)
    typedef std::map<string, XPathFunction>    FunctionLookup;
    
    static void _XMLFunctionWrapper(xmlXPathParserContextPtr ctx, int nargs);
    void PerformFunction(xmlXPathParserContextPtr ctx, const string & name, const string & uri, int nargs);
#endif
    string             _xpath;
    const class Document *  _document;
#if EPUB_USE(LIBXML2)
    _xmlXPathContext *      _ctx;
    _xmlXPathCompExpr *     _compiled;
    FunctionLookup          _functions;
    
    _xmlXPathObject *       _lastResult;
#elif EPUB_USE(WIN_XML)

#endif
};

EPUB3_XML_END_NAMESPACE

#endif /* defined(__ePub3_xml_xpath__) */
