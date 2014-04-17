//
//  WinEpubCollection.h
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
#include "PropertyHolderImpl.h"
#include <ePub3/epub_collection.h>

BEGIN_READIUM_API

ref class Link;
ref class Package;
ref class Property;
ref class PropertyExtension;

public ref class Collection sealed : public IPropertyHolder
{
	_DECLARE_BRIDGE_API_(::ePub3::CollectionPtr, Collection^);

internal:
	Collection(::ePub3::CollectionPtr native) : _native(native) {}

public:
	virtual ~Collection() {}

	static ::Platform::String^ IndexRole() { return TEXT("index"); }
	static ::Platform::String^ManifestRole() { return TEXT("manifest"); }
	static ::Platform::String^PreviewRole() { return TEXT("preview"); }

	property Package^ Owner { Package^ get(); }

	property ::Platform::String^ Role { ::Platform::String^ get(); }
	
	property UINT ChildCollectionCount { UINT get(); }
	Collection^ ChildCollectionWithRole(::Platform::String^ role);

	property UINT LinkCount { UINT get(); }
	Link^ LinkAt(UINT idx);

#include "PropertyHolderSubclassDecl.h"

};

END_READIUM_API
