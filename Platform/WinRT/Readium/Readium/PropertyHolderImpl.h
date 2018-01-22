//
//  PropertyHolderImpl.h
//  Readium
//
//  Created by Jim Dovey on 2013-09-27.
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
