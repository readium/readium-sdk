//
//  WinProperty.h
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

#ifndef __Readium_Property_h__
#define __Readium_Property_h__

#include "Readium.h"
#include "PropertyTypes.h"
#include "WinPropertyExtension.h"
#include <ePub3/property.h>

BEGIN_READIUM_API

using ::Platform::String;
using ::Windows::Foundation::Uri;
using ::Windows::Foundation::Collections::IVectorView;

interface class IPropertyHolder;

public ref class Property sealed
{
	_DECLARE_BRIDGE_API_(::ePub3::PropertyPtr, Property^);

internal:
	Property(::ePub3::PropertyPtr native);

public:
	Property(IPropertyHolder^ holder);
	Property(IPropertyHolder^ holder, Uri^ identifier, String^ value);
	virtual ~Property() {}

	static Uri^ IRIForDCType(Readium::DCType type);
	static DCType DCTypeFromIRI(Uri^ iri);

	property DCType Type { DCType get(); void set(DCType); }
	property Uri^ PropertyIdentifier { Uri^ get(); void set(Uri^); }
	property String^ Value { String^ get(); void set(String^); }
	property String^ Language { String^ get(); void set(String^); }

	property String^ LocalizedValue { String^ get(); }

	property IVectorView<PropertyExtension^>^ Extensions { IVectorView<PropertyExtension^>^ get(); }

	PropertyExtension^ ExtensionWithIdentifier(Uri^ identifier);
	IVectorView<PropertyExtension^>^ AllExtensionsWithIdentifier(Uri^ identifier);

	void AddExtension(PropertyExtension^ extension);
	bool HasExtensionWithIdentifier(Uri^ identifier);

};

END_READIUM_API

#endif	/* __Readium_Property_h__ */