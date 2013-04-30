//
//  spine.h
//  ePub3
//
//  Created by Jim Dovey on 2012-11-29.
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

#ifndef __ePub3__spine__
#define __ePub3__spine__

#include <ePub3/epub3.h>
#include <ePub3/utilities/utfstring.h>
#include <vector>
#include <libxml/tree.h>

EPUB3_BEGIN_NAMESPACE

class ManifestItem;
class Package;
class SpineItem;
class IRI;

/**
 The SpineItem class provides access to the spine of a publication.
 
 A publication's spine describes the top-level items contained in that publication
 in navigation order, such as the chapters of a book and its frontispiece,
 library/copyright page, dedication, index, and so on. This differs from a Table Of
 Contents in that the TOC might omit some of these items: it's not common for a TOC
 to reference the frontispiece of a book, or its cover.
 
 Additionally, the spine is used to determine where to go next when stepping from one
 content document to the next or the previous document. The appropriate spine item is
 used to determine what the next and previous items are, rather than relying on the
 TOC, which might choose not to reference certain items. The intention is that the
 spine be a one-dimensional array of all the visitable content in a publication in
 the order that it would be traversed.
 
 Most importantly, a content document **must** be referenced in the spine in order
 for it to be navigable. A TOC which references a document that's not in the spine
 is considered badly-formed.
 
 Spine items can be consedered either linear or non-linear. Non-linear items are
 those which are part of the publication's flow but which are not considered as
 candidates for next/previous steps. An example of this might be the front cover or
 the table of contents: these would typically be displayed elsewhere by a
 sufficiently advanced reading system, so simply flipping through the publication
 they would not be encountered; placing them in the spine, however, allows a CFI or
 hyperlink to still reference them directly.
 
 SpineItems in Readium are implemented as a doubly-linked list. Each SpineItem has a
 pointer to the items preceeding and succeeding it in the spine. These can be
 accessed directly using the Next() and Previous() methods. When stepping between
 spine items, however, the NextStep() and PriorStep() methods can be used to
 implicitly skip any non-linear items.
 
 @remarks As a linked-list structure, each SpineItem holds an *owning reference* to the
 following item, and a *non-owning reference* to the preceding item. When a
 SpineItem is destroyed, it will delete the next SpineItem in the chain, and will
 set its prior item's pointer-to-next to be `nullptr`.
 
 @ingroup epub-model
 */
class SpineItem
{
public:
    typedef std::vector<IRI>        PropertyList;
    
    static const IRI                PageSpreadRightPropertyIRI;
    static const IRI                PageSpreadLeftPropertyIRI;
    
private:
    ///
    /// No default constructor.
                        SpineItem()                                     _DELETED_;
    ///
    /// There is no copy constructor.
                        SpineItem(const SpineItem&)                     _DELETED_;

public:
    /**
     Constructs a new SpineItem.
     @param node The `<itemref>` element node for this spine item.
     @param owner The package contaning this spine item.
     */
                        SpineItem(xmlNodePtr node, Package * owner);
    ///
    /// C++11 move constructor.
                        SpineItem(SpineItem&&);
    
    // NB: deleting a spine item will delete its next, etc.
    // It will also reach back into _prev and nullify its _next
    virtual             ~SpineItem();
    
    /// @{
    /// @name Metadata
    
    ///
    /// Returns an O(n) count of items in the spine (starting with this item).
    inline size_t       Count()             const       { return (_next == nullptr ? 1 : 1 + _next->Count()); }
    ///
    /// Returns the index of the current item in the overall spine. O(n).
    inline size_t       Index()             const       { return (_prev == nullptr ? 0 : _prev->Index() + 1); }
    
    ///
    /// Returns this item's identifier (if any).
    const string&       Identifier()        const       { return _ident; }
    ///
    /// Returns the `idref` identifying the manifest item for this spine item.
    const string&       Idref()             const       { return _idref; }
    ///
    /// Obtains the manifest item corresponding to this spine item.
    const ManifestItem* ManifestItem()      const;
    ///
    /// Returns `true` if this item is linear, `false` otherwise.
    bool                Linear()            const       { return _linear; }
    ///
    /// Returns the properties of this item.
    const PropertyList& Properties()        const       { return _properties; }
    
    ///
    /// Page-spread identifiers.
    enum class PageSpread : uint8_t
    {
        Either,             ///< No page spread property was defined, or both were.
        Left,               ///< Item has the `page-spread-left` property.
        Right               ///< Item has the `page-spread-right` property.
    };
    
    ///
    /// Determine the spread location for this item (or for the first page thereof).
    PageSpread          Spread()            const;
    
    /// @}
    
    /// @{
    /// @name Navigation
    
    ///
    /// Retrieves a pointer to the spine item following this one, or `nullptr`.
    SpineItem*          Next()                          { return _next; }
    ///
    /// Retrieves a pointer to the spine item following this one, or `nullptr`.
    const SpineItem*    Next()              const       { return _next; }
    ///
    /// Retrieves a pointer to the spine item preceding this one, or `nullptr`.
    SpineItem*          Previous()                      { return _prev; }
    ///
    /// Retrieves a pointer to the spine item preceding this one, or `nullptr`.
    const SpineItem*    Previous()          const       { return _prev; }
    
    ///
    /// Retrieves a pointer to the next linear item in the spine, or `nullptr`.
    SpineItem*          NextStep();
    ///
    /// Retrieves a pointer to the next linear item in the spine, or `nullptr`.
    const SpineItem*    NextStep()          const;
    ///
    /// Retrieves a pointer to the previous linear item in the spine, or `nullptr`.
    SpineItem*          PriorStep();
    ///
    /// Retrieves a pointer to the previous linear item in the spine, or `nullptr`.
    const SpineItem*    PriorStep()         const;
    
    /// @}
    
    /// @{
    /// @name Indexed Accessors
    /**
     @note The index is always considered relative to `this`, and has the following
     invariants:
     
     - `item.at(0)`  = `&item`
     - `item.at(1)`  = `item.Next()`
     - `item.at(-1)` = `item.Previous()`
     */
    
    /**
     Retrieves the spine item at a relative index.
     @param idx The relative index of the item to retrieve.
     @result A SpineItem.
     @throws std::out_of_range if `idx` is out of bounds.
     */
    SpineItem*          at(ssize_t idx);
    
    /**
     Retrieves the spine item at a relative index.
     @param idx The relative index of the item to retrieve.
     @result A SpineItem.
     @throws std::out_of_range if `idx` is out of bounds.
     */
    const SpineItem*    at(ssize_t idx)         const;
    
    /**
     Retrieves the spine item at a relative index.
     @param idx The relative index of the item to retrieve.
     @result A SpineItem.
     @throws std::out_of_range if `idx` is out of bounds.
     */
    SpineItem*          operator[](ssize_t idx)                                   { return at(idx); }
    
    /**
     Retrieves the spine item at a relative index.
     @param idx The relative index of the item to retrieve.
     @result A SpineItem.
     @throws std::out_of_range if `idx` is out of bounds.
     */
    const SpineItem*    operator[](ssize_t idx) const                             { return at(idx); }
    
    /// @}
    
protected:
    string       _ident;            ///< The spine item's `id`, if it has one.
    string       _idref;            ///< The `idref` value targetting a ManifestItem.
    Package*     _owner;            ///< The Package containing this SpineItem.
    bool         _linear;           ///< `true` if the item is linear (the default).
    PropertyList _properties;       ///< A list of property IRIs.
    
    SpineItem* _prev;               ///< The SpineItem preceding this one in the spine.
    SpineItem* _next;               ///< The SpineItem following this one in the spine.
    
    friend class Package;
    void SetNextItem(SpineItem* next) {
        next->_next = _next;
        next->_prev = this;
        _next = next;
    }
};

EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__spine__) */
