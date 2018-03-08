//
//  WinContainer.cpp
//  Readium
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

String^ Container::Path::get()
{
	return StringFromNative(_native->Path());
}

IVectorView<EncryptionInfo^>^ Container::EncryptionData()
{
	return ref new BridgedObjectVectorView<EncryptionInfo^, ::ePub3::EncryptionInfoPtr>(_native->EncryptionData());
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
