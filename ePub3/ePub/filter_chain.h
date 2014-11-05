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

EPUB3_BEGIN_NAMESPACE

class FilterContext;
struct ByteRange;

typedef std::pair<ContentFilterPtr, std::unique_ptr<FilterContext>>	FilterNode;

// -------------------------------------------------------------------------------------------

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
    std::shared_ptr<AsyncByteStream> GetFilteredOutputStreamForManifestItem(ConstManifestItemPtr item) const;
	std::shared_ptr<ByteStream> GetSyncFilteredOutputStreamForManifestItem(ConstManifestItemPtr item) const;
    /// Returns stream with the data only from the given range while reading. Also returns nullptr if there is no ContentFilter supporting ranges.
    std::shared_ptr<ByteStream> GetSyncFilteredByteRangeOfManifestItem(ConstManifestItemPtr item) const;
    
protected:
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
	std::unique_ptr<thread_pool>		_filterThreadPool;
    
private:
    FilterList              _filters;

};

// -------------------------------------------------------------------------------------------

class ByteRangeFilterSyncStream : public ByteStream
{
public:
    ByteRangeFilterSyncStream(std::unique_ptr<SeekableByteStream> &&input, ContentFilterPtr &filter, ConstManifestItemPtr manifestItem);
    ByteRangeFilterSyncStream(std::unique_ptr<SeekableByteStream> &&input);
    virtual ~ByteRangeFilterSyncStream() { }
    
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
    shared_ptr<FilterNode> m_filterNode;
    ByteBuffer m_readCache;
};

// -------------------------------------------------------------------------------------------

class FilterChainSyncStream : public ByteStream
{
private:
	std::unique_ptr<ByteStream>		_input;
	std::vector<FilterNode>		_filters;
    
	bool							_needs_cache;
	ByteBuffer						_cache;
	ByteBuffer						_read_cache;
    
public:
    FilterChainSyncStream(std::vector<ContentFilterPtr>& filters, ConstManifestItemPtr &manifestItem);
	FilterChainSyncStream(std::unique_ptr<ByteStream>&& input, std::vector<ContentFilterPtr>& filters, ConstManifestItemPtr manifestItem);
	virtual ~FilterChainSyncStream() {}
    
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

#endif /* defined(__ePub3__filter_chain__) */
