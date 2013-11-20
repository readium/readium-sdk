//
//  WinSpine.h
//  Readium
//
//  Created by Jim Dovey on 2013-09-26.
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

#ifndef __Readium_Spine_h__
#define __Readium_Spine_h__

#include "Readium.h"
#include "PropertyHolderImpl.h"
#include <ePub3/spine.h>

BEGIN_READIUM_API

ref class Package;
ref class ManifestItem;

public ref class SpineItem sealed : IPropertyHolder
{
	_DECLARE_BRIDGE_API_(::ePub3::SpineItemPtr, SpineItem^);

internal:
	SpineItem(::ePub3::SpineItemPtr native);

public:
	virtual ~SpineItem() {}

	static property ::Windows::Foundation::Uri^ PageSpreadRightPropertyIRI { ::Windows::Foundation::Uri^ get(); }
	static property ::Windows::Foundation::Uri^ PageSpreadLeftPropertyIRI  { ::Windows::Foundation::Uri^ get(); }

	property Package^ ParentPackage { Package^ get(); }

	property UINT ItemCount { UINT get(); }
	property UINT Index { UINT get(); }

	property String^ Identifier { String^ get(); }
	property String^ Idref { String^ get(); }

	property ManifestItem^ ManifestItemRef { ManifestItem^ get(); }
	property bool Linear { bool get(); }

	property PageSpread Spread { PageSpread get(); }

	property ::Platform::String^ Title { ::Platform::String^ get(); void set(::Platform::String^); }

	property SpineItem^ Next { SpineItem^ get(); }
	property SpineItem^ Previous { SpineItem^ get(); }
	property SpineItem^ NextStep { SpineItem^ get(); }
	property SpineItem^ PriorStep { SpineItem^ get(); }

	SpineItem^ SpineItemAt(UINT idx);

#include "PropertyHolderSubclassDecl.h"

};

END_READIUM_API

#endif	/* __Readium_Spine_h__ */