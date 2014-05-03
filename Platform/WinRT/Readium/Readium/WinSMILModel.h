//
//  WinSMILModel.h
//  Readium
//
//  Created by Jim Dovey on 2013-11-27.
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
