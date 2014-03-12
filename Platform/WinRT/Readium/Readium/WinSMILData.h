//
//  WinSMILData.h
//  Readium
//
//  Created by Jim Dovey on 2013-11-27.
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

#ifndef __Readium_SMIL_Data_h__
#define __Readium_SMIL_Data_h__

#include "Readium.h"
#include <ePub3/media-overlays_smil_data.h>

BEGIN_READIUM_API

ref class MediaOverlaysSMILModel;
ref class ManifestItem;
ref class SpineItem;

ref class SMILData;

interface class ISMILSequence;
interface class ISMILParallel;
interface class ISMILTimeContainer;
interface class ISMILAudio;
interface class ISMILText;
interface class ISMILTimeNode;

public interface class ISMILTimeNode
{
	property ::Platform::String^ Name { ::Platform::String^ get(); }
	property ISMILTimeContainer^ Parent { ISMILTimeContainer^ get(); }
};

public interface class ISMILTimeContainer : ISMILTimeNode
{
	property ISMILSequence^ ParentSequence { ISMILSequence^ get(); }
	property ::Platform::String^ Type { ::Platform::String^ get(); }
	property bool IsParallel { bool get(); }
	property bool IsSequence { bool get(); }
};

public interface class ISMILSequence : ISMILTimeContainer
{
	property ::Platform::String^ TextRefFile { ::Platform::String^ get(); }
	property ::Platform::String^ TextRefFragmentID { ::Platform::String^ get(); }
	property ManifestItem^ TextRefManifestItem { ManifestItem^ get(); }
	property UINT DurationMilliseconds { UINT get(); }

	property UINT ChildCount { UINT get(); }
	ISMILTimeContainer^ ChildAt(UINT idx);
};

public interface class ISMILMedia : ISMILTimeNode
{
	property ISMILParallel^ ParentParallel { ISMILParallel^ get(); }
	property ::Platform::String^ SrcFile { ::Platform::String^ get(); }
	property ::Platform::String^ SrcFragmentID { ::Platform::String^ get(); }
	property ManifestItem^ SrcManifestItem { ManifestItem^ get(); }
	property bool IsAudio { bool get(); }
	property bool IsText { bool get(); }
};

public interface class ISMILAudio : ISMILMedia
{
	property UINT ClipBeginMilliseconds { UINT get(); }
	property UINT ClipEndMilliseconds { UINT get(); }
	property UINT ClipDurationMilliseconds { UINT get(); }
};

public interface class ISMILText : ISMILMedia
{
};

public interface class ISMILParallel : ISMILTimeContainer
{
	property ISMILAudio^ Audio { ISMILAudio^ get(); }
	property ISMILText^ Text { ISMILText^ get(); }
};

public ref class SMILData sealed
{
public:
	_DECLARE_BRIDGE_API_(::ePub3::SMILDataPtr, SMILData^);

internal:
	SMILData(::ePub3::SMILDataPtr native) : _native(native) {}

	// access to the (internal) model object wrappers, for use by MediaOverlaysSMILModel.
	static ISMILParallel^ WrappedParallel(std::shared_ptr<const ::ePub3::SMILData::Parallel>);

public:
	virtual ~SMILData() {}

	property SpineItem^ XhtmlSpineItem { SpineItem^ get(); }
	property ManifestItem^ SmilManifestItem { ManifestItem^ get(); }
	property ISMILSequence^ Body { ISMILSequence^ get(); }
	
	property UINT DurationMilliseconds_Metadata { UINT get(); }
	property UINT DurationMilliseconds_Calculated { UINT get(); }

};

END_READIUM_API

#endif	/* __Readium_SMIL_Data_h__ */