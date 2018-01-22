//
//  WinContainer.h
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
