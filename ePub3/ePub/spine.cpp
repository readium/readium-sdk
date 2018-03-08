//
//  spine.cpp
//  ePub3
//
//  Created by Jim Dovey on 2012-11-29.
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
bool SpineItem::ParseXML(shared_ptr<xml::Node> node)
{
    SetXMLIdentifier(_getProp(node, "id"));
    _idref = _getProp(node, "idref");
    if ( _getProp(node, "linear").tolower() == "no" )
        _linear = false;
    
    auto holder = CastPtr<PropertyHolder>();
    
    string properties = _getProp(node, "properties");
    if ( !properties.empty() )
    {
        for ( auto& property : properties.split(REGEX_NS::regex(",?\\s+")) )
        {
            PropertyPtr prop = std::make_shared<Property>(holder); //Property::New(holder);
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
    ForEachProperty([&](PropertyPtr item) {
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
    SpineItemPtr result = std::const_pointer_cast<SpineItem>(Ptr());
    
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
