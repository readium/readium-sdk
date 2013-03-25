//
//  spine.cpp
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

#include "spine.h"
#include "package.h"

EPUB3_BEGIN_NAMESPACE

SpineItem::SpineItem(xmlNodePtr node, Package * owner) : _idref(), _owner(owner), _linear(true), _next(nullptr), _prev(nullptr)
{
    _prev = nullptr;
    _next = nullptr;
    _idref = _getProp(node, "idref");
    if ( _getProp(node, "linear").tolower() == U"false" )
        _linear = false;
}
SpineItem::SpineItem(SpineItem&& o) : _idref(std::move(o._idref)), _owner(o._owner), _linear(o._linear), _prev(o._prev), _next(std::move(o._next))
{
    o._owner = nullptr;
    o._prev = nullptr;
}
SpineItem::~SpineItem()
{
}
const ManifestItem* SpineItem::ManifestItem() const
{
    return _owner->ManifestItemWithID(Idref());
}
SpineItem* SpineItem::NextStep()
{
    SpineItem* n = Next();
    while ( n != nullptr && n->Linear() == false )
        n = n->Next();
    return n;
}
const SpineItem* SpineItem::NextStep() const
{
    return const_cast<SpineItem*>(this)->NextStep();
}
SpineItem* SpineItem::PriorStep()
{
    SpineItem* p = Previous();
    while ( p != nullptr && p->Linear() == false )
        p = p->Previous();
    return p;
}
const SpineItem* SpineItem::PriorStep() const
{
    return const_cast<SpineItem*>(this)->PriorStep();
}
SpineItem* SpineItem::at(ssize_t idx) throw (std::out_of_range)
{
    SpineItem* result(this);
    ssize_t i = idx;
    
    if ( i > 0 )
    {
        while ( result != nullptr && i > 0 )
        {
            result = result->Next();
            i--;
        }
    }
    else if ( idx < 0 )
    {
        while ( result != nullptr && i < 0 )
        {
            result = result->Previous();
            i++;
        }
    }
    
    // Q: maybe just return nullptr?
    if ( result == nullptr )
        throw std::out_of_range(_Str("Index ", i, " is out of range"));
    
    return result;
}
const SpineItem* SpineItem::at(ssize_t idx) const throw (std::out_of_range)
{
    return const_cast<SpineItem*>(this)->at(idx);
}

EPUB3_END_NAMESPACE
