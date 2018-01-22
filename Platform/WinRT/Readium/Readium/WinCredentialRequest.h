//
//  WinCredentialRequest.h
//  Readium
//
//  Created by Jim Dovey on 2013-11-01.
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

#pragma once

#include "Readium.h"
#include <ePub3/credential_request.h>

BEGIN_READIUM_API

ref class CredentialRequest;

using Credentials = ::Windows::Foundation::Collections::IMapView<::Platform::String^, ::Platform::String^>;
using CredentialType = ::Platform::String^;

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
	void SignalException(::Platform::Exception^ except);

};

END_READIUM_API
