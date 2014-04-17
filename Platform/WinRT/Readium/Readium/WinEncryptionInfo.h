//
//  WinEncryptionInfo.h
//  Readium
//
//  Created by Jim Dovey on 2013-09-26.
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

#ifndef __Readium_EncryptionInfo_h__
#define __Readium_EncryptionInfo_h__

#include "Readium.h"
#include <ePub3/encryption.h>

BEGIN_READIUM_API

ref class Container;

using ::Platform::String;

public ref class EncryptionInfo sealed
{
	_DECLARE_BRIDGE_API_(::ePub3::EncryptionInfoPtr, EncryptionInfo^);

internal:
	EncryptionInfo(::ePub3::EncryptionInfoPtr native);

public:
	virtual ~EncryptionInfo() {}

	property Container^ ParentContainer { Container^ get(); }

	property String^ Algorithm { String^ get(); void set(String^); }
	property String^ Path { String^ get(); void set(String^); }
	
	// more to come, eventually...

};

END_READIUM_API

#endif	/* __Readium_Manifest_h__ */
