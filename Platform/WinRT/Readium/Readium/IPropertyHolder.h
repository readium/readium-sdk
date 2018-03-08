//
//  IPropertyHolder.h
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

#ifndef __Readium_IPropertyHolder_h__
#define __Readium_IPropertyHolder_h__

#include "Readium.h"
#include "PropertyTypes.h"
#include "WinProperty.h"

BEGIN_READIUM_API

using ::Platform::String;
using ::Windows::Foundation::Uri;
using ::Windows::Foundation::Collections::IVectorView;
using ::Windows::Foundation::Collections::IIterable;

public interface class IPropertyHolder : IIterable<Property^>
{
	property unsigned int Count { unsigned int get(); }

	[::Windows::Foundation::Metadata::DefaultOverload]
	void Append(Property^ prop);
	void Append(IPropertyHolder^ allProps);

	[::Windows::Foundation::Metadata::DefaultOverload]
	void Remove(Uri^ propertyIRI);
	void Remove(String^ reference);
	void Remove(String^ prefix, String^ reference);

	Property^ At(unsigned int idx);
	void EraseAt(unsigned int idx);

	bool Contains(DCType type);
	[::Windows::Foundation::Metadata::DefaultOverload]
	bool Contains(Uri^ propertyIRI);
	bool Contains(String^ reference);
	bool Contains(String^ prefix, String^ reference);

	Property^ PropertyMatching(DCType type);
	[::Windows::Foundation::Metadata::DefaultOverload]
	Property^ PropertyMatching(Uri^ propertyIRI);
	Property^ PropertyMatching(String^ reference);
	Property^ PropertyMatching(String^ prefix, String^ reference);

	IVectorView<Property^>^ PropertiesMatching(DCType type);
	[::Windows::Foundation::Metadata::DefaultOverload]
	IVectorView<Property^>^ PropertiesMatching(Uri^ propertyIRI);
	IVectorView<Property^>^ PropertiesMatching(String^ reference);
	IVectorView<Property^>^ PropertiesMatching(String^ prefix, String^ reference);

	void RegisterPrefixIRIStem(String^ prefix, String^ iriStem);
	
	Uri^ MakePropertyIRI(String^ reference);
	Uri^ MakePropertyIRI(String^ prefix, String^ reference);
	Uri^ PropertyIRIFromString(String^ str);

};

END_READIUM_API

#endif	/* __Readium_IPropertyHolder_h__ */
