//
//  WinLibrary.cpp
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

#include "WinLibrary.h"
#include "WinContainer.h"
#include "WinPackage.h"
#include "WinManifest.h"
#include "WinCFI.h"
#include "Streams.h"

#include <ePub3/package.h>
#include <ePub3/container.h>
#include <ePub3/manifest.h>
#include <ePub3/cfi.h>

#include <ppltasks.h>

using namespace ::concurrency;
using namespace ::Platform;
using namespace ::Windows::Foundation;
using namespace ::Windows::Storage;

BEGIN_READIUM_API

Library::Library(::ePub3::Library* native)
{
	native->SetBridge<Library>(this);
}

Library^ Library::MainLibrary()
{
	::ePub3::Library* native = ::ePub3::Library::MainLibrary();
	if (native == nullptr)
		throw ref new COMException(E_FAIL, TEXT("ePub3::Library::MainLibrary() returned nullptr"));
	
	Library^ bridge = native->GetBridge<Library>();
	if (bridge == nullptr)
		bridge = ref new Library(native);
	return bridge;
}
IAsyncOperation<Library^>^ Library::LoadLibrary(IStorageFile^ file)
{
	return create_async([file]() -> Library^ {
		::ePub3::string str(StringToNative(file->Path));
		::ePub3::Library* native = ::ePub3::Library::MainLibrary(str);
		if (native == nullptr)
			throw ref new COMException(E_FAIL, TEXT("ePub3::Library::MainLibrary() returned nullptr"));

		Library^ bridge = native->GetBridge<Library>();
		if (bridge == nullptr)
			bridge = ref new Library(native);
		return bridge;
	});
}

IAsyncOperation<IStorageFile^>^ Library::FileForEPubWithUniqueID(String^ uniqueID)
{
	::ePub3::string idstr(StringToNative(uniqueID));
	::ePub3::string path = ::ePub3::Library::MainLibrary()->PathForEPubWithUniqueID(idstr);
	if (path.empty())
		return nullptr;

	DeclareFastPassString(path, winPath);
	return reinterpret_cast<IAsyncOperation<IStorageFile^>^>(StorageFile::GetFileFromPathAsync(winPath));
}
IAsyncOperation<IStorageFile^>^ Library::FileForEPubWithPackageID(String^ packageID)
{
	::ePub3::string idstr(StringToNative(packageID));
	::ePub3::string path = ::ePub3::Library::MainLibrary()->PathForEPubWithPackageID(idstr);
	if (path.empty())
		return nullptr;

	DeclareFastPassString(path, winPath);
	return reinterpret_cast<IAsyncOperation<IStorageFile^>^>(StorageFile::GetFileFromPathAsync(winPath));
}

void Library::AddPublicationsInContainer(Container^ container, IStorageFile^ file)
{
	if (container == nullptr)
		return;

	::ePub3::ContainerPtr native = container->NativeObject;
	::ePub3::string path = StringToNative(file->Path);
	::ePub3::Library::MainLibrary()->AddPublicationsInContainer(native, path);
}
IAsyncAction^ Library::AddPublicationsInContainer(IStorageFile^ file)
{
	return create_async([this, file]() {
		auto container = Container::OpenContainer(file);
		if (container != nullptr)
			AddPublicationsInContainer(container, file);
	});
}

Uri^ Library::EPubURLForPublication(Package^ pkg)
{
	::ePub3::IRI iri(::ePub3::Library::MainLibrary()->EPubURLForPublication(pkg->NativeObject));
	::ePub3::string nstr = iri.IRIString();
	if (nstr.empty())
		nstr = iri.URIString();

	DeclareFastPassString(nstr, uristr);
	return ref new Uri(uristr);
}
Uri^ Library::EPubURLForPackageID(String^ packageID)
{
	return IRIToURI(::ePub3::Library::MainLibrary()->EPubURLForPublicationID(StringToNative(packageID)));
}

IAsyncOperation<Package^>^ Library::LoadPackage(Uri^ epubURL)
{
	return create_async([epubURL]() -> Package^ {
		::ePub3::IRI iri = URIToIRI(epubURL);
		return Package::Wrapper(::ePub3::Library::MainLibrary()->PackageForEPubURL(iri));
	});
}
IAsyncOperation<Package^>^ Library::LoadPackage(Uri^ ePubURL, bool allowLoad)
{
	return create_async([ePubURL, allowLoad]() -> Package^ {
		return Package::Wrapper(::ePub3::Library::MainLibrary()->PackageForEPubURL(URIToIRI(ePubURL), allowLoad));
	});
}

Uri^ Library::EPubCFIURLForManifestItem(ManifestItem^ item)
{
	return IRIToURI(::ePub3::Library::MainLibrary()->EPubCFIURLForManifestItem(item->NativeObject));
}
ManifestItem^ Library::ManifestItemForCFIURL(Uri^ urlWithCFI, _Out_ CFI^ remainingCFI)
{
	::ePub3::CFI nativeCFI;
	::ePub3::ManifestItemPtr native = ::ePub3::Library::MainLibrary()->ManifestItemForCFI(URIToIRI(urlWithCFI), &nativeCFI);
	remainingCFI = CFI::Wrapper(nativeCFI);
	return ManifestItem::Wrapper(native);
}
IClosableStream^ Library::StreamForEPubURL(Uri^ urlWithCFI, _Out_ CFI^ remainingCFI)
{
	::ePub3::CFI nativeCFI;
	::ePub3::ByteStreamPtr native = ::ePub3::Library::MainLibrary()->ReadStreamForEPubURL(URIToIRI(urlWithCFI), &nativeCFI);
	return ref new Stream(native);
}

IAsyncAction^ Library::WriteToFile(IStorageFile^ file)
{
	return create_async([file]() {
		::ePub3::Library::MainLibrary()->WriteToFile(StringToNative(file->Path));
	});
}

END_READIUM_API
