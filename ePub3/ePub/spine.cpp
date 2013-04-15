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
#include REGEX_INCLUDE

EPUB3_BEGIN_NAMESPACE

const IRI SpineItem::PageSpreadRightPropertyIRI("http://idpf.org/epub/vocab/package/#page-spread-right");
const IRI SpineItem::PageSpreadLeftPropertyIRI("http://idpf.org/epub/vocab/package/#page-spread-left");

SpineItem::SpineItem(xmlNodePtr node, Package * owner) : _ident(), _idref(), _owner(owner), _linear(true), _next(nullptr), _prev(nullptr)
{
    _prev = nullptr;
    _next = nullptr;
    _ident = _getProp(node, "id");
    _idref = _getProp(node, "idref");
    if ( _getProp(node, "linear").tolower() == U"false" )
        _linear = false;
    
    string properties = _getProp(node, "properties");
    if ( !properties.empty() )
    {
        for ( auto& property : properties.split(REGEX_NS::regex(",\\s*")) )
        {
            _properties.push_back(owner->PropertyIRIFromAttributeValue(property));
        }
    }
}
SpineItem::SpineItem(SpineItem&& o) : _ident(std::move(o._ident)), _idref(std::move(o._idref)), _owner(o._owner), _linear(o._linear), _properties(std::move(o._properties)), _prev(o._prev), _next(std::move(o._next))
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
SpineItem::PageSpread SpineItem::Spread() const
{
    if ( _properties.empty() )
        return PageSpread::Either;
    
    bool left = false, right = false;
    for ( auto& item : _properties )
    {
        if ( !left && item == PageSpreadLeftPropertyIRI )
            left = true;
        else if ( !right && item == PageSpreadRightPropertyIRI )
            right = true;
        
        // return early if both set
        if ( left && right )
            return PageSpread::Either;
    }
    
    // only one (or neither) set here
    if ( left )
        return PageSpread::Left;
    if ( right )
        return PageSpread::Right;
    
    return PageSpread::Either;
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
