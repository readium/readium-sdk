//
//  WinMediaSupport.h
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