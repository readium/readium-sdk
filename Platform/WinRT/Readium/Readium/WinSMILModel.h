//
//  WinSMILModel.h
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

#ifndef __Readium_SMIL_Model_h__
#define __Readium_SMIL_Model_h__

#include "Readium.h"
#include "WinSMILData.h"
#include <ePub3/media-overlays_smil_model.h>

BEGIN_READIUM_API

public value class SMILPosition sealed
{
public:
	UINT SmilDataIndex;
	UINT ParIndex;
	UINT Milliseconds;
};

public ref class MediaOverlaysSMILModel sealed
{
public:
	_DECLARE_BRIDGE_API_(std::shared_ptr<::ePub3::MediaOverlaysSmilModel>, MediaOverlaysSMILModel^);

internal:
	MediaOverlaysSMILModel(std::shared_ptr<::ePub3::MediaOverlaysSmilModel> native) : _native(native) {}

public:
	virtual ~MediaOverlaysSMILModel() {}

	property ::Platform::String^ Narrator { ::Platform::String^ get(); }
	property ::Platform::String^ ActiveClass { ::Platform::String^ get(); }
	property ::Platform::String^ PlaybackActiveClass { ::Platform::String^ get(); }

	property UINT DurationMilliseconds_Metadata { UINT get(); }
	property UINT DurationMilliseconds_Calculated { UINT get(); }

	property UINT SMILDataCount { UINT get(); }
	SMILData^ SMILDataAt(UINT idx);

	double PositionToPercent(SMILPosition position);
	SMILPosition PercentToPosition(double percent, SMILData^* outSmilData, ISMILParallel^* outPar);

	property UINT SkippablesCount { UINT get(); }
	::Platform::String^ SkippableAt(UINT idx);

	property UINT EscapablesCount { UINT get(); }
	::Platform::String^ EscapableAt(UINT idx);

};

END_READIUM_API

#endif	/* __Readium_SMIL_Model_h__ */
