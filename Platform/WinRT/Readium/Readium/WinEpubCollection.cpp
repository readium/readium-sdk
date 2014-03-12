//
//  WinEpubCollection.cpp
//  Readium
//
//  Created by Jim Dovey on 2013-11-29.
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
