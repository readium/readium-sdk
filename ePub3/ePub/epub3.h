//
//  epub3.h
//  ePub3
//
//  Created by Jim Dovey on 2012-11-27.
//  Copyright (c) 2012 Kobo Inc. All rights reserved.
//

#ifndef ePub3_epub3_h
#define ePub3_epub3_h

#define EPUB3_BEGIN_NAMESPACE namespace ePub3 {
#define EPUB3_END_NAMESPACE };

#define ePub3NamespaceURI "http://www.idpf.org/2007/ops"
#define MathMLNamespaceURI "http://www.w3.org/1998/Math/MathML"
#define PLSNamespaceURI "http://www.w3.org/2005/01/pronunciation-lexicon"
#define SSMLNamespaceURI "http://www.w3.org/2001/10/synthesis"
#define SVGNamespaceURI "http://www.w3.org/2000/svg"
#define OCSNamespaceURI "urn:oasis:names:tc:opendocument:xmlns:container"
#define XMLENCNamespaceURI "http://www.w3.org/2001/04/xmlenc#"
#define XMLDSigNamespaceURI "http://www.w3.org/2000/09/xmldsig#"

// this chunk will go away once we have the C++ XML interface complete
#include <string>
#include <sstream>
#include <libxml/tree.h>

EPUB3_BEGIN_NAMESPACE

static inline std::string _getProp(xmlNodePtr node, const char *name, const char *nsURI = nullptr)
{
    if ( node == nullptr )
        return "";
    
    const xmlChar * ch = nullptr;
    if ( nsURI == nullptr )
    {
        ch = xmlGetProp(node, reinterpret_cast<const xmlChar*>(name));
    }
    else
    {
        ch = xmlGetNsProp(node, reinterpret_cast<const xmlChar*>(name), reinterpret_cast<const xmlChar*>(nsURI));
    }
    
    if ( ch == nullptr )
        return "";
    return reinterpret_cast<const char*>(ch);
}

//////////////////////////////////////////////////////////////////////////////
// nicer way of constructing a C++ string from randomly-typed arguments

static inline std::stringstream& __format(std::stringstream& s) { return s; }

template <typename Arg1, typename... Args>
static inline std::stringstream& __format(std::stringstream& s, const Arg1& arg1, const Args&... args)
{
    s << arg1;
    return __format(s, args...);
}

template <typename... Args>
static inline std::string _Str(const Args&... args)
{
    std::stringstream s;
    return __format(s, args...).str();
}

EPUB3_END_NAMESPACE

#endif
