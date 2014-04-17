//
//  WinLink.h
//  Readium
//
//  Created by Jim Dovey on 2013-11-29.
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//  
//  This program is distributed in the hope that it will be useful, but WITHOUT ANY 
//  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
//  
//  Licensed under Gnu Affero General Public License Version 3 (provided, notwithstanding this notice, 
//  Readium Foundation reserves the right to license this material under a different separate license, 
//  and if you have done so, the terms of that separate license control and the following references 
//  to GPL do not apply).
//  
//  This program is free software: you can redistribute it and/or modify it under the terms of the GNU 
//  Affero General Public License as published by the Free Software Foundation, either version 3 of 
//  the License, or (at your option) any later version. You should have received a copy of the GNU 
//  Affero General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.

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
