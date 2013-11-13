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

#include "WinFilterManager.h"
#include "WinPackage.h"
#include "WinManifest.h"
#include "PluginMaps.h"
#include <Windows.h>
#include <thread>

BEGIN_READIUM_API

IContentFilter^ FilterManager::GetFilterByName(String^ name, Package^ package)
{
	::ePub3::ContentFilterPtr native = ::ePub3::FilterManager::Instance()->GetFilterByName(StringToNative(name), package->NativeObject);
	if (!bool(native))
		return nullptr;

	auto wrapper = std::dynamic_pointer_cast<WinRTContentFilter>(native);
	if (bool(wrapper))
		return wrapper->GetBridge<IContentFilter>();

	return ContentFilterWrapper::Wrapper(native);
}

void FilterManager::RegisterFilter(String^ name, FilterPriority priority, ContentFilterFactory^ factory)
{
	auto instance = ::ePub3::FilterManager::Instance();
	instance->RegisterFilter(StringToNative(name), ::ePub3::ContentFilter::FilterPriority(priority), [factory](::ePub3::ConstPackagePtr pkg) -> std::shared_ptr<WinRTContentFilter> {
		auto rtPkg = Package::Wrapper(std::const_pointer_cast<::ePub3::Package>(pkg));
		IContentFilter^ filter = factory(rtPkg);
		if (filter == nullptr)
			return nullptr;
		return std::make_shared<WinRTContentFilter>(filter);
	});
}

FilterChain^ FilterManager::BuildFilterChain(Package^ package)
{
	return FilterChain::Wrapper(::ePub3::FilterManager::Instance()->BuildFilterChainForPackage(package->NativeObject));
}

END_READIUM_API
