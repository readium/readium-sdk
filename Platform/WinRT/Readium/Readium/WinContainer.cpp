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
#include "IContentModule.h"
#include "PluginMaps.h"
#include "WinContentModuleManager.h"

#include <ppltasks.h>

using namespace ::concurrency;

BEGIN_READIUM_API

_BRIDGE_API_IMPL_(::ePub3::ContainerPtr, Container)

Container::Container(::ePub3::ContainerPtr native) : _native(native)
{
	_native->SetBridge(this);
}
IAsyncOperation<Container^>^ Container::OpenContainerAsync(IStorageFile^ file)
{
	return create_async([file]() -> Container^ {
		auto native = ::ePub3::Container::OpenSynchronouslyForWinRT(StringToNative(file->Path));
		return Wrapper(native);
	});
}
Container^ Container::OpenContainer(IStorageFile^ file)
{
	auto native = ::ePub3::Container::OpenSynchronouslyForWinRT(StringToNative(file->Path));
	return Wrapper(native);
}
Container^ Container::OpenContainerForContentModule(IStorageFile^ file)
{
	return Wrapper(::ePub3::Container::OpenContainerForContentModule(StringToNative(file->Path)));
}

IVectorView<String^>^ Container::PackageLocations()
{
	return ref new BridgedStringVectorView(_native->PackageLocations());
}

IVectorView<Package^>^ Container::Packages()
{
	return ref new BRIDGED_OBJECT_VECTOR(Package^, ::ePub3::PackagePtr)(_native->Packages());
}

Package^ Container::DefaultPackage()
{
	return Package::Wrapper(_native->DefaultPackage());
}

String^ Container::Version()
{
	return StringFromNative(_native->Version());
}

String^ Container::Path::get()
{
	return StringFromNative(_native->Path());
}

IVectorView<EncryptionInfo^>^ Container::EncryptionData()
{
	return ref new BRIDGED_OBJECT_VECTOR(EncryptionInfo^, ::ePub3::EncryptionInfoPtr)(_native->EncryptionData());
}

EncryptionInfo^ Container::EncryptionInfoForPath(String^ path)
{
	return EncryptionInfo::Wrapper(_native->EncryptionInfoForPath(StringToNative(path)));
}

bool Container::FileExistsAtPath(String^ path)
{
	return _native->FileExistsAtPath(StringToNative(path));
}

IClosableStream^ Container::ReadStreamAtPath(String^ path)
{
	return ref new Stream(_native->ReadStreamAtPath(StringToNative(path)));
}

IContentModule^ Container::Creator::get()
{
	std::shared_ptr<ePub3::ContentModule> __n = _native->Creator();
	auto __w = std::dynamic_pointer_cast<__WinRTContentModule>(__n);
	if (bool(__w))
		return __w->__rtObj();

	return ref new ContentModuleWrapper(__n);
}
void Container::Creator::set(IContentModule^ module)
{
	try
	{
		_native->SetCreator(std::make_shared<__WinRTContentModule>(module));
	}
	catch (std::exception& exc)
	{
		typedef std::wstring_convert<std::codecvt_utf8<wchar_t>>	_Converter;
		std::wstring wstr = _Converter().from_bytes(exc.what());
		throw ref new ::Platform::COMException(E_ILLEGAL_METHOD_CALL, ::Platform::StringReference(wstr.c_str()));
	}
	catch (...)
	{
		throw ref new ::Platform::COMException(E_ILLEGAL_METHOD_CALL, TEXT("Container::Creator::set()"));
	}
}

END_READIUM_API
