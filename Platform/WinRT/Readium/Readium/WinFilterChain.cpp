//
//  WinFilterChain.cpp
//  Readium
//
//  Created by Jim Dovey on 2013-10-10.
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
