//
//  WinProperty.h
//  Readium
//
//  Created by Jim Dovey on 2013-09-26.
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
