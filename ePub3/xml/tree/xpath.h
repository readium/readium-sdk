//
//  xpath.h
//  ePub3
//
//  Created by Jim Dovey on 2012-11-19.
//  Copyright (c) 2012-2013 The Readium Foundation.
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

#include "base.h"
#include "node.h"
#include <libxml/xpath.h>
#include <pthread.h>

EPUB3_XML_BEGIN_NAMESPACE

class Document;
class Element;

class XPathEvaluator : public WrapperBase
{
public:
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
    bool BooleanResult() const throw (InternalError);
    double NumberResult() const throw (InternalError);
    string StringResult() const throw (InternalError);
    xml::NodeSet NodeSetResult() const throw (InternalError, std::domain_error);
    
    //////////////////////////////////////////////////////////////////
    // XPath Environment Setup
    
    bool RegisterNamespace(const string & prefix, const string & uri);
    bool RegisterNamespaces(const NamespaceMap & namespaces);
    bool RegisterAllNamespacesForElement(const Element * element);
    
    bool RegisterFunction(const string & name, XPathFunction fn);
    bool RegisterFunction(const string & name, const string & namespaceURI, XPathFunction fn);
    
    bool RegisterVariable(const string & name, void * data, ObjectType type, const string & namespaceURI = string());
    
protected:
    typedef std::map<string, XPathFunction>    FunctionLookup;
    
    static void _XMLFunctionWrapper(xmlXPathParserContextPtr ctx, int nargs);
    void PerformFunction(xmlXPathParserContextPtr ctx, const string & name, const string & uri, int nargs);
    
    string             _xpath;
    const class Document *  _document;
    
    _xmlXPathContext *      _ctx;
    _xmlXPathCompExpr *     _compiled;
    FunctionLookup          _functions;
    
    _xmlXPathObject *       _lastResult;
    
};

EPUB3_XML_END_NAMESPACE

#endif /* defined(__ePub3_xml_xpath__) */
