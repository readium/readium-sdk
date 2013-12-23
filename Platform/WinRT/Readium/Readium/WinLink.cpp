//
//  WinLink.h
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

#include "WinLink.h"
#include "WinEpubCollection.h"
#include "WinManifest.h"

BEGIN_READIUM_API

_BRIDGE_API_IMPL_(::ePub3::LinkPtr, Link)

Collection^ Link::Owner::get()
{
	return Collection::Wrapper(_native->Owner());
}
String^ Link::Rel::get()
{
	return StringFromNative(_native->Rel());
}
String^ Link::Href::get()
{
	return StringFromNative(_native->Href());
}
String^ Link::MediaType::get()
{
	return StringFromNative(_native->MediaType());
}
ManifestItem^ Link::ReferencedManifestItem::get()
{
	return ManifestItem::Wrapper(std::const_pointer_cast<::ePub3::ManifestItem>(_native->ReferencedManifestItem()));
}

END_READIUM_API
