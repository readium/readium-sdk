//
//  ns.h
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
    explicit Namespace(_xmlNs * ns) : _xml(ns) {}
    Namespace() : _xml(nullptr) {}
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
