//
//  epub3.h
//  ePub3
//
//  Created by Jim Dovey on 2012-11-27.
//  Copyright (c) 2012-2013 The Readium Foundation.
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

#include "../base.h"
#include "../utilities/basic.h"
#include "../utilities/utfstring.h"

// this chunk will go away once we have the C++ XML interface complete
#include <string>
#include <sstream>
#include <libxml/tree.h>

#define ePub3NamespaceURI "http://www.idpf.org/2007/ops"
#define MathMLNamespaceURI "http://www.w3.org/1998/Math/MathML"
#define PLSNamespaceURI "http://www.w3.org/2005/01/pronunciation-lexicon"
#define SSMLNamespaceURI "http://www.w3.org/2001/10/synthesis"
#define SVGNamespaceURI "http://www.w3.org/2000/svg"
#define OCFNamespaceURI "urn:oasis:names:tc:opendocument:xmlns:container"
#define XMLENCNamespaceURI "http://www.w3.org/2001/04/xmlenc#"
#define XMLDSigNamespaceURI "http://www.w3.org/2000/09/xmldsig#"

EPUB3_BEGIN_NAMESPACE

static inline string _getProp(xmlNodePtr node, const char *name, const char *nsURI = nullptr)
{
    if ( node == nullptr )
        return string::EmptyString;
    
    xmlChar * ch = nullptr;
    if ( nsURI == nullptr )
    {
        ch = xmlGetProp(node, reinterpret_cast<const xmlChar*>(name));
    }
    else
    {
        ch = xmlGetNsProp(node, reinterpret_cast<const xmlChar*>(name), reinterpret_cast<const xmlChar*>(nsURI));
    }
    
    if ( ch == nullptr )
        return string::EmptyString;
    
    string result(ch);
    xmlFree(ch);
    return result;
}

EPUB3_END_NAMESPACE

#endif
