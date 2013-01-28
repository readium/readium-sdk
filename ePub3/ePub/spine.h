//
//  spine.h
//  ePub3
//
//  Created by Jim Dovey on 2012-11-29.
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

#ifndef __ePub3__spine__
#define __ePub3__spine__

#include "epub3.h"
#include "utfstring.h"
#include <vector>
#include <libxml/tree.h>

EPUB3_BEGIN_NAMESPACE

class ManifestItem;
class Package;
class SpineItem;

// SpineItems form a doubly-linked list
class SpineItem
{
public:
                        SpineItem()                                     = delete;
                        SpineItem(xmlNodePtr node, Package * owner);
                        SpineItem(const SpineItem&)                     = delete;
                        SpineItem(SpineItem&&);
    
    // NB: deleting a spine item will delete its next, etc.
    // It will also reach back into _prev and nullify its _next
    virtual             ~SpineItem();
    
    // O(n) count of items
    // Would be nice if the compiler could unroll it...
    inline size_t       Count()             const       { return (_next == nullptr ? 1 : 1 + _next->Count()); }
    inline size_t       Index()             const       { return (_prev == nullptr ? 0 : _prev->Index() + 1); }
    
    const string&       Idref()             const       { return _idref; }
    const ManifestItem* ManifestItem()      const;
    bool                Linear()            const       { return _linear; }
    
    // these are direct
    SpineItem*          Next()                          { return _next; }
    const SpineItem*    Next()              const       { return _next; }
    SpineItem*          Previous()                      { return _prev; }
    const SpineItem*    Previous()          const       { return _prev; }
    
    // these will skip past non-linear spine items
    SpineItem*          NextStep();
    const SpineItem*    NextStep()          const;
    SpineItem*          PriorStep();
    const SpineItem*    PriorStep()         const;
    
    // index is relative to receiving item
    // invariants:
    //   item.at(0)  == &item
    //   item.at(1)  == item._next
    //   item.at(-1) == item._prev
    SpineItem*          at(ssize_t idx)                 throw (std::out_of_range);
    const SpineItem*    at(ssize_t idx)         const   throw (std::out_of_range);
    
    SpineItem*          operator[](ssize_t idx)         throw (std::out_of_range) { return at(idx); }
    const SpineItem*    operator[](ssize_t idx) const   throw (std::out_of_range) { return at(idx); }
    
protected:
    string      _idref;
    Package*    _owner;
    bool        _linear;
    
    SpineItem* _prev;
    SpineItem* _next;
    
    friend class Package;
    void SetNextItem(SpineItem* next) {
        next->_next = _next;
        next->_prev = this;
        _next = next;
    }
};

EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__spine__) */
