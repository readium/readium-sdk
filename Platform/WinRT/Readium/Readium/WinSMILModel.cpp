//
//  WinSMILModel.cpp
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

#include "WinSMILModel.h"

BEGIN_READIUM_API

using ::Platform::String;

_BRIDGE_API_IMPL_(std::shared_ptr<::ePub3::MediaOverlaysSmilModel>, MediaOverlaysSMILModel)

String^ MediaOverlaysSMILModel::Narrator::get()
{
	return StringFromNative(_native->Narrator());
}
String^ MediaOverlaysSMILModel::ActiveClass::get()
{
	return StringFromNative(_native->ActiveClass());
}
String^ MediaOverlaysSMILModel::PlaybackActiveClass::get()
{
	return StringFromNative(_native->PlaybackActiveClass());
}
UINT MediaOverlaysSMILModel::DurationMilliseconds_Metadata::get()
{
	return static_cast<UINT>(_native->DurationMilliseconds_Metadata());
}
UINT MediaOverlaysSMILModel::DurationMilliseconds_Calculated::get()
{
	return static_cast<UINT>(_native->DurationMilliseconds_Calculated());
}
UINT MediaOverlaysSMILModel::SMILDataCount::get()
{
	return static_cast<UINT>(_native->GetSmilCount());
}
SMILData^ MediaOverlaysSMILModel::SMILDataAt(UINT idx)
{
	return SMILData::Wrapper(_native->GetSmil(static_cast<std::vector<::ePub3::SMILDataPtr>::size_type>(idx)));
}
double MediaOverlaysSMILModel::PositionToPercent(SMILPosition position)
{
	return _native->PositionToPercent(
		static_cast<std::vector<::ePub3::SMILDataPtr>::size_type>(position.SmilDataIndex),
		static_cast<uint32_t>(position.ParIndex),
		static_cast<uint32_t>(position.Milliseconds));
}
SMILPosition MediaOverlaysSMILModel::PercentToPosition(double percent, SMILData^* outSmilData, ISMILParallel^* outPar)
{
	//std::vector<::ePub3::SMILDataPtr>::size_type idx = 0;
	uint32_t idx = 0;
	uint32_t parIdx = 0;
	uint32_t milliseconds = 0;
	::ePub3::SMILDataPtr outDataPtr;
	std::shared_ptr<const ::ePub3::SMILData::Parallel> outParPtr;

	_native->PercentToPosition(percent, outDataPtr, idx, outParPtr, parIdx, milliseconds);
	// double, SMILDataPtr, size_type, *Parallel, uint32_t, uint32_t
	
	if (outSmilData != nullptr)
		*outSmilData = SMILData::Wrapper(outDataPtr);
	if (outPar != nullptr)
		*outPar = SMILData::WrappedParallel(outParPtr);

	return SMILPosition { static_cast<UINT>(idx), static_cast<UINT>(parIdx), static_cast<UINT>(milliseconds) };
}
UINT MediaOverlaysSMILModel::SkippablesCount::get()
{
	return static_cast<UINT>(_native->GetSkippablesCount());
}
String^ MediaOverlaysSMILModel::SkippableAt(UINT idx)
{
	return StringFromNative(_native->GetSkippable(static_cast<std::vector<::ePub3::string>::size_type>(idx)));
}
UINT MediaOverlaysSMILModel::EscapablesCount::get()
{
	return static_cast<UINT>(_native->GetEscapablesCount());
}
String^ MediaOverlaysSMILModel::EscapableAt(UINT idx)
{
	return StringFromNative(_native->GetEscapable(static_cast<std::vector<::ePub3::string>::size_type>(idx)));
}

END_READIUM_API
