//
//  filter_chain.cpp
//  ePub3
//
//  Created by Jim Dovey on 2013-08-27.
//  Copyright (c) 2013 The Readium Foundation and contributors. All rights reserved.
//

#include "filter_chain.h"
#include "../ePub/manifest.h"
#include "filter.h"
#include "byte_buffer.h"
#include "make_unique.h"
#include <iostream>

#define ASYNC_BUF_SIZE 4096*4

EPUB3_BEGIN_NAMESPACE

thread_pool FilterChain::_filterThreadPool(thread_pool::Automatic);

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
            
            thisChain.push_back(ChainLinkProcessor::New(filter, input));
            linkPipe = AsyncPipe::LinkedPair();
            input = linkPipe.second;
        }
    }
    
    // if no filters apply, read raw bytes
    if ( thisChain.empty() )
        return input;
    
    // otherwise, attach the output pipe
    thisChain.back()->SetOutputLink(linkPipe.first);
    
    // set it to run on a runloop somewhere...
    _filterThreadPool.add([thisChain]() mutable {
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

FilterChain::ChainLinkProcessor::ChainLinkProcessor(ContentFilterPtr filter, ChainLink input)
  : _filter(filter),
    _context(filter->MakeFilterContext()),
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
    
    std::weak_ptr<typeof(*this)> weakSelf(Ptr());
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
                        throw std::logic_error("ChainLinkProcessor: ContentFilter::FilterData() returned no data!");
                    }
                    
                    // (securely) remove all bytes from the buffer
                    _collectionBuffer.RemoveBytes(_collectionBuffer.GetBufferSize());
                    
                    // forward the data
                    uint8_t *outputData = reinterpret_cast<uint8_t*>(filteredData);
                    size_t offset = 0;
                    
                    while ( offset < filteredLen )
                    {
                        offset += _output->WriteBytes(&outputData[offset], filteredLen-offset);
                        if ( offset < filteredLen )
                            std::this_thread::yield();
                    }
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
                _input->Close();
                break;
                
            case AsyncEvent::EndEncountered:
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
    ssize_t bytesToMove = std::min(_input->BytesAvailable(), _output->SpaceAvailable());
    
    while ( bytesToMove > 0 )
    {
        ssize_t thisChunk = std::min(ssize_t(ASYNC_BUF_SIZE), bytesToMove);
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
                throw std::logic_error("ChainLinkProcessor: ContentFilter::FilterData() returned no data!");
            }
            
            _output->WriteBytes(filteredData, filteredLen);
        }
        
        bytesToMove -= thisChunk;
    }
    
    return bytesToMove;
}

EPUB3_END_NAMESPACE
