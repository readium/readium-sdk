//
//  WinSMILModel.cpp
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
	std::vector<::ePub3::SMILDataPtr>::size_type idx = 0;
	uint32_t parIdx = 0;
	uint32_t milliseconds = 0;
	::ePub3::SMILDataPtr outDataPtr;
	std::shared_ptr<const ::ePub3::SMILData::Parallel> outParPtr;

	_native->PercentToPosition(percent, outDataPtr, idx, outParPtr, parIdx, milliseconds);
	
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
