//
//  ns.h
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

#ifndef __ePub3_xml_ns__
#define __ePub3_xml_ns__

#include <ePub3/xml/base.h>
#include <ePub3/xml/xmlstring.h>
#include <vector>

#if EPUB_USE(LIBXML2)
#include <libxml/tree.h>
#endif

EPUB3_XML_BEGIN_NAMESPACE

class Document;

class Namespace;
/**
 @ingroup validation
 */
typedef std::vector<std::shared_ptr<Namespace>> NamespaceList;

/**
 @ingroup validation
 */
#if EPUB_USE(LIBXML2)

class Namespace : public WrapperBase<Namespace>
{
public:
    explicit Namespace(_xmlNs * ns) : _xml(ns), bShouldxmlFreeNs(false) {}
    Namespace() : _xml(nullptr), bShouldxmlFreeNs(false) {}
    Namespace(std::shared_ptr<Document> doc, const string & prefix, const string & uri);
    virtual ~Namespace();
    
    bool IsEmpty() const { return _xml == nullptr; }
    
    Namespace * Next() const { return reinterpret_cast<Namespace*>(_xml->_private); }
    bool IsGlobal() const { return _xml->type != XML_LOCAL_NAMESPACE; }
    string URI() const { return _xml->href; }
    string Prefix() const { return _xml->prefix; }
    
    _xmlNs * xml() { return _xml; }
    const _xmlNs * xml() const { return _xml; }
    
    virtual
    void release()
        { _xml = nullptr; }
    
protected:
    bool bShouldxmlFreeNs;
    _xmlNs *    _xml;
    
};

#elif EPUB_USE(WIN_XML)

class Namespace
{
protected:
	string			_href;
	string			_prefix;
	Namespace*		_next;

public:
	Namespace() : _href(), _prefix() {}
	Namespace(std::shared_ptr<Document> doc, ::Platform::StringReference prefix, ::Platform::StringReference uri) : _href(uri), _prefix(prefix) {}
	Namespace(std::shared_ptr<Document> doc, ::Platform::String^ prefix, ::Platform::String^ uri) : _href(uri), _prefix(prefix) {}
	virtual ~Namespace() {}

	bool IsEmpty() const { return _href.empty(); }

	Namespace* Next() const { return _next; }
	bool IsGlobal() const { return true; }

	const string& URI() const { return _href; }
	const string& Prefix() const { return _prefix; }

};

#endif

EPUB3_XML_END_NAMESPACE

#endif /* defined(__ePub3_xml_ns__) */
