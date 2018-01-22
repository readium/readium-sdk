//
//  WinEpubCollection.cpp
//  Readium
//
//  Created by Jim Dovey on 2013-11-29.
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

#include "WinEpubCollection.h"
#include "WinPackage.h"
#include "WinLink.h"
#include "WinProperty.h"
#include "WinPropertyExtension.h"
#include "WinManifest.h"

BEGIN_READIUM_API

using ::Platform::String;

_BRIDGE_API_IMPL_(::ePub3::CollectionPtr, Collection)

Package^ Collection::Owner::get()
{
	return Package::Wrapper(_native->Owner());
}
String^ Collection::Role::get()
{
	return StringFromNative(_native->Role());
}
UINT Collection::ChildCollectionCount::get()
{
	return static_cast<UINT>(_native->ChildCollectionCount());
}
Collection^ Collection::ChildCollectionWithRole(String^ role)
{
	return Wrapper(_native->ChildCollectionWithRole(StringToNative(role)));
}
UINT Collection::LinkCount::get()
{
	return static_cast<UINT>(_native->LinkCount());
}
Link^ Collection::LinkAt(UINT idx)
{
	return Link::Wrapper(_native->LinkAt(static_cast<std::size_t>(idx)));
}

#define PropertyHolder Collection
#include "PropertyHolderSubclassImpl.h"
#undef PropertyHolder

END_READIUM_API
