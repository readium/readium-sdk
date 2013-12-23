//
//  WinNavPoint.cpp
//  Readium
//
//  Created by Jim Dovey on 2013-10-11.
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
