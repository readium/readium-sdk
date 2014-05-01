//
//  NativeBridge.h
//  ePub3
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

#ifndef __ePub3_NativeBridge_h__
#define __ePub3_NativeBridge_h__

#include <ePub3/base.h>

#if EPUB_PLATFORM(WINRT)
#include <memory>
#include <ePub3/utilities/utfstring.h>

#include <wrl.h>
#include <wrl/client.h>
#include <wrl/implements.h>

using namespace ::Microsoft::WRL;

EPUB3_BEGIN_NAMESPACE

class NativeBridge
{
private:
	mutable WeakRef			_weakRef;

public:
	NativeBridge() : _weakRef(nullptr) {}
	NativeBridge(const NativeBridge&) : _weakRef(nullptr) {}
	NativeBridge(NativeBridge&& o) : _weakRef(std::move(o._weakRef)) {}
	virtual ~NativeBridge() {}

	NativeBridge& operator=(const NativeBridge&) = default;
	NativeBridge& operator=(NativeBridge&& o) { _weakRef = std::move(o._weakRef); return *this; }

	template <typename _Tp>
	_Tp^ GetBridge() const {
		ComPtr<IInspectable> raw(nullptr);
		try {
			_weakRef.As(&raw);
		}
		catch (...) {
			return nullptr;
		}
		if (raw == nullptr)
			return nullptr;
		return reinterpret_cast<_Tp^>(raw.Get());
	}

	template <typename _Tp>
	void SetBridge(_Tp^ obj) {
		AsWeak(reinterpret_cast<IInspectable*>(obj), &_weakRef);
	}

};

EPUB3_END_NAMESPACE

#endif

#endif	/* __ePub3_NativeBridge_h__ */

