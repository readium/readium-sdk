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
#include "PropertyHolderImpl.h"
#include "Streams.h"
#include "WinNavTable.h"
#include "WinMediaSupport.h"
#include <ePub3/package.h>

BEGIN_READIUM_API

ref class Container;
ref class ManifestItem;
ref class SpineItem;
ref class CFI;
interface class IContentHandler;
ref class MediaHandler;
ref class FilterChain;

using ::Platform::String;
using ::Windows::Foundation::DateTime;
using ::Windows::Foundation::Collections::IVectorView;
using ::Windows::Foundation::Collections::IMapView;

public ref class Package sealed : public IPropertyHolder
{
private:
	bool _returnLocalized;
	_DECLARE_BRIDGE_API_(::ePub3::PackagePtr, Package^);

private:
	Package(::ePub3::PackagePtr native);

public:
	virtual ~Package() {}

	// from PackageBase:

	String^ BasePath();

	property IMapView<String^, ManifestItem^>^ Manifest { IMapView<String^, ManifestItem^>^ get(); }
	property IMapView<String^, NavigationTable^>^ NavigationTables { IMapView<String^, NavigationTable^>^ get(); }
	property SpineItem^ FirstSpineItem { SpineItem^ get(); }

	SpineItem^ SpineItemAt(unsigned int idx);
	int IndexOfSpineItemWithIDRef(String^ idref);

	ManifestItem^ ManifestItemWithID(String^ id);

	String^ CFISubpathForManifestItemWithID(String^ id);

	IVectorView<ManifestItem^>^ ManifestItemsWithProperties(IVectorView<Uri^>^ iriList);

	ManifestItem^ ManifestItemForRelativePath(::Platform::String^ path);

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

	void AddMediaHandler(IContentHandler^ handler);

	SpineItem^ SpineItemWithIDRef(String^ idref);

	CFI^ CFIForManifestItem(ManifestItem^ item);
	CFI^ CFIForSpineItem(SpineItem^ item);

	ManifestItem^ ManifestItemForCFI(CFI^ cfi, CFI^* remainingCFI);

	IClosableStream^ ReadStreamForRelativePath(String^ relativePath);

	IClosableStream^ ContentStreamFor(SpineItem^ item);
	[::Windows::Foundation::Metadata::DefaultOverload]
	IClosableStream^ ContentStreamFor(ManifestItem^ item);

	IClosableStream^ SyncContentStreamFor(SpineItem^ item);
	[::Windows::Foundation::Metadata::DefaultOverload]
	IClosableStream^ SyncContentStreamFor(ManifestItem^ item);

	property NavigationTable^ TableOfContents { NavigationTable^ get(); }
	property NavigationTable^ ListOfFigures { NavigationTable^ get(); }
	property NavigationTable^ ListOfIllustrations { NavigationTable^ get(); }
	property NavigationTable^ ListOfTables { NavigationTable^ get(); }
	property NavigationTable^ PageList { NavigationTable^ get(); }

	////////////////////////////////////////////////////////////////
	// OPF metadata (property) access

	property bool ReturnsLocalizedProperties {
		bool get()			{ return _returnLocalized; }
		void set(bool arg)	{ _returnLocalized = arg; }
	}

	property String^ Title { String^ get(); }
	property String^ Subtitle { String^ get(); }
	property String^ ShortTitle { String^ get(); }
	property String^ CollectionTitle { String^ get(); }
	property String^ EditionTitle { String^ get(); }
	property String^ ExpandedTitle { String^ get(); }
	property String^ FullTitle { String^ get(); }

	property IVectorView<String^>^ AuthorNames { IVectorView<String^>^ get(); }
	property IVectorView<String^>^ AttributionNames { IVectorView<String^>^ get(); }
	property String^ Authors { String^ get(); }
	property IVectorView<String^>^ ContributorNames { IVectorView<String^>^ get(); }
	property String^ Contributors { String^ get(); }

	property String^ Language { String^ get(); }
	property String^ Source { String^ get(); }
	property String^ CopyrightOwner { String^ get(); }

	property DateTime ModificationDate { DateTime get(); }

	property String^ ISBN { String^ get(); }

	property IVectorView<String^>^ Subjects { IVectorView<String^>^ get(); }

	property PageProgression PageProgressionDirection { PageProgression get(); }

	////////////////////////////////////////////////////////////////
	// Media Handling

	IVectorView<String^>^ MediaTypesWithDHTMLHandlers();
	IVectorView<IContentHandler^>^ HandlersForMediaType(String^ mediaType);
	MediaHandler^ OPFHandlerForMediaType(String^ mediaType);

	property IVectorView<String^>^ AllMediaTypes { IVectorView<String^>^ get(); }
	property IVectorView<String^>^ UnsupportedMediaTypes { IVectorView<String^>^ get(); }
	
	property IMapView<String^, MediaSupportInfo^>^ MediaSupport { IMapView<String^, MediaSupportInfo^>^ get(); void set(IMapView<String^, MediaSupportInfo^>^); }

	void SetFilterChain(FilterChain^ chain);

#include "PropertyHolderSubclassDecl.h"

};

END_READIUM_API

#endif	/* __Readium_Package_h__ */