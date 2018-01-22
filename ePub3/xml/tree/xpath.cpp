//
//  xpath.cpp
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

#include "xpath.h"
#include "node.h"
#include "document.h"
#include <libxml/xpathInternals.h>

EPUB3_XML_BEGIN_NAMESPACE

static const xmlChar * _XMLInstanceVarName = BAD_CAST "instance";
static const xmlChar * _XMLInstanceNamespace = BAD_CAST "urn:kobo:ePub3:xml:XPathInstance";

static XPathEvaluator::ObjectType __get_obj_type(xmlXPathObjectPtr ptr)
{
    return (ptr == nullptr ? XPathEvaluator::ObjectType::Undefined : XPathEvaluator::ObjectType(ptr->type));
}

void XPathEvaluator::_XMLFunctionWrapper(xmlXPathParserContextPtr ctx, int nargs)
{
    // find the C++ wrapper object
    XPathEvaluator * evaluator = nullptr;
    xmlXPathObjectPtr thisVar = xmlXPathVariableLookupNS(ctx->context, _XMLInstanceVarName, _XMLInstanceNamespace);
    if ( thisVar != nullptr )
        evaluator = reinterpret_cast<XPathEvaluator *>(thisVar->user);
    if ( evaluator == nullptr )
    {
        // pop all arguments and return 'zero'
        for ( int i = 0; i < nargs; i++ )
            valuePop(ctx);
        valuePush(ctx, xmlXPathNewBoolean(0));
        return;
    }
    
    evaluator->PerformFunction(ctx, ctx->context->function, ctx->context->functionURI, nargs);
}

XPathEvaluator::XPathEvaluator(const string & xpath, std::shared_ptr<const class Document> document)
: _xpath(xpath), _document(document), _ctx(nullptr), _compiled(nullptr), _lastResult(NULL)
{
    xmlDocPtr doc = const_cast<_xmlDoc*>(document->xml());
    _ctx = xmlXPathNewContext(doc);
    xmlXPathRegisterAllFunctions(_ctx);
    
    //_compiled = xmlXPathCompile(xpath.utf8());
    
    // store a pointer back to the C++ object in the xpath context
    xmlXPathObject obj;
    obj.type = XPATH_USERS;
    obj.user = this;
    xmlXPathObjectPtr p = xmlXPathObjectCopy(&obj);
    xmlXPathRegisterVariableNS(_ctx, _XMLInstanceVarName, _XMLInstanceNamespace, p);
}
XPathEvaluator::~XPathEvaluator()
{
    if ( _compiled != nullptr )
        xmlXPathFreeCompExpr(_compiled);
    if ( _lastResult != nullptr )
        xmlXPathFreeObject(_lastResult);
    if ( _ctx != nullptr )
        xmlXPathFreeContext(_ctx);
}

bool XPathEvaluator::Compile()
{
    if (_compiled)
        return true;
    
    _compiled = xmlXPathCompile(_xpath.utf8());
    return _compiled != nullptr;
}

#if 0
#pragma mark - XPath Environment
#endif

bool XPathEvaluator::RegisterNamespace(const string &prefix, const string &uri)
{
    return (xmlXPathRegisterNs(_ctx, prefix.utf8(), uri.utf8()) == 0);
}
bool XPathEvaluator::RegisterNamespaces(const NamespaceMap &namespaces)
{
    for ( auto item : namespaces )
    {
        if ( RegisterNamespace(item.first, item.second) == false )
            return false;
    }
    return true;
}
bool XPathEvaluator::RegisterAllNamespacesForElement(std::shared_ptr<const Element> element)
{
    // TBI
    return false;
}
bool XPathEvaluator::RegisterFunction(const string &name, XPathFunction fn)
{
    if ( fn.target<std::nullptr_t>() == nullptr )
    {
        if ( xmlXPathRegisterFunc(_ctx, name.utf8(), nullptr) == 0 )
        {
            _functions.erase(name);
            return true;
        }
        return false;
    }
    
    if ( xmlXPathRegisterFunc(_ctx, name.utf8(), XPathEvaluator::_XMLFunctionWrapper) == 0 )
    {
        _functions[name] = fn;
        return true;
    }
    return false;
}
bool XPathEvaluator::RegisterFunction(const string &name, const string &namespaceURI, XPathFunction fn)
{
    if ( fn.target<std::nullptr_t>() == nullptr )
    {
        if ( xmlXPathRegisterFuncNS(_ctx, name.utf8(), namespaceURI.utf8(), nullptr) == 0 )
        {
            _functions.erase(namespaceURI + "::" + name);
            return true;
        }
        return false;
    }
    
    if ( xmlXPathRegisterFuncNS(_ctx, name.utf8(), namespaceURI.utf8(), XPathEvaluator::_XMLFunctionWrapper) == 0 )
    {
        _functions[namespaceURI + "::" + name] = fn;
        return true;
    }
    return false;
}
void XPathEvaluator::PerformFunction(xmlXPathParserContextPtr ctx, const string &name, const string &uri, int nargs)
{
    string key;
    if ( uri.length() > 0 )
        key = uri + "::" + name;
    else
        key = name;
    
    auto iter = _functions.find(key);
    if ( iter == _functions.end() )
    {
        // eat the stack & push false
        for ( int i = 0; i < nargs; i++ )
            valuePop(ctx);
        valuePush(ctx, xmlXPathNewBoolean(0));
        return;
    }
    
    // call the function
    iter->second(ctx, nargs);
}
bool XPathEvaluator::RegisterVariable(const string &name, void *data, ObjectType type, const string &namespaceURI)
{
    xmlXPathObjectPtr xValue = nullptr;
    switch ( type )
    {
        case ObjectType::Undefined:
        default:
            return false;
            
        case ObjectType::NodeSet:
        {
            const xml::NodeSet * nodes = reinterpret_cast<const xml::NodeSet*>(data);
            if ( typeid(xml::NodeSet) != typeid(nodes) )
                return false;
            
            xmlNodePtr * ptr = reinterpret_cast<xmlNodePtr*>(xmlMemMalloc(sizeof(xmlNodePtr) * nodes->size()));
            if ( ptr == nullptr )
                return false;
            
            xmlNodeSetPtr set = xmlXPathNodeSetCreate(nullptr);
            if ( set == nullptr )
                return false;
            
            for ( size_t i = 0; i < nodes->size(); i++ )
                xmlXPathNodeSetAdd(set, nodes->at(i)->xml());
            
            xValue = xmlXPathNewNodeSetList(set);
            break;
        }
            
        case ObjectType::Boolean:
        {
            const bool * val = reinterpret_cast<const bool *>(data);
            if ( typeid(bool) != typeid(val) )
                return false;
            xValue = xmlXPathNewBoolean((*val ? 1 : 0));
            break;
        }
            
        case ObjectType::Number:
        {
            const double * val = reinterpret_cast<const double *>(data);
            xValue = xmlXPathNewFloat(*val);
            break;
        }
            
        case ObjectType::String:
        {
            const string * str = reinterpret_cast<const string*>(data);
            if ( typeid(string) == typeid(str) )
            {
                xValue = xmlXPathNewString(str->utf8());
                break;
            }
            
            const xmlChar * chars = reinterpret_cast<const xmlChar *>(data);
            xValue = xmlXPathNewString(chars);
            break;
        }
    }
    
    if ( namespaceURI.length() == 0 )
        return ( xmlXPathRegisterVariable(_ctx, name.utf8(), xValue) == 0 );
    
    return ( xmlXPathRegisterVariableNS(_ctx, name.utf8(), namespaceURI.utf8(), xValue) == 0 );
}

#if 0
#pragma mark - XPath Evaluation
#endif

bool XPathEvaluator::Evaluate(std::shared_ptr<const Node> node, ObjectType * resultType)
{
    if ( _lastResult != nullptr )
        xmlXPathFreeObject(_lastResult);
    
    _ctx->node = const_cast<xmlNodePtr>(node->xml());
    if (_compiled != nullptr)
        _lastResult = xmlXPathCompiledEval(_compiled, _ctx);
    else
        _lastResult = xmlXPathEval(_xpath.utf8(), _ctx);
    if (resultType != nullptr)
        *resultType = __get_obj_type(_lastResult);
    return ( _lastResult != nullptr );
}
bool XPathEvaluator::EvaluateAsBoolean(std::shared_ptr<const Node> node)
{
    if ( _lastResult != nullptr )
        xmlXPathFreeObject(_lastResult);
    
    _ctx->node = const_cast<xmlNodePtr>(node->xml());
    int r = 0;
    if (_compiled != nullptr) {
        r = xmlXPathCompiledEvalToBoolean(_compiled, _ctx);
    } else {
        xmlXPathObjectPtr obj = xmlXPathEval(_xpath.utf8(), _ctx);
        if (obj != nullptr)
            r = xmlXPathCastToBoolean(obj);
    }
    return ( r != 0 );
}
bool XPathEvaluator::BooleanResult() const
{
    if ( _lastResult == nullptr )
        throw InternalError(std::string(__PRETTY_FUNCTION__) + " called when no result available");
    return (xmlXPathCastToBoolean(_lastResult) != 0);
}
double XPathEvaluator::NumberResult() const
{
    if ( _lastResult == nullptr )
        throw InternalError(std::string(__PRETTY_FUNCTION__) + " called when no result available");
    return xmlXPathCastToNumber(_lastResult);
}
string XPathEvaluator::StringResult() const
{
    if ( _lastResult == nullptr )
        throw InternalError(std::string(__PRETTY_FUNCTION__) + " called when no result available");
    return xmlXPathCastToString(_lastResult);
}
ePub3::xml::NodeSet XPathEvaluator::NodeSetResult() const
{
    if ( _lastResult == nullptr )
        throw InternalError(std::string(__PRETTY_FUNCTION__) + " called when no result available");
    if ( _lastResult->type != XPATH_NODESET )
        throw std::domain_error(std::string(__PRETTY_FUNCTION__) + " called when last result is not a Node Set");
    
    NodeSet nodes;
    xmlNodeSetPtr ns = _lastResult->nodesetval;
    
    for ( int i = 0; i < xmlXPathNodeSetGetLength(ns); i++ )
    {
        xmlNodePtr xml = xmlXPathNodeSetItem(ns, i);
        auto node = Wrapped<Node>(xml);
        if ( dynamic_cast<Node*>(node.get()) != nullptr )
        {
            nodes.push_back(node);
            continue;
        }
    }
    
    return nodes;
}

EPUB3_XML_END_NAMESPACE
