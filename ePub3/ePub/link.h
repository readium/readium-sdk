//
//  link.h
//  ePub3
//
//  Created by Jim Dovey on 11/28/2013.
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

#ifndef __ePub3__link__
#define __ePub3__link__

#include <ePub3/epub3.h>
#include <ePub3/Forward.h>
#include <ePub3/utilities/string_view.h>

EPUB3_BEGIN_NAMESPACE

class Link;
typedef shared_vector<Link>         LinkList;

/**
 The Link class reperesents a <link> element within a <collection> item.
 */
class Link
    : public PointerType<Link>, public OwnedBy<Collection>
#if EPUB_PLATFORM(WINRT)
    , public NativeBridge
#endif
{
private:
    ///
    /// No default constructor.
    Link()      _DELETED_;
    
public:
    /**
     Constructs a new Link.
     @param owner The Collection to which this link belongs.
     */
    EPUB3_EXPORT
    Link(CollectionPtr owner);
    
#if EPUB_COMPILER_SUPPORTS(CXX_DEFAULTED_FUNCTIONS)
    EPUB3_EXPORT
    Link(const Link&) = default;
    EPUB3_EXPORT
    Link& operator=(const Link&) = default;
    
    EPUB3_EXPORT
    Link(Link&&) = default;
    EPUB3_EXPORT
    Link& operator=(Link&&) = default;
#else
    EPUB3_EXPORT
    Link(const Link&);
    EPUB3_EXPORT
    Link& operator=(const Link&);
    
    EPUB3_EXPORT
    Link(Link&&);
    EPUB3_EXPORT
    Link& operator=(Link&&);
#endif
    
    virtual
    ~Link() {}
    
    EPUB3_EXPORT FORCE_INLINE
    CollectionPtr GetCollection() const
        { return Owner(); }
    
    EPUB3_EXPORT
    bool ParseXML(shared_ptr<xml::Node> node);
    
    EPUB3_EXPORT FORCE_INLINE
    const string& Rel() const
        { return _rel; }
    
    EPUB3_EXPORT FORCE_INLINE
    const string& Href() const
        { return _href; }
    
    EPUB3_EXPORT FORCE_INLINE
    const string& MediaType() const
        { return _type; }
    
    /**
     Obtain the ManifestItem referenced by this link.
     
     Note that this may legally return `nullptr`-- <link> elements within a
     <collection> aren't required to target manifest items, and <link> elements
     within <package/metadata> are required *not* to.
     
     @result The referenced ManifestItem, or `nullptr` if the link doesn't reference
             a manifest item from this Package.
     */
    EPUB3_EXPORT
    ConstManifestItemPtr ReferencedManifestItem() const;
    
private:
    string      _href;
    string      _rel;
    string      _type;
    
};

EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__link__) */
