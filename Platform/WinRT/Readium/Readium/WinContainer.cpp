//
//  WinContainer.cpp
//  Readium
//
//  Created by Jim Dovey on 2013-09-26.
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

#include "WinContainer.h"
#include "WinPackage.h"
#include "WinEncryptionInfo.h"
#include "Streams.h"
#include "WinManifest.h"
#include "CollectionBridges.h"

#include <ppltasks.h>

using namespace ::concurrency;

BEGIN_READIUM_API

_BRIDGE_API_IMPL_(::ePub3::ContainerPtr, Container)

Container::Container(::ePub3::ContainerPtr native) : _native(native)
{
	_native->SetBridge(this);
}
IAsyncOperation<Container^>^ Container::OpenContainer(IStorageFile^ file)
{
	return create_async([file]() -> Container^ {
		auto native = ::ePub3::Container::OpenContainer(StringToNative(file->Path));
		return Wrapper(native);
	});
}

IVectorView<String^>^ Container::PackageLocations()
{
	return ref new BridgedStringVectorView(_native->PackageLocations());
}

IVectorView<Package^>^ Container::Packages()
{
	return ref new BridgedObjectVectorView<Package^, ::ePub3::PackagePtr>(_native->Packages());
}

Package^ Container::DefaultPackage()
{
	return Package::Wrapper(_native->DefaultPackage());
}

String^ Container::Version()
{
	return StringFromNative(_native->Version());
}

IVectorView<EncryptionInfo^>^ Container::EncryptionData()
{
	return ref new BridgedObjectVectorView<EncryptionInfo^, ::ePub3::EncryptionInfoPtr>(_native->EncryptionData());
}

EncryptionInfo^ Container::EncryptionInfoForPath(String^ path)
{
	return EncryptionInfo::Wrapper(_native->EncryptionInfoForPath(StringToNative(path)));
}

IClosableStream^ Container::ReadStreamAtPath(String^ path)
{
	return ref new Stream(_native->ReadStreamAtPath(StringToNative(path)));
}

END_READIUM_API
