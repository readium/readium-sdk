//
//  filter_chain.h
//  ePub3
//
//  Created by Jim Dovey on 2013-08-27.
//  Copyright (c) 2013 The Readium Foundation and contributors. All rights reserved.
//

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

EPUB3_BEGIN_NAMESPACE

class FilterContext;

class FilterChain : public PointerType<FilterChain>
{
public:
    typedef shared_vector<ContentFilter>    FilterList;
    
public:
    FilterChain(FilterList filters) : _filters(filters) {}
#if EPUB_COMPILER_SUPPORTS(CXX_DEFAULTED_FUNCTIONS)
    FilterChain(FilterChain&& o)            = default;
    virtual ~FilterChain()                  = default;
    FilterChain& operator=(FilterChain&& o) = default;
#else
    FilterChain(FilterChain&& o) : _filters(std::move(o._filters)) {}
    virtual ~FilterChain() {}
    FilterChain& operator=(FilterChain&& o) { swap(std::move(o)); return *this; }
#endif
    
    void swap(FilterChain&& __o) { _filters.swap(__o._filters); }
    
    // obtains a stream which can be used to read filtered bytes from the chain
    std::shared_ptr<AsyncByteStream> GetFilteredOutputStreamForManifestItem(ConstManifestItemPtr item) const;
    
protected:
    typedef std::shared_ptr<AsyncByteStream>    ChainLink;
    
    class ChainLinkProcessor
    {
    public:
        ChainLinkProcessor(ContentFilterPtr filter, ChainLink input);
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
        
    };
    
    static thread_pool      _filterThreadPool;
    
private:
    FilterList              _filters;

};

EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__filter_chain__) */
