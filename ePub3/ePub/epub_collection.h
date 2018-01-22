//
//  collection.h
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

#ifndef __ePub3__collection__
#define __ePub3__collection__

#include <ePub3/epub3.h>
#include <ePub3/link.h>
#include <ePub3/utilities/xml_identifiable.h>
#include <ePub3/property_holder.h>
#include <ePub3/utilities/string_view.h>

EPUB3_BEGIN_NAMESPACE

class Collection;
typedef std::map<string, shared_ptr<Collection>>    CollectionList;

class Package;

/**
 The Collection class implements the interface to an EPUB 3.0.1 <collection>.
 */
class Collection
    : public PointerType<Collection>, public OwnedBy<Package>,
      public XMLIdentifiable, public PropertyHolder
#if EPUB_PLATFORM(WINRT)
    , public NativeBridge
#endif
{
public:
    ///
    /// This role identifies a collection as an Index, per EPUB Indexes 1.0
    /// @see http://www.idpf.org/epub/idx/
    static EPUB3_EXPORT const string_view IndexRole;
    ///
    /// This role identifies a collection as a manifest.
    /// @see http://www.idpf.org/epub/vocab/package/roles/index.html#roles
    static EPUB3_EXPORT const string_view ManifestRole;
    ///
    /// This role identifies a collection as a list of content visible in a preview.
    /// @see https://docs.google.com/document/d/1GLSA-1hEkxZRlHW59yuh5gRIwNQaZCHuCr9CDqhayOg/
    static EPUB3_EXPORT const string_view PreviewRole;
    
private:
    ///
    /// No default constructor.
    Collection()            _DELETED_;
    
public:
    /**
     Constructs a new Collection.
     @param owner The package which owns this collection.
     @param parent The parent Collection object, if any.
     */
    EPUB3_EXPORT
    Collection(const PackagePtr& owner, const CollectionPtr& parent);
    
#if EPUB_COMPILER_SUPPORTS(CXX_DEFAULTED_FUNCTIONS)
    EPUB3_EXPORT
    Collection(const Collection&) = default;
    EPUB3_EXPORT
    Collection& operator=(const Collection&) = default;
    
    EPUB3_EXPORT
    Collection(Collection&&) = default;
    EPUB3_EXPORT
    Collection& operator=(Collection&&) = default;
#else
    EPUB3_EXPORT
    Collection(const Collection&);
    EPUB3_EXPORT
    Collection& operator=(const Collection&);
    
    EPUB3_EXPORT
    Collection(Collection&&);
    EPUB3_EXPORT
    Collection& operator=(Collection&&);
#endif
    
    virtual
    ~Collection() {}
    
    EPUB3_EXPORT FORCE_INLINE
    PackagePtr GetPackage() const
        { return Owner(); }
    
    EPUB3_EXPORT
    bool ParseXML(shared_ptr<xml::Node> node);
    
    EPUB3_EXPORT FORCE_INLINE
    const string& Role() const
        { return _role; }
    
    EPUB3_EXPORT FORCE_INLINE
    std::size_t ChildCollectionCount() const
        { return _childCollections.size(); }
    
    EPUB3_EXPORT FORCE_INLINE
    CollectionPtr ChildCollectionWithRole(string_view role)
        {
            auto found = _childCollections.find(role);
            if (found == _childCollections.end())
                return nullptr;
            return found->second;
        }
    
    EPUB3_EXPORT FORCE_INLINE
    std::size_t LinkCount() const
        { return _links.size(); }
    
    EPUB3_EXPORT FORCE_INLINE
    LinkPtr LinkAt(std::size_t idx)
        { return _links.at(idx); }
    
private:
    weak_ptr<Collection>    _parentCollection;
    string                  _role;
    CollectionList          _childCollections;
    LinkList                _links;
    
    void ParseMetadata(shared_ptr<xml::Node> node);
    
};

EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__collection__) */
