//
//  WinPackage.cpp
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

#include "WinPackage.h"
#include "CollectionBridges.h"
#include "WinSpine.h"
#include "WinManifest.h"
#include "WinMediaSupport.h"
#include "WinNavTable.h"

BEGIN_READIUM_API

typedef BridgedStringKeyedObjectMapView<ManifestItem^, ::ePub3::ManifestItemPtr>					ManifestMapView;
typedef BridgedStringKeyedObjectMapView<NavigationTable^, ::ePub3::NavigationTablePtr>				NavTableMapView;
typedef BridgedObjectVectorView<ManifestItem^, ::ePub3::ManifestItemPtr>							ManifestList;
typedef BridgedObjectVectorView<IContentHandler^, ::ePub3::ContentHandlerPtr>						ContentHandlerList;
typedef BridgedObjectVectorView<MediaSupportInfo^, ::std::shared_ptr<::ePub3::MediaSupportInfo>>	MediaSupportList;

Package^ Package::Wrapper(::ePub3::PackagePtr native)
{
	if (!bool(native))
		return nullptr;

	Package^ result = native->GetBridge<Package>();
	if (result == nullptr)
		result = ref new Package(native);
	return result;
}

String^ Package::BasePath()
{
	auto nativeStr = _native->BasePath();
	DeclareFastPassString(nativeStr, result);
	return result;
}

IMapView<String^, ManifestItem^>^ Package::Manifest::get()
{
	auto nativeMap = _native->Manifest();
	return ref new ManifestMapView(nativeMap);
}
IMapView<String^, NavigationTable^>^ Package::NavigationTables::get()
{
	auto native = _native->NavigationTables();
	return ref new NavTableMapView(native);
}

SpineItem^ Package::FirstSpineItem::get()
{
	return SpineItem::Wrapper(_native->FirstSpineItem());
}

SpineItem^ Package::SpineItemAt(unsigned int idx)
{
	return SpineItem::Wrapper(_native->SpineItemAt(idx));
}
int Package::IndexOfSpineItemWithIDRef(String^ idref)
{
	return static_cast<int>(_native->IndexOfSpineItemWithIDRef(StringToNative(idref)));
}

ManifestItem^ Package::ManifestItemWithID(String^ id)
{
	return ManifestItem::Wrapper(_native->ManifestItemWithID(StringToNative(id)));
}

String^ Package::CFISubpathForManifestItemWithID(String^ id)
{
	return StringFromNative(_native->CFISubpathForManifestItemWithID(StringToNative(id)));
}

IVectorView<ManifestItem^>^ Package::ManifestItemsWithProperties(IVectorView<Uri^>^ iriList)
{
	size_t i = 0;
	std::vector<::ePub3::IRI> nativeList(iriList->Size);
	for (Uri^ uri : iriList)
	{
		nativeList[i++] = URIToIRI(uri);
	}
	return ref new ManifestList(_native->ManifestItemsWithProperties(nativeList));
}

NavigationTable^ Package::GetNavigationTable(String^ type)
{
	return NavigationTable::Wrapper(_native->NavigationTable(StringToNative(type)));
}

IClosableStream^ Package::ReadStreamForItemAtPath(String^ path)
{
	return ref new Stream(_native->ReadStreamForItemAtPath(StringToNative(path)));
}

unsigned int Package::SpineCFIIndex::get()
{
	return static_cast<unsigned int>(_native->SpineCFIIndex());
}

END_READIUM_API