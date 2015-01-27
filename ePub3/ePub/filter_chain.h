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

#ifndef __ePub3__filter_chain__
#define __ePub3__filter_chain__

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
