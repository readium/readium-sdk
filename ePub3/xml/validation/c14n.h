//
//  c14n.h
//  ePub3
//
//  Created by Jim Dovey on 2012-11-21.
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

#ifndef __ePub3__c14n__
#define __ePub3__c14n__

#include <ePub3/xml/base.h>

#if EPUB_ENABLE(XML_C14N)

#include <ePub3/xml/io.h>
#include <ePub3/utilities/utfstring.h>
#include <sstream>

EPUB3_XML_BEGIN_NAMESPACE

class Document;
class Element;
class Node;

// Version codes
/**
 @ingroup validation
 */
enum class C14NVersion : uint8_t {
    v1_0,
    v1_0_Exclusive,
    v1_1,
    v2_0
};

/**
 @ingroup validation
 */
template <C14NVersion _Version, bool _WithComments>
struct C14NParams
{
public:
    typedef std::map<string, string> QNameAwarenessMap;
    
public:
    C14NParams() : withComments(_WithComments) {}
    ~C14NParams() {}
    
    CONSTEXPR C14NVersion Version() const { return _Version; }
    CONSTEXPR bool Exclusive() const { return _Version == C14NVersion::v1_0_Exclusive; }
    CONSTEXPR bool TrimWhitespace() const { return trimWhitespace; }
    CONSTEXPR bool PreserveComments() const { return withComments; }
    CONSTEXPR bool RewritePrefixes() const { return rewritePrefixes; }
    
    const QNameAwarenessMap QNameAwareElements() const { return elements; }
    const QNameAwarenessMap QNameAwareXPathElements() const { return xPathElements; }
    const QNameAwarenessMap QNameAwareQualifiedAttrs() const { return qualifiedAttributes; }
    const QNameAwarenessMap QNameAwareUnqualifiedAttrs() const { return unqualifiedAttributes; }
    
protected:
    bool                trimWhitespace;
    bool                withComments;
    bool                rewritePrefixes;
    QNameAwarenessMap   elements;
    QNameAwarenessMap   xPathElements;
    QNameAwarenessMap   qualifiedAttributes;
    QNameAwarenessMap   unqualifiedAttributes;
};

/**
 @ingroup validation
 */
struct C14N
{
    // namespaced symbolic constants to use for canonicalization types
    static const C14NParams<C14NVersion::v1_0, false>           V1_0;
    static const C14NParams<C14NVersion::v1_0, true>            V1_0_WithComments;
    static const C14NParams<C14NVersion::v1_0_Exclusive, false> V1_0_Exclusive;
    static const C14NParams<C14NVersion::v1_0_Exclusive, true>  V1_0_Exclusive_WithComments;
    static const C14NParams<C14NVersion::v1_1, false>           V1_1;
    static const C14NParams<C14NVersion::v1_1, true>            V1_1_WithComments;
    static const C14NParams<C14NVersion::v2_0, false>           V2_0;
    static const C14NParams<C14NVersion::v2_0, true>            V2_0_WithComments;
    
    // handle a c14n2::blah blob
    static const C14NParams<C14NVersion::v2_0, false> C14N2Parameters(InputBuffer & input);
    static const C14NParams<C14NVersion::v2_0, false> C14N2Parameters(const Element * element);
    
    template<C14NVersion _Version, bool _WithComments>
    static bool Canonicalize(const Document * doc, OutputBuffer & output, const C14NParams<_Version,_WithComments> & params);
    
    template<C14NVersion _Version, bool _WithComments>
    static string Canonicalize(const Document * doc, const C14NParams<_Version,_WithComments> & params)
    {
        std::ostringstream __o;
        if ( !Canonicalize(doc, StreamOutputBuffer(__o), params) )
            return string();
        return xmlString(__o.str());
    }
};

EPUB3_XML_END_NAMESPACE

#endif	// EPUB_ENABLE(XML_C14N)

#endif /* defined(__ePub3__c14n__) */
