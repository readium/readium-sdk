//
//  Readium.h
//  Readium-WinRT
//
//  Created by Jim Dovey on 2013-09-25.
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
	std::remove_reference<_Native>::type _native; \
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