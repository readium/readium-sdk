//
//  byte_stream.h
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

#ifndef __ePub3__byte_stream__
#define __ePub3__byte_stream__

#include "epub3.h"
#include "ring_buffer.h"
#include <functional>
#include <ios>
#include <thread>

struct zip;
struct zip_file;

EPUB3_BEGIN_NAMESPACE

class ByteStream
{
public:
    typedef std::streamsize         size_type;
    
    static const size_type          UnknownSize = std::numeric_limits<size_type>::min();
    
public:
                            ByteStream()                            {}
    virtual                 ~ByteStream()                           {}
    
                            ByteStream(const ByteStream&)           = delete;
                            ByteStream(ByteStream&&)                = delete;
    ByteStream&             operator=(const ByteStream&)            = delete;
    ByteStream&             operator=(ByteStream&& o)               = delete;
    
    virtual size_type       BytesAvailable()                        const noexcept  { return UnknownSize; }
    virtual size_type       SpaceAvailable()                        const noexcept  { return UnknownSize; }
    
    // can't define an Open() here because that requires implementation-specific information
    virtual bool            IsOpen()                                const noexcept  = 0;
    virtual void            Close()                                                 = 0;
    
    virtual size_type       ReadBytes(void* buf, size_type len)                     = 0;
    virtual size_type       WriteBytes(const void* buf, size_type len)              = 0;
    
    virtual bool            AtEnd()                                 const noexcept  { return _eof; }
    virtual int             Error()                                 const noexcept  { return _err; }
    
protected:
    bool                    _eof;
    int                     _err;
    
};

enum class AsyncEvent : uint8_t
{
    None,
    OpenCompleted,
    HasBytesAvailable,
    HasSpaceAvailable,
    ErrorOccurred,
    EndEncountered
};

class AsyncByteStream;
using StreamEventHandler = std::function<void(AsyncEvent, AsyncByteStream*)>;

class AsyncByteStream : public ByteStream
{
protected:
    typedef uint8_t             ThreadEvent;
    static const ThreadEvent    Wait                    = 0;
    static const ThreadEvent    ReadSpaceAvailable      = 1 << 0;
    static const ThreadEvent    DataToWrite             = 1 << 1;
    static const ThreadEvent    Terminate               = std::numeric_limits<ThreadEvent>::max();
    
public:
                                AsyncByteStream(size_type bufsize=4096);
                                AsyncByteStream(StreamEventHandler handler, size_type bufsize=4096);
    virtual                     ~AsyncByteStream();
    
                                AsyncByteStream(const AsyncByteStream&)         = delete;
                                AsyncByteStream(AsyncByteStream&&)              = delete;
    AsyncByteStream&            operator=(const AsyncByteStream&)               = delete;
    AsyncByteStream&            operator=(AsyncByteStream&&)                    = delete;
    
    StreamEventHandler          GetEventHandler()                   const           { return _eventHandler; }
    void                        SetEventHandler(StreamEventHandler handler)         { _eventHandler = handler; }
    
    virtual size_type           BytesAvailable()                    const noexcept  { return _ringbuf.BytesAvailable(); }
    virtual size_type           SpaceAvailable()                    const noexcept  { return _ringbuf.SpaceAvailable(); }
    
    virtual void                Close();
    
    // subclassers can implement these and call AsyncByteStream's implementations
    // to interact with the ring buffer
    virtual size_type           ReadBytes(void* buf, size_type len);
    virtual size_type           WriteBytes(const void* buf, size_type len);
    
protected:
    
    RingBuffer                  _ringbuf;
    StreamEventHandler          _eventHandler;
    std::thread                 _asyncIOThread;
    std::mutex                  _condMutex;
    std::condition_variable     _threadSignal;
    std::atomic<ThreadEvent>    _event;
    
    virtual void                InitAsyncHandler();
    virtual size_type           read_for_async(void* buf, size_type len)        = 0;
    virtual size_type           write_for_async(const void* buf, size_type len) = 0;
};

class FileByteStream : public ByteStream
{
public:
                            FileByteStream()                        : ByteStream(), _file(nullptr) {}
                            FileByteStream(const string& pathToOpen, std::ios::openmode mode = std::ios::in | std::ios::out);
    virtual                 ~FileByteStream();
    
                            FileByteStream(const FileByteStream& o)             = delete;
                            FileByteStream(FileByteStream&& o)                  = delete;
    FileByteStream&         operator=(FileByteStream&)                          = delete;
    FileByteStream&         operator=(FileByteStream&&)                         = delete;
    
    virtual size_type       BytesAvailable()                        const noexcept;
    virtual size_type       SpaceAvailable()                        const noexcept;
    
    virtual bool            IsOpen()                                const noexcept;
    virtual bool            Open(const string& path, std::ios::openmode mode = std::ios::in | std::ios::out);
    virtual void            Close();
    
    virtual size_type       ReadBytes(void* buf, size_type len);
    virtual size_type       WriteBytes(const void* buf, size_type len);
    
    virtual size_type       Seek(size_type by, std::ios::seekdir dir);
    
protected:
    FILE*                   _file;
};

class ZipFileByteStream : public ByteStream
{
public:
                            ZipFileByteStream() : ByteStream(), _file(nullptr) {}
                            ZipFileByteStream(struct zip* archive, const string& pathToOpen, int zipFlags=0);
    virtual                 ~ZipFileByteStream();
    
                            ZipFileByteStream(const ZipFileByteStream&)         = delete;
                            ZipFileByteStream(ZipFileByteStream&&)              = delete;
    ZipFileByteStream&      operator=(const ZipFileByteStream&)                 = delete;
    ZipFileByteStream&      operator=(ZipFileByteStream&&)                      = delete;
    
    virtual size_type       BytesAvailable()                        const noexcept;
    virtual size_type       SpaceAvailable()                        const noexcept;
    
    virtual bool            IsOpen()                                const noexcept;
    virtual bool            Open(struct zip* archive, const string& path, int zipFlags=0);
    virtual void            Close();
    
    virtual size_type       ReadBytes(void* buf, size_type len);
    virtual size_type       WriteBytes(const void* buf, size_type len);
    
protected:
    struct zip_file*        _file;
};

class AsyncFileByteStream : public AsyncByteStream, public FileByteStream
{
private:
    typedef FileByteStream  __F;
    typedef AsyncByteStream __A;
    
public:
                            AsyncFileByteStream() : AsyncByteStream(), FileByteStream() {}
                            AsyncFileByteStream(StreamEventHandler handler) : AsyncByteStream(handler), FileByteStream() {}
                            AsyncFileByteStream(StreamEventHandler handler, const string& path, std::ios::openmode mode = std::ios::in | std::ios::out) : AsyncByteStream(handler), FileByteStream(path, mode) {}
    virtual                 ~AsyncFileByteStream();
    
                            AsyncFileByteStream(const AsyncFileByteStream&) = delete;
                            AsyncFileByteStream(AsyncFileByteStream&&)      = delete;
    AsyncFileByteStream&    operator=(const AsyncFileByteStream&)           = delete;
    AsyncFileByteStream&    operator=(AsyncFileByteStream&&)                = delete;
    
    // use the ringbuffer-based availability functions from AsyncByteStream
    using                   __A::BytesAvailable;
    using                   __A::SpaceAvailable;
    
    // use the file stream's IsOpen()
    using                   __F::IsOpen;
    
    // use the async stream's read/writers
    using                   __A::ReadBytes;
    using                   __A::WriteBytes;
    
    virtual bool            IsOpen()                        const noexcept  { return __F::IsOpen(); }
    virtual bool            Open(const string& path, std::ios::openmode mode = std::ios::in | std::ios::out);
    virtual void            Close();
    
private:
    virtual size_type       Seek();
    
protected:
    virtual size_type       read_for_async(void* buf, size_type len)        { return __F::ReadBytes(buf, len); }
    virtual size_type       write_for_async(const void* buf, size_type len) { return __F::WriteBytes(buf, len); }
};

class AsyncZipFileByteStream : public AsyncByteStream, public ZipFileByteStream
{
private:
    typedef ZipFileByteStream   __F;
    typedef AsyncByteStream     __A;
    
public:
                            AsyncZipFileByteStream() : AsyncByteStream(), ZipFileByteStream() {}
                            AsyncZipFileByteStream(StreamEventHandler handler) : AsyncByteStream(handler), ZipFileByteStream() {}
                            AsyncZipFileByteStream(StreamEventHandler handler, struct zip* archive, const string& path, int zipFlags=0) : AsyncByteStream(handler), ZipFileByteStream(archive, path, zipFlags) {}
    virtual                 ~AsyncZipFileByteStream();
    
                            AsyncZipFileByteStream(const AsyncZipFileByteStream&)   = delete;
                            AsyncZipFileByteStream(AsyncZipFileByteStream&&)        = delete;
    AsyncZipFileByteStream& operator=(const AsyncZipFileByteStream&)                = delete;
    AsyncZipFileByteStream& operator=(AsyncZipFileByteStream&&)                     = delete;
    
    // use the ringbuffer-based availability functions from AsyncByteStream
    using                   __A::BytesAvailable;
    using                   __A::SpaceAvailable;
    
    // use the file stream's IsOpen()
    using                   __F::IsOpen;
    
    // use the async stream's read/writers
    using                   __A::ReadBytes;
    using                   __A::WriteBytes;
    
    virtual bool            IsOpen()                        const noexcept  { return __F::IsOpen(); }
    virtual bool            Open(struct zip* archive, const string& path, int zipFlags=0);
    virtual void            Close();
    
protected:
    virtual size_type       read_for_async(void* buf, size_type len)        { return __F::ReadBytes(buf, len); }
    virtual size_type       write_for_async(const void* buf, size_type len) { return __F::WriteBytes(buf, len); }
};

EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__byte_stream__) */
