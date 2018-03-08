//
//  PassThroughFilter.cpp
//  ePub3
//
//  Created by Nelson Leme on 10/30/14.
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//  
//  Redistribution and use in source and binary forms, with or without modification, 
//  are permitted provided that the following conditions are met:
//  1. Redistributions of source code must retain the above copyright notice, this 
//  list of conditions and the following disclaimer.
//  2. Redistributions in binary form must reproduce the above copyright notice, 
//  this list of conditions and the following disclaimer in the documentation and/or 
//  other materials provided with the distribution.
//  3. Neither the name of the organization nor the names of its contributors may be 
//  used to endorse or promote products derived from this software without specific 
//  prior written permission.
//  
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
//  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
//  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
//  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
//  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
//  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
//  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
//  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE 
//  OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED 
//  OF THE POSSIBILITY OF SUCH DAMAGE.

#include <stdio.h>

#include <ePub3/base.h>

#include "PassThroughFilter.h"
#include "container.h"
#include "package.h"
#include "filter_manager.h"
#include "byte_stream.h"

//#include <cmath>

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

    return std::make_shared<PassThroughFilter>(); //New();
    //return nullptr;
}

FilterContext *PassThroughFilter::InnerMakeFilterContext(ConstManifestItemPtr item) const
{
    return new PassThroughContext;
}

ByteStream::size_type PassThroughFilter::BytesAvailable(FilterContext *context, SeekableByteStream *byteStream) const
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
        byteStream->Seek(ptContext->GetByteRange().Location(), std::ios::beg);
    }
    else // whole file  only
    {
        byteStream->Seek(0, std::ios::beg);
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

