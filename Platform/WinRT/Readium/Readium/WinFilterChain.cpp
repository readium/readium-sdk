//
//  WinFilterChain.cpp
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

#include "WinFilterChain.h"
#include "WinManifest.h"
#include "PluginMaps.h"

BEGIN_READIUM_API

_BRIDGE_API_IMPL_(::ePub3::FilterChainPtr, FilterChain)

FilterChain::FilterChain(::ePub3::FilterChainPtr native) : _native(native)
{
	_native->SetBridge(this);
}
FilterChain::FilterChain(IVectorView<IContentFilter^>^ filters)
{
	::ePub3::FilterChain::FilterList list;
	list.reserve(filters->Size);

	for (IContentFilter^ filter : filters)
	{
		::ePub3::ContentFilterPtr native = GetNativeContentFilter(filter);
		if (native == nullptr)
			native = std::make_shared<WinRTContentFilter>(filter);
		list.push_back(native);
	}

	_native = ::ePub3::FilterChain::New(list);
}
IClosableStream^ FilterChain::ByteStreamForManifestItem(ManifestItem^ item)
{
	return ref new Stream(_native->GetFilteredOutputStreamForManifestItem(item->NativeObject));
}

END_READIUM_API
