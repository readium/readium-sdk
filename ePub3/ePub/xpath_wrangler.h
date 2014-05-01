//
//  xpath_wrangler.h
//  ePub3
//
//  Created by Jim Dovey on 2012-11-29.
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

// this is a temporary solution which will be replaced with the XML object library soon

#ifndef __ePub3__xpath_wrangler__
#define __ePub3__xpath_wrangler__

#include <ePub3/epub3.h>
#include <ePub3/utilities/utfstring.h>
#include <ePub3/xml/xpath.h>
#include <map>
#include <vector>

EPUB3_BEGIN_NAMESPACE

/**
 A simple object which encapsulates the use of an XPath expression in libxml2.
 
 @ingroup utilities
 */
class XPathWrangler
{
public:
    ///
    /// A list of namespace prefix to URI pairs.
    typedef std::map<string, string>    NamespaceList;
    ///
    /// A list of strings.
    typedef std::vector<string>         StringList;
    
public:
    /**
     Create a new XPathWrangler instance.
     @param doc The XML document upon which all operations are to be performend.
     @param namespaces A list of namespace prefix to URI pairs defining namespaces
     that the XPath interpreter should expect to find.
     @see RegisterNamespaces(const NamespaceList&)
     */
    EPUB3_EXPORT
    XPathWrangler(shared_ptr<xml::Document> doc, const NamespaceList & namespaces = NamespaceList());
    ///
    /// Copy constructor.
    EPUB3_EXPORT
    XPathWrangler(const XPathWrangler& o);
    ///
    /// C++11 move constructor.
    EPUB3_EXPORT
    XPathWrangler(XPathWrangler&& o);
    ~XPathWrangler();
    
    /// @{
    /// @name XPath Evaluation
    
    /**
     Evaluates an XPath, returning a list of strings as a result.
     @param xpath The XPath expression to evaluate.
     @param node The root node upon which to evaluate the XPath expression. If not
     supplied, the document root node will be used.
     @result The results of evaluating the XPath expression, coerced to string
     values.
     */
    EPUB3_EXPORT
	StringList      Strings(const string& xpath, shared_ptr<xml::Node> node = nullptr);
    
    /**
     Evaluates an XPath as a boolean expression.
     
     This method will aim to return a Boolean result value at the earliest possible
     opportunity rather than constructing a potentially large node-set as a result
     only to cast it to a Boolean for return.
     
     @remarks Note that this will explicitly adjust the XPath such that it will stop
     evaluation upon finding the first matching node.
     @param xpath The XPath expression to evaluate.
     @param node The root node upon which to evaluate the XPath expression. If not
     supplied, the document root node will be used.
     @result Returns `false` if the XPath did not match any node in the specified
     subtree. Otherwise, returns `true` to indicate that at least one result would
     be returned from either Strings(const string&, xmlNodePtr) or
     Nodes(const string&, xmlNodePtr).
     */
    EPUB3_EXPORT
	bool            Matches(const string& xpath, shared_ptr<xml::Node> node = nullptr);
    
    /**
     Evaluates an XPath, returning a set of all matching nodes.
     @param xpath The XPath expression to evaluate.
     @param node The root node upon which to evaluate the XPath expression. If not
     supplied, the document root node will be used.
     @result The nodes which result from evaluating the XPath expression on the
     specified subtree.
     */
    EPUB3_EXPORT
    xml::NodeSet	Nodes(const string& xpath, shared_ptr<xml::Node> node=nullptr);
    
    /// @}
    
    /// @{
    /// @name Namespace Handling
    
    /**
     Adds a namespace prefix:URI pair to the XPath evaluation context.
     
     If a document uses namespaces, then an XPath expression which does not
     explicitly reference those namespaces will *not* match. Additionally, the XPath
     evaluation context will not automatically load namespace details from a
     document.
     
     For example, an OPF document uses elements from the DCMES namespace, defined as
     `http://purl.org/dc/elements/1.1/`. Regardless of any prefix assigned to that
     namespace by the source document, an XPath evaluation context must be informed
     of the prefix that will be used to reference that namespace *in the XPath
     expression*. Thus to match a `<dc:title>` element, the XPath evaluation context
     must be told to associate `http://purl.org/dc/elements/1.1/` with the prefix 
     `dc`. It will now recognize XPaths such as `//dc:title/text()`.
     @param namespaces A list of namespace prefix to URI pairs to register with the
     XPath evaluation context.
     */
    EPUB3_EXPORT
    void            RegisterNamespaces(const NamespaceList& namespaces);
    
    /**
     Assigns a prefix to the default namespace of the source document.
     
     As described in the documentation for RegisterNamespaces(const NamespaceList&),
     the XPath evaluation context needs to be informed explicitly of the existence of
     every single namespace. This also applies to the default namespace, which
     otherwise does not have a prefix assigned.
     
     When the default namespace is known in advance, it is possible and indeed
     recommended that it be registered using RegisterNamespaces(const NamespaceList&).
     This method will look at the document to see if it defines a document-wide
     default namespace, and will assign it a given prefix in the XPath evaluation
     context.
     
     This can be useful where an XPath is selecting an element which could be a
     member of two or more different namespaces, such as OEBPS 1.2 vs. OPF 3.0.
     When the exact version is unknown (and unneeded) then the caller can simply
     request that the default namespace be called e.g. 'opf'. This way, an XPath of
     `//opf:manifest/opf:item/@opf:href` could be run against both an OPF 3.0
     document and an OEBPS 1.2 document.
     @param name The prefix to assign to the default namespace within the XPath
     evaluation context.
     */
    EPUB3_EXPORT
    void            NameDefaultNamespace(const string& name);
    
    /// @}
    
protected:
	shared_ptr<xml::Document>	_doc;			///< The XML document on which this will operate.
	NamespaceList				_namespaces;	///< The namespaces to register when running XPath queries.
};

EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__xpath_wrangler__) */
