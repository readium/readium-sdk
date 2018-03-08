//
//  WinSMILData.h
//  Readium
//
//  Created by Jim Dovey on 2013-11-27.
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
