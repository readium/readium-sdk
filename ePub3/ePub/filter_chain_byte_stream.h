//
//  filter_chain.h
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

#ifndef __ePub3__filter_chain_byte_stream__
#define __ePub3__filter_chain_byte_stream__

#include <ePub3/epub3.h>
#include <ePub3/utilities/byte_stream.h>
#include <ePub3/utilities/byte_buffer.h>
#include <memory>
#include <algorithm>
#include <utility>

#if FUTURE_ENABLED
#include <thread>
#include <condition_variable>
#include <ePub3/utilities/run_loop.h>
#include <ePub3/utilities/executor.h>
#endif //FUTURE_ENABLED

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
    FilterChainByteStream() : ByteStream(), _cacheHasBeenFilledUp(false) {}
    //EPUB3_EXPORT FilterChainByteStream(std::vector<ContentFilterPtr>& filters, ConstManifestItemPtr &manifestItem);
    EPUB3_EXPORT FilterChainByteStream(std::unique_ptr<SeekableByteStream>&& input, std::vector<ContentFilterPtr>& filters, ConstManifestItemPtr manifestItem);
    virtual ~FilterChainByteStream();
    
    virtual size_type BytesAvailable() _NOEXCEPT OVERRIDE
    {
        if (_needs_cache)
		{
			if (_cache.GetBufferSize() == 0 && !_cacheHasBeenFilledUp)
			{
				CacheBytes();
			}
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
    
    bool _cacheHasBeenFilledUp;
};

EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__filter_chain_byte_stream__) */
