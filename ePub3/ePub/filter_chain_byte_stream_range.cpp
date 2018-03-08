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

FilterChainByteStreamRange::~FilterChainByteStreamRange()
{
}

FilterChainByteStreamRange::FilterChainByteStreamRange(std::unique_ptr<SeekableByteStream> &&input, ContentFilterPtr filter, ConstManifestItemPtr manifestItem)
: m_input(std::move(input)), m_filter(filter), m_filterContext(filter != nullptr ? std::unique_ptr<FilterContext>(filter->MakeFilterContext(manifestItem)) : nullptr)
{
}

//FilterChainByteStreamRange::FilterChainByteStreamRange(std::unique_ptr<SeekableByteStream> &&input) : m_input(std::move(input))
//{
//}

ByteStream::size_type FilterChainByteStreamRange::BytesAvailable() _NOEXCEPT
{
    if (m_filter)
    {
        ByteStream::size_type size = m_filter->BytesAvailable(m_filterContext.get(), m_input.get());
        return size;
    }

    return m_input->BytesAvailable();
}

ByteStream::size_type FilterChainByteStreamRange::WriteBytes(const void *bytes, size_type len)
{
    throw std::system_error(std::make_error_code(std::errc::operation_not_supported));
}

ByteStream::size_type FilterChainByteStreamRange::ReadBytes(void *bytes, size_type len)
{
    ByteRange fullByteRange;
    return ReadBytes(bytes, len, fullByteRange);
}

ByteStream::size_type FilterChainByteStreamRange::ReadBytes(void *bytes, size_type len, ByteRange &byteRange)
{
    if (len == 0) return 0;

    if (byteRange.Length() == 0 && !byteRange.IsFullRange()) // invalid range
    {
        return 0;
    }
    
    if (byteRange.Length() > len && !byteRange.IsFullRange()) // too small buffer
    {
        return 0;
    }
    
    if (!m_filter)
    {
        // There are no ContentFilters that applied. In this case, the caller is just interested
        // in getting the raw bytes out of the ZIP file. So, then, just read the raw bytes.
        return ReadRawBytes(bytes, len, byteRange);
    }

    RangeFilterContext *filterContext = dynamic_cast<RangeFilterContext *>(m_filterContext.get());
    if (filterContext != nullptr)
    {
        filterContext->GetByteRange() = byteRange;
        filterContext->SetSeekableByteStream(m_input.get());
    }

    size_type filteredLen = 0;
    void *filteredData = nullptr;

    if (filterContext != nullptr) {
        filteredData = m_filter->FilterData(m_filterContext.get(), nullptr, 0, &filteredLen);
    } else {
        size_type result = m_input->ReadBytes(bytes, len);
        if (result == 0) return 0;

        ByteBuffer buf(reinterpret_cast<uint8_t*>(bytes), result);
        buf.SetUsesSecureErasure();

        filteredData = m_filter->FilterData(m_filterContext.get(), buf.GetBytes(), buf.GetBufferSize(), &filteredLen);
    }

    if (filterContext != nullptr)
    {
        filterContext->GetByteRange().Reset();
        filterContext->ResetSeekableByteStream();
    }
    
    if (filteredData == nullptr || filteredLen == 0)
    {
        if (filteredData != nullptr && filteredData != bytes)
        {
            if (filterContext == nullptr || reinterpret_cast<uint8_t*>(filteredData) != filterContext->GetCurrentTemporaryByteBuffer())
            {
                delete[] reinterpret_cast<uint8_t *>(filteredData);
            }
        }

        return 0;
    }
    
    if (filteredData != bytes)
    {
        ByteStream::size_type toCopy = std::min(len, filteredLen);
        ::memcpy_s(bytes, len, filteredData, toCopy);

        if (filteredLen > len)
        {
            //ByteStream::size_type toCache = filteredLen - len;

            //TODO cache remainder bytes for use next time round...
            // (e.g. when decrypted bytes are greater than original request?)
        }

        if (filterContext == nullptr || reinterpret_cast<uint8_t*>(filteredData) != filterContext->GetCurrentTemporaryByteBuffer())
        {
            delete[] reinterpret_cast<uint8_t *>(filteredData);
        }
    }

    return filteredLen;
}

ByteStream::size_type FilterChainByteStreamRange::ReadRawBytes(void *bytes, size_type len, ePub3::ByteRange &byteRange)
{
    if (len == 0) return 0;

    size_type bytesToRead = 0;
    if (!byteRange.IsFullRange())
    {
        m_input->Seek(byteRange.Location(), std::ios::beg);
        bytesToRead = std::min(len, (size_type)byteRange.Length());
    }
    else
    {
        m_input->Seek(0, std::ios::beg);
        if (m_input->BytesAvailable() > len)
        {
            return 0; // Buffer is not big enough to take the entire file.
        }
        bytesToRead = len;
    }

    if (bytesToRead == 0) return 0;

    return m_input->ReadBytes(bytes, bytesToRead);
}

EPUB3_END_NAMESPACE
