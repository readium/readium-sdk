//
//  WinNavPoint.cpp
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

#include "WinNavPoint.h"
#include "CollectionBridges.h"
#include "WinNavTable.h"
#include "NavElementList.h"

using namespace ::Platform;

BEGIN_READIUM_API

_BRIDGE_API_IMPL_(::ePub3::NavigationPointPtr, NavigationPoint)

NavigationPoint::NavigationPoint(::ePub3::NavigationPointPtr native) : _native(native)
{
	_native->SetBridge(this);
}

String^ NavigationPoint::Title::get()
{
	return StringFromNative(_native->Title());
}
void NavigationPoint::Title::set(String^ value)
{
	_native->SetTitle(std::move(StringToNative(value)));
}

IVectorView<INavigationElement^>^ NavigationPoint::Children::get()
{
	return ref new NavElementList(_native->Children());
}

void NavigationPoint::AppendChild(INavigationElement^ child)
{
	auto native = NavElementToNative()(child);
	if (bool(native))
		_native->AppendChild(native);
}

String^ NavigationPoint::Content::get()
{
	return StringFromNative(_native->Content());
}
void NavigationPoint::Content::set(String^ value)
{
	_native->SetContent(std::move(StringToNative(value)));
}

INavigationElement^ NavigationPoint::Parent::get()
{
	return NavElementFromNative()(_native->Owner());
}

END_READIUM_API
