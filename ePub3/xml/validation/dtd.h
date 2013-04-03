//
//  dtd.h
//  ePub3
//
//  Created by Jim Dovey on 2012-11-27.
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

#ifndef __ePub3_xml_dtd__
#define __ePub3_xml_dtd__

#include "document.h"

EPUB3_XML_BEGIN_NAMESPACE

/**
 @ingroup validation
 */
class DTD : public Node
{
public:
    explicit DTD(_xmlDtd * dtd) : Node(reinterpret_cast<_xmlNode*>(dtd)) {}
    DTD(class Document * doc, const string & name, const string & externalID, const string & systemID) : Node(reinterpret_cast<_xmlNode*>(xmlNewDtd(doc->xml(), name.utf8(), externalID.utf8(), systemID.utf8()))) {}
    virtual ~DTD() {}
    
    // Name() is provided by Node already
    string ExternalID() const { return xml()->ExternalID; }
    string SystemID() const { return xml()->SystemID; }
    
    _xmlDtd * xml() { return reinterpret_cast<_xmlDtd*>(Node::xml()); }
    const _xmlDtd * xml() const { return reinterpret_cast<const _xmlDtd*>(Node::xml()); }
};

EPUB3_XML_END_NAMESPACE

#endif /* defined(__ePub3__dtd__) */
