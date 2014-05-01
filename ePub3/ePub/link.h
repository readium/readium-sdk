//
//  link.h
//  ePub3
//
//  Created by Jim Dovey on 11/28/2013.
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
