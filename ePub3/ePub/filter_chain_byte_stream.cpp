//
//  filter_chain.cpp
//  ePub3
//
//  Created by Jim Dovey on 2013-08-27.
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

#include "filter_chain_byte_stream.h"
#include "../ePub/manifest.h"
#include "filter.h"
#include "byte_buffer.h"
#include "make_unique.h"
#include <iostream>

#if !EPUB_OS(WINDOWS)
# define memcpy_s(dst, dstLen, src, srcLen) memcpy(dst, src, srcLen)
#endif

EPUB3_BEGIN_NAMESPACE

FilterChainByteStream::~FilterChainByteStream()
{
}

//FilterChainByteStream::FilterChainByteStream(std::vector<ContentFilterPtr>& filters, ConstManifestItemPtr &manifestItem)
//: m_filters(), m_filterContexts(), _needs_cache(false), _cache(), _read_cache()
//{
//    _input = NULL;
//    _cache.SetUsesSecureErasure();
//    _read_cache.SetUsesSecureErasure();
//
//    for (ContentFilterPtr filter : filters)
//    {
//        m_filters.push_back(filter);
//        m_filterContexts.push_back(std::unique_ptr<FilterContext>(filter->MakeFilterContext(manifestItem)));
//
//        if (filter->GetOperatingMode() == ContentFilter::OperatingMode::RequiresCompleteData && !_needs_cache)
//            _needs_cache = true;
//    }
//}

FilterChainByteStream::FilterChainByteStream(std::unique_ptr<SeekableByteStream>&& input, std::vector<ContentFilterPtr>& filters, ConstManifestItemPtr manifestItem)
: _input(std::move(input)), m_filters(), m_filterContexts(), _needs_cache(false), _cache(), _read_cache(), _cacheHasBeenFilledUp(false)
{
    _cache.SetUsesSecureErasure();
    _read_cache.SetUsesSecureErasure();

    for (ContentFilterPtr filter : filters)
    {
        m_filters.push_back(filter);
        m_filterContexts.push_back(std::unique_ptr<FilterContext>(filter->MakeFilterContext(manifestItem)));
    }
	
	// We are currently hardcoding _needs_cache to true, because we realized that this is the only way to realiably compute
	// the content length of any resource being read by FilterChainByteStream. Only by processing the raw conten of a given
	// resource through all the filters in the chaing, and storing the result in the cache, that we can reliably stablish the
	// size of the resource after being processed.
	_needs_cache = true;
}

ByteStream::size_type FilterChainByteStream::ReadBytes(void* bytes, size_type len)
{
    if (len == 0) return 0;

    if (_needs_cache)
    {
        if (_cache.GetBufferSize() == 0 && !_cacheHasBeenFilledUp)
            CacheBytes();

        return ReadBytesFromCache(bytes, len);
    }

    if (_read_cache.GetBufferSize() > 0)
    {
        size_type toMove = std::min(len, _read_cache.GetBufferSize());
        ::memcpy_s(bytes, len, _read_cache.GetBytes(), toMove);
        _read_cache.RemoveBytes(toMove);
        return toMove;
    }
    else 
    {
        if (!_input->IsOpen())
        {
            return 0;
        }

        size_type result = _input->ReadBytes(bytes, len);
        if (result == 0)
        {
            return 0;
        }

        result = FilterBytes(bytes, result);
        if (result == 0)
        {
            return 0;
        }

        size_type toMove = std::min(len, _read_cache.GetBufferSize());
        if (toMove > 0)
        {
            ::memcpy_s(bytes, len, _read_cache.GetBytes(), toMove);
            _read_cache.RemoveBytes(toMove);
        }
        return toMove;
    }
}

ByteStream::size_type FilterChainByteStream::FilterBytes(void* bytes, size_type len)
{
    if (len == 0) return 0;

    size_type result = len;
    ByteBuffer buf(reinterpret_cast<uint8_t*>(bytes), len);
    buf.SetUsesSecureErasure();

    for (int i = 0; i < m_filters.size(); i++)
    {
        ContentFilterPtr filter = m_filters.at(i);
        FilterContext * filterContext = m_filterContexts.at(i).get();

        size_type streamPos = 0;

        // A filter may support ranges, but may be invoked in a non-HTTP-byte-range scenario
        RangeFilterContext *filterContextRange = dynamic_cast<RangeFilterContext *>(filterContext);
        if (filterContextRange != nullptr)
        {
            // ASSERT i == 0 ? (decryption filter always first, can access raw byte stream directly)

            ByteRange byteRange;
            if (!_needs_cache)
            {
                // ASSERT m_filters.size() == 1 ? (decryption filter on its own might be subject to sequential consecutive byte chunks)

                if (!_input->IsOpen())
                {
                    throw std::logic_error("ChainLinkProcessor: Byte stream not open?!");
                }

                streamPos = _input->Position();

                byteRange.Location((uint32_t)(streamPos - result));
                byteRange.Length((uint32_t)result);
            }
            filterContextRange->GetByteRange() = byteRange;
            filterContextRange->SetSeekableByteStream(_input.get());
        }

        size_type filteredLen = 0;
        void *filteredData = nullptr;

        if (filterContextRange != nullptr)
        {
            filteredData = filter->FilterData(filterContext, nullptr, 0, &filteredLen);
        }
        else
        {
            filteredData = filter->FilterData(filterContext, buf.GetBytes(), buf.GetBufferSize(), &filteredLen);
        }

        if (filterContextRange != nullptr)
        {
            filterContextRange->GetByteRange().Reset();
            filterContextRange->ResetSeekableByteStream();

            if (!_needs_cache)
            {
                if (_input->IsOpen())
                {
                    _input->Seek(streamPos, std::ios::beg);
                }
            }
        }

        if (filteredData == nullptr || filteredLen == 0)
        {
            if (filteredData != nullptr && filteredData != buf.GetBytes())
            {
                if (filterContextRange == nullptr || reinterpret_cast<uint8_t*>(filteredData) != filterContextRange->GetCurrentTemporaryByteBuffer())
                {
                    delete[] reinterpret_cast<uint8_t*>(filteredData);
                }
            }

            return 0;
        }

        result = filteredLen;

        if (filteredData != buf.GetBytes())
        {
            // NOTE: destroys previous buffer, allocates new memory block! (memcpy)
            buf = ByteBuffer(reinterpret_cast<uint8_t*>(filteredData), result);

            if (filterContextRange == nullptr || reinterpret_cast<uint8_t*>(filteredData) != filterContextRange->GetCurrentTemporaryByteBuffer())
            {
                delete[] reinterpret_cast<uint8_t*>(filteredData);
            }
        }
        else if (result < buf.GetBufferSize())
        {
            buf.RemoveBytes(buf.GetBufferSize() - result, result);

            // NOTE: destroys previous buffer, allocates new memory block! (memcpy)
            //buf = ByteBuffer(reinterpret_cast<uint8_t*>(filteredData), result);
        }
        else if (result > buf.GetBufferSize())
        {
            // This should never happen! (returned more bytes than could fit!)

            // NOTE: destroys previous buffer, allocates new memory block! (memcpy)
            buf = ByteBuffer(reinterpret_cast<uint8_t*>(filteredData), result);
        }
    }
    
    _read_cache = std::move(buf);

    // ASSERT result == _read_cache.GetBufferSize()
    return result;
}

ByteStream::size_type FilterChainByteStream::ReadBytesFromCache(void* bytes, size_type len)
{
    if (len == 0) return 0;

    size_type numToRead = std::min(len, size_type(_cache.GetBufferSize()));
    ::memcpy_s(bytes, len, _cache.GetBytes(), numToRead);
    _cache.RemoveBytes(numToRead);
    return numToRead;
}

void FilterChainByteStream::CacheBytes()
{
    // read everything from the input stream
#define _TMP_BUF_LEN 16*1024
    uint8_t buf[_TMP_BUF_LEN] = {};
    while (_input->AtEnd() == false)
    {
        size_type numRead = _input->ReadBytes(buf, _TMP_BUF_LEN);
        if (numRead == 0)
            break;
        if (numRead > 0)
            _cache.AddBytes(buf, numRead);
    }

    if (_cache.GetBufferSize() == 0) return;

    // filter everything completely
    size_type filtered = FilterBytes(_cache.GetBytes(), _cache.GetBufferSize());

    if (filtered > 0)
    {
        // ASSERT filtered == _read_cache.GetBufferSize()

        _cache = std::move(_read_cache);
        _read_cache.RemoveBytes(_read_cache.GetBufferSize());
        _cacheHasBeenFilledUp = true;
    }

    // this potentially contains decrypted data, so use secure erasure
    _cache.SetUsesSecureErasure();
}

EPUB3_END_NAMESPACE
