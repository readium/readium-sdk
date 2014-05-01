//
//  collection.h
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
