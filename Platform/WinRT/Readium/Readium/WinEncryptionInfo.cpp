//
//  WinEncryptionInfo.cpp
//  Readium
//
//  Created by Jim Dovey on 2013-10-10.
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

#include "WinEncryptionInfo.h"
#include "WinContainer.h"

using namespace ::Platform;

BEGIN_READIUM_API

_BRIDGE_API_IMPL_(::ePub3::EncryptionInfoPtr, EncryptionInfo)

EncryptionInfo::EncryptionInfo(::ePub3::EncryptionInfoPtr native) : _native(native)
{
	_native->SetBridge(this);
}

Container^ EncryptionInfo::ParentContainer::get()
{
	return Container::Wrapper(_native->Owner());
}

String^ EncryptionInfo::Algorithm::get()
{
	return StringFromNative(_native->Algorithm());
}
void EncryptionInfo::Algorithm::set(String^ value)
{
	_native->SetAlgorithm(std::move(StringToNative(value)));
}

String^ EncryptionInfo::Path::get()
{
	return StringFromNative(_native->Path());
}
void EncryptionInfo::Path::set(String^ value)
{
	_native->SetPath(std::move(StringToNative(value)));
}

END_READIUM_API
