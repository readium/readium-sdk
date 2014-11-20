//
//  filter_chain.cpp
//  ePub3
//
//  Created by Jim Dovey on 2013-08-27.
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

#include "filter_chain.h"
#include "filter_chain_byte_stream.h"
#include "filter_chain_byte_stream_range.h"
#include "../ePub/manifest.h"
#include "filter.h"
#include "byte_buffer.h"
#include "make_unique.h"
#include <iostream>

#if !EPUB_OS(WINDOWS)
# define memcpy_s(dst, dstLen, src, srcLen) memcpy(dst, src, srcLen)
#endif

EPUB3_BEGIN_NAMESPACE

std::shared_ptr<ByteStream> FilterChain::GetFilterChainByteStream(ConstManifestItemPtr item) const
{
	std::unique_ptr<ByteStream> rawInput = item->Reader();
	if (rawInput->IsOpen() == false)
    {
		return nullptr;
    }
    
    return shared_ptr<ByteStream>(GetFilterChainByteStream(item, rawInput.release()).release());
}

std::unique_ptr<ByteStream> FilterChain::GetFilterChainByteStream(ConstManifestItemPtr item, ByteStream *rawInput) const
{
    std::vector<ContentFilterPtr> thisChain;
    for (ContentFilterPtr filter : _filters)
    {
        if (filter->TypeSniffer()(item))
            thisChain.push_back(filter);
    }
    
    unique_ptr<ByteStream> rawInputPtr(rawInput);
    return unique_ptr<FilterChainByteStream>(new FilterChainByteStream(std::move(rawInputPtr), thisChain, item));
}

std::shared_ptr<ByteStream> FilterChain::GetFilterChainByteStreamRange(ConstManifestItemPtr item) const
{
    unique_ptr<SeekableByteStream> byteStream(dynamic_cast<SeekableByteStream *>(item->Reader().release()));
    if (!byteStream)
    {
        return nullptr;
    }
    
    return shared_ptr<ByteStream>(GetFilterChainByteStreamRange(item, byteStream.release()).release());
}

std::unique_ptr<ByteStream> FilterChain::GetFilterChainByteStreamRange(ConstManifestItemPtr item, SeekableByteStream *rawInput) const
{
    unique_ptr<ByteStream> resultStream;
    uint nFilters = 0;
    for (ContentFilterPtr filter : _filters)
    {
        if (filter->TypeSniffer()(item))
        {
            nFilters++;
            if (nFilters > 1)
            {
                continue;
            }
            
            if (filter->GetOperatingMode() == ContentFilter::OperatingMode::SupportsByteRanges)
            {
                unique_ptr<SeekableByteStream> rawInputPtr(rawInput);
                resultStream.reset(new FilterChainByteStreamRange(std::move(rawInputPtr), filter, item));
            }
        }
    }
    
    if (nFilters > 1)
    {
        // more than one filter...abort!
        return nullptr;
    }
    
    // There are no ContentFilter classes that curretly apply.
    // In this case, return an empty FilterChainByteStreamRange, that will simply put out raw bytes.
    if (!resultStream)
    {
        unique_ptr<SeekableByteStream> rawInputPtr(rawInput);
        resultStream.reset(new FilterChainByteStreamRange(std::move(rawInputPtr)));
    }
    
    return resultStream;
}

size_t FilterChain::GetFilterChainSize(ConstManifestItemPtr item) const
{
    size_t numFilters = 0;
    
    for (ContentFilterPtr filter : _filters)
    {
        if (filter->TypeSniffer()(item))
        {
            numFilters++;
        }
    }

    return numFilters;
}

EPUB3_END_NAMESPACE
