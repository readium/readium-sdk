//
//  WinNavTable.cpp
//  Readium
//
//  Created by Jim Dovey on 2013-10-11.
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
