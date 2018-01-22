//
//  WinNavTable.cpp
//  Readium
//
//  Created by Jim Dovey on 2013-10-11.
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

#include "WinNavTable.h"
#include "CollectionBridges.h"
#include "NavElementList.h"
#include "WinNavPoint.h"

using namespace ::Platform;
using namespace ::Windows::Foundation;
using namespace ::Windows::Foundation::Collections;

BEGIN_READIUM_API

_BRIDGE_API_IMPL_(::ePub3::NavigationTablePtr, NavigationTable)

NavigationTable::NavigationTable(::ePub3::NavigationTablePtr native) : _native(native)
{
	_native->SetBridge(this);
}

String^ NavigationTable::Title::get()
{
	return StringFromNative(_native->Title());
}
void NavigationTable::Title::set(String^ value)
{
	_native->SetTitle(std::move(StringToNative(value)));
}

IVectorView<INavigationElement^>^ NavigationTable::Children::get()
{
	return ref new NavElementList(_native->Children());
}

void NavigationTable::AppendChild(INavigationElement^ child)
{
	auto native = NavElementToNative()(child);
	if (bool(native))
		_native->AppendChild(native);
}

String^ NavigationTable::Type::get()
{
	return StringFromNative(_native->Type());
}
void NavigationTable::Type::set(String^ value)
{
	_native->SetType(std::move(StringToNative(value)));
}

String^ NavigationTable::SourceHref::get()
{
	return StringFromNative(_native->SourceHref());
}
void NavigationTable::SourceHref::set(String^ value)
{
	_native->SetSourceHref(std::move(StringToNative(value)));
}

END_READIUM_API
