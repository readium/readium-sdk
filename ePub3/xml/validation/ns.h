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

#include <ePub3/xml/base.h>
#include <ePub3/utilities/utfstring.h>
#include <vector>
#include <libxml/tree.h>

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

EPUB3_XML_END_NAMESPACE

#endif /* defined(__ePub3_xml_ns__) */
