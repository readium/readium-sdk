//
//  Readium.h
//  Readium-WinRT
//
//  Created by Jim Dovey on 2013-09-25.
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

#ifndef __Readium_h__
#define __Readium_h__

#include <ePub3/utilities/NativeBridge.h>
#include <ePub3/utilities/iri.h>

#define BEGIN_READIUM_API	namespace Readium {
#define END_READIUM_API		};

BEGIN_READIUM_API

#define DeclareFastPassString(native, var) \
	std::wstring __ ## var = native.wchar_string(); \
	::Platform::StringReference var(__##var.data(), __##var.length())

static inline
::Platform::String^ StringFromNative(const ::ePub3::string& native)
{
	std::wstring wstr(native.wchar_string());
	return ref new ::Platform::String(wstr.data(), (unsigned int)wstr.length());
}

static inline
::Platform::String^ StringFromCString(const char* cstr)
{
	typedef std::wstring_convert<std::codecvt_utf8<wchar_t>> _Convert;
	std::wstring wstr(_Convert().from_bytes(cstr));
	return ref new ::Platform::String(wstr.data(), (unsigned int)wstr.length());
}

static inline
::ePub3::string StringToNative(::Platform::String^ str)
{
	return ::ePub3::string(str->Data(), str->Length());
}

static inline ::Windows::Foundation::Uri^ IRIToURI(const ::ePub3::IRI& iri)
{
	::ePub3::string nstr = iri.IRIString();
	if (nstr.empty())
		nstr = iri.URIString();

	DeclareFastPassString(nstr, uristr);
	return ref new ::Windows::Foundation::Uri(uristr);
}

static inline ::ePub3::IRI URIToIRI(::Windows::Foundation::Uri^ uri)
{
	return ::ePub3::IRI(StringToNative(uri->AbsoluteCanonicalUri));
}

#define _DECLARE_BRIDGE_API_(_Native, _Wrapped) \
	private: \
	std::remove_reference_t<_Native> _native; \
	internal: \
	property _Native NativeObject { _Native get() { return _native; } } \
	static _Wrapped Wrapper(_Native native)

#define _BRIDGE_API_IMPL_(_Native, _Wrapped) \
	_Wrapped^ _Wrapped::Wrapper(_Native native) \
	{ \
		if (!bool(native)) \
			return nullptr; \
		_Wrapped^ result = native->GetBridge<_Wrapped>(); \
		if (result == nullptr) \
			result = ref new _Wrapped(native); \
		return result; \
	}

END_READIUM_API

#endif	/* __Readium_h__ */
