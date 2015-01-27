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

#ifndef __ePub3__filter_chain_byte_stream_range__
#define __ePub3__filter_chain_byte_stream_range__

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

class FilterChainByteStreamRange : public ByteStream
{

private:
    FilterChainByteStreamRange(const FilterChainByteStreamRange& o)             _DELETED_;
    FilterChainByteStreamRange(FilterChainByteStreamRange&& o)                  _DELETED_;
    FilterChainByteStreamRange&         operator=(FilterChainByteStreamRange&)                          _DELETED_;
    FilterChainByteStreamRange&         operator=(FilterChainByteStreamRange&&)                         _DELETED_;

public:
    FilterChainByteStreamRange() : ByteStream() {}
    EPUB3_EXPORT FilterChainByteStreamRange(std::unique_ptr<SeekableByteStream> &&input, ContentFilterPtr filter, ConstManifestItemPtr manifestItem);
    //EPUB3_EXPORT FilterChainByteStreamRange(std::unique_ptr<SeekableByteStream> &&input);
    virtual ~FilterChainByteStreamRange();
    
    virtual size_type BytesAvailable() const _NOEXCEPT OVERRIDE;
    virtual size_type SpaceAvailable() const _NOEXCEPT OVERRIDE { return 0; }
    virtual bool IsOpen() const _NOEXCEPT OVERRIDE { return m_input->IsOpen(); }
    virtual void Close() OVERRIDE { m_input->Close(); }
    virtual size_type ReadBytes(void *bytes, size_type len) OVERRIDE;
    virtual size_type ReadBytes(void *bytes, size_type len, ByteRange &byteRange);
    virtual size_type WriteBytes(const void *bytes, size_type len) OVERRIDE;
    virtual bool AtEnd() const _NOEXCEPT OVERRIDE { return m_input->AtEnd(); }
    virtual int Error() const _NOEXCEPT OVERRIDE { return m_input->Error(); }
    
private:
    size_type ReadRawBytes(void *bytes, size_type len, ByteRange &byteRange);
    
    unique_ptr<SeekableByteStream> m_input;

    ContentFilterPtr m_filter;
    std::unique_ptr<FilterContext> m_filterContext;

    ByteBuffer m_readCache;
};

EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__filter_chain_byte_stream_range__) */
