//
//  epub3.h
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

#ifndef ePub3_epub3_h
#define ePub3_epub3_h

#include <ePub3/base.h>
#include <ePub3/utilities/basic.h>
#include <ePub3/utilities/utfstring.h>

// this chunk will go away once we have the C++ XML interface complete
#include <string>
#include <sstream>

#include <ePub3/xml/node.h>
#include <ePub3/Forward.h>

#if EPUB_PLATFORM(WINRT)
#include <ePub3/utilities/NativeBridge.h>
#endif

#define ePub3NamespaceURI "http://www.idpf.org/2007/ops"
#define MathMLNamespaceURI "http://www.w3.org/1998/Math/MathML"
#define SMILNamespaceURI "http://www.w3.org/ns/SMIL"
#define PLSNamespaceURI "http://www.w3.org/2005/01/pronunciation-lexicon"
#define SSMLNamespaceURI "http://www.w3.org/2001/10/synthesis"
#define SVGNamespaceURI "http://www.w3.org/2000/svg"
#define OCFNamespaceURI "urn:oasis:names:tc:opendocument:xmlns:container"
#define XMLENCNamespaceURI "http://www.w3.org/2001/04/xmlenc#"
#define XMLDSigNamespaceURI "http://www.w3.org/2000/09/xmldsig#"
#define XHTMLNamespaceURI "http://www.w3.org/1999/xhtml"
#define NCXNamespaceURI "http://www.daisy.org/z3986/2005/ncx/"

#define NCXContentType "application/x-dtbncx+xml"

EPUB3_BEGIN_NAMESPACE

/**
 Reads a property (attribute) value from an XML node.
 
 Thi method performs all the book-keeping of strings copied from `libxml` properties.
 @param node The node whose property to read.
 @param name The name of the property.
 @param nsURI The URI of the namespace that defines the property.
 @result A string containing the property value, or an empty string if none was found.
 @ingroup utilities
 */
static inline string _getProp(std::shared_ptr<xml::Node> node, const char *name, const char *nsURI = "")
{
	return node->AttributeValue(name, nsURI);
}

EPUB3_END_NAMESPACE

#endif
