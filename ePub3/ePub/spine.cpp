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

SpineItem::SpineItem(const shared_ptr<Package>& owner) : OwnedBy(owner), PropertyHolder(owner), _idref(), _linear(true), _next(), _prev()
{
}
SpineItem::SpineItem(SpineItem&& o) : OwnedBy(std::move(o)), PropertyHolder(std::move(o)), XMLIdentifiable(std::move(o)), _idref(std::move(o._idref)), _linear(o._linear), _prev(std::move(o._prev)), _next(std::move(o._next))
{
}
SpineItem::~SpineItem()
{
}
bool SpineItem::ParseXML(SpineItemPtr& sharedMe, xmlNodePtr node)
{
    SetXMLIdentifier(_getProp(node, "id"));
    _idref = _getProp(node, "idref");
    if ( _getProp(node, "linear").tolower() == "false" )
        _linear = false;
    
    auto holder = std::dynamic_pointer_cast<PropertyHolder>(sharedMe);
    
    string properties = _getProp(node, "properties");
    if ( !properties.empty() )
    {
        for ( auto& property : properties.split(REGEX_NS::regex(",?\\s+")) )
        {
            PropertyPtr prop = std::make_shared<Property>(holder);
            prop->SetPropertyIdentifier(this->PropertyIRIFromString(property));
            this->AddProperty(prop);
        }
    }
    return true;
}
shared_ptr<ManifestItem> SpineItem::ManifestItem() const
{
    auto package = this->Owner();
    if ( !package )
        return nullptr;
    return package->ManifestItemWithID(Idref());
}
PageSpread SpineItem::Spread() const
{
    if ( NumberOfProperties() == 0 )
        return PageSpread::Automatic;
    
    bool left = false, right = false;
    ForEachProperty([&](shared_ptr<Property> item) {
        // return early if both set
        if ( left && right )
            return;
        
        if ( !left && item->PropertyIdentifier() == PageSpreadLeftPropertyIRI )
            left = true;
        else if ( !right && item->PropertyIdentifier() == PageSpreadRightPropertyIRI )
            right = true;
    });
    
    // only one (or neither) set here
    if ( left )
        return PageSpread::Left;
    if ( right )
        return PageSpread::Right;
    
    return PageSpread::Automatic;
}
shared_ptr<SpineItem> SpineItem::NextStep() const
{
    auto n = Next();
    while ( n != nullptr && n->Linear() == false )
        n = n->Next();
    return n;
}
shared_ptr<SpineItem> SpineItem::PriorStep() const
{
    auto p = Previous();
    while ( p != nullptr && p->Linear() == false )
        p = p->Previous();
    return p;
}
shared_ptr<SpineItem> SpineItem::at(ssize_t idx) const
{
    shared_ptr<SpineItem> result = std::const_pointer_cast<SpineItem>(enable_shared_from_this<SpineItem>::shared_from_this());
    
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
void SpineItem::SetNextItem(const shared_ptr<SpineItem>& next)
{
    next->_next = _next;
    next->_prev = enable_shared_from_this<SpineItem>::shared_from_this();
    _next = next;
}

EPUB3_END_NAMESPACE
