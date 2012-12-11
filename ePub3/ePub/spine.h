//
//  spine.h
//  ePub3
//
//  Created by Jim Dovey on 2012-11-29.
//  Copyright (c) 2012 Kobo Inc. All rights reserved.
//

#ifndef __ePub3__spine__
#define __ePub3__spine__

#include "epub3.h"
#include <vector>
#include <string>
#include <libxml/tree.h>

EPUB3_BEGIN_NAMESPACE

class SpineItem;
class ManifestItem;
class Package;

// SpineItems form a doubly-linked list
class SpineItem
{
public:
    SpineItem() = delete;
    SpineItem(xmlNodePtr node, Package * owner);
    SpineItem(const SpineItem&) = delete;
    SpineItem(SpineItem&&);
    
    // NB: deleting a spine item will delete its next, etc.
    // It will also reach back into _prev and nullify its _next
    virtual ~SpineItem();
    
    const std::string& Idref() const { return _idref; }
    const class ManifestItem* ManifestItem() const;
    bool Linear() const { return _linear; }
    
    SpineItem* Next() { return _next; }
    const SpineItem* Next() const { return _next; }
    SpineItem* Previous() { return _prev; }
    const SpineItem* Previous() const { return _prev; }
    
    SpineItem* NextStep();
    const SpineItem* NextStep() const;
    SpineItem* PriorStep();
    const SpineItem* PriorStep() const;
    
protected:
    std::string _idref;
    Package *   _owner;
    bool        _linear;
    
    SpineItem*  _prev;
    SpineItem*  _next;
    
    friend class Package;
    void SetNextItem(SpineItem* next) { _next = next; _next->_prev = this; }
};

EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__spine__) */
