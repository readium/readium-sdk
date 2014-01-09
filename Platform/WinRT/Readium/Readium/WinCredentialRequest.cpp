//
//  WinCredentialRequest.cpp
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

//void CredentialRequest::SignalException(COMException^ exc)
//{
//	_native.SignalException(std::make_exception_ptr(__WinRTException(exc)));
//}

END_READIUM_API
