//
//  WinMediaHandler.cpp
//  Readium
//
//  Created by Jim Dovey on 2013-10-10.
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

#include "WinMediaHandler.h"
#include "WinPackage.h"
#include "WinManifest.h"

using ::Platform::String;
using namespace ::Windows::Foundation;
using namespace ::Windows::Foundation::Collections;

BEGIN_READIUM_API

_BRIDGE_API_IMPL_(::ePub3::MediaHandlerPtr, MediaHandler)

MediaHandler::MediaHandler(::ePub3::MediaHandlerPtr native) : _native(native)
{
	_native->SetBridge(this);
}

Package^ MediaHandler::Owner::get()
{
	return Package::Wrapper(_native->Owner());
}
String^ MediaHandler::MediaType::get()
{
	return StringFromNative(_native->MediaType());
}

void MediaHandler::Invoke(String^ packageRelativeSourcePath, IMapView<String^, String^>^ parameters)
{
	std::map<::ePub3::string, ::ePub3::string> nativeParams;
	auto pos = parameters->First();
	while (pos->HasCurrent)
	{
		nativeParams[StringToNative(pos->Current->Key)] = StringToNative(pos->Current->Value);
		pos->MoveNext();
	}

	(*_native)(StringToNative(packageRelativeSourcePath), nativeParams);
}

Uri^ MediaHandler::Target(String^ packageRelativeSourcePath, IMapView<String^, String^>^ parameters)
{
	std::map<::ePub3::string, ::ePub3::string> nativeParams;
	auto pos = parameters->First();
	while (pos->HasCurrent)
	{
		nativeParams[StringToNative(pos->Current->Key)] = StringToNative(pos->Current->Value);
		pos->MoveNext();
	}

	return IRIToURI(_native->Target(StringToNative(packageRelativeSourcePath), nativeParams));
}

END_READIUM_API
