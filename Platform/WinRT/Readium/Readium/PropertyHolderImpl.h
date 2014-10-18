//
//  PropertyHolderImpl.h
//  Readium
//
//  Created by Jim Dovey on 2013-09-27.
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

#ifndef __Readium_PropertyHolderImpl_h__
#define __Readium_PropertyHolderImpl_h__

#include "IPropertyHolder.h"
#include <ePub3/property_holder.h>
#include "CollectionBridges.h"

using ::Windows::Foundation::Collections::IIterator;

BEGIN_READIUM_API

#define _PROPERTY_HOLDER_NATIVE_IMPL() \
	internal: \
	property ::ePub3::PropertyHolderPtr Native { \
		virtual ::ePub3::PropertyHolderPtr get() override { \
			return std::dynamic_pointer_cast<::ePub3::PropertyHolder>(NativeObject); \
		} \
	}

typedef BridgedObjectVectorView<Property^, ::ePub3::PropertyPtr>	PropertyVectorView;

ref class PropertyIteratorImpl : public IIterator<Property^>
{
private:
	IPropertyHolder^	_holder;
	unsigned int		_idx;

internal:
	PropertyIteratorImpl(IPropertyHolder^ holder) : _holder(holder), _idx(0) {}

public:
	virtual ~PropertyIteratorImpl() {}

	// IIterator

	virtual unsigned int GetMany(::Platform::WriteOnlyArray<Property^>^ items) {
		if (HasCurrent) {
			unsigned int max = items->Length;
			unsigned int i = 0;
			do {
				items[i++] = Current;
			} while (i < max && MoveNext());
			return i;
		}
		return 0;
	}
	virtual bool MoveNext() {
		if (!HasCurrent)
			return false;
		return (++_idx < _holder->Count);
	}

	property Property^ Current
	{
		virtual Property^ get() {
			if (!HasCurrent)
				return nullptr;
			return _holder->At(_idx);
		}
	}
	property bool HasCurrent
	{
		virtual bool get() {
			return _idx < _holder->Count;
		}
	}
};

END_READIUM_API

#endif	/* __Readium_PropertyHolderImpl_h__ */
