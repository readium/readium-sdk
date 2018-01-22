//
//  WinMediaSupport.cpp
//  Readium
//
//  Created by Jim Dovey on 2013-10-10.
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
