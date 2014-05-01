//
//  IPropertyHolder.h
//  Readium
//
//  Created by Jim Dovey on 2013-09-26.
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
