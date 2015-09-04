//
//  byte_stream.cpp
//  ePub3
//
//  Created by Jim Dovey on 2013-02-05.
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

#include "byte_stream.h"
#include <cstdio>
#include <iostream>
#include <libzip/zip.h>
#include <libzip/zipint.h>          // for internals of zip_file
#include <sys/stat.h>
#if EPUB_OS(ANDROID) || EPUB_OS(LINUX) || EPUB_OS(WINDOWS)
# include <condition_variable>
#endif
#if EPUB_OS(WINDOWS)
# include <io.h>
#endif
#if EPUB_OS(UNIX)
# include <unistd.h>    // for dup()
#endif

#include <ePub3/utilities/make_unique.h>

#ifdef SUPPORT_ASYNC
// I'm putting this here because it's the AsyncFileByteStream class that needs it
#if defined(_LIBCPP_VERSION) && _LIBCPP_VERSION <= 1101
////////////////////////////////////////////////////////////////////////////////////
// libcxxabi fix
//
// When a virtual function is marked deleted, the Clang compiler outputs a reference
// to __cxa_deleted_virtual. Unfortunately, as of right now (Clang 3.3), libcxxabi
// doesn't actually export that symbol.
extern "C" _LIBCPP_NORETURN void __cxa_deleted_virtual()
{
    abort();
}
#endif
#endif /* SUPPORT_ASYNC */

EPUB3_BEGIN_NAMESPACE

#ifdef SUPPORT_ASYNC
std::thread         AsyncByteStream::_asyncIOThread;
RunLoopPtr          AsyncByteStream::_asyncRunLoop(nullptr);
std::atomic_flag    AsyncByteStream::_asyncInited;

AsyncByteStream::AsyncByteStream(size_type bufsize)
  : _bufsize(bufsize),
    _eventHandler(nullptr),
    _eventSource(nullptr),
    _event(ReadSpaceAvailable),
    _targetRunLoop(nullptr),
    _eventDispatchSource(nullptr)
{
}
AsyncByteStream::AsyncByteStream(StreamEventHandler handler, size_type bufsize)
  : _bufsize(bufsize),
    _eventHandler(handler),
    _eventSource(nullptr),
    _event(ReadSpaceAvailable),
    _targetRunLoop(nullptr),
    _eventDispatchSource(nullptr)
{
}
AsyncByteStream::~AsyncByteStream()
{
    Close();
}
void AsyncByteStream::SetTargetRunLoop(RunLoopPtr rl) _NOEXCEPT
{
    if ( _eventDispatchSource == nullptr )
        _eventDispatchSource = EventDispatchSource();
    
    if ( _targetRunLoop != nullptr )
        _targetRunLoop->RemoveEventSource(_eventDispatchSource);
    
    _targetRunLoop = rl;
    
    if ( _targetRunLoop != nullptr )
        _targetRunLoop->AddEventSource(_eventDispatchSource);
    
    if ( (bool)_streamScheduled )
    {
        try
        {
            _streamScheduled(_targetRunLoop, this);
        }
        catch (std::exception& e)
        {
            std::cerr << "AsyncByteStream: exception calling _streamScheduled : " << e.what() << std::endl;
        }
        catch (...)
        {
            std::cerr << "AsyncByteStream: unknown exception calling _streamScheduled" << std::endl;
        }
    }
    
    if ( IsOpen() )
        ReadyToRun();
}
void AsyncByteStream::Close()
{
    if ( _closing.test_and_set() )
        return;
    
    if ( bool(_eventSource) )
    {
        if ( !(_eventSource->IsCancelled()) )
            _eventSource->Cancel();
        _eventSource = nullptr;
    }
    if ( bool(_eventDispatchSource) )
    {
        if ( !(_eventDispatchSource->IsCancelled()) )
            _eventDispatchSource->Cancel();
        _eventDispatchSource = nullptr;
    }
    
    _readbuf = nullptr;
    _writebuf = nullptr;
}
void AsyncByteStream::Open(std::ios::openmode mode)
{
    if ( (mode & std::ios::in) == std::ios::in )
    {
        _readbuf = std::make_shared<RingBuffer>(_bufsize);
    }
    if ( (mode & std::ios::out) == std::ios::out )
    {
        _writebuf = std::make_shared<RingBuffer>(_bufsize);
    }
    
    if ( _targetRunLoop != nullptr )
        ReadyToRun();
}
ByteStream::size_type AsyncByteStream::ReadBytes(void *buf, size_type len)
{
    if (len == 0) return 0;

    if ( !bool(_readbuf) )
        throw InvalidDuplexStreamOperationError("Stream not opened for reading");
    
    size_type result =_readbuf->ReadBytes(reinterpret_cast<uint8_t*>(buf), len);
    if ( result > 0 )
    {
        _readbuf->RemoveBytes(result);
        _event |= ReadSpaceAvailable;
        _eventSource->Signal();
    }
    return result;
}
ByteStream::size_type AsyncByteStream::WriteBytes(const void *buf, size_type len)
{
    if ( !bool(_writebuf) )
        throw InvalidDuplexStreamOperationError("Stream not opened for writing");
    
    size_type result = _writebuf->WriteBytes(reinterpret_cast<const uint8_t*>(buf), len);
    _event |= DataToWrite;
    _eventSource->Signal();
    return result;
}
AsyncEvent AsyncByteStream::WaitNextEvent(timeout_type timeout)
{
    AsyncEvent event = AsyncEvent::None;
    
    StreamEventHandler oldHandler = GetEventHandler();
    RunLoopPtr oldRunLoop = EventTargetRunLoop();
    
    SetTargetRunLoop(RunLoop::CurrentRunLoop());
    SetEventHandler([&](AsyncEvent evt, AsyncByteStream* st) {
        event = evt;
        RunLoop::CurrentRunLoop()->Stop();
    });
    RunLoop::CurrentRunLoop()->Run(false, timeout);
    
    SetTargetRunLoop(oldRunLoop);
    SetEventHandler(oldHandler);
    
    return event;
}
void AsyncByteStream::InitAsyncHandler()
{
    if ( _eventSource != nullptr )
        throw std::logic_error("This stream is already set up for async operation.");
    
    _eventSource = AsyncEventSource();
    
    static std::mutex __mut;
    static std::condition_variable __inited;
    
    if ( _asyncInited.test_and_set() == false )
    {
        std::unique_lock<std::mutex> __lock(__mut);
        _asyncIOThread = std::thread([&]() {
            AsyncByteStream::_asyncRunLoop = RunLoop::CurrentRunLoop();
            {
                std::unique_lock<std::mutex> __(__mut);
                __inited.notify_all();
            }
            
            // now run the run loop
            
            // only spin an empty run loop a certain amount of time before giving up
            // and exiting the thread entirely
            // FIXME: There's a gap here where a race could lose an EventSource addition
            static CONSTEXPR unsigned kMaxEmptyTicks(1000);
            static CONSTEXPR std::chrono::milliseconds kTickLen(10);
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
                    continue;
                }
                
                // by definition not an empty runloop
                __emptyTickCounter = 0;
                
            } while (1);
            
            // nullify the global before we quit
            // deletion isn't necessary, it's done by TLS in run_loop.cpp
            _asyncRunLoop = nullptr;
            _asyncInited.clear();
        });
        
        // detach the new thread
        _asyncIOThread.detach();
        
        // wait for the runloop to be set
        __inited.wait(__lock, [](){return bool(_asyncRunLoop);});
    }
    else if ( _asyncRunLoop == nullptr )
    {
        std::unique_lock<std::mutex> __lock(__mut);
        __inited.wait_for(__lock, std::chrono::milliseconds(150), [](){return bool(_asyncRunLoop);});
    }
    
    // install the event source into the run loop, then we're all done
    _asyncRunLoop->AddEventSource(_eventSource);
}
RunLoop::EventSourcePtr AsyncByteStream::AsyncEventSource()
{
    weak_ptr<RingBuffer> weakReadBuf = _readbuf;
    weak_ptr<RingBuffer> weakWriteBuf = _writebuf;
    
    return RunLoop::EventSource::New([=](RunLoop::EventSource&) {
        // atomically pull out the event flags here
        ThreadEvent t = _event.exchange(Wait);
        if ( t == Wait )
            return;
        
        bool hasRead = false, hasWritten = false;
        
        uint8_t buf[4096];
        
        shared_ptr<RingBuffer> readBuf = weakReadBuf.lock();
        shared_ptr<RingBuffer> writeBuf = weakWriteBuf.lock();
        
        if ( (t & ReadSpaceAvailable) == ReadSpaceAvailable && readBuf )
        {
            std::lock_guard<RingBuffer> _(*readBuf);
            size_type read = this->read_for_async(buf, readBuf->SpaceAvailable());
            if ( read != 0 )
            {
                readBuf->WriteBytes(buf, read);
                hasRead = true;
            }
            else
            {
                _eof = true;
            }
        }
        if ( (t & DataToWrite) == DataToWrite && writeBuf )
        {
            std::lock_guard<RingBuffer> _(*writeBuf);
            size_type written = writeBuf->ReadBytes(buf, writeBuf->BytesAvailable());
            written = this->write_for_async(buf, written);
            if ( written != 0 )
            {
                // only remove as much as actually went out
                writeBuf->RemoveBytes(written);
                hasWritten = true;
            }
            else
            {
                _eof = true;
            }
        }
        
        if ( _targetRunLoop != nullptr )
        {
            _eventDispatchSource->Signal();
        }
        else if ( bool(_eventHandler) )
        {
            if ( readBuf->HasData() )
                _eventHandler(AsyncEvent::HasBytesAvailable, this);
            if ( writeBuf->HasSpace() )
                _eventHandler(AsyncEvent::HasSpaceAvailable, this);
        }
    });
}
RunLoop::EventSourcePtr AsyncByteStream::EventDispatchSource()
{
    return RunLoop::EventSource::New([this](RunLoop::EventSource&) {
        if ( _err != 0 )
        {
            if ( bool(_eventHandler) )
                _eventHandler(AsyncEvent::ErrorOccurred, this);
            if ( bool(_eventDispatchSource) )
                _eventDispatchSource->Cancel();
            if ( bool(_eventSource) )
                _eventSource->Cancel();
            return;
        }
        if ( _eof )
        {
            if ( bool(_eventHandler) )
                _eventHandler(AsyncEvent::EndEncountered, this);
            if ( bool(_eventDispatchSource) )
                _eventDispatchSource->Cancel();
            if ( bool(_eventSource) )
                _eventSource->Cancel();
            return;
        }
        
        if ( BytesAvailable() && bool(_eventHandler) )
            _eventHandler(AsyncEvent::HasBytesAvailable, this);
        if ( SpaceAvailable() && bool(_eventHandler) )
            _eventHandler(AsyncEvent::HasSpaceAvailable, this);
    });
}
void AsyncByteStream::ReadyToRun()
{
    if ( _eventSource == nullptr )
        InitAsyncHandler();
    
    ThreadEvent wakeEvent = Wait;
    if ( _readbuf->HasSpace() )
        wakeEvent |= ReadSpaceAvailable;
    if ( _writebuf->HasData() )
        wakeEvent |= DataToWrite;
    
    if ( wakeEvent != Wait )
    {
        _event |= wakeEvent;
        _eventSource->Signal();
    }
}

#if 0
#pragma mark -
#endif

AsyncPipe::Pair AsyncPipe::LinkedPair(size_type bufsize)
{
    Pair result = std::make_pair(std::make_shared<AsyncPipe>(bufsize), std::make_shared<AsyncPipe>(bufsize));
    
    result.first->Open();
    result.second->_readbuf = result.first->_writebuf;
    result.second->_writebuf = result.first->_readbuf;
    
    result.first->_counterpart = result.second;
    result.second->_counterpart = result.first;
    
    return result;
}
AsyncPipe::~AsyncPipe()
{
    Close();
}
void AsyncPipe::Close()
{
    _self_closed = true;
    
    auto handler = _eventHandler;
    if ( bool(handler) )
        handler(AsyncEvent::EndEncountered, this);
    
    AsyncByteStream::Close();
    
    auto counterpart = _counterpart.lock();
    if ( bool(counterpart) )
        counterpart->CounterpartClosed();
}
void AsyncPipe::CounterpartClosed()
{
    _counterpart.reset();
    _pair_closed = true;
    
    if ( _readbuf->BytesAvailable() == 0 )
    {
        _eof = true;
        _event |= Exceptional;
        _eventSource->Signal();
    }
}
AsyncPipe::size_type AsyncPipe::ReadBytes(void *buf, size_type len)
{
    if (len == 0) return 0;

    size_type result = AsyncByteStream::ReadBytes(buf, len);
    if ( _readbuf->BytesAvailable() == 0 && _pair_closed )
    {
        _eof = true;
        _event |= Exceptional;
        _eventSource->Signal();
    }
    return result;
}
AsyncPipe::size_type AsyncPipe::WriteBytes(const void *buf, size_type len)
{
    if ( _pair_closed )
    {
        Close();
        return 0;
    }
    return AsyncByteStream::WriteBytes(buf, len);
}
void AsyncPipe::SetTargetRunLoop(RunLoopPtr rl) _NOEXCEPT
{
    AsyncByteStream::SetTargetRunLoop(rl);
    auto counterpart = _counterpart.lock();
    if ( bool(counterpart) && !_pair_closed && counterpart->_targetRunLoop.get() == nullptr )
        counterpart->SetTargetRunLoop(rl);
}
ByteStream::size_type AsyncPipe::read_for_async(void *buf, size_type len)
{
    return _readbuf->BytesAvailable();
}
ByteStream::size_type AsyncPipe::write_for_async(const void* buf, size_type len)
{
    return _writebuf->BytesAvailable();
}
RunLoop::EventSourcePtr AsyncPipe::AsyncEventSource()
{
    return RunLoop::EventSource::New([this](RunLoop::EventSource&) {
        // atomically pull out the event flags here
        ThreadEvent t = _event.exchange(Wait);
        if ( t == Wait )
            return;
        
        if ( (t & Exceptional) == Exceptional )
        {
            // fire our own item
            if ( _eventDispatchSource != nullptr && _targetRunLoop != nullptr )
            {
                _eventDispatchSource->Signal();
            }
            else if ( bool(_eventHandler) )
            {
                if ( _err != 0 ) {
                    _eventHandler(AsyncEvent::ErrorOccurred, this);
                } else if ( _eof != 0 ) {
                    _eventHandler(AsyncEvent::EndEncountered, this);
                }
            }
        }
        
        bool hasRead = false, hasWritten = false;
        
        if ( (t & ReadSpaceAvailable) == ReadSpaceAvailable )
        {
            // data pulled out of here, so room to write there
            hasRead = true;
        }
        if ( (t & DataToWrite) == DataToWrite )
        {
            hasWritten = true;
        }
        
        auto counterpart = _counterpart.lock();
        if ( (hasRead || hasWritten) && bool(counterpart) )
        {
            auto source = counterpart->_eventDispatchSource;
            if ( bool(source) && !source->IsCancelled() )
            {
                source->Signal();
            }
            else
            {
                auto handler = counterpart->_eventHandler;
                if ( bool(handler) ) {
                    if ( hasRead ) {
                        handler(AsyncEvent::HasSpaceAvailable, counterpart.get());
                    }
                    if ( hasWritten ) {
                        handler(AsyncEvent::HasBytesAvailable, counterpart.get());
                    }
                }
            }
        }
    });
}
#endif /* SUPPORT_ASYNC */

#if 0
#pragma mark -
#endif

static const char* fmode_from_openmode(std::ios::openmode mode)
{
	// switch statement SHAMELESSLY nicked from libc++ std::ios::basic_filebuf
	const char* __mdstr = nullptr;
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
		break;
	}
	return __mdstr;
}

FileByteStream::FileByteStream(const string& path, std::ios::openmode mode) : SeekableByteStream(), _file(nullptr)
{
    Open(path, mode);
}
FileByteStream::~FileByteStream()
{
    Close();
}
ByteStream::size_type FileByteStream::BytesAvailable() _NOEXCEPT
{
    if ( !IsOpen() )
        return 0;
    
    if ( ::feof(const_cast<FILE*>(_file)) )
        return 0;
    
    struct stat sb;
#if EPUB_OS(WINDOWS)
    int fd = _fileno(const_cast<FILE*>(_file));
#else
    int fd = fileno(const_cast<FILE*>(_file));
#endif
    if ( ::fstat(fd, &sb) != 0 )
        return 0;
    
    return (static_cast<size_type>(sb.st_size) - static_cast<size_type>(::ftell(const_cast<FILE*>(_file))));
}
ByteStream::size_type FileByteStream::SpaceAvailable() const _NOEXCEPT
{
    // essentially unlimited, it seems
    return (std::numeric_limits<size_type>::max());
}
bool FileByteStream::IsOpen() const _NOEXCEPT
{
    return _file != nullptr;
}
bool FileByteStream::Open(const string &path, std::ios::openmode mode)
{
	Close();

	// switch statement SHAMELESSLY nicked from libc++ std::ios::basic_filebuf
	const char* fmode = fmode_from_openmode(mode);
	if (fmode == nullptr)
	{
		fmode = "a+b";
		mode = std::ios::in | std::ios::out | std::ios::app | std::ios::binary;
	}
#if EPUB_OS(WINDOWS)
	::fopen_s(&_file, path.c_str(), fmode);
#else
    _file = ::fopen(path.c_str(), fmode);
#endif
    if ( _file == nullptr )
        return false;
    
    if ( mode & (std::ios::ate | std::ios::app) )
    {
        if ( ::fseek(_file, 0, SEEK_END) != 0 )
        {
            Close();
            return false;
        }
    }
	// store the mode so we can Clone() later
	_mode = mode;
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
    if (len == 0) return 0;

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
#if EPUB_OS(WINDOWS)
	::fseek(_file, static_cast<long>(by), whence);
#else
    ::fseek(_file, by, whence);
#endif
    return ::ftell(_file);
}
ByteStream::size_type FileByteStream::Position() const
{
	return ::ftell(const_cast<FILE*>(_file));
}
void FileByteStream::Flush()
{
	::fflush(_file);
}
std::shared_ptr<SeekableByteStream> FileByteStream::Clone() const
{
	if (_file == nullptr)
		return nullptr;

#if EPUB_OS(WINDOWS)
	int fd = _dup(_fileno(_file));
#else
	int fd = dup(fileno(_file));
#endif
	if (fd == -1)
		return nullptr;

#if EPUB_OS(WINDOWS)
	FILE* newFile = _fdopen(fd, fmode_from_openmode(_mode));
#else
	FILE* newFile = fdopen(fd, fmode_from_openmode(_mode));
#endif
	if (newFile == nullptr)
	{
#if EPUB_OS(WINDOWS)
		_close(fd);
#else
		close(fd);
#endif
		return nullptr;
	}

	auto result = std::make_shared<FileByteStream>();
	if (bool(result))
	{
		result->_file = newFile;
		result->_mode = _mode;
	}

	return result;
}

#if 0
#pragma mark -
#endif

ZipFileByteStream::ZipFileByteStream(struct zip* archive, const string& path, int flags) : SeekableByteStream(), _file(nullptr), _mode(0)
{
    bytes_left = total_size = 0;
    Open(archive, path, flags);
}
ZipFileByteStream::~ZipFileByteStream()
{
    Close();
}
ByteStream::size_type ZipFileByteStream::BytesAvailable() _NOEXCEPT
{
    //if ( _file == nullptr )
        //return 0;
    //return _file->bytes_left;

    if (_file == nullptr)
        return 0;
    return bytes_left;
}
ByteStream::size_type ZipFileByteStream::SpaceAvailable() const _NOEXCEPT
{
    // no write support just now
    return 0;
}
bool ZipFileByteStream::IsOpen() const _NOEXCEPT
{
    return _file != nullptr;
}

bool ZipFileByteStream::Open(struct zip *archive, const string &path, int flags)
{
    if ( _file != nullptr )
        Close();

    _path = path;
    _openFlags = flags;
    
    _file = zip_fopen(archive, Sanitized(path).c_str(), flags);

    if (_file != nullptr)
    {
        zip_stat_t _st;
        zip_flags_t _flags=0;

        _idx = zip_name_locate(archive, Sanitized(path).c_str(), 0);
        
        if (0==zip_stat_index(_file->za, _idx, _flags, &_st))  // worked
        //if (zip_source_stat(_file->src, &_st) == 0) // didn't work
        {
            total_size = _st.size;
            bytes_left = total_size;
        }
//        _supports = zip_source_supports(_file->src);
        _supports = _file->src->supports;
    }

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
    if (len == 0) return 0;

    if ( _file == nullptr )
        return 0;
    
    ssize_t numRead = zip_fread(_file, buf, len);
    if ( numRead < 0 )
    {
        Close();
        return 0;
    }

	//_eof = (_file->bytes_left == 0);
    bytes_left -= numRead;
    _eof = (bytes_left == 0);

    
    //if ((_idx = zip_name_locate(_file, path.c_str(), 0)) < 0)
    //    return 0;
    //zip_error_t error;   zip_uint64_t val = _zip_file_get_offset(_archive, _idx, &error);
    ///zip_int64_t tellVal = zip_source_tell(_file->src->src->src);
    //zip_int64_t tellVal = zip_source_tell(_file->src->src);
    
    //zip_int64_t tellVal = zip_source_tell(_file->src);
    //int iRet = zip_source_seek(_file->src, tellVal-1, SEEK_SET);
    //iRet = zip_source_seek(_file->src, tellVal/2, SEEK_SET);
    //iRet = zip_source_seek(_file->src, 0, SEEK_SET);

    
    return numRead;
}
ByteStream::size_type ZipFileByteStream::WriteBytes(const void *buf, size_type len)
{
    // no write support at this moment
    return 0;
}
int ZipFileByteStream::SeekByRewind(long pos, int whence)
{
    long abspos = 0, flen = total_size;

    if (Position() == 0 && whence == SEEK_CUR)
        return 0;
    
    switch (whence)
    {
    case SEEK_CUR:
        abspos = Position() + pos;
        break;
    case SEEK_SET:
        abspos = pos;
        break;
    case SEEK_END:
        abspos = flen + pos;
        break;
    default:    // incorrect/unspecified 'whence' parameter 
        return -1;
    }

    if (abspos == Position())
        return 0;       // no change 

    if (abspos >= flen) 
    {
        return SeekByReading(bytes_left);   // just move to the end of stream
    }
    else if (abspos > (long)Position())
    {
        // read & decompress bytes until we reach the right position
        return SeekByReading(abspos - Position());
    }

    //at this point, we're definitely moving backwards 
    if (abspos < 0) // can't set a negative offset 
        return -1;
    if (!SeekToStart())
        return -1;      // error already set */
    // this is a no-op for abspos == 0 *
    return SeekByReading(abspos);
}

bool ZipFileByteStream::SeekToStart()
{
    return Open(_file->za, _path, _openFlags);
}
int ZipFileByteStream::SeekByReading(size_t toread)
{
    char bytes[1024];
    while (toread > 0) 
    {
        ssize_t numRead = ReadBytes(bytes, (toread < 1024 ? toread : 1024));
        if (numRead < 0)
            return -1;      // error already set 

        if (numRead == 0)   // avoid infinite loops 
            return -1;
        
        toread -= numRead;
    }

    /* zf has been updated for us by zip_fread() already */
    return 0;
}


ByteStream::size_type ZipFileByteStream::Seek(size_type by, std::ios::seekdir dir)
{
    int whence = SEEK_SET;
    switch (dir)
    {
        case std::ios::beg:
            break;
        case std::ios::cur:
            whence = SEEK_CUR;
            break;
        case std::ios::end:
            whence = SEEK_END;
            break;
        default:
            return Position();
    }
    
    if ((_file->src->supports & ZIP_SOURCE_MAKE_COMMAND_BITMASK(ZIP_SOURCE_SEEK)) == 0)     // if the source doesn't support seek operation
    {
        return SeekByRewind(by, whence);    // Emulation of Seek operation by rewind
    }
    
    // this is called if the libzip source below supports the ZIP_SOURCE_SEEK operation
    if (-1 != zip_source_seek(_file->src, long(by), whence))
    {
        bytes_left = total_size - by;
        _eof = (bytes_left == 0);
        return Position();
    }
    return -1;
}
ByteStream::size_type ZipFileByteStream::Position() const
{
    if ((_file->src->supports & ZIP_SOURCE_MAKE_COMMAND_BITMASK(ZIP_SOURCE_TELL)) == 0)     // if the source doesn't support seek operation
    {
        return total_size - bytes_left;
    }

    return zip_source_tell(_file->src); //return size_type(zip_ftell(_file));
}
std::shared_ptr<SeekableByteStream> ZipFileByteStream::Clone() const
{
	if (_file == nullptr)
		return nullptr;

    struct zip_file* newFile = zip_fopen_index(_file->za, _idx, _file->za->flags);
	if (newFile == nullptr)
		return nullptr;
    
    zip_source_seek(newFile->src, 0, SEEK_SET);

	auto result = std::make_shared<ZipFileByteStream>();
	if (bool(result))
	{
		result->_file = newFile;
		result->_mode = _mode;
	}

	return result;
}

#ifdef SUPPORT_ASYNC
#if 0
#pragma mark -
#endif

AsyncFileByteStream::AsyncFileByteStream(StreamEventHandler handler, const string& path, std::ios::openmode mode, size_type bufsize)
  : AsyncByteStream(handler, bufsize),
    FileByteStream()
{
    if ( !Open(path, mode) )
        throw std::invalid_argument("AsyncFileByteStream: failed to Open() file");
}
bool AsyncFileByteStream::Open(const string &path, std::ios::openmode mode)
{
    if ( __F::Open(path, mode) == false )
        return false;
    
    __A::Open(mode);
    return true;
}
void AsyncFileByteStream::Close()
{
    __A::Close();
    __F::Close();
}
std::shared_ptr<SeekableByteStream> AsyncFileByteStream::Clone() const
{
	if (_file == nullptr)
		return nullptr;

#if EPUB_OS(WINDOWS)
	int fd = _dup(_fileno(_file));
#else
	int fd = dup(fileno(_file));
#endif
	if (fd == -1)
		return nullptr;

#if EPUB_OS(WINDOWS)
	FILE* newFile = _fdopen(fd, fmode_from_openmode(_mode));
#else
	FILE* newFile = fdopen(fd, fmode_from_openmode(_mode));
#endif
	if (newFile == nullptr)
	{
#if EPUB_OS(WINDOWS)
		_close(fd);
#else
		close(fd);
#endif
		return nullptr;
	}

	auto result = std::make_shared<AsyncFileByteStream>();
	if (bool(result))
	{
		result->_file = newFile;
		result->_mode = _mode;
	}

	return result;
}

#if 0
#pragma mark -
#endif

AsyncZipFileByteStream::AsyncZipFileByteStream(struct zip* archive, const string& path, int zipFlags)
 : AsyncByteStream(),
   ZipFileByteStream()
{
    if ( !Open(archive, path, zipFlags) )
        throw std::invalid_argument("AsyncZipFileByteStream: failed to Open() archive");
}
bool AsyncZipFileByteStream::Open(struct zip *archive, const string &path, int flags)
{
    if ( __F::Open(archive, path, flags) == false )
        return false;
    
    __A::Open(std::ios::in|std::ios::out);
    return true;
}
void AsyncZipFileByteStream::Close()
{
    __A::Close();
    __F::Close();
}
std::shared_ptr<SeekableByteStream> AsyncZipFileByteStream::Clone() const
{
	if (_file == nullptr)
		return nullptr;


	struct zip_file* newFile = zip_fopen_index(_file->za, _file->file_index, _file->flags);
	if (newFile == nullptr)
		return nullptr;

	auto result = std::make_shared<AsyncZipFileByteStream>();
	if (bool(result))
	{
		result->_file = newFile;
		result->_mode = _mode;
	}

	return result;
}
#endif /* SUPPORT_ASYNC */

EPUB3_END_NAMESPACE
