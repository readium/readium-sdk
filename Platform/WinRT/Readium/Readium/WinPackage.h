//
//  WinPackage.h
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

#ifndef __Readium_Package_h__
#define __Readium_Package_h__

#include "Readium.h"
#include "IPropertyHolder.h"
#include "Streams.h"
#include <ePub3/package.h>

BEGIN_READIUM_API

ref class Container;
ref class ManifestItem;
ref class SpineItem;
ref class CFI;
ref class NavigationTable;
ref class ContentHandler;

using namespace ::Windows::Foundation::Collections;

public ref class Package sealed : public IPropertyHolder
{
	_DECLARE_BRIDGE_API_(::ePub3::PackagePtr, Package^);

private:
	Package(::ePub3::PackagePtr native);

public:
	virtual ~Package() {}

	// from PackageBase:

	String^ BasePath();

	property IMap<String^, ManifestItem^>^ Manifest { IMap<String^, ManifestItem^>^ get(); }
	property IMap<String^, NavigationTable^>^ NavigationTables { IMap<String^, NavigationTable^>^ get(); }
	property SpineItem^ FirstSpineItem { SpineItem^ get(); }

	SpineItem^ SpineItemAt(unsigned int idx);
	int IndexOfSpineItemWithIDRef(String^ idref);

	ManifestItem^ ManifestItemWithID(String^ id);

	String^ CFISubpathForManifestItemWithID(String^ id);

	IVector<ManifestItem^>^ ManifestItemsWithProperties(IVectorView<Uri^>^ iriList);

	NavigationTable^ GetNavigationTable(String^ type);

	IClosableStream^ ReadStreamForItemAtPath(String^ path);

	property unsigned int SpineCFIIndex { unsigned int get(); }

	// from Package:

	Container^ GetContainer();

	property String^ UniqueID { String^ get(); }
	property String^ URLSafeUniqueID { String^ get(); }
	property String^ PackageID { String^ get(); }
	property String^ Type { String^ get(); }
	property String^ Version { String^ get(); }

	void AddMediaHandler(ContentHandler^ handler);

	SpineItem^ SpineItemWithIDRef(String^ idref);

	CFI^ CFIForManifestItem(ManifestItem^ item);
	CFI^ CFIForSpineItem(SpineItem^ item);

	ManifestItem^ ManifestItemForCFI(CFI^ cfi, CFI^* remainingCFI);

	IClosableStream^ ReadStreamForRelativePath(String^ relativePath);

	IClosableStream^ ContentStreamFor(SpineItem^ item);
	IClosableStream^ ContentStreamFor(ManifestItem^ item);

	property NavigationTable^ TableOfContents { NavigationTable^ get(); }
	property NavigationTable^ ListOfFigures { NavigationTable^ get(); }
	property NavigationTable^ ListOfIllustrations { NavigationTable^ get(); }
	property NavigationTable^ ListOfTables { NavigationTable^ get(); }
	property NavigationTable^ PageList { NavigationTable^ get(); }



};

END_READIUM_API

#endif	/* __Readium_Package_h__ */