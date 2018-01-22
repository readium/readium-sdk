//
//  WinEncryptionInfo.cpp
//  Readium
//
//  Created by Jim Dovey on 2013-10-10.
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
