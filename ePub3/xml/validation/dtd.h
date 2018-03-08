//
//  dtd.h
//  ePub3
//
//  Created by Jim Dovey on 2012-11-27.
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
