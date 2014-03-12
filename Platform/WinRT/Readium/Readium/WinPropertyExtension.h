//
//  WinPropertyExtension.h
//  Readium
//
//  Created by Jim Dovey on 2013-10-11.
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

#ifndef __Readium_PropertyExtension_h__
#define __Readium_PropertyExtension_h__

#include "Readium.h"
#include <ePub3/property_extension.h>

using ::Platform::String;
using ::Windows::Foundation::Uri;

BEGIN_READIUM_API

ref class Property;

public ref class PropertyExtension sealed
{
	_DECLARE_BRIDGE_API_(::ePub3::PropertyExtensionPtr, PropertyExtension^);

internal:
	PropertyExtension(::ePub3::PropertyExtensionPtr native);

public:
	PropertyExtension(Property^ owner);
	virtual ~PropertyExtension() {}

	property Uri^ PropertyIdentifier { Uri^ get(); void set(Uri^); }
	property String^ Scheme { String^ get(); void set(String^); }
	property String^ Value { String^ get(); void set(String^); }
	property String^ Language { String^ get(); void set(String^); }

};

END_READIUM_API

#endif	/* __Readium_PropertyExtension_h__ */
