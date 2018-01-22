//
//  WinMediaSupport.h
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

#ifndef __Readium_MediaSupportInfo_h__
#define __Readium_MediaSupportInfo_h__

#include "Readium.h"
#include "PropertyHolderImpl.h"
#include <ePub3/media_support_info.h>

BEGIN_READIUM_API

ref class Package;
ref class ManifestItem;

/// Possible types of support provided.
public enum class MediaSupportType
{
	/// The media type is not supported.
	Unsupported = 0,
	/// The rendering engine understands this media type with no outside help.
	IntrinsicSupport = 1,
	/// The publication provides a DHTML handler for this media type.
	SupportedWithHandler = 2
};

public ref class MediaSupportInfo sealed
{
	_DECLARE_BRIDGE_API_(::ePub3::MediaSupportInfoPtr, MediaSupportInfo^);

internal:
	MediaSupportInfo(::ePub3::MediaSupportInfoPtr native) : _native(native) {}

public:
	MediaSupportInfo(Package^ owner);
	MediaSupportInfo(Package^ owner, ::Platform::String^ mediaType);
	MediaSupportInfo(Package^ owner, ::Platform::String^ mediaType, MediaSupportType supportType);

	virtual ~MediaSupportInfo() {}

	property String^ MediaType { ::Platform::String^ get(); void set(::Platform::String^ newValue); }
	property MediaSupportType Support { MediaSupportType get(); void set(MediaSupportType value); }

	property bool HasIntrinsicSupport { bool get(); }
	property bool RequiresMediaHandler { bool get(); }

	IVectorView<ManifestItem^>^ MatchingManifestItems(Package^ pkg);

};

END_READIUM_API

#endif	/* __Readium_MediaSupportInfo_h__ */
