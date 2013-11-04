//
//  WinFilterManager.h
//  Readium
//
//  Created by Jim Dovey on 2013-10-30.
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

#pragma once

#include "Readium.h"
#include "IContentFilter.h"
#include "WinFilterChain.h"
#include <ePub3/filter_manager.h>

using ::Platform::String;

BEGIN_READIUM_API

ref class Package;
ref class ManifestItem;

public ref class FilterManager sealed
{
private:
	FilterManager() {}

public:
	virtual ~FilterManager() {}

	static IContentFilter^ GetFilterByName(String^ name, Package^ package);
	static void RegisterFilter(String^ name, FilterPriority priority, ContentFilterFactory^ factory);

	static FilterChain^ BuildFilterChain(Package^ package);

};

END_READIUM_API
