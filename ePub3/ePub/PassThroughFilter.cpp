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

#define MINIZ_NO_ZLIB_COMPATIBLE_NAMES
#include "miniz.c"
//extern int mz_uncompress(unsigned char *pDest, unsigned long *pDest_len, const unsigned char *pSource, unsigned long source_len);

EPUB3_BEGIN_NAMESPACE


bool PassThroughFilter::SniffPassThroughContent(ConstManifestItemPtr item)
{
	// This class can be used to test or debug a chain of filters. If you want to do so,
	// you can use the following commented out code for doing that. The code below will make this filter
	// apply when a media resource is being read.
	//
auto mediaType = item->MediaType();
    // return (mediaType == "audio/mp4" || mediaType == "audio/mpeg" || mediaType == "video/mp4" || mediaType == "video/mpeg");
    if (mediaType == "application/xhtml+xml" || mediaType == "text/html")
    {
        bool isCrypt = false;

        IRI iri1 = IRI("http://github.com/danielweck#CRYPT");
        if (item->ContainsProperty(iri1, false))
        {
            PropertyPtr prop1 = item->PropertyMatching(iri1, false);
            if (prop1->Value() == "TRUE")
                isCrypt = true;
        }

        IRI iri2 = IRI("http://github.com/danielweck/cryptz");
        if (item->ContainsProperty(iri2, false))
        {
            PropertyPtr prop2 = item->PropertyMatching(iri2, false);
            if (prop2->Value() == "true")
                isCrypt = true;
        }

// Custom properties values are not allowed.
//        if (item->HasProperty("http://github.com/danielweck#encrypted"))
//        {
//            isCrypt = true;
//        }

        if (isCrypt)
        {
            return true;
        }
    }

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
//    ByteStream::size_type m_totalRawBytesAvailable = 0;
//    ByteStream::size_type m_totalDecryptedBytesAvailable = 0;
//
//    ByteStream::size_type BUFFER_SIZE = 1024 * 1; // kilo bytes
//    ByteStream::size_type PADDING_BYTES = (ByteStream::size_type)(1024 * 0.5f); // must be less than BUFFER_SIZE


    ByteStream::size_type PassThroughFilter::BytesAvailable(SeekableByteStream *byteStream) const
    {
        ByteStream::size_type bytesAvailable = byteStream->BytesAvailable();

//        // upper bound estimate of deflate/compress data
//        mz_ulong compressedSize = deflateBound(NULL, (mz_ulong)bytesAvailable);
//        mz_ulong compressedSize_ = compressBound((mz_ulong)bytesAvailable);

        return bytesAvailable * 1032; // upper bound estimate of inflate/decompress data

//        ByteStream::size_type posRaw = byteStream->Position();
//        ByteStream::size_type remainderRaw = byteStream->BytesAvailable();
//        ByteStream::size_type sizeRaw = posRaw + remainderRaw;
//
//        double nChunksPos = posRaw / (BUFFER_SIZE + PADDING_BYTES);
//        ByteStream::size_type nWholeChunksPos = (ByteStream::size_type)floor(nChunksPos);
//        ByteStream::size_type accumulatedPaddingPos = PADDING_BYTES * nWholeChunksPos;
//        ByteStream::size_type posDecrypted = posRaw - accumulatedPaddingPos;
//        double nChunksPos2 = posDecrypted / BUFFER_SIZE;
//        ByteStream::size_type nWholeChunksPos2 = (ByteStream::size_type)floor(nChunksPos2);
//        if (nWholeChunksPos2 > nWholeChunksPos)
//        {
//            posDecrypted = nWholeChunksPos2 * BUFFER_SIZE;
//        }
//
//        double nChunksEnd = sizeRaw / (BUFFER_SIZE + PADDING_BYTES);
//        ByteStream::size_type nWholeChunksEnd = (ByteStream::size_type)floor(nChunksEnd);
//        ByteStream::size_type accumulatedPaddingEnd = PADDING_BYTES * nWholeChunksEnd;
//        ByteStream::size_type sizeDecrypted = sizeRaw - accumulatedPaddingEnd;
//        double nChunksEnd2 = sizeDecrypted / BUFFER_SIZE;
//        ByteStream::size_type nWholeChunksEnd2 = (ByteStream::size_type)floor(nChunksEnd2);
//        if (nWholeChunksEnd2 > nWholeChunksEnd)
//        {
//            sizeDecrypted = nWholeChunksEnd2 * BUFFER_SIZE;
//        }
//
//        return sizeDecrypted - posDecrypted;
    }

    mz_stream stream;

    void *PassThroughFilter::FilterData(FilterContext *context, void *data, size_t len, size_t *outputLen) {
        *outputLen = 0;

        PassThroughContext *ptContext = dynamic_cast<PassThroughContext *>(context);
        if (ptContext == nullptr) {
            return nullptr;
        }

        SeekableByteStream *byteStream = ptContext->GetSeekableByteStream();
        if (byteStream == nullptr) {
            *outputLen = len;
            return data;
        }

        if (!byteStream->IsOpen()) {
            printf("BYTE STREAM CLOSED 1!\n");
            return nullptr;
        }

        byteStream->Seek(0, std::ios::seekdir::beg);
        ByteStream::size_type bytesAvailable = byteStream->BytesAvailable();

        unsigned long readSize = bytesAvailable;
        if (!ptContext->GetByteRange().IsFullRange()) {
            ByteStream::size_type pos = ptContext->GetByteRange().Location();
            pos = std::min(pos, byteStream->Position() + byteStream->BytesAvailable());
            byteStream->Seek(pos, std::ios::seekdir::beg);

            if (byteStream->AtEnd()) {
                readSize = 0;
            }
            else {
                readSize = std::min((ByteStream::size_type) ptContext->GetByteRange().Length(), byteStream->BytesAvailable());
            }
        }

        if (readSize <= 0) {
            return nullptr;
        }

        uint8_t *readBuffer = new uint8_t[readSize];
        ByteStream::size_type actuallyRead = byteStream->ReadBytes(readBuffer, readSize);
        //ASSERT actuallyRead == readSize

        if (actuallyRead <= 0) {
            return nullptr;
        }


//        // upper bound estimate of deflate/compress data
//        mz_ulong compressedSize = deflateBound(NULL, (mz_ulong)bytesAvailable);
//        mz_ulong compressedSize_ = compressBound((mz_ulong)bytesAvailable);

        unsigned long uncompressedSize = actuallyRead * 1032; // upper bound estimate of inflate/decompress data


        bool m_streamFirst = ptContext->GetCurrentTemporaryByteBuffer() == nullptr;

        //unsigned char
        uint8_t *buffer = ptContext->GetAllocateTemporaryByteBuffer(uncompressedSize);

        //int mz_uncompress(unsigned char *pDest, mz_ulong *pDest_len, const unsigned char *pSource, mz_ulong source_len)
        //int res = mz_uncompress(buffer, &uncompressedSize, readBuffer, actuallyRead);

        // In case mz_ulong is 64-bits (argh I hate longs).
        if ((actuallyRead | uncompressedSize) > 0xFFFFFFFFU) {
            return nullptr;
        }

        if (m_streamFirst) {
            memset(&stream, 0, sizeof(stream));
        }

        stream.next_in = readBuffer;
        stream.avail_in = (mz_uint32) actuallyRead;
        stream.next_out = buffer;
        stream.avail_out = (mz_uint32) uncompressedSize;

        int res = MZ_OK;

        int status = MZ_OK;
        if (m_streamFirst) {
            status = mz_inflateInit2(&stream, -MZ_DEFAULT_WINDOW_BITS);
        }
        if (status != MZ_OK) {
            res = MZ_PARAM_ERROR;
        }

        status = mz_inflate(&stream, MZ_SYNC_FLUSH); //MZ_FULL_FLUSH //MZ_NO_FLUSH); //MZ_FINISH);

        uncompressedSize = uncompressedSize - stream.avail_out;

        if (status == MZ_OK)
        {

        }
        else if ((status == Z_STREAM_END) || (!stream.avail_out))
        {
            res = mz_inflateEnd(&stream);
        }
        else
        {
            res = mz_inflateEnd(&stream);
            res = ((status == MZ_BUF_ERROR) && (!stream.avail_in)) ? MZ_DATA_ERROR : status;
        }

        delete [] readBuffer;

        if (res != MZ_OK && res != MZ_STREAM_END)
        {
            return nullptr;
        }

//        for (int i = 0; i < uncompressedSize; i++)
//        {
//            printf("%c", buffer[i] == ' ' ? '_' : (buffer[i] == '\0' ? '@' : buffer[i]));
//        }
//        printf("\n<<-- %d -->>\n", uncompressedSize);

        *outputLen = uncompressedSize;
        return buffer;

//
//        if (m_totalRawBytesAvailable == 0 || m_totalDecryptedBytesAvailable == 0) byteStream->Seek(0, std::ios::seekdir::beg);
//        if (m_totalRawBytesAvailable == 0) m_totalRawBytesAvailable = byteStream->BytesAvailable();
//        if (m_totalDecryptedBytesAvailable == 0) m_totalDecryptedBytesAvailable = this->BytesAvailable(byteStream);
//
////printf("===== totalRawBytesAvailable: %d\n", totalRawBytesAvailable);
////printf("===== totalDecryptedBytesAvailable: %d\n", totalDecryptedBytesAvailable);
//
//        ByteStream::size_type bytesToReadInDecrypted = (ByteStream::size_type)(ptContext->GetByteRange().IsFullRange() ? m_totalDecryptedBytesAvailable : ptContext->GetByteRange().Length());
//        if (bytesToReadInDecrypted == 0)
//        {
//            return nullptr;
//        }
//
////printf("bytesToReadInDecrypted: %d\n", bytesToReadInDecrypted);
//
//        ByteStream::size_type beginOffsetInDecrypted = (ByteStream::size_type)(ptContext->GetByteRange().IsFullRange() ? 0 : ptContext->GetByteRange().Location());
//        double nChunks_begin = beginOffsetInDecrypted / BUFFER_SIZE;
//        ByteStream::size_type nWholeChunks_begin = (ByteStream::size_type)floor(nChunks_begin);
//        ByteStream::size_type accumulatedPadding_begin = PADDING_BYTES * nWholeChunks_begin;
//        ByteStream::size_type beginOffsetInRaw = beginOffsetInDecrypted + accumulatedPadding_begin;
//
////printf("--- beginOffsetInRaw: %d\n", beginOffsetInRaw);
//
//        if (beginOffsetInRaw > (m_totalRawBytesAvailable-1))
//        {
////printf("--- beginOffsetInRaw overflow! %d - %d\n", beginOffsetInRaw, m_totalRawBytesAvailable);
//            return nullptr;
//        }
//
//
//        ByteStream::size_type endOffsetInDecrypted = beginOffsetInDecrypted + bytesToReadInDecrypted;
//        double nChunks_end = endOffsetInDecrypted / BUFFER_SIZE;
//        ByteStream::size_type nWholeChunks_end = (ByteStream::size_type)floor(nChunks_end);
//        ByteStream::size_type accumulatedPadding_end = PADDING_BYTES * nWholeChunks_end;
//        ByteStream::size_type endOffsetInRaw = endOffsetInDecrypted + accumulatedPadding_end;
//
//        if (endOffsetInRaw > (m_totalRawBytesAvailable-1))
//        {
////printf("--- endOffsetInRaw overflow! %d - %d\n", endOffsetInRaw, m_totalRawBytesAvailable);
//            endOffsetInRaw = m_totalRawBytesAvailable - 1;
//        }
//
//        ByteStream::size_type bytesToReadInRaw = endOffsetInRaw - beginOffsetInRaw;
////printf("bytesToReadInRaw: %d\n", bytesToReadInRaw);
//
//        uint8_t * buffer = ptContext->GetAllocateTemporaryByteBuffer(bytesToReadInDecrypted);
//
//        ByteStream::size_type totalReadInDecrypted = 0;
//        ByteStream::size_type totalReadInRaw = 0;
//
//        ByteStream::size_type currentBeginRaw = beginOffsetInRaw;
//        ByteStream::size_type currentBeginWholeChunks = nWholeChunks_begin;
//
//
//        if (!byteStream->IsOpen())
//        {
//            printf("BYTE STREAM CLOSED 2!\n");
//            return nullptr;
//        }
//        byteStream->Seek(currentBeginRaw, std::ios::seekdir::beg);
//
//        while (totalReadInDecrypted < bytesToReadInDecrypted)
//        {
//            bool toSkip = false;
//            int distanceToNext = ((currentBeginWholeChunks + 1) * (BUFFER_SIZE + PADDING_BYTES)) - currentBeginRaw - PADDING_BYTES;
//            ByteStream::size_type bytesToProcess = 0;
//            if (distanceToNext <= 0) {
//                toSkip = true;
//                bytesToProcess = (ByteStream::size_type)(PADDING_BYTES + distanceToNext);
//
////printf("bytesToProcess (SKIP): %d\n", bytesToProcess);
//            }
//            else {
//                toSkip = false;
//                bytesToProcess = (ByteStream::size_type)distanceToNext;
//
////printf("bytesToProcess (READ): %d\n", bytesToProcess);
//            }
//
//            if (!byteStream->IsOpen())
//            {
//                printf("BYTE STREAM CLOSED 3!\n");
//                break;
//            }
//
//            ByteStream::size_type currentRawBytesAvailable = byteStream->BytesAvailable();
////printf("currentRawBytesAvailable: %d\n", currentRawBytesAvailable);
//
//            ByteStream::size_type currentDecryptedBytesAvailable = this->BytesAvailable(byteStream);
////printf("currentDecryptedBytesAvailable: %d\n", currentDecryptedBytesAvailable);
//
//            ByteStream::size_type remainderToRead = bytesToReadInDecrypted - totalReadInDecrypted;
////printf("remainderToRead: %d\n", remainderToRead);
//
//            ByteStream::size_type chunkToRead = std::min(bytesToProcess, remainderToRead);
//            chunkToRead = std::min(chunkToRead, currentRawBytesAvailable);
//
////printf("chunkToRead (adjusted): %d\n", chunkToRead);
//
//            if (chunkToRead <= 0)
//            {
//                break;
//            }
//
//            ByteStream::size_type readBytes = byteStream->ReadBytes(buffer + totalReadInDecrypted, chunkToRead);
//
////printf("readBytes: %d\n", readBytes);
//
//            if (readBytes <= 0)
//            {
//                break;
//            }
//
//            totalReadInRaw += readBytes;
//            if (!toSkip)
//            {
//                totalReadInDecrypted += readBytes;
//            }
//
//            currentBeginRaw += readBytes;
//            double nChunks = currentBeginRaw / (BUFFER_SIZE + PADDING_BYTES);
//            currentBeginWholeChunks = (ByteStream::size_type)floor(nChunks);
//
//        }
//
////printf("~~~~~~~~~~~~~~~~~~~ totalReadInDecrypted: %d\n", totalReadInDecrypted);
////printf("~~~~~~~~~~~~~~~~~~~ totalReadInRaw: %d\n", totalReadInRaw);
//
//        *outputLen = totalReadInDecrypted;
//
//        return buffer;
    }

void PassThroughFilter::Register()
{
    // The PassThroughFilter is put as the very first filter in the filter chain.
    // Given that it will only return the bytes exactly as they are, this is OK because
    // it will just pass the raw bytes along.
    FilterManager::Instance()->RegisterFilter("PassThroughFilter", MustAccessRawBytes, PassThroughFactory);
}


EPUB3_END_NAMESPACE

