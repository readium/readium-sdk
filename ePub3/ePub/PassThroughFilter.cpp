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


bool PassThroughFilter::SniffPassThoughContent(ConstManifestItemPtr item)
{
    auto mediaType = item->MediaType();
    //printf("PASS THROUGH FILTER, MEDIA TYPE CHECK: %s\n", mediaType.c_str());

    // This is just for testing, feel free to configure at will.
    //return (mediaType == "audio/mp4" || mediaType == "audio/mpeg" || mediaType == "video/mp4" || mediaType == "video/mpeg");
    return false;
}

ContentFilterPtr PassThroughFilter::PassThroughFactory(ConstPackagePtr package)
{
    // If you want to disable the PassThroughFilter for good, just hardcode the
    // value below to be nullptr. To turn the PassThroughFilter back on, just
    // replace nullptr with New().

    // HOWEVER, a better method is to edit the PopulateFilterManager() function in initialization.cpp,
    // and comment the call to PassThroughFilter::Register()
    return New();
}

FilterContext *PassThroughFilter::InnerMakeFilterContext(ConstManifestItemPtr item) const
{
    return new PassThroughContext;
}

ByteStream::size_type PassThroughFilter::BytesAvailable(SeekableByteStream *byteStream) const
{
#ifdef SKIP_FAKE_DECRYPT_TEST // simulate decryption, see contentFilterChainEncode.java

    ByteStream::size_type BUFFER_SIZE = 1024 * 11; // kilo bytes
    ByteStream::size_type PADDING_BYTES = 1024 * 9; // must be less than BUFFER_SIZE

    ByteStream::size_type posRaw = byteStream->Position();
    ByteStream::size_type remainderRaw = byteStream->BytesAvailable();
    ByteStream::size_type sizeRaw = posRaw + remainderRaw;

    double nChunksPos = posRaw / (BUFFER_SIZE + PADDING_BYTES);
    ByteStream::size_type nWholeChunksPos = (ByteStream::size_type)floor(nChunksPos);
    ByteStream::size_type accumulatedPaddingPos = PADDING_BYTES * nWholeChunksPos;
    ByteStream::size_type posDecrypted = posRaw - accumulatedPaddingPos;
    double nChunksPos2 = posDecrypted / BUFFER_SIZE;
    ByteStream::size_type nWholeChunksPos2 = (ByteStream::size_type)floor(nChunksPos2);
    if (nWholeChunksPos2 > nWholeChunksPos)
    {
        posDecrypted = nWholeChunksPos2 * BUFFER_SIZE;
    }

    double nChunksEnd = sizeRaw / (BUFFER_SIZE + PADDING_BYTES);
    ByteStream::size_type nWholeChunksEnd = (ByteStream::size_type)floor(nChunksEnd);
    ByteStream::size_type accumulatedPaddingEnd = PADDING_BYTES * nWholeChunksEnd;
    ByteStream::size_type sizeDecrypted = sizeRaw - accumulatedPaddingEnd;
    double nChunksEnd2 = sizeDecrypted / BUFFER_SIZE;
    ByteStream::size_type nWholeChunksEnd2 = (ByteStream::size_type)floor(nChunksEnd2);
    if (nWholeChunksEnd2 > nWholeChunksEnd)
    {
        sizeDecrypted = nWholeChunksEnd2 * BUFFER_SIZE;
    }

    return sizeDecrypted - posDecrypted;
#else
    return byteStream->BytesAvailable();
#endif
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
        printf("BYTE STREAM CLOSED ??!\n");
        return nullptr;
    }

    // The SeekableByteStream is valid. That means that this filter is acting alone, and it
    // should read directly on the SeekableByteStream to get the bytes to operate on.
    // This way, this filter (and this filter alone) can choose which bytes it needs to read
    // from the ePub resource.

#ifdef SKIP_FAKE_DECRYPT_TEST // simulate decryption, see contentFilterChainEncode.java

    ByteStream::size_type BUFFER_SIZE = 1024 * 11; // kilo bytes
    ByteStream::size_type PADDING_BYTES = 1024 * 9; // must be less than BUFFER_SIZE

    if (!byteStream->IsOpen())
    {
        printf("BYTE STREAM CLOSED 1!\n");
        return nullptr;
    }
    byteStream->Seek(0, std::ios::seekdir::beg);
    ByteStream::size_type totalRawBytesAvailable = byteStream->BytesAvailable();
    ByteStream::size_type totalDecryptedBytesAvailable = this->BytesAvailable(byteStream);
//printf("===== totalRawBytesAvailable: %d\n", totalRawBytesAvailable);
//printf("===== totalDecryptedBytesAvailable: %d\n", totalDecryptedBytesAvailable);

    ByteStream::size_type bytesToReadInDecrypted = (ByteStream::size_type)(ptContext->GetByteRange().IsFullRange() ? totalDecryptedBytesAvailable : ptContext->GetByteRange().Length());
    if (bytesToReadInDecrypted == 0)
    {
        return nullptr;
    }

//printf("bytesToReadInDecrypted: %d\n", bytesToReadInDecrypted);

    ByteStream::size_type beginOffsetInDecrypted = (ByteStream::size_type)(ptContext->GetByteRange().IsFullRange() ? 0 : ptContext->GetByteRange().Location());
    double nChunks_begin = beginOffsetInDecrypted / BUFFER_SIZE;
    ByteStream::size_type nWholeChunks_begin = (ByteStream::size_type)floor(nChunks_begin);
    ByteStream::size_type accumulatedPadding_begin = PADDING_BYTES * nWholeChunks_begin;
    ByteStream::size_type beginOffsetInRaw = beginOffsetInDecrypted + accumulatedPadding_begin;

//printf("--- beginOffsetInRaw: %d\n", beginOffsetInRaw);

    if (beginOffsetInRaw >= totalRawBytesAvailable)
    {
        printf("--- beginOffsetInRaw overflow!\n");
        return nullptr;
    }


    ByteStream::size_type endOffsetInDecrypted = beginOffsetInDecrypted + bytesToReadInDecrypted;
    double nChunks_end = endOffsetInDecrypted / BUFFER_SIZE;
    ByteStream::size_type nWholeChunks_end = (ByteStream::size_type)floor(nChunks_end);
    ByteStream::size_type accumulatedPadding_end = PADDING_BYTES * nWholeChunks_end;
    ByteStream::size_type endOffsetInRaw = endOffsetInDecrypted + accumulatedPadding_end;

    if (endOffsetInRaw >= totalRawBytesAvailable)
    {
        printf("--- endOffsetInRaw overflow!\n");
        endOffsetInRaw = totalRawBytesAvailable - 1;
    }

    ByteStream::size_type bytesToReadInRaw = endOffsetInRaw - beginOffsetInRaw;
//printf("bytesToReadInRaw: %d\n", bytesToReadInRaw);

    uint8_t *buffer = new uint8_t[bytesToReadInDecrypted];

    ByteStream::size_type totalReadInDecrypted = 0;
    ByteStream::size_type totalReadInRaw = 0;

    ByteStream::size_type currentBeginRaw = beginOffsetInRaw;
    ByteStream::size_type currentBeginWholeChunks = nWholeChunks_begin;


    if (!byteStream->IsOpen())
    {
        printf("BYTE STREAM CLOSED 2!\n");
        return nullptr;
    }
    byteStream->Seek(currentBeginRaw, std::ios::seekdir::beg);

    while (totalReadInDecrypted < bytesToReadInDecrypted)
    {
        bool toSkip = false;
        int distanceToNext = ((currentBeginWholeChunks + 1) * (BUFFER_SIZE + PADDING_BYTES)) - currentBeginRaw - PADDING_BYTES;
        ByteStream::size_type bytesToProcess = 0;
        if (distanceToNext <= 0) {
            toSkip = true;
            bytesToProcess = (ByteStream::size_type)(PADDING_BYTES + distanceToNext);

//printf("bytesToProcess (SKIP): %d\n", bytesToProcess);
        }
        else {
            toSkip = false;
            bytesToProcess = (ByteStream::size_type)distanceToNext;

//printf("bytesToProcess (READ): %d\n", bytesToProcess);
        }

        if (!byteStream->IsOpen())
        {
            printf("BYTE STREAM CLOSED 3!\n");
            break;
        }

        ByteStream::size_type currentRawBytesAvailable = byteStream->BytesAvailable();
//printf("currentRawBytesAvailable: %d\n", currentRawBytesAvailable);

        ByteStream::size_type currentDecryptedBytesAvailable = this->BytesAvailable(byteStream);
//printf("currentDecryptedBytesAvailable: %d\n", currentDecryptedBytesAvailable);

        ByteStream::size_type remainderToRead = bytesToReadInDecrypted - totalReadInDecrypted;
//printf("remainderToRead: %d\n", remainderToRead);

        ByteStream::size_type chunkToRead = std::min(bytesToProcess, remainderToRead);
        chunkToRead = std::min(chunkToRead, currentRawBytesAvailable);

//printf("chunkToRead (adjusted): %d\n", chunkToRead);

        if (chunkToRead <= 0)
        {
            break;
        }

        ByteStream::size_type readBytes = byteStream->ReadBytes(buffer + totalReadInDecrypted, chunkToRead);

//printf("readBytes: %d\n", readBytes);

        if (readBytes <= 0)
        {
            break;
        }

        totalReadInRaw += readBytes;
        if (!toSkip)
        {
            totalReadInDecrypted += readBytes;
        }

        currentBeginRaw += readBytes;
        double nChunks = currentBeginRaw / (BUFFER_SIZE + PADDING_BYTES);
        currentBeginWholeChunks = (ByteStream::size_type)floor(nChunks);

    }

//printf("~~~~~~~~~~~~~~~~~~~ totalReadInDecrypted: %d\n", totalReadInDecrypted);
//printf("~~~~~~~~~~~~~~~~~~~ totalReadInRaw: %d\n", totalReadInRaw);

    *outputLen = totalReadInDecrypted;
    return buffer;

#else

    ByteStream::size_type bytesToRead = 0;
    if (!ptContext->GetByteRange().IsFullRange()) // range requests only
    {
        bytesToRead = (ByteStream::size_type)(ptContext->GetByteRange().Length());

        byteStream->Seek(0, std::ios::seekdir::beg);
//printf("==== READ: %d - %d (%d) / %d\n", ptContext->GetByteRange().Location(), ptContext->GetByteRange().Location() + bytesToRead, bytesToRead, byteStream->BytesAvailable());

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

    uint8_t *buffer = new uint8_t[bytesToRead];
    ByteStream::size_type readBytes = byteStream->ReadBytes(buffer, bytesToRead);

    //byteStream->Seek(0, std::ios::seekdir::beg);

    *outputLen = readBytes;
    return buffer;

#endif
}

void PassThroughFilter::Register()
{
    // The PassThroughFilter is put as the very first filter in the filter chain.
    // Given that it will only return the bytes exactly as they are, this is OK because
    // it will just pass the raw bytes along.
    FilterManager::Instance()->RegisterFilter("PassThroughFilter", MustAccessRawBytes, PassThroughFactory);
}


EPUB3_END_NAMESPACE

