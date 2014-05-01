//
//  WinMediaSupport.cpp
//  Readium
//
//  Created by Jim Dovey on 2013-10-10.
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//  
//  This program is distributed in the hope that it will be useful, but WITHOUT ANY 
//  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
//  
//  Licensed under Gnu Affero General Public License Version 3 (provided, notwithstanding this notice, 
//  Readium Foundation reserves the right to license this material under a different separate license, 
//  and if you have done so, the terms of that separate license control and the following references 
//  to GPL do not apply).
//  
//  This program is free software: you can redistribute it and/or modify it under the terms of the GNU 
//  Affero General Public License as published by the Free Software Foundation, either version 3 of 
//  the License, or (at your option) any later version. You should have received a copy of the GNU 
//  Affero General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "WinMediaSupport.h"
#include "WinPackage.h"
#include "WinManifest.h"
#include "CollectionBridges.h"

using ::Platform::String;

BEGIN_READIUM_API

using ManifestItemList = BridgedObjectVectorView<ManifestItem^, ::ePub3::ManifestItemPtr>;

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
