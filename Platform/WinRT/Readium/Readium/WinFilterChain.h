//
//  WinFilterChain.h
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

#ifndef __Readium_FilterChain_h__
#define __Readium_FilterChain_h__

#include "Readium.h"
#include "IContentFilter.h"
#include "Streams.h"
#include <ePub3/filter_chain.h>

using ::Windows::Foundation::Collections::IVectorView;

BEGIN_READIUM_API

ref class ManifestItem;

public ref class FilterChain sealed
{
	_DECLARE_BRIDGE_API_(::ePub3::FilterChainPtr, FilterChain^);

internal:
	FilterChain(::ePub3::FilterChainPtr native);

public:
	FilterChain(IVectorView<IContentFilter^>^ filters);

	IClosableStream^ ByteStreamForManifestItem(ManifestItem^ item);

};

END_READIUM_API

#endif	/* __Readium_FilterChain_h__ */
