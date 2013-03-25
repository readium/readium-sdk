//
//  xpath.cpp
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

#include "xpath.h"
#include "node.h"
#include <libxml/xpathInternals.h>

EPUB3_XML_BEGIN_NAMESPACE

static const xmlChar * _XMLInstanceVarName = BAD_CAST "instance";
static const xmlChar * _XMLInstanceNamespace = BAD_CAST "urn:kobo:ePub3:xml:XPathInstance";

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

XPathEvaluator::XPathEvaluator(const string & xpath, const class Document * document)
: _xpath(xpath), _document(document), _lastResult(NULL)
{
    xmlDocPtr doc = nullptr;//const_cast<_xmlDoc*>(document->xml());
    _ctx = xmlXPathNewContext(doc);
    _compiled = xmlXPathCompile(xpath.utf8());
    
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

#pragma mark - XPath Environment

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
bool XPathEvaluator::RegisterAllNamespacesForElement(const Element *element)
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
            
            for ( int i = 0; i < nodes->size(); i++ )
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

#pragma mark - XPath Evaluation

bool XPathEvaluator::Evaluate(const Node *node, ObjectType * resultType)
{
    if ( _lastResult != nullptr )
        xmlXPathFreeObject(_lastResult);
    
    _ctx->node = const_cast<xmlNodePtr>(node->xml());
    _lastResult = xmlXPathCompiledEval(_compiled, _ctx);
    return ( _lastResult != nullptr );
}
bool XPathEvaluator::EvaluateAsBoolean(const Node *node)
{
    if ( _lastResult != nullptr )
        xmlXPathFreeObject(_lastResult);
    
    _ctx->node = const_cast<xmlNodePtr>(node->xml());
    int r = xmlXPathCompiledEvalToBoolean(_compiled, _ctx);
    return ( r != 0 );
}
bool XPathEvaluator::BooleanResult() const throw(ePub3::xml::InternalError)
{
    if ( _lastResult == nullptr )
        throw InternalError(std::string(__PRETTY_FUNCTION__) + " called when no result available");
    return xmlXPathCastToBoolean(_lastResult);
}
double XPathEvaluator::NumberResult() const throw(ePub3::xml::InternalError)
{
    if ( _lastResult == nullptr )
        throw InternalError(std::string(__PRETTY_FUNCTION__) + " called when no result available");
    return xmlXPathCastToNumber(_lastResult);
}
string XPathEvaluator::StringResult() const throw(ePub3::xml::InternalError)
{
    if ( _lastResult == nullptr )
        throw InternalError(std::string(__PRETTY_FUNCTION__) + " called when no result available");
    return xmlXPathCastToString(_lastResult);
}
ePub3::xml::NodeSet XPathEvaluator::NodeSetResult() const throw(ePub3::xml::InternalError, std::domain_error)
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
        if ( xml->_private != nullptr )
        {
            Node * node = reinterpret_cast<Node*>(xml->_private);
            if ( typeid(*node) == typeid(Node) )
            {
                nodes.push_back(node);
                continue;
            }
        }
        
        // need to build a wrapper
        nodes.push_back(new Node(xml));
    }
    
    return nodes;
}

EPUB3_XML_END_NAMESPACE
