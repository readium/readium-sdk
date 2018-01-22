//
//  NativeBridge.h
//  ePub3
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

