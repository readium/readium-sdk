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

#pragma once

#include "Readium.h"
#include <ePub3/link.h>

BEGIN_READIUM_API

ref class ManifestItem;
ref class Collection;

public ref class Link sealed
{
	_DECLARE_BRIDGE_API_(::EPUB3_NAMESPACE::LinkPtr, Link^);

internal:
	Link(::EPUB3_NAMESPACE::LinkPtr native) : _native(native) {}

public:
	virtual ~Link() {}

	property Collection^ Owner { Collection^ get(); }

	property ::Platform::String^ Rel { ::Platform::String^ get(); }
	property ::Platform::String^ Href { ::Platform::String^ get(); }
	property ::Platform::String^ MediaType { ::Platform::String^ get(); }

	property ManifestItem^ ReferencedManifestItem { ManifestItem^ get(); }
};

END_READIUM_API
