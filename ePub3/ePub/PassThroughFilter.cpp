//
//  PassThroughFilter.cpp
//  ePub3
//
//  Created by Nelson Leme on 10/30/14.
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
//

#include <stdio.h>

#include <ePub3/base.h>

#include "PassThroughFilter.h"
#include "container.h"
#include "package.h"
#include "filter_manager.h"
#include "byte_stream.h"

#include <cmath>

EPUB3_BEGIN_NAMESPACE


bool PassThroughFilter::SniffPassThroughContent(ConstManifestItemPtr item)
{
	// This class can be used to test or debug a chain of filters. If you want to do so,
	// you can use the following commented out code for doing that. The code below will make this filter
	// apply when a media resource is being read.
	//
    // auto mediaType = item->MediaType();
    // return (mediaType == "audio/mp4" || mediaType == "audio/mpeg" || mediaType == "video/mp4" || mediaType == "video/mpeg");

	return false;
}

ContentFilterPtr PassThroughFilter::PassThroughFactory(ConstPackagePtr package)
{
    // If you want to disable the PassThroughFilter for good, just hardcode the
    // value below to be nullptr. To turn the PassThroughFilter back on, just
    // replace nullptr with New().

    // Alternatively (depending on testing needs), the SniffPassThoughContent() function above
    // can return false, in which case the PassThroughFilter will be available in the FilterChain
    // (i.e. will be invoked in the ContentFilter query loop), but will never actually be elected.

    // HOWEVER, a cleaner (more permanent) method is to edit the PopulateFilterManager() function in initialization.cpp,
    // and comment the call to PassThroughFilter::Register()

    return New();
    //return nullptr;
}

FilterContext *PassThroughFilter::InnerMakeFilterContext(ConstManifestItemPtr item) const
{
    return new PassThroughContext;
}

ByteStream::size_type PassThroughFilter::BytesAvailable(SeekableByteStream *byteStream) const
{
    return byteStream->BytesAvailable();
}

void *PassThroughFilter::FilterData(FilterContext *context, void *data, size_t len, size_t *outputLen)
{
    *outputLen = 0;
    
    PassThroughContext *ptContext = dynamic_cast<PassThroughContext *>(context);
    if (ptContext == nullptr)
    {
        return nullptr;
    }
        
    SeekableByteStream *byteStream = ptContext->GetSeekableByteStream();
    if (byteStream == nullptr)
    {
        // If no SeekableByteStream was passed to this filter via the FilterContext object,
        // it means that this filter should operate on top of the bytes that were passed in
        // the data parameter. What that means is that, in this circumstance, this filter
        // is one filter in a chain of filters.
        *outputLen = len;
        return data;
    }

    if (!byteStream->IsOpen())
    {
        return nullptr;
    }

    // The SeekableByteStream is valid. That means that this filter is acting alone, and it
    // should read directly on the SeekableByteStream to get the bytes to operate on.
    // This way, this filter (and this filter alone) can choose which bytes it needs to read
    // from the ePub resource.

    ByteStream::size_type bytesToRead = 0;
    if (!ptContext->GetByteRange().IsFullRange()) // range requests only
    {
        bytesToRead = (ByteStream::size_type)(ptContext->GetByteRange().Length());
        byteStream->Seek(ptContext->GetByteRange().Location(), std::ios::seekdir::beg);
    }
    else // whole file  only
    {
        byteStream->Seek(0, std::ios::seekdir::beg);
        bytesToRead = byteStream->BytesAvailable();
    }
    
    if (bytesToRead == 0)
    {
        return nullptr;
    }

    uint8_t *buffer = ptContext->GetAllocateTemporaryByteBuffer(bytesToRead);

    ByteStream::size_type readBytes = byteStream->ReadBytes(buffer, bytesToRead);
    *outputLen = readBytes;
    return buffer;
}

void PassThroughFilter::Register()
{
    // The PassThroughFilter is put as the very first filter in the filter chain.
    // Given that it will only return the bytes exactly as they are, this is OK because
    // it will just pass the raw bytes along.
    FilterManager::Instance()->RegisterFilter("PassThroughFilter", MustAccessRawBytes, PassThroughFactory);
}


EPUB3_END_NAMESPACE

