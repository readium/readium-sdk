//
//  WinCFI.h
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

#if !EPUB_PLATFORM(WIN_PHONE)
	virtual
#endif
		String^ ToString()
#if !EPUB_PLATFORM(WIN_PHONE)
		override
#endif
		;

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