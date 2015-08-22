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

#include "filter_chain.h"
#include "filter_chain_byte_stream.h"
#include "filter_chain_byte_stream_range.h"
#include "../ePub/manifest.h"
#include "filter.h"
#include "byte_buffer.h"
#include "make_unique.h"
#include <iostream>

#define ASYNC_BUF_SIZE 4096*4

#if !EPUB_OS(WINDOWS)
# define memcpy_s(dst, dstLen, src, srcLen) memcpy(dst, src, srcLen)
#endif

EPUB3_BEGIN_NAMESPACE

#ifdef SUPPORT_ASYNC
std::unique_ptr<thread_pool> FilterChain::_filterThreadPool(nullptr);

std::shared_ptr<AsyncByteStream> FilterChain::GetFilteredOutputStreamForManifestItem(ConstManifestItemPtr item) const
{
    std::unique_ptr<AsyncByteStream> rawInput = item->AsyncReader();
    
    // transfer ownership to a new shared_ptr
    ChainLink input = ChainLink(rawInput.release());
    shared_vector<ChainLinkProcessor> thisChain;
    
    AsyncPipe::Pair linkPipe;
    
    for ( ContentFilterPtr filter : _filters )
    {
        if ( filter->TypeSniffer()(item) )
        {
            if ( !thisChain.empty() )
                thisChain.back()->SetOutputLink(linkPipe.first);
            
            thisChain.push_back(ChainLinkProcessor::New(filter, input, item));
            linkPipe = AsyncPipe::LinkedPair();
            input = linkPipe.second;
        }
    }
    
    // if no filters apply, read raw bytes
    if ( thisChain.empty() )
        return input;
    
    // otherwise, attach the output pipe
    thisChain.back()->SetOutputLink(linkPipe.first);

    static std::once_flag __once;
    std::call_once(__once, [](){
        _filterThreadPool.reset(new thread_pool(thread_pool::Automatic));
    });
    
    // set it to run on a runloop somewhere...
    _filterThreadPool->add([thisChain]() mutable {
        RunLoopPtr myRunLoop = RunLoop::CurrentRunLoop();
        
        auto pos = thisChain.rbegin();
        auto end = thisChain.rend();
        while ( pos != end )
        {
            (*pos)->ScheduleProcessor(myRunLoop);
            ++pos;
        }
        
        // as a file/zip-based async stream, the raw input stream will automatically
        // read bytes from the underlying resource on a shared background thread,
        // thus feeding the chain that follows
        
        // Note that the input stream is returned to us already opened.
        
        // run until all sources have been removed
        myRunLoop->Run();
    });
    
    // return the output pipe
    return linkPipe.second;
}
#endif /* SUPPORT_ASYNC */

std::shared_ptr<ByteStream> FilterChain::GetFilterChainByteStream(ConstManifestItemPtr item) const
{
    unique_ptr<SeekableByteStream> byteStream(dynamic_cast<SeekableByteStream *>(item->Reader().release()));
    if (!byteStream || !byteStream->IsOpen())
    {
        return nullptr;
    }

    return shared_ptr<ByteStream>(GetFilterChainByteStream(item, byteStream.release()).release());
}

std::unique_ptr<ByteStream> FilterChain::GetFilterChainByteStream(ConstManifestItemPtr item, SeekableByteStream *rawInput) const
{
    std::vector<ContentFilterPtr> thisChain;
    for (ContentFilterPtr filter : _filters)
    {
        if (filter->TypeSniffer()(item))
            thisChain.push_back(filter);
    }
    
    unique_ptr<SeekableByteStream> rawInputPtr(rawInput);
    return unique_ptr<FilterChainByteStream>(new FilterChainByteStream(std::move(rawInputPtr), thisChain, item));
}

std::shared_ptr<ByteStream> FilterChain::GetFilterChainByteStreamRange(ConstManifestItemPtr item) const
{
    unique_ptr<SeekableByteStream> byteStream(dynamic_cast<SeekableByteStream *>(item->Reader().release()));
    if (!byteStream || !byteStream->IsOpen())
    {
        return nullptr;
    }
    
    return shared_ptr<ByteStream>(GetFilterChainByteStreamRange(item, byteStream.release()).release());
}

std::unique_ptr<ByteStream> FilterChain::GetFilterChainByteStreamRange(ConstManifestItemPtr item, SeekableByteStream *rawInput) const
{
    unique_ptr<ByteStream> resultStream;
    int nFilters = 0;
    for (ContentFilterPtr filter : _filters)
    {
        if (filter->TypeSniffer()(item))
        {
            nFilters++;
            if (nFilters > 1)
            {
                continue;
            }
            
            if (filter->GetOperatingMode() == ContentFilter::OperatingMode::SupportsByteRanges)
            {
                unique_ptr<SeekableByteStream> rawInputPtr(rawInput);
                resultStream.reset(new FilterChainByteStreamRange(std::move(rawInputPtr), filter, item));
            }
        }
    }
    
    if (nFilters > 1)
    {
        // more than one filter...abort!
        return nullptr;
    }
    
    // There are no ContentFilter classes that curretly apply.
    // In this case, return an empty FilterChainByteStreamRange, that will simply put out raw bytes.
    if (!resultStream)
    {
        unique_ptr<SeekableByteStream> rawInputPtr(rawInput);
        resultStream.reset(new FilterChainByteStreamRange(std::move(rawInputPtr), nullptr, nullptr));
    }
    
    return resultStream;
}

size_t FilterChain::GetFilterChainSize(ConstManifestItemPtr item) const
{
    size_t numFilters = 0;
    
    for (ContentFilterPtr filter : _filters)
    {
        if (filter->TypeSniffer()(item))
        {
            numFilters++;
        }
    }

    return numFilters;
}

#ifdef SUPPORT_ASYNC
FilterChain::ChainLinkProcessor::ChainLinkProcessor(ContentFilterPtr filter, ChainLink input, ConstManifestItemPtr item)
  : _filter(filter),
    _context(filter->MakeFilterContext(item)),
    _input(input),
    _output(nullptr),
    _collectionBuffer()
{
}

FilterChain::ChainLinkProcessor::~ChainLinkProcessor()
{
}

void FilterChain::ChainLinkProcessor::ScheduleProcessor(RunLoopPtr runLoop)
{
    if ( _filter->RequiresCompleteData() )
        _collectionBuffer.SetUsesSecureErasure();
    
    std::weak_ptr<ChainLinkProcessor> weakSelf(Ptr());
    _input->SetEventHandler([this, weakSelf](AsyncEvent evt, AsyncByteStream* stream) {
        // we use this variable ONLY to ensure that the 'this' ptr is still valid
        auto strongSelf = weakSelf.lock();
        if ( !bool(strongSelf) )
            return;
        
        switch ( evt )
        {
            case AsyncEvent::HasBytesAvailable:
            {
                // we *know* that all links on a given chain fire events serially on
                // the same thread, so we don't need to dick about with locks here
                
                // there are bytes available to read -- pump as many through as we can
                ssize_t numMoved = FunnelBytes();
                if ( numMoved == 0 && !_output->IsOpen() )
                    stream->Close();
                
                break;
            }
                
            case AsyncEvent::ErrorOccurred:
                std::cerr << "ChainLinkProcessor input stream error: " << stream->Error() << std::endl;
                _output->Close();
                break;
                
            case AsyncEvent::EndEncountered:
            {
                if ( _filter->RequiresCompleteData() )
                {
                    size_t filteredLen = 0;
                    void* filteredData = _filter->FilterData(_context.get(), _collectionBuffer.GetBytes(), _collectionBuffer.GetBufferSize(), &filteredLen);
                    if ( filteredData == nullptr || filteredLen == 0 ) {
                        if (filteredData != nullptr && filteredData != _collectionBuffer.GetBytes())
                            delete[] reinterpret_cast<uint8_t*>(filteredData);
                        throw std::logic_error("ChainLinkProcessor: ContentFilter::FilterData() returned no data!");
                    }

                    // forward the data
                    uint8_t *outputData = reinterpret_cast<uint8_t*>(filteredData);
                    size_t offset = 0;
                    
                    while ( offset < filteredLen )
                    {
                        offset += _output->WriteBytes(&outputData[offset], filteredLen-offset);
                        if ( offset < filteredLen )
                            std::this_thread::yield();
                    }

                    if (filteredData != _collectionBuffer.GetBytes())
                        delete[] reinterpret_cast<uint8_t*>(filteredData);

                    // (securely) remove all bytes from the buffer
                    _collectionBuffer.RemoveBytes(_collectionBuffer.GetBufferSize());
                }
                
                _output->Close();
                break;
            }
            default:
                break;
        }
    });
    
    // if this is the last link in the chain, this will ultimately be replaced by the
    // SDK client. If it's only an intermediary link, however, we need to ensure that
    // notification of read-space-available messages is passed back down the chain to
    // the actual resource stream, otherwise the whole process may stall...
    _output->SetEventHandler([this](AsyncEvent evt, AsyncByteStream* stream) {
        switch ( evt )
        {
            case AsyncEvent::HasSpaceAvailable:
            {
                // pull more data through into the output stream
                if ( FunnelBytes() <= 0 && (_input->AtEnd() || !_input->IsOpen()) )
                {
                    stream->Close();
                    return;
                }
                
                break;
            }
                
            case AsyncEvent::ErrorOccurred:
                std::cerr << "ChainLinkProcessor input stream error: " << stream->Error() << std::endl;
                stream->Close();
                if (bool(_input))
                    _input->Close();
                break;
                
            case AsyncEvent::EndEncountered:
                if (bool(_input))
                    _input->Close();
                break;
                
            default:
                break;
        }
    });
    
    _input->SetTargetRunLoop(runLoop);
    _output->SetTargetRunLoop(runLoop);
}

ssize_t FilterChain::ChainLinkProcessor::FunnelBytes()
{
    uint8_t buf[ASYNC_BUF_SIZE];
    size_t bytesToMove = std::min(_input->BytesAvailable(), _output->SpaceAvailable());
    
    while ( bytesToMove > 0 )
    {
        size_t thisChunk = std::min(size_t(ASYNC_BUF_SIZE), bytesToMove);
        thisChunk = _input->ReadBytes(buf, thisChunk);      // consumes read bytes from the buffer
        
        if ( _filter->RequiresCompleteData() )
        {
            _collectionBuffer.AddBytes(buf, thisChunk);
        }
        else
        {
            size_t filteredLen = 0;
            void* filteredData = _filter->FilterData(_context.get(), buf, thisChunk, &filteredLen);
            if ( filteredData == nullptr || filteredLen == 0 ) {
                if (filteredData != nullptr && filteredData != buf)
                    delete[] reinterpret_cast<uint8_t*>(filteredData);
                throw std::logic_error("ChainLinkProcessor: ContentFilter::FilterData() returned no data!");
            }

            _output->WriteBytes(filteredData, filteredLen);

            if (filteredData != buf)
                delete[] reinterpret_cast<uint8_t*>(filteredData);
        }
        
        bytesToMove -= thisChunk;
    }
    
    return (ssize_t)bytesToMove;
}
#endif /* SUPPORT_ASYNC */

EPUB3_END_NAMESPACE
