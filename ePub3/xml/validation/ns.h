//
//  ns.h
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

#ifndef __ePub3_xml_ns__
#define __ePub3_xml_ns__

#include "../utilities/base.h"
#include "../utilities/xmlstring.h"
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
typedef std::vector<Namespace *> NamespaceList;

/**
 @ingroup validation
 */
#if EPUB_USE(LIBXML2)

class Namespace : public WrapperBase
{
public:
    explicit Namespace(_xmlNs * ns) : _xml(ns) {}
    Namespace() : _xml(nullptr) {}
    Namespace(Document * doc, const string & prefix, const string & uri);
    virtual ~Namespace();
    
    bool IsEmpty() const { return _xml == nullptr; }
    
    Namespace * Next() const { return reinterpret_cast<Namespace*>(_xml->_private); }
    bool IsGlobal() const { return _xml->type != XML_LOCAL_NAMESPACE; }
    string URI() const { return _xml->href; }
    string Prefix() const { return _xml->prefix; }
    
    _xmlNs * xml() { return _xml; }
    const _xmlNs * xml() const { return _xml; }
    
protected:
    _xmlNs *    _xml;
    
};

#elif EPUB_USE(WIN_XML)

// optimization: use Platform::String directly
class Namespace
{
protected:
	::Platform::String^		_href;
	::Platform::String^		_prefix;
	Namespace*				_next;

public:
	Namespace() : _href(), _prefix() {}
	Namespace(Document* doc, ::Platform::StringReference prefix, ::Platform::StringReference uri) : _href(uri), _prefix(prefix) {}
	Namespace(Document* doc, ::Platform::String^ prefix, ::Platform::String^ uri) : _href(uri), _prefix(prefix) {}
	virtual ~Namespace() {}

	bool IsEmpty() const { return _href->IsEmpty(); }

	Namespace* Next() const { return _next; }
	bool IsGlobal() const { return true; }

	::Platform::String^ URI() const { return _href; }
	::Platform::String^ Prefix() const { return _prefix; }

};

#endif

EPUB3_XML_END_NAMESPACE

#endif /* defined(__ePub3_xml_ns__) */
