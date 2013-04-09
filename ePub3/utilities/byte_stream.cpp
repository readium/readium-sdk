//
//  byte_stream.cpp
//  ePub3
//
//  Created by Jim Dovey on 2013-02-05.
//  Copyright (c) 2012-2013 The Readium Foundation and contributors.
//  
//  The Readium SDK is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//  
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include "byte_stream.h"
#include <libzip/zip.h>
#include <libzip/zipint.h>          // for internals of zip_file
#include <sys/stat.h>

EPUB3_BEGIN_NAMESPACE

std::thread AsyncByteStream::_asyncIOThread;
RunLoop*    AsyncByteStream::_asyncRunLoop(nullptr);

AsyncByteStream::AsyncByteStream(size_type bufsize) : AsyncByteStream(nullptr, bufsize)
{
}
AsyncByteStream::AsyncByteStream(StreamEventHandler handler, size_type bufsize)
  : _ringbuf(bufsize),
    _eventHandler(handler),
    _eventSource(nullptr),
    _event(ReadSpaceAvailable)
{
}
AsyncByteStream::~AsyncByteStream()
{
    Close();
}
void AsyncByteStream::Close()
{
    if ( _eventSource != nullptr )
    {
        _eventSource->Cancel();
        delete _eventSource;
    }
}
ByteStream::size_type AsyncByteStream::ReadBytes(void *buf, size_type len)
{
    size_type result =_ringbuf.ReadBytes(reinterpret_cast<uint8_t*>(buf), len);
    _event |= ReadSpaceAvailable;
    _eventSource->Signal();
    return result;
}
ByteStream::size_type AsyncByteStream::WriteBytes(const void *buf, size_type len)
{
    size_type result = _ringbuf.WriteBytes(reinterpret_cast<const uint8_t*>(buf), len);
    _event |= DataToWrite;
    _eventSource->Signal();
    return result;
}
void AsyncByteStream::InitAsyncHandler()
{
    if ( _eventSource != nullptr )
        throw std::logic_error("This stream is already set up for async operation.");
    
    _eventSource = new RunLoop::EventSource([=](RunLoop::EventSource&) {
        // atomically pull out the event flags here
        ThreadEvent t = _event.exchange(Wait);
        if ( t == Terminate )
        {
            return;
        }
        
        bool hasRead = false, hasWritten = false;
        
        uint8_t buf[4096];
        
        if ( (t & ReadSpaceAvailable) == ReadSpaceAvailable )
        {
            std::lock_guard<RingBuffer> _(_ringbuf);
            size_type read = this->read_for_async(buf, _ringbuf.SpaceAvailable());
            if ( read != 0 )
            {
                _ringbuf.WriteBytes(buf, read);
                hasRead = true;
            }
        }
        if ( (t & DataToWrite) == DataToWrite )
        {
            std::lock_guard<RingBuffer> _(_ringbuf);
            size_type written = _ringbuf.ReadBytes(buf, _ringbuf.BytesAvailable());
            written = this->write_for_async(buf, written);
            if ( written != 0 )
            {
                // only remove as much as actually went out
                _ringbuf.RemoveBytes(written);
                hasWritten = true;
            }
        }
        
        auto invocation = [this, hasRead, hasWritten] () {
            if ( hasRead )
                _eventHandler(AsyncEvent::HasBytesAvailable, this);
            if ( hasWritten )
                _eventHandler(AsyncEvent::HasSpaceAvailable, this);
        };
        
        if ( _targetRunLoop != nullptr )
        {
            _targetRunLoop->PerformFunction(invocation);
        }
        else
        {
            invocation();
        }
    });
    
    if ( _asyncRunLoop == nullptr )
    {
        std::mutex __mut;
        std::condition_variable __inited;
        
        std::unique_lock<std::mutex> __lock(__mut);
        _asyncIOThread = std::thread([&](){
            AsyncByteStream::_asyncRunLoop = RunLoop::CurrentRunLoop();
            {
                std::unique_lock<std::mutex> __(__mut);
                __inited.notify_all();
            }
            
            // now run the run loop
            
            // only spin an empty run loop a certain amount of time before giving up
            // and exiting the thread entirely
            // FIXME: There's a gap here where a race could lose an EventSource addition
            static constexpr unsigned kMaxEmptyTicks(1000);
            static constexpr std::chrono::milliseconds kTickLen(10);
            unsigned __emptyTickCounter = 0;
            
            do
            {
                RunLoop::ExitReason __r = RunLoop::CurrentRunLoop()->Run(true, std::chrono::seconds(20));
                if ( __r == RunLoop::ExitReason::RunFinished )
                {
                    if ( ++__emptyTickCounter == kMaxEmptyTicks )
                        break;      // exit the thread
                    
                    // wait a bit and try again
                    std::this_thread::sleep_for(kTickLen);
                }
                
                // by definition not an empty runloop
                __emptyTickCounter = 0;
            } while (1);
            
            // nullify the global before we quit
            // deletion isn't necessary, it's done by TLS in run_loop.cpp
            _asyncRunLoop = nullptr;
        });
        
        // wait for the runloop to be set
        __inited.wait(__lock, [&](){return _asyncRunLoop != nullptr;});
    }
    
    // install the event source into the run loop, then we're all done
    _asyncRunLoop->AddEventSource(_eventSource);
}

#if 0
#pragma mark -
#endif

FileByteStream::FileByteStream(const string& path, std::ios::openmode mode) : ByteStream(), _file(nullptr)
{
    Open(path, mode);
}
FileByteStream::~FileByteStream()
{
    Close();
}
ByteStream::size_type FileByteStream::BytesAvailable() const noexcept
{
    if ( !IsOpen() )
        return 0;
    
    if ( ::feof(const_cast<FILE*>(_file)) )
        return 0;
    
    struct stat sb;
    if ( ::fstat(::fileno(const_cast<FILE*>(_file)), &sb) != 0 )
        return 0;
    
    return (static_cast<size_type>(sb.st_size) - static_cast<size_type>(::ftell(const_cast<FILE*>(_file))));
}
ByteStream::size_type FileByteStream::SpaceAvailable() const noexcept
{
    // essentially unlimited, it seems
    return (std::numeric_limits<size_type>::max());
}
bool FileByteStream::IsOpen() const noexcept
{
    return _file != nullptr;
}
bool FileByteStream::Open(const string &path, std::ios::openmode mode)
{
    Close();
    
    // switch statement SHAMELESSLY nicked from libc++ std::ios::basic_filebuf
    const char* __mdstr;
    switch (mode & ~std::ios::ate)
    {
        case std::ios::out:
        case std::ios::out | std::ios::trunc:
            __mdstr = "w";
            break;
        case std::ios::out | std::ios::app:
        case std::ios::app:
            __mdstr = "a";
            break;
        case std::ios::in:
            __mdstr = "r";
            break;
        case std::ios::in | std::ios::out:
            __mdstr = "r+";
            break;
        case std::ios::in | std::ios::out | std::ios::trunc:
            __mdstr = "w+";
            break;
        case std::ios::in | std::ios::out | std::ios::app:
        case std::ios::in | std::ios::app:
            __mdstr = "a+";
            break;
        case std::ios::out | std::ios::binary:
        case std::ios::out | std::ios::trunc | std::ios::binary:
            __mdstr = "wb";
            break;
        case std::ios::out | std::ios::app | std::ios::binary:
        case std::ios::app | std::ios::binary:
            __mdstr = "ab";
            break;
        case std::ios::in | std::ios::binary:
            __mdstr = "rb";
            break;
        case std::ios::in | std::ios::out | std::ios::binary:
            __mdstr = "r+b";
            break;
        case std::ios::in | std::ios::out | std::ios::trunc | std::ios::binary:
            __mdstr = "w+b";
            break;
        case std::ios::in | std::ios::out | std::ios::app | std::ios::binary:
        case std::ios::in | std::ios::app | std::ios::binary:
            __mdstr = "a+b";
            break;
        default:
            return false;
    }
    
    _file = fopen(path.c_str(), __mdstr);
    if ( _file == nullptr )
        return false;
    
    if ( mode & (std::ios::ate | std::ios::app) )
    {
        if ( ::fseeko(_file, 0, SEEK_END) != 0 )
        {
            Close();
            return false;
        }
    }
    
    return true;
}
void FileByteStream::Close()
{
    if ( _file == nullptr )
        return;
    
    ::fclose(_file);
    _file = nullptr;
}
ByteStream::size_type FileByteStream::ReadBytes(void *buf, size_type len)
{
    if ( _file == nullptr )
        return 0;
    return ::fread(buf, 1, len, _file);
}
ByteStream::size_type FileByteStream::WriteBytes(const void* buf, size_type len)
{
    if ( _file == nullptr )
        return 0;
    return ::fwrite(buf, 1, len, _file);
}
ByteStream::size_type FileByteStream::Seek(size_type by, std::ios::seekdir dir)
{
    if ( _file == nullptr )
        return 0;
    
    int whence = SEEK_SET;
    switch ( dir )
    {
        case std::ios::beg:
        default:
            break;
        case std::ios::cur:
            whence = SEEK_CUR;
            break;
        case std::ios::end:
            whence = SEEK_END;
            break;
    }
    
    ::fseek(_file, by, whence);
    return ::ftell(_file);
}

#if 0
#pragma mark -
#endif

ZipFileByteStream::ZipFileByteStream(struct zip* archive, const string& path, int flags) : ZipFileByteStream()
{
    Open(archive, path, flags);
}
ZipFileByteStream::~ZipFileByteStream()
{
    Close();
}
ByteStream::size_type ZipFileByteStream::BytesAvailable() const noexcept
{
    if ( _file == nullptr )
        return 0;
    return _file->bytes_left;
}
ByteStream::size_type ZipFileByteStream::SpaceAvailable() const noexcept
{
    // no write support just now
    return 0;
}
bool ZipFileByteStream::IsOpen() const noexcept
{
    return _file != nullptr;
}
bool ZipFileByteStream::Open(struct zip *archive, const string &path, int flags)
{
    if ( _file != nullptr )
        Close();
    
    _file = zip_fopen(archive, path.c_str(), flags);
    return ( _file != nullptr );
}
void ZipFileByteStream::Close()
{
    if ( _file == nullptr )
        return;

    zip_fclose(_file);
    _file = nullptr;
}
ByteStream::size_type ZipFileByteStream::ReadBytes(void *buf, size_type len)
{
    if ( _file == nullptr )
        return 0;
    
    ssize_t numRead = zip_fread(_file, buf, len);
    if ( numRead < 0 )
    {
        Close();
        return 0;
    }
    
    return numRead;
}
ByteStream::size_type ZipFileByteStream::WriteBytes(const void *buf, size_type len)
{
    // no write support at this moment
    return 0;
}

#if 0
#pragma mark -
#endif

bool AsyncFileByteStream::Open(const string &path, std::ios::openmode mode)
{
    if ( __F::Open(path, mode) == false )
        return false;
    
    InitAsyncHandler();
    return true;
}
void AsyncFileByteStream::Close()
{
    __A::Close();
    __F::Close();
}

#if 0
#pragma mark -
#endif

bool AsyncZipFileByteStream::Open(struct zip *archive, const string &path, int flags)
{
    if ( __F::Open(archive, path, flags) == false )
        return false;
    
    InitAsyncHandler();
    return true;
}
void AsyncZipFileByteStream::Close()
{
    __A::Close();
    __F::Close();
}

EPUB3_END_NAMESPACE
