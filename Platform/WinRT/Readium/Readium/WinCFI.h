//
//  WinCFI.h
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

#ifndef __Readium_CFI_h__
#define __Readium_CFI_h__

#include "Readium.h"
#include <ePub3/cfi.h>

BEGIN_READIUM_API

using ::Platform::String;

public enum class CFISideBias
{
	Unspecified = ::ePub3::CFI::SideBias::Unspecified,
	Before		= ::ePub3::CFI::SideBias::Before,
	After		= ::ePub3::CFI::SideBias::After
};

public ref class CFI sealed
{ 
	_DECLARE_BRIDGE_API_(::ePub3::CFI&, CFI^);

internal:
	static CFI^ Wrapper(::ePub3::CFI&& native);

internal:
	CFI(const ::ePub3::CFI& native);
	CFI(::ePub3::CFI&& native);

public:
	CFI();
	CFI(CFI^ base, CFI^ start, CFI^ end);
	CFI(String^ stringRepresentation);
	CFI(CFI^ otherCFI, UINT fromIndex);
	virtual ~CFI() {}

	virtual String^ ToString() override;

	property bool IsRangeTriplet { bool get(); }
	property bool IsEmpty { bool get(); }

	void Clear();

	[::Windows::Foundation::Metadata::DefaultOverloadAttribute]
	bool Equals(::Platform::Object^ obj);
	bool Equals(CFI^ otherCFI);
	bool Equals(String^ stringRepresentation);
	static bool ReferenceEquals(CFI^ a, CFI^ b) { return a->Equals(b); }

	[::Windows::Foundation::Metadata::DefaultOverloadAttribute]
	CFI^ Assign(CFI^ newValue);
	CFI^ Assign(String^ stringRepresentation);
	CFI^ Assign(CFI^ baseCFI, UINT fromIndex);

	[::Windows::Foundation::Metadata::DefaultOverloadAttribute]
	CFI^ Append(CFI^ otherCFI);
	CFI^ Append(String^ stringRepresentation);

	property CFISideBias CharacterSideBias { CFISideBias get(); }

};

END_READIUM_API

#endif	/* __Readium_CFI_h__ */
