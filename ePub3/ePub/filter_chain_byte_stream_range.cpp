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

FilterChainByteStreamRange::FilterChainByteStreamRange(std::unique_ptr<SeekableByteStream> &&input, ContentFilterPtr &filter, ConstManifestItemPtr manifestItem) : m_input(std::move(input)), m_filterNode(new FilterNode(filter, std::unique_ptr<FilterContext>(filter->MakeFilterContext(manifestItem))))
{
}

FilterChainByteStreamRange::FilterChainByteStreamRange(std::unique_ptr<SeekableByteStream> &&input) : m_input(std::move(input))
{
}

ByteStream::size_type FilterChainByteStreamRange::BytesAvailable() const _NOEXCEPT
{
    if (m_filterNode)
    {
        ByteStream::size_type size = m_filterNode->first->BytesAvailable(m_input.get());
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
    
    if (!m_filterNode)
    {
        // There are no ContentFilters that applied. In this case, the caller is just interested
        // in getting the raw bytes out of the ZIP file. So, then, just read the raw bytes.
        return ReadRawBytes(bytes, len, byteRange);
    }

    RangeFilterContext *filterContext = dynamic_cast<RangeFilterContext *>(m_filterNode->second.get());
    if (filterContext != nullptr)
    {
        filterContext->GetByteRange() = byteRange;
        filterContext->SetSeekableByteStream(m_input.get());
    }

    size_type filteredLen = 0;
    void *filteredData = nullptr;

    if (filterContext != nullptr) {
        filteredData = m_filterNode->first->FilterData(m_filterNode->second.get(), nullptr, 0, &filteredLen);
    } else {
        size_type result = m_input->ReadBytes(bytes, len);
        if (result == 0) return 0;
        len = result;
        ByteBuffer buf(reinterpret_cast<uint8_t*>(bytes), len);
        buf.SetUsesSecureErasure();

        filteredData = m_filterNode->first->FilterData(m_filterNode->second.get(), buf.GetBytes(), buf.GetBufferSize(), &filteredLen);
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
            if (filterContext != nullptr && reinterpret_cast<uint8_t*>(filteredData) != filterContext->GetCurrentTemporaryByteBuffer())
            {
                delete[] reinterpret_cast<uint8_t *>(filteredData);
            }
        }

        return 0;
    }
    
    if (filteredData != bytes)
    {
        ::memcpy_s(bytes, len, filteredData, filteredLen);

        if (filterContext != nullptr && reinterpret_cast<uint8_t*>(filteredData) != filterContext->GetCurrentTemporaryByteBuffer())
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
        m_input->Seek(byteRange.Location(),
#if EPUB_OS(ANDROID)
std::ios::beg
#else
std::ios::seekdir::beg
#endif
            );
        bytesToRead = std::min(len, (size_type)byteRange.Length());
    }
    else
    {
        m_input->Seek(0,
#if EPUB_OS(ANDROID)
std::ios::beg
#else
std::ios::seekdir::beg
#endif
        );
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
