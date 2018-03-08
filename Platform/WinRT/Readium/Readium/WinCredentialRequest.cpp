//
//  WinCredentialRequest.cpp
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

#include "WinCredentialRequest.h"
#include "ExceptionWrapper.h"
#include "CollectionBridges.h"

using namespace ::Platform;

BEGIN_READIUM_API

typedef BridgedStringToStringMapView	Credentials_Type;

CredentialRequest::CredentialRequest(String^ title, String^ message)
	: _native(StringToNative(title), StringToNative(message))
{
}
CredentialRequest::~CredentialRequest()
{
}

String^ CredentialRequest::Title::get()
{
	return StringFromNative(_native.GetTitle());
}
String^ CredentialRequest::Message::get()
{
	return StringFromNative(_native.GetMessage());
}
Readium::Credentials^ CredentialRequest::Credentials::get()
{
	return ref new Credentials_Type(_native.GetCredentials());
}

CredentialRequestFieldType CredentialRequest::GetItemType(size_t idx)
{
	return CredentialRequestFieldType(_native.GetItemType(idx));
}
String^ CredentialRequest::GetItemTitle(size_t idx)
{
	return StringFromNative(_native.GetItemTitle(idx));
}

size_t CredentialRequest::AddCredential(String^ title, bool secret, String^ defaultValue)
{
	return _native.AddCredential(StringToNative(title), secret, StringToNative(defaultValue));
}
size_t CredentialRequest::AddButton(String^ title, CredentialRequestButtonHandler^ handler)
{
	return _native.AddButton(StringToNative(title), [this, handler](const ::ePub3::CredentialRequest*, size_t idx) {
		handler->Invoke(this, idx);
	});
}

void CredentialRequest::SignalCompletion()
{
	_native.SignalCompletion();
}

void CredentialRequest::SignalException(Exception^ exc)
{
	_native.SignalException(std::make_exception_ptr(__WinRTException(exc)));
}

END_READIUM_API
