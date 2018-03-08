//
//  WinSpine.cpp
//  Readium
//
//  Created by Jim Dovey on 2013-10-10.
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

#include "WinSpine.h"
#include "WinManifest.h"
#include "WinPackage.h"

using namespace ::Windows::Foundation;

BEGIN_READIUM_API

_BRIDGE_API_IMPL_(::ePub3::SpineItemPtr, SpineItem)

SpineItem::SpineItem(::ePub3::SpineItemPtr native) : _native(native)
{
	_native->SetBridge(this);
}

Uri^ SpineItem::PageSpreadRightPropertyIRI::get()
{
	return IRIToURI(::ePub3::SpineItem::PageSpreadRightPropertyIRI);
}
Uri^ SpineItem::PageSpreadLeftPropertyIRI::get()
{
	return IRIToURI(::ePub3::SpineItem::PageSpreadLeftPropertyIRI);
}

Package^ SpineItem::ParentPackage::get()
{
	return Package::Wrapper(_native->GetPackage());
}

UINT SpineItem::ItemCount::get()
{
	return _native->Count();
}
UINT SpineItem::Index::get()
{
	return _native->Index();
}

String^ SpineItem::Identifier::get()
{
	return StringFromNative(_native->Identifier());
}
String^ SpineItem::Idref::get()
{
	return StringFromNative(_native->Idref());
}

ManifestItem^ SpineItem::ManifestItemRef::get()
{
	return ManifestItem::Wrapper(_native->ManifestItem());
}
bool SpineItem::Linear::get()
{
	return _native->Linear();
}

PageSpread SpineItem::Spread::get()
{
	return PageSpread(_native->Spread());
}

String^ SpineItem::Title::get()
{
	return StringFromNative(_native->Title());
}
void SpineItem::Title::set(String^ title)
{
	_native->SetTitle(StringToNative(title));
}

SpineItem^ SpineItem::Next::get()
{
	return Wrapper(_native->Next());
}
SpineItem^ SpineItem::Previous::get()
{
	return Wrapper(_native->Previous());
}
SpineItem^ SpineItem::NextStep::get()
{
	return Wrapper(_native->NextStep());
}
SpineItem^ SpineItem::PriorStep::get()
{
	return Wrapper(_native->PriorStep());
}

SpineItem^ SpineItem::SpineItemAt(UINT idx)
{
	return Wrapper(_native->at(idx));
}

#define PropertyHolder SpineItem
#include "PropertyHolderSubclassImpl.h"
#undef PropertyHolder

END_READIUM_API
