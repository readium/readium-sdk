//
//  byte_stream.cpp
//  ePub3
//
//  Created by Jim Dovey on 2013-02-05.
//  Copyright (c) 2012-2013 The Readium Foundation.
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
#include <sys/stat.h>

EPUB3_BEGIN_NAMESPACE

AsyncByteStream::AsyncByteStream(size_type bufsize) : AsyncByteStream(nullptr, bufsize)
{
}
AsyncByteStream::AsyncByteStream(StreamEventHandler handler, size_type bufsize)
  : _ringbuf(bufsize),
    _eventHandler(handler),
    _asyncIOThread(),
    _condMutex(),
    _threadSignal(),
    _event(ReadSpaceAvailable)
{
}
AsyncByteStream::~AsyncByteStream()
{
    Close();
}
void AsyncByteStream::Close()
{
    if ( _asyncIOThread.joinable() )
    {
        _event = Terminate;
        _threadSignal.notify_all();
        _asyncIOThread.join();
    }
}
ByteStream::size_type AsyncByteStream::ReadBytes(void *buf, size_type len)
{
    size_type result =_ringbuf.ReadBytes(reinterpret_cast<uint8_t*>(buf), len);
    _event |= ReadSpaceAvailable;
    _threadSignal.notify_all();
    return result;
}
ByteStream::size_type AsyncByteStream::WriteBytes(const void *buf, size_type len)
{
    size_type result = _ringbuf.WriteBytes(reinterpret_cast<const uint8_t*>(buf), len);
    _event |= DataToWrite;
    _threadSignal.notify_all();
    return result;
}
void AsyncByteStream::InitAsyncHandler()
{
    if ( _asyncIOThread.joinable() )
        throw std::logic_error("The async IO thread already been started");
    
    _asyncIOThread = std::thread([&](){
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
        
        if ( hasRead )
            _eventHandler(AsyncEvent::HasBytesAvailable, this);
        if ( hasWritten )
            _eventHandler(AsyncEvent::HasSpaceAvailable, this);
        
        std::unique_lock<std::mutex> __l(_condMutex);
        _threadSignal.wait(__l, [&](){ return _event != Wait; });
    });
}

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
    
    return sb.st_size - ::ftell(const_cast<FILE*>(_file));
}
ByteStream::size_type FileByteStream::SpaceAvailable() const noexcept
{
    // essentially unlimited, it seems
    return std::numeric_limits<size_type>::max();
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

EPUB3_END_NAMESPACE
