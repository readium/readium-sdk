//
//  PropertyHolderImpl.h
//  Readium
//
//  Created by Jim Dovey on 2013-09-27.
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

#ifndef __Readium_PropertyHolderImpl_h__
#define __Readium_PropertyHolderImpl_h__

#include "IPropertyHolder.h"
#include <ePub3/property_holder.h>

BEGIN_READIUM_API

#define _PROPERTY_HOLDER_NATIVE_IMPL() \
	protected private: \
	property ::ePub3::PropertyHolderPtr Native { \
		virtual ::ePub3::PropertyHolderPtr get() override { \
			return std::dynamic_pointer_cast<::ePub3::PropertyHolder>(NativeObject); \
		} \
	}

[::Windows::Foundation::Metadata::WebHostHidden]
public ref class PropertyHolder : ::Windows::UI::Xaml::DependencyObject, IPropertyHolder
{
internal:
	PropertyHolder() {}

protected private:
	property ::ePub3::PropertyHolderPtr Native { virtual ::ePub3::PropertyHolderPtr get() { return nullptr; } }

public:
	property unsigned int Count { virtual unsigned int get(); }

	virtual void Append(Property^ prop);
	virtual void Append(IPropertyHolder^ allProps);

	virtual void Remove(Uri^ propertyIRI);
	virtual void Remove(String^ reference);
	virtual void Remove(String^ prefix, String^ reference);

	virtual Property^ At(unsigned int idx);
	virtual void EraseAt(unsigned int idx);

	virtual bool Contains(DCType type);
	virtual bool Contains(Uri^ propertyIRI);
	virtual bool Contains(String^ reference);
	virtual bool Contains(String^ prefix, String^ reference);

	virtual Property^ PropertyMatching(DCType type);
	virtual Property^ PropertyMatching(Uri^ propertyIRI);
	virtual Property^ PropertyMatching(String^ reference);
	virtual Property^ PropertyMatching(String^ prefix, String^ reference);

	virtual IVectorView<Property^>^ PropertiesMatching(DCType type);
	virtual IVectorView<Property^>^ PropertiesMatching(Uri^ propertyIRI);
	virtual IVectorView<Property^>^ PropertiesMatching(String^ reference);
	virtual IVectorView<Property^>^ PropertiesMatching(String^ prefix, String^ reference);

	virtual void RegisterPrefixIRIStem(String^ prefix, String^ iriStem);

	virtual Uri^ MakePropertyIRI(String^ reference);
	virtual Uri^ MakePropertyIRI(String^ prefix, String^ reference);
	virtual Uri^ PropertyIRIFromString(String^ str);

	virtual IIterator<Property^>^ First();

};

END_READIUM_API

#endif	/* __Readium_PropertyHolderImpl_h__ */
