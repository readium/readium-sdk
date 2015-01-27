//
//  filter_chain.h
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

#ifndef __ePub3__filter_chain_byte_stream__
#define __ePub3__filter_chain_byte_stream__

#include <ePub3/epub3.h>
#include <ePub3/utilities/byte_stream.h>
#include <ePub3/utilities/run_loop.h>
#include <ePub3/utilities/executor.h>
#include <ePub3/utilities/byte_buffer.h>
#include <memory>
#include <thread>
#include <condition_variable>
#include <algorithm>
#include <utility>

#include <ePub3/filter.h>

EPUB3_BEGIN_NAMESPACE

class FilterContext;
class ByteRange;

class FilterChainByteStream : public ByteStream
{
private:
    std::unique_ptr<SeekableByteStream>        _input;

    std::vector<ContentFilterPtr> m_filters;
    std::vector<std::unique_ptr<FilterContext>> m_filterContexts;
    
    bool                            _needs_cache;
    ByteBuffer                        _cache;
    ByteBuffer                        _read_cache;

private:
    FilterChainByteStream(const FilterChainByteStream& o)             _DELETED_;
    FilterChainByteStream(FilterChainByteStream&& o)                  _DELETED_;
    FilterChainByteStream&         operator=(FilterChainByteStream&)                          _DELETED_;
    FilterChainByteStream&         operator=(FilterChainByteStream&&)                         _DELETED_;

public:
    FilterChainByteStream() : ByteStream() {}
    //EPUB3_EXPORT FilterChainByteStream(std::vector<ContentFilterPtr>& filters, ConstManifestItemPtr &manifestItem);
    EPUB3_EXPORT FilterChainByteStream(std::unique_ptr<SeekableByteStream>&& input, std::vector<ContentFilterPtr>& filters, ConstManifestItemPtr manifestItem);
    virtual ~FilterChainByteStream();
    
    virtual size_type BytesAvailable() const _NOEXCEPT OVERRIDE
    {
        if (_needs_cache && _input->AtEnd()) {
            return _cache.GetBufferSize();
        } else {
            return _input->BytesAvailable();
        }
    }
    virtual size_type SpaceAvailable() const _NOEXCEPT OVERRIDE
    {
        return 0;
    }
    virtual bool IsOpen() const _NOEXCEPT OVERRIDE
    {
        return _input->IsOpen();
    }
    virtual void Close() OVERRIDE
    {
        _input->Close();
    }
    virtual size_type ReadBytes(void* bytes, size_type len) OVERRIDE;
    //virtual size_type ReadBytes(void* bytes, size_type len, ByteRange &byteRange);
    virtual size_type WriteBytes(const void* bytes, size_type len) OVERRIDE
    {
        throw std::system_error(std::make_error_code(std::errc::operation_not_supported));
    }
    
    virtual bool AtEnd() const _NOEXCEPT OVERRIDE
    {
        if (_needs_cache && _input->AtEnd()) {
            return _cache.IsEmpty();
        } else {
            return _input->AtEnd();
        }
    }
    virtual int Error() const _NOEXCEPT OVERRIDE
    {
        return _input->Error();
    }
    
private:
    size_type ReadBytesFromCache(void* bytes, size_type len);
    void CacheBytes();
    size_type FilterBytes(void* bytes, size_type len);
    //size_type FilterBytes(void* bytes, ByteRange &byteRange);
};

EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__filter_chain_byte_stream__) */
