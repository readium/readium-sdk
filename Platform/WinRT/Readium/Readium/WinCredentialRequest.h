//
//  WinCredentialRequest.h
//  Readium
//
//  Created by Jim Dovey on 2013-11-01.
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

#pragma once

#include "Readium.h"
#include <ePub3/credential_request.h>

BEGIN_READIUM_API

ref class CredentialRequest;

typedef ::Windows::Foundation::Collections::IMapView<::Platform::String^, ::Platform::String^> Credentials;
typedef ::Platform::String^ CredentialType;

public enum class CredentialRequestFieldType
{
	Message			= int(::ePub3::CredentialRequest::Type::Message),
	TextInput		= int(::ePub3::CredentialRequest::Type::TextInput),
	MaskedInput		= int(::ePub3::CredentialRequest::Type::MaskedInput),
	Button			= int(::ePub3::CredentialRequest::Type::Button)
};

public delegate void CredentialRequestButtonHandler(CredentialRequest^, size_t);

public ref class CredentialRequest sealed
{
private:
	::ePub3::CredentialRequest _native;

internal:
	property ::ePub3::CredentialRequest& NativeObject { ::ePub3::CredentialRequest& get() { return _native; } }

public:
	CredentialRequest(::Platform::String^ title, ::Platform::String^ message);
	virtual ~CredentialRequest();

	property ::Platform::String^ Title { ::Platform::String^ get(); }
	property ::Platform::String^ Message { ::Platform::String^ get(); }
	property Credentials^ Credentials { Readium::Credentials^ get(); }

	size_t AddCredential(::Platform::String^ title, bool secret, ::Platform::String^ defaultValue);
	size_t AddButton(::Platform::String^ title, CredentialRequestButtonHandler^ handler);

	CredentialRequestFieldType GetItemType(size_t idx);
	::Platform::String^ GetItemTitle(size_t idx);

	void SignalCompletion();
	//void SignalException(::Platform::COMException^ except);

};

END_READIUM_API