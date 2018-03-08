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

#ifndef __ePub3__filter_chain__
#define __ePub3__filter_chain__

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
//#include <ePub3/filter_chain_byte_stream.h>
//#include <ePub3/filter_chain_byte_stream_range.h>

EPUB3_BEGIN_NAMESPACE

class FilterContext;
class ByteRange;


class FilterChain : public PointerType<FilterChain>
#if EPUB_PLATFORM(WINRT)
    , public NativeBridge
#endif
{
public:
    typedef shared_vector<ContentFilter>    FilterList;
    
public:
    FilterChain(FilterList filters) : _filters(filters) {}
#if EPUB_COMPILER_SUPPORTS(CXX_DEFAULTED_FUNCTIONS)
    FilterChain(FilterChain&& o) : _filters(std::move(o._filters)) {}
    virtual ~FilterChain()                  = default;
    FilterChain& operator=(FilterChain&& o) {
        _filters = std::move(o._filters);
        return *this;
    }
#else
    FilterChain(FilterChain&& o) : _filters(std::move(o._filters)) {}
    virtual ~FilterChain() {}
    FilterChain& operator=(FilterChain&& o) { swap(std::move(o)); return *this; }
#endif
    
    void swap(FilterChain&& __o) { _filters.swap(__o._filters); }
    
    // obtains a stream which can be used to read filtered bytes from the chain

#ifdef SUPPORT_ASYNC
    std::shared_ptr<AsyncByteStream> GetFilteredOutputStreamForManifestItem(ConstManifestItemPtr item) const;
#endif /* SUPPORT_ASYNC */

    std::shared_ptr<ByteStream> GetFilterChainByteStream(ConstManifestItemPtr item) const;
    std::unique_ptr<ByteStream> GetFilterChainByteStream(ConstManifestItemPtr item, SeekableByteStream *rawInput) const;
    std::shared_ptr<ByteStream> GetFilterChainByteStreamRange(ConstManifestItemPtr item) const;
    std::unique_ptr<ByteStream> GetFilterChainByteStreamRange(ConstManifestItemPtr item, SeekableByteStream *rawInput) const;
    size_t GetFilterChainSize(ConstManifestItemPtr item) const;
    
protected:

#ifdef SUPPORT_ASYNC
    typedef std::shared_ptr<AsyncByteStream>    ChainLink;

    class ChainLinkProcessor : public PointerType<ChainLinkProcessor>
    {
    public:
        ChainLinkProcessor(ContentFilterPtr filter, ChainLink input, ConstManifestItemPtr manifestItem);
        ChainLinkProcessor(const ChainLinkProcessor& o) : _filter(o._filter), _context(o._context), _input(o._input), _output(o._output), _collectionBuffer(o._collectionBuffer) {}
        ChainLinkProcessor(ChainLinkProcessor&& o) : _filter(std::move(o._filter)), _context(o._context), _input(std::move(o._input)), _output(std::move(o._output)) {}
        virtual ~ChainLinkProcessor();
        
        virtual void SetOutputLink(ChainLink output) { _output = output; }
        
        virtual void ScheduleProcessor(RunLoopPtr runLoop);
        
    protected:
        ContentFilterPtr                _filter;
        std::shared_ptr<FilterContext>  _context;
        ChainLink                       _input;
        ChainLink                       _output;
        ByteBuffer                      _collectionBuffer;
        
        ssize_t FunnelBytes();
        
    };

    static
    std::unique_ptr<thread_pool>        _filterThreadPool;
#endif /* SUPPORT_ASYNC */

    private:
    FilterList              _filters;

};

EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__filter_chain__) */
