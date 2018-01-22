//
//  WinCFI.cpp
//  Readium
//
//  Created by Jim Dovey on 2013-10-14.
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

#include "WinCFI.h"

BEGIN_READIUM_API

using namespace ::Platform;

CFI^ CFI::Wrapper(::ePub3::CFI& native)
{
	CFI^ result = native.GetBridge<CFI>();
	if (result == nullptr)
		result = ref new CFI(native);
	return result;
}
CFI^ CFI::Wrapper(::ePub3::CFI&& native)
{
	CFI^ result = native.GetBridge<CFI>();
	if (result == nullptr)
		result = ref new CFI(std::move(native));
	return result;
}

CFI::CFI(const ::ePub3::CFI& native) : _native(native)
{
	_native.SetBridge(this);
}
CFI::CFI(::ePub3::CFI&& native) : _native(std::move(native))
{
	_native.SetBridge(this);
}

CFI::CFI() : CFI(std::move(::ePub3::CFI()))
{
}
CFI::CFI(CFI^ base, CFI^ start, CFI^ end) : CFI(std::move(::ePub3::CFI(base->NativeObject, start->NativeObject, end->NativeObject)))
{
}
CFI::CFI(String^ stringRepresentation) : CFI(std::move(::ePub3::CFI(StringToNative(stringRepresentation))))
{
}
CFI::CFI(CFI^ otherCFI, UINT fromIndex) : CFI(std::move(::ePub3::CFI(otherCFI->NativeObject, fromIndex)))
{
}

String^ CFI::ToString()
{
	return StringFromNative(_native.String());
}

bool CFI::IsRangeTriplet::get()
{
	return _native.IsRangeTriplet();
}
bool CFI::IsEmpty::get()
{
	return _native.Empty();
}

void CFI::Clear()
{
	_native.Clear();
}

bool CFI::Equals(Object^ obj)
{
	CFI^ cfi = dynamic_cast<CFI^>(obj);
	if (cfi != nullptr)
		return Equals(cfi);

	String^ str = dynamic_cast<String^>(obj);
	if (str != nullptr)
		return Equals(str);

	return false;
}
bool CFI::Equals(CFI^ otherCFI)
{
	return _native == otherCFI->NativeObject;
}
bool CFI::Equals(String^ stringRepresentation)
{
	return _native == StringToNative(stringRepresentation);
}

CFI^ CFI::Assign(CFI^ newValue)
{
	_native = newValue->NativeObject;
	return this;
}
CFI^ CFI::Assign(String^ stringRepresentation)
{
	_native = StringToNative(stringRepresentation);
	return this;
}
CFI^ CFI::Assign(CFI^ base, UINT fromIndex)
{
	_native.Assign(base->NativeObject, fromIndex);
	return this;
}

CFI^ CFI::Append(CFI^ otherCFI)
{
	_native += otherCFI->NativeObject;
	return this;
}
CFI^ CFI::Append(String^ stringRepresentation)
{
	_native += StringToNative(stringRepresentation);
	return this;
}

CFISideBias CFI::CharacterSideBias::get()
{
	return CFISideBias(_native.CharacterSideBias());
}

END_READIUM_API
