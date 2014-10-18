//
//  dtd.h
//  ePub3
//
//  Created by Jim Dovey on 2012-11-27.
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

#ifndef __ePub3_xml_dtd__
#define __ePub3_xml_dtd__

#include <ePub3/xml/base.h>
#include <ePub3/xml/document.h>

EPUB3_XML_BEGIN_NAMESPACE

/**
 @ingroup validation
 */
class DTD : public Node
{
public:
#if EPUB_USE(LIBXML2)
	typedef _xmlDtd*								NativeDtdPtr;
#elif EPUB_USE(WIN_XML)
	typedef Windows::Data::Xml::Dom::DtdNotation^	NativeDtdPtr;
#endif
public:
    explicit DTD(NativeDtdPtr dtd) : Node(xml_native_cast<NativePtr>(dtd)) {}
#if EPUB_USE(LIBXML2) && EPUB_ENABLE(XML_BUILDER)
    DTD(class Document * doc, const string & name, const string & externalID, const string & systemID) : Node(xml_native_cast<NativePtr>(xmlNewDtd(doc->xml(), name.utf8(), externalID.utf8(), systemID.utf8()))) {}
#endif
    virtual ~DTD() {}
    
    // Name() is provided by Node already
#if EPUB_USE(LIBXML2)
    string ExternalID() const { return xml()->ExternalID; }
    string SystemID() const { return xml()->SystemID; }
#elif EPUB_USE(WIN_XML)
	string ExternalID() const { return dynamic_cast<::Platform::String^>(const_cast<NativeDtdPtr>(xml())->PublicId); }
	string SystemID() const { return dynamic_cast<::Platform::String^>(const_cast<NativeDtdPtr>(xml())->SystemId); }
#endif
    
    NativeDtdPtr xml() { return xml_native_cast<NativeDtdPtr>(Node::xml()); }
    const NativeDtdPtr xml() const { return xml_native_cast<const NativeDtdPtr>(Node::xml()); }
};

EPUB3_XML_END_NAMESPACE

#endif /* defined(__ePub3__dtd__) */
