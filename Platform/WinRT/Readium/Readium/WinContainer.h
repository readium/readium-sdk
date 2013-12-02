//
//  WinContainer.h
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

#ifndef __Readium_Container_h__
#define __Readium_Container_h__

#include "Readium.h"
#include "Streams.h"
#include <ePub3/container.h>
#include <collection.h>

BEGIN_READIUM_API

ref class Package;
ref class ManifestItem;
ref class SpineItem;
ref class CFI;
ref class EncryptionInfo;

interface class IContentModule;

using ::Platform::String;
using ::Windows::Foundation::Collections::IVectorView;
using ::Windows::Foundation::IAsyncOperation;
using ::Windows::Storage::IStorageFile;


public ref class Container sealed
{
	_DECLARE_BRIDGE_API_(::ePub3::ContainerPtr, Container^);

private:
	Container(::ePub3::ContainerPtr native);

public:
	virtual ~Container() {}

	static Container^ OpenContainer(IStorageFile^ file);
	static IAsyncOperation<Container^>^ OpenContainerAsync(IStorageFile^ file);
	static Container^ OpenContainerForContentModule(IStorageFile^ file);

	/// ePub-relative paths for all packages
	IVectorView<String^>^ PackageLocations();

	/// All instantiated packages
	IVectorView<Package^>^ Packages();

	/// The default (first in the list) package instance
	Package^ DefaultPackage();

	/// The version number of this container's data.
	String^ Version();

	property String^ Path { String^ get(); }

	/// All information from META-INF/encryption.xml
	IVectorView<EncryptionInfo^>^ EncryptionData();

	/**
	<summary>
		Retrieves the encryption information for a specific file within the container.
	</summary>
	<param name="path">
		A container-relative path to the item whose encryption information to retrieve.
	</param>
	<result>
		Returns the encryption information, or <c>nullptr</c> if none was found.
	</result>
	*/
	EncryptionInfo^ EncryptionInfoForPath(String^ path);

	/**
	<summary>
		Determines whether a given file is present in the container.
	</summary>
	<param name="path">
		The absolute path of the item.
	</param>
	<result>
		<c>true</c> if the item is present, <c>false</c> otherwise.
	*/
	bool FileExistsAtPath(::Platform::String^ path);

	/**
	<summary>
		Obtains a pointer to a ReadStream for a specific file within the container.
	</summary>
	<param name="path">
		A container-relative path to the file whose data to read.
	</param>
	<result>
		A stream to the specified file.
	</result>
	*/
	IClosableStream^ ReadStreamAtPath(String^ path);

	/**
	<summary>
		Gets/sets the ContentModule which created this container, if any.

		Note that a Creator may only be set once. Any attempt to set another Creator will raise an exception.
	</summary>
	*/
	property IContentModule^ Creator { IContentModule^ get(); void set(IContentModule^); }

};

END_READIUM_API

#endif	/* __Readium_Container_h__ */