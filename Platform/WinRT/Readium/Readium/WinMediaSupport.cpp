//
//  WinMediaSupport.cpp
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

#include "WinMediaSupport.h"
#include "WinPackage.h"
#include "WinManifest.h"
#include "CollectionBridges.h"

using ::Platform::String;

BEGIN_READIUM_API

typedef BRIDGED_OBJECT_VECTOR(ManifestItem^, ::ePub3::ManifestItemPtr) ManifestItemList;

_BRIDGE_API_IMPL_(::ePub3::MediaSupportInfoPtr, MediaSupportInfo)

MediaSupportInfo::MediaSupportInfo(Package^ owner) : _native(::ePub3::MediaSupportInfo::New(owner->NativeObject))
{
	_native->SetBridge(this);
}
MediaSupportInfo::MediaSupportInfo(Package^ owner, String^ mediaType) : _native(::ePub3::MediaSupportInfo::New(owner->NativeObject, StringToNative(mediaType)))
{
	_native->SetBridge(this);
}
MediaSupportInfo::MediaSupportInfo(Package^ owner, String^ mediaType, MediaSupportType supportType) : _native(::ePub3::MediaSupportInfo::New(owner->NativeObject, StringToNative(mediaType), ::ePub3::MediaSupportInfo::SupportType(supportType)))
{
	_native->SetBridge(this);
}

String^ MediaSupportInfo::MediaType::get()
{
	return StringFromNative(_native->MediaType());
}
void MediaSupportInfo::MediaType::set(String^ str)
{
	_native->SetTypeAndSupport(StringToNative(str), _native->Support());
}

MediaSupportType MediaSupportInfo::Support::get()
{
	return MediaSupportType(_native->Support());
}
void MediaSupportInfo::Support::set(MediaSupportType type)
{
	_native->SetTypeAndSupport(_native->MediaType(), ::ePub3::MediaSupportInfo::SupportType(type));
}

bool MediaSupportInfo::HasIntrinsicSupport::get()
{
	return _native->HasIntrinsicSupport();
}
bool MediaSupportInfo::RequiresMediaHandler::get()
{
	return _native->RequiresMediaHandler();
}

IVectorView<ManifestItem^>^ MediaSupportInfo::MatchingManifestItems(Package^ pkg)
{
	auto nativeList = _native->MatchingManifestItems(pkg->NativeObject);
	return ref new ManifestItemList(nativeList);
}

END_READIUM_API
