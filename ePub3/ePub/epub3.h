//
//  epub3.h
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

#ifndef ePub3_epub3_h
#define ePub3_epub3_h

#include <ePub3/base.h>
#include <ePub3/utilities/basic.h>
#include <ePub3/utilities/utfstring.h>

// this chunk will go away once we have the C++ XML interface complete
#include <string>
#include <sstream>

#if EPUB_USE(LIBXML2)
#include <libxml/tree.h>
#elif EPUB_USE(WIN_XML)
#include <ePub3/xml/node.h>
#endif

#include <ePub3/Forward.h>

#if EPUB_PLATFORM(WINRT)
#include <ePub3/utilities/NativeBridge.h>
#endif

#define ePub3NamespaceURI "http://www.idpf.org/2007/ops"
#define MathMLNamespaceURI "http://www.w3.org/1998/Math/MathML"
#define PLSNamespaceURI "http://www.w3.org/2005/01/pronunciation-lexicon"
#define SSMLNamespaceURI "http://www.w3.org/2001/10/synthesis"
#define SVGNamespaceURI "http://www.w3.org/2000/svg"
#define OCFNamespaceURI "urn:oasis:names:tc:opendocument:xmlns:container"
#define XMLENCNamespaceURI "http://www.w3.org/2001/04/xmlenc#"
#define XMLDSigNamespaceURI "http://www.w3.org/2000/09/xmldsig#"

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
#if EPUB_USE(LIBXML2)
static inline string _getProp(xmlNodePtr node, const char *name, const char *nsURI = nullptr)
{
    if ( node == nullptr )
        return string::EmptyString;
    
    xmlChar * ch = nullptr;
    if ( nsURI != nullptr )
    {
        ch = xmlGetNsProp(node, reinterpret_cast<const xmlChar*>(name), reinterpret_cast<const xmlChar*>(nsURI));
    }
    
    if ( ch == nullptr )
    {
        ch = xmlGetProp(node, reinterpret_cast<const xmlChar*>(name));
    }
    
    if ( ch == nullptr )
        return string::EmptyString;
    
    string result(ch);
    xmlFree(ch);
    return result;
}
#elif EPUB_USE(WIN_XML)
 static inline string _getProp(std::shared_ptr<xml::Node> node, const char *name, const char *nsURI = "")
{
	return node->AttributeValue(name, nsURI);
}
#endif	// EPUB_USE(LIBXML2)

EPUB3_END_NAMESPACE

#endif
