//
//  byte_stream.h
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

#ifndef __ePub3__byte_stream__
#define __ePub3__byte_stream__

#include <ePub3/epub3.h>
#include <ePub3/utilities/ring_buffer.h>
#include <functional>
#include <ios>
#include <thread>
#include <ePub3/utilities/run_loop.h>
#include <ePub3/utilities/make_unique.h>

struct zip;
struct zip_file;

//#include "iri.h"
#include <google-url/url_canon.h>
#include <google-url/url_util.h>

EPUB3_BEGIN_NAMESPACE

static string Sanitized(const string& path)
{
    if ( path.find("%") != std::string::npos )
    {
        url_canon::RawCanonOutputW<256> output;

        // note that std::string .size() is the same as
        // ePub3:string .utf8_size() defined in utfstring.h (equivalent to strlen(str.c_str()) ),
        // but not the same as ePub3:string .size() !!
        // WATCH OUT!
        url_util::DecodeURLEscapeSequences(path.c_str(), static_cast<int>(path.utf8_size()), &output);

        string path_(output.data(), output.length());

        if ( path_.find('/') == 0 )
            return path_.substr(1);
        return path_;
    }

    if ( path.find('/') == 0 )
        return path.substr(1);
    return path;
}

    /**
 The abstract base class for all stream and pipe objects used by the Readium SDK.
 
 This class declares the standard interface for a stream-- that is, an object to
 which bytes can be sent and from which bytes can be read. By itself, a stream is
 not considered to be seekable, and has no concept of position. Subclasses are free
 to add that where applicable, however.
 
 This class does not define any interface for asynchronous I/O; that is the purview
 of the AsyncByteStream class, which inherits this one.
 
 @ingroup utilities
 */
class ByteStream
{
public:
    ///
    /// The type for all byte-counts used with the ByteStream API.
    typedef std::size_t             size_type;
    
    ///
    /// A value to be returned when a real count is not possible.
    static const size_type          UnknownSize = 0;

public:
                             ByteStream() : _eof(false), _err(0)    {}
    virtual                 ~ByteStream()                           {}
    
private:
    ///
    /// ByteStreams cannot be copied, moved, or assigned.
                            ByteStream(const ByteStream&)           _DELETED_;
                            ByteStream(ByteStream&&)                _DELETED_;
    ByteStream&             operator=(const ByteStream&)            _DELETED_;
    ByteStream&             operator=(ByteStream&& o)               _DELETED_;
    
public:
    ///
    /// Returns the number of bytes that can be read at this time.
    virtual size_type       BytesAvailable()                        _NOEXCEPT  { return UnknownSize; }
    ///
    /// Returns the amount of space available for writing at this time.
    virtual size_type       SpaceAvailable()                        const _NOEXCEPT  { return UnknownSize; }
    
    /**
     Determine whether the stream is currently open (i.e. usable).
     
     I can't define an Open() method on ByteStream here because that would require
     implementation-specific parameters.
     */
    virtual bool            IsOpen()                                const _NOEXCEPT  = 0;
    ///
    /// Close the stream.
    virtual void            Close()                                                 = 0;
    
    /**
     Read some data from the stream.
     @param buf A buffer into which to place any retrieved data.
     @param len The number of bytes that can be stored in `buf`.
     @result Returns the number of bytes actually copied into `buf`.
     */
    virtual size_type       ReadBytes(void* buf, size_type len)                     = 0;
	
	/**
	 Read all data from the stream.
	 @param buf A pointer to a buffer which will be allocated
	 @result Returns the number of bytes copied into `buf`.
	 */
	virtual size_type       ReadAllBytes(void** buf)
	{
        unsigned char* resbuf = nullptr;
        unsigned char* temp;
        size_t resbuflen = 0;
        
        unsigned char rdbuf [4096] = {0};
        size_t rdbuflen = 4096;
        std::size_t count = this->ReadBytes(rdbuf, rdbuflen);
        if(count)
        {
            resbuf = (unsigned char*)malloc(count);
            memcpy(resbuf, rdbuf, count);
            resbuflen = count;
        }
        while(count)
        {
            count = this->ReadBytes(rdbuf, rdbuflen);
            
            temp = (unsigned char*)malloc(count + resbuflen);
            memcpy(temp, resbuf, resbuflen);
            free(resbuf);
            resbuf = temp;
            memcpy(resbuf+resbuflen, rdbuf, count);
            resbuflen += count;
        }
		
		if (resbuflen > 0)
			*buf = resbuf;
		
		return resbuflen;
	}
	
    /**
     Write some data to the stream.
     @param buf A buffer containing data to write.
     @param len The number of bytes to send.
     @result Returns the number of bytes actually written to the stream.
     */
    virtual size_type       WriteBytes(const void* buf, size_type len)              = 0;
    
    ///
    /// Returns `true` if an EOF status has occurred.
    virtual bool            AtEnd()                                 const _NOEXCEPT  { return _eof; }
    ///
    /// Returns any error code reported by the underlying system.
    virtual int             Error()                                 const _NOEXCEPT  { return _err; }
    
protected:
    bool                    _eof;   ///< Whether the end of a finite-length data stream has been reached.
    int                     _err;   ///< Any system error which has occurred on this stream.
    
};

#ifdef SUPPORT_ASYNC
/**
 Event codes for asynchronous stream events.
 @ingroup utilities
 */
enum class EPUB3_EXPORT AsyncEvent : uint8_t
{
    None,                   ///< No event (invalid code, or maybe just a wakeup call)
    OpenCompleted,          ///< The stream opened successfully.
    HasBytesAvailable,      ///< There is some data ready to read from the stream.
    HasSpaceAvailable,      ///< There is room in the output buffer to write more data.
    ErrorOccurred,          ///< An error occurred. Call ByteStream::Error() for details.
    EndEncountered          ///< The end of a finite-length stream was encountered; no more data is available. Also posted when a pipe stream is closed.
};

class AsyncByteStream;
///
/// The type of an asynchronous stream's event-handler function.
#if EPUB_COMPILER_SUPPORTS(CXX_ALIAS_TEMPLATES)
using StreamEventHandler = std::function<void(AsyncEvent, AsyncByteStream*)>;
#else
typedef std::function<void(AsyncEvent, AsyncByteStream*)> StreamEventHandler;
#endif

/**
 An exception posted when a non-duplex stream is used in the wrong direction.
 */
class InvalidDuplexStreamOperationError : public std::logic_error
{
public:
    EPUB3_EXPORT InvalidDuplexStreamOperationError(const string& str) : std::logic_error(str.stl_str()) {}
    EPUB3_EXPORT InvalidDuplexStreamOperationError(const char* str) : std::logic_error(str) {}
    EPUB3_EXPORT ~InvalidDuplexStreamOperationError() _NOEXCEPT {}
};

/**
 A simple asynchronous stream class.
 
 This class spawns a single shared thread on which reads and writes are issued. Each
 async stream uses a RunLoop::EventSource to notify the shared thread when the
 stream's ReadBytes() or WriteBytes() methods have been called. Similarly, a stream
 may be given a RunLoop on which to fire events advertising the availablility of
 either data to read or space to write.
 @ingroup utilities
 */
class AsyncByteStream : public ByteStream
{
protected:
    ///
    /// Internal event type-- used to signal the shared I/O thread.
    typedef uint8_t             ThreadEvent;
    ///
    /// Take no action: wait for a different event.
    static const ThreadEvent    Wait                    = 0;
    ///
    /// Space is available in the stream's RingBuffer to receive resource data.
    static const ThreadEvent    ReadSpaceAvailable      = 1 << 0;
    ///
    /// Data has been written to the stream and can be written to the resource now.
    static const ThreadEvent    DataToWrite             = 1 << 1;
    ///
    /// An exceptional circumstance has occurred (EOF, error)
    static const ThreadEvent    Exceptional             = 1 << 2;
    
public:
    ///
    /// Timeouts are expressed as seconds in floating-point (as in OS X/iOS)
    typedef std::chrono::duration<double>   timeout_type;
    
    ///
    /// The type of a callback used to notify assignment to a runloop.
    typedef std::function<void(RunLoopPtr, AsyncByteStream*)> StreamScheduledHandler;
    
public:
    /**
     Create a new AsyncByteStream.
     @param bufsize The size, in bytes, of the read/write buffers. The default is 4KiB.
     */
    EPUB3_EXPORT                AsyncByteStream(size_type bufsize=4096);
    /**
     Create a new AsyncByteStream with an event handler.
     @param handler The event-handling function to call when the stream's status changes.
     @param bufsize The size, in bytes, of the read/write buffers. The default is 4KiB.
     */
    EPUB3_EXPORT                AsyncByteStream(StreamEventHandler handler, size_type bufsize=4096);
    virtual                     ~AsyncByteStream();
    
private:
                                AsyncByteStream(const AsyncByteStream&)         _DELETED_;
                                AsyncByteStream(AsyncByteStream&&)              _DELETED_;
    AsyncByteStream&            operator=(const AsyncByteStream&)               _DELETED_;
    AsyncByteStream&            operator=(AsyncByteStream&&)                    _DELETED_;
    
public:
    ///
    /// Retrieve the stream's event-handler function.
    StreamEventHandler          GetEventHandler()                   const _NOEXCEPT { return _eventHandler; }
    ///
    /// Assign an event-handler to an asynchronous stream.
    void                        SetEventHandler(StreamEventHandler handler) _NOEXCEPT { _eventHandler = handler; }
    
    ///
    /// Synchronously wait for an event to occur.
    AsyncEvent                  WaitNextEvent(timeout_type timeout=timeout_type::max());
    
    /**
     Retrieve the RunLoop on which the event-handler will be invoked.
     
     If no RunLoop has been assigned, the event-handler will be invoked from the
     shared I/O thread directly.
     */
    virtual RunLoopPtr            EventTargetRunLoop()                const _NOEXCEPT { return _targetRunLoop; }
    ///
    /// Assign a RunLoop on which to invoke the event-handler.
    virtual void                SetTargetRunLoop(RunLoopPtr rl)             _NOEXCEPT;
    
    ///
    /// Retrieve the runloop-assignment callback.
    StreamScheduledHandler      GetScheduledHandler()               const _NOEXCEPT {
        return _streamScheduled;
    }
    ///
    /// Assign a handler function to be called when the stream is scheduled on a runloop.
    void                        SetScheduledHandler(StreamScheduledHandler handler) _NOEXCEPT {
        _streamScheduled = handler;
    }
    
    ///
    /// @copydoc ByteStream::BytesAvailable()
    virtual size_type           BytesAvailable()                    _NOEXCEPT  {
        return (_readbuf ? _readbuf->BytesAvailable() : 0);
    }
    ///
    /// @copydoc ByteStream::BytesAvailable()
    virtual size_type           SpaceAvailable()                    const _NOEXCEPT  {
        return (_writebuf ? _writebuf->SpaceAvailable() : 0);
    }
    
    /**
     Initializes the input/output buffers of the stream.
     
     Subclasses *must* call this as part of their stream opening sequence to
     initialize the required stream resources.
     @param mode Only `std::ios::in` and `std::ios::out` are supported. This flag
     controls the creation of the read/write buffers; the default is to create a
     bidirectional stream with both input and output buffers.
     */
    virtual void                Open(std::ios::openmode mode = std::ios::in|std::ios::out);
    
    ///
    /// @copydoc ByteStream::Close()
    virtual void                Close();
    
    /**
     @copydoc ByteStream::ReadBytes()
     All reads are serviced from the async stream's read buffer, which is filled
     asynchronously from the underlying resource.
     */
    virtual size_type           ReadBytes(void* buf, size_type len);
    /**
     @copydoc ByteStream::ReadBytes()
     All writes go to the async stream's write buffer, and will be written from
     there to the underlying resource asynchronously.
     */
    virtual size_type           WriteBytes(const void* buf, size_type len);
    
private:
    size_type                   _bufsize;           ///< The size of the read/write data buffers.
    shared_ptr<RingBuffer>      _readbuf;           ///< The read buffer, if opened for reading.
    shared_ptr<RingBuffer>      _writebuf;          ///< The write buffer, if opened for writing.
    StreamEventHandler          _eventHandler;      ///< The event-handler function to notify of stream status changes.
    
    std::atomic_flag            _closing;           ///< A flag used to prevent double-closures.
    
    static std::thread          _asyncIOThread;     ///< The shared async I/O thread.
    static RunLoopPtr           _asyncRunLoop;      ///< The shared I/O thread's run loop, to which streams will attach.
    static std::atomic_flag     _asyncInited;       ///< Set when the async thread is live, unset otherwise.
    
    RunLoop::EventSourcePtr     _eventSource;       ///< The event source used to communicate with the shared I/O thread.
    std::atomic<ThreadEvent>    _event;             ///< The internal event bitmask. @see ThreadEvent.
    RunLoopPtr                  _targetRunLoop;     ///< The runloop on which this stream should post status events.
    RunLoop::EventSourcePtr     _eventDispatchSource;   ///< The source used to post events to _targetRunLoop.
    
    StreamScheduledHandler      _streamScheduled;   ///< A callback to invoke when _targetRunLoop is assigned.
    
    // The AsyncPipe class wants to assign the buffers itself.
    friend class AsyncPipe;
    
protected:
    ///
    /// Called by subclasses to initialize the asynchronous event handler and RunLoop.
    /// @throw std::logic_error if this stream has already set up its RunLoop::EventSource.
    virtual void                InitAsyncHandler();
    ///
    /// Subclasses can override this to return their own EventSource. AsyncByteStream's
    /// implementation uses read_for_async() and write_for_async().
    virtual RunLoop::EventSourcePtr AsyncEventSource();
    ///
    /// Subclasses can implement this to return an event-dispatch source.
    virtual RunLoop::EventSourcePtr EventDispatchSource();
    ///
    /// Implemented by subclasses to synchronously read data from the underlying resource.
    /// @see ByteStream::ReadBytes(void*, size_type)
    virtual size_type           read_for_async(void* buf, size_type len)        = 0;
    ///
    /// Implemented by subclasses to synchronously write data to the underlying resource.
    /// @see ByteStream::WriteBytes(const void*, size_type)
    virtual size_type           write_for_async(const void* buf, size_type len) = 0;
    
    ///
    /// Used to prod the event source for this stream into action (if appropriate) when
    /// the stream is assigned to a runloop.
    void                        ReadyToRun();
};

/**
 A concrete AsyncByteStream subclass, providing a Unix-like bidirectional pipe.
 
 Pipes are created only through the Pair() static function. This returns a pair of
 streams which share their buffers, in reverse order. In other words, stream A's
 read-buffer is also stream B's write-buffer, and vice versa.
 @ingroup utilities
 */
class AsyncPipe : public AsyncByteStream
{
public:
    typedef std::pair<std::shared_ptr<AsyncPipe>, std::shared_ptr<AsyncPipe>> Pair;
    
    ///
    /// Create a pair of linked pipes. Writes to one will be available to read on the other.
    static Pair LinkedPair(size_type bufsize=4096);
    
    // The real constructor should be considered protected. Use Pair() to get a pipe.
    AsyncPipe(size_type bufsize=4096) : AsyncByteStream(bufsize), _counterpart(), _self_closed(false), _pair_closed(false) {}
    ~AsyncPipe();
    
private:
    AsyncPipe(const AsyncPipe&)                 _DELETED_;
    AsyncPipe(AsyncPipe&&)                      _DELETED_;
    AsyncPipe&                  operator=(const AsyncPipe&)               _DELETED_;
    AsyncPipe&                  operator=(AsyncPipe&&)                    _DELETED_;
    
protected:
    ///
    /// This is not callable by those on the outside...
    virtual void Open(std::ios::openmode mode=std::ios::in|std::ios::out) {
        _self_closed = false;
        AsyncByteStream::Open(mode);
    }
    
public:
    virtual void                Close() OVERRIDE;
    
    virtual bool                IsOpen() const _NOEXCEPT OVERRIDE { return !_self_closed; }
    
    virtual void                SetTargetRunLoop(RunLoopPtr rl) _NOEXCEPT OVERRIDE;
    
    virtual size_type           ReadBytes(void* buf, size_type len) OVERRIDE;
    virtual size_type           WriteBytes(const void* buf, size_type len) OVERRIDE;
    
protected:
    std::weak_ptr<AsyncPipe>    _counterpart;
    bool                        _self_closed;
    bool                        _pair_closed;
    
    // These do nothing here -- all the action is in this class's implementation of
    //  InitAsyncHandler()
    virtual size_type read_for_async(void* buf, size_type len);
    virtual size_type write_for_async(const void* buf, size_type len);
    
    virtual RunLoop::EventSourcePtr AsyncEventSource();
    
    virtual void                CounterpartClosed();
    
};
#endif /* SUPPORT_ASYNC */

/**
 An abstract ByteStream subclass representing (potentially limited) random-access capability.

 This is largely here for WinRT wrapper assistance.
 */
class SeekableByteStream : public ByteStream
{
public:
	SeekableByteStream() : ByteStream() {}
	virtual ~SeekableByteStream() {}

	/**
	Seek to a position within the target file.
	@param by The amount to move the file position.
	@param dir The starting point for the position calculation: current position,
	start of file, or end of file.
	@result The new file position. This may be different from the requested position,
	if for instance the file was not large enough to accomodate the request.
	*/
	virtual size_type       Seek(size_type by, std::ios::seekdir dir) { return 0; }

	/**
	Returns the current position within the target file.
	@result The current file position.
	*/
	virtual size_type		Position() const = 0;

	/**
	Ensures that all written data is pushed to permanent storage.
	*/
	virtual void			Flush() {}

	/**
	Creates a new independent stream object referring to the same file.

	The returned stream is already open with the same privileges as the receiver.
	@result A new FileByteStream instance.
	*/
	virtual std::shared_ptr<SeekableByteStream> Clone() const = 0;
};

/**
 A concrete ByteStream providing synchronous access to a resource on a filesystem.
 @ingroup utilities
 */
class FileByteStream : public SeekableByteStream
{
public:
    ///
    /// Create a new stream unassociated with any file.
                            FileByteStream()                        : SeekableByteStream(), _file(nullptr) {}
    /**
     Create a new stream to a given file and open it for reading and/or writing.
     @param pathToOpen The path to the file to open.
     @param mode Whether to open with read and/or write access. Default is to open
     for both reading and writing.
     */
    EPUB3_EXPORT            FileByteStream(const string& pathToOpen, std::ios::openmode mode = std::ios::in | std::ios::out);
    virtual                 ~FileByteStream();
    
private:
                            FileByteStream(const FileByteStream& o)             _DELETED_;
                            FileByteStream(FileByteStream&& o)                  _DELETED_;
    FileByteStream&         operator=(FileByteStream&)                          _DELETED_;
    FileByteStream&         operator=(FileByteStream&&)                         _DELETED_;
    
public:
    ///
    /// @copydoc ByteStream::BytesAvailable()
    virtual size_type       BytesAvailable()                        _NOEXCEPT;
    ///
    /// @copydoc ByteStream::SpaceAvailable()
    virtual size_type       SpaceAvailable()                        const _NOEXCEPT;
    
    ///
    /// @copydoc ByteStream::IsOpen()
    virtual bool            IsOpen()                                const _NOEXCEPT;
    /**
     Opens the stream for reading and/or writing a file at a given path.
     @param path The filesystem path to the file to access.
     @param mode Whether to open with read and/or write access. The default is to
     open for both reading and writing.
     @result Returns `true` if the file opened successfully, `false` otherwise.
     */
    virtual bool            Open(const string& path, std::ios::openmode mode = std::ios::in | std::ios::out);
    ///
    /// @copydoc ByteStream::Close()
    virtual void            Close();
    
    ///
    /// @copydoc ByteStream::ReadBytes()
    virtual size_type       ReadBytes(void* buf, size_type len);
    ///
    /// @copydoc ByteStream::WriteBytes()
    virtual size_type       WriteBytes(const void* buf, size_type len);
    
    /**
     Seek to a position within the target file.
     @param by The amount to move the file position.
     @param dir The starting point for the position calculation: current position,
     start of file, or end of file.
	 @result The new file position. This may be different from the requested position,
	 if for instance the file was not large enough to accomodate the request.
     */
    virtual size_type       Seek(size_type by, std::ios::seekdir dir) OVERRIDE;

	/**
	 Returns the current position within the target file.
	 @result The current file position.
	 */
	virtual size_type		Position() const OVERRIDE;

	/**
	 Ensures that all written data is pushed to permanent storage.
	 */
	virtual void			Flush() OVERRIDE;

	/**
	 Creates a new independent stream object referring to the same file.

	 The returned stream is already open with the same privileges as the receiver.
	 @result A new FileByteStream instance.
	 */
	virtual std::shared_ptr<SeekableByteStream> Clone() const OVERRIDE;
    
protected:
    FILE*                   _file;  ///< The underlying system file stream.
	std::ios::openmode		_mode;	///< The mode used to open the file (used by Clone()).
};

/**
 A concrete ByteStream providing access to a file within a Zip archive.
 @ingroup utilities
 */
class ZipFileByteStream : public SeekableByteStream
{
public:
    
    ///
    /// Create a new unattached stream.
                            ZipFileByteStream() : SeekableByteStream(), _file(nullptr) {}
    /**
     Create a new stream to a file within a zip archive.
     @param archive The Zip arrchive containing the target file.
     @param pathToOpen The path within the archive of the resource to open.
     @param zipFlags Flags such as whether to read the raw compressed data.
     */
    EPUB3_EXPORT            ZipFileByteStream(struct zip* archive, const string& pathToOpen, int zipFlags=0);
    virtual                 ~ZipFileByteStream();
    
private:
                            ZipFileByteStream(const ZipFileByteStream&)         _DELETED_;
                            ZipFileByteStream(ZipFileByteStream&&)              _DELETED_;
    ZipFileByteStream&      operator=(const ZipFileByteStream&)                 _DELETED_;
    ZipFileByteStream&      operator=(ZipFileByteStream&&)                      _DELETED_;
    
public:
    ///
    /// @copydoc ByteStream::BytesAvailable()
    virtual size_type       BytesAvailable()                        _NOEXCEPT;
    ///
    /// @copydoc ByteStream::SpaceAvailable
    virtual size_type       SpaceAvailable()                        const _NOEXCEPT;
    
    ///
    /// @copydoc ByteStream::IsOpen()
    virtual bool            IsOpen()                                const _NOEXCEPT;
    /**
     Opens a file within an archive and attaches the stream.
     @param archive The Zip arrchive containing the target file.
     @param pathToOpen The path within the archive of the resource to open.
     @param zipFlags Flags such as whether to read the raw compressed data.
     @result Returns `true` if the file opened successfully, `false` otherwise.
     */
    virtual bool            Open(struct zip* archive, const string& path, int zipFlags=0);
    ///
    /// @copydoc ByteStream::Close()
    virtual void            Close();
    
    ///
    /// @copydoc ByteStream::ReadBytes()
    virtual size_type       ReadBytes(void* buf, size_type len);
    ///
    /// @copydoc ByteStream::WriteBytes()
	virtual size_type       WriteBytes(const void* buf, size_type len);
    
    /**
     Seek to a position within the target file.
     @param by The amount to move the file position.
     @param dir The starting point for the position calculation: current position,
     start of file, or end of file.
	 @result The new file position. This may be different from the requested position,
	 if for instance the file was not large enough to accomodate the request.
    */
    virtual size_type       Seek(size_type by, std::ios::seekdir dir) OVERRIDE;

	/**
	Returns the current position within the target file.
	@result The current file position.
	*/
	virtual size_type		Position() const OVERRIDE;

	/**
	Creates a new independent stream object referring to the same file.

	The returned stream is already open with the same privileges as the receiver.
	@result A new FileByteStream instance.
	*/
	virtual std::shared_ptr<SeekableByteStream> Clone() const OVERRIDE;
    
protected:

    struct zip_file*        _file;      ///< The underlying Zip file stream.
	std::ios::openmode		_mode;		///< The mode used to open the file (used by Clone()).

    // Seek by rewind emulation support
    size_type bytes_left;
    size_type total_size;
    uint64_t _idx;          // the file index identifier inside zip file structure
private:
    // helper functions to emulate seek operation if it is not supported by lower layers of libzip
    int     SeekByReading(size_t toread);
    bool    SeekToStart();
    int     SeekByRewind(long pos, int whence);
    
    int64_t _supports;
    string  _path;
    int     _openFlags;
};

#ifdef SUPPORT_ASYNC
/**
 A concrete AsyncByteStream subclass providing access to a filesystem resource.
 @ingroup utilities
 */
class AsyncFileByteStream : public AsyncByteStream, public FileByteStream
{
private:
    typedef FileByteStream  __F;
    typedef AsyncByteStream __A;
    
public:
    ///
    /// Create a new unattached stream.
                            //AsyncFileByteStream() : AsyncByteStream(), FileByteStream() {}
    
    ///
    /// Create a new stream attached to a filesystem resource and no event handler.
                            AsyncFileByteStream(size_type bufsize=4096) : AsyncByteStream(bufsize), FileByteStream() {}
    
    ///
    /// Create a new unattached stream with a given event-handler.
    /// @see AsyncByteStream::AsyncByteStream(StreamEventHandler,size_type)
                            AsyncFileByteStream(StreamEventHandler handler, size_type bufsize=4096) : AsyncByteStream(handler, bufsize), FileByteStream() {}
    ///
    /// Create a new stream attached to a filesystem resource with an event-handler.
    /// @see AsyncByteStream::AsyncByteStream(StreamEventHandler,size_type)
    /// @see FileByteStream::FileByteStream(const string&,std::ios::openmode)
                            AsyncFileByteStream(StreamEventHandler handler, const string& path, std::ios::openmode mode = std::ios::in | std::ios::out, size_type bufsize=4096);
    virtual                 ~AsyncFileByteStream() {}
    
private:
                            AsyncFileByteStream(const AsyncFileByteStream&) _DELETED_;
                            AsyncFileByteStream(AsyncFileByteStream&&)      _DELETED_;
    AsyncFileByteStream&    operator=(const AsyncFileByteStream&)           _DELETED_;
    AsyncFileByteStream&    operator=(AsyncFileByteStream&&)                _DELETED_;
    
public:
    // use the ringbuffer-based availability functions from AsyncByteStream
    ///
    /// @copydoc AsyncByteStream::BytesAvailable
    virtual size_type       BytesAvailable()    _NOEXCEPT              { return __A::BytesAvailable(); }
    ///
    /// @copydoc AsyncByteStream::SpaceAvailable
    virtual size_type       SpaceAvailable()    const _NOEXCEPT              { return __A::SpaceAvailable(); }
    
    // use the file stream's IsOpen()
    ///
    /// @copydoc FileByteStream::IsOpen()
    virtual bool            IsOpen()            const _NOEXCEPT              { return __F::IsOpen(); }
    
    // use the async stream's read/writers
    ///
    /// @copydoc AsyncByteStream::ReadBytes()
    virtual size_type       ReadBytes(void* buf, size_type len)             { return __A::ReadBytes(buf, len); }
    ///
    /// @copydoc AsyncByteStream::WriteBytes()
    virtual size_type       WriteBytes(const void* buf, size_type len)      { return __A::WriteBytes(buf, len); }
    
    ///
    /// @copydoc FileByteStream::Open()
    virtual bool            Open(const string& path, std::ios::openmode mode = std::ios::in | std::ios::out);
    ///
    /// @copydoc FileByteStream::Close()
	virtual void            Close();

	/**
	Creates a new independent stream object referring to the same file.

	The returned stream is already open with the same privileges as the receiver.
	@result A new FileByteStream instance.
	*/
	virtual std::shared_ptr<SeekableByteStream> Clone() const OVERRIDE;
    
private:
    // seeking disabled on async streams, because I value my sanity
    virtual size_type       Seek()                                          { return 0; }

protected:
    virtual size_type       read_for_async(void* buf, size_type len)        { return __F::ReadBytes(buf, len); }
    virtual size_type       write_for_async(const void* buf, size_type len) { return __F::WriteBytes(buf, len); }
};

/**
 A concrete AsyncByteStream subclass providing access to a file within a Zip archive.
 @ingroup utilities
 */
class AsyncZipFileByteStream : public AsyncByteStream, public ZipFileByteStream
{
private:
    typedef ZipFileByteStream   __F;
    typedef AsyncByteStream     __A;
    
public:
    ///
    /// Create a new unattached stream.
                            AsyncZipFileByteStream() : AsyncByteStream(), ZipFileByteStream() {}
    
    ///
    /// Create a new opened stream with no default handler.
                            AsyncZipFileByteStream(struct zip* archive, const string& path, int zipFlags=0);
    ///
    /// @copydoc AsyncFileByteStream::AsyncFileByteStream(StreamEventHandler)
                            AsyncZipFileByteStream(StreamEventHandler handler) : AsyncByteStream(handler), ZipFileByteStream() {}
    ///
    /// Create a new stream attached to a file in a given Zip archive with an event-handler.
    /// @see AsyncByteStream::AsyncByteStream(StreamEventHandler,size_type)
    /// @see ZipFileByteStream::ZipFileByteStream(struct zip*,const string&,int)
                            AsyncZipFileByteStream(StreamEventHandler handler, struct zip* archive, const string& path, int zipFlags=0) : AsyncByteStream(handler), ZipFileByteStream(archive, path, zipFlags) {}
    virtual                 ~AsyncZipFileByteStream() {}
    
private:
                            AsyncZipFileByteStream(const AsyncZipFileByteStream&)   _DELETED_;
                            AsyncZipFileByteStream(AsyncZipFileByteStream&&)        _DELETED_;
    AsyncZipFileByteStream& operator=(const AsyncZipFileByteStream&)                _DELETED_;
    AsyncZipFileByteStream& operator=(AsyncZipFileByteStream&&)                     _DELETED_;
    
public:
    // use the ringbuffer-based availability functions from AsyncByteStream
    ///
    /// @copydoc AsyncByteStream::BytesAvailable
    virtual size_type       BytesAvailable()    _NOEXCEPT              { return __A::BytesAvailable(); }
    ///
    /// @copydoc AsyncByteStream::SpaceAvailable
    virtual size_type       SpaceAvailable()    const _NOEXCEPT              { return __A::SpaceAvailable(); }
    
    // use the file stream's IsOpen()
    ///
    /// @copydoc FileByteStream::IsOpen()
    virtual bool            IsOpen()            const _NOEXCEPT              { return __F::IsOpen(); }
    
    // use the async stream's read/writers
    ///
    /// @copydoc AsyncByteStream::ReadBytes()
    virtual size_type       ReadBytes(void* buf, size_type len)             { return __A::ReadBytes(buf, len); }
    ///
    /// @copydoc AsyncByteStream::WriteBytes()
    virtual size_type       WriteBytes(const void* buf, size_type len)      { return __A::WriteBytes(buf, len); }
    
    ///
    /// @copydoc ZipFileByteStream::Open()
    virtual bool            Open(struct zip* archive, const string& path, int zipFlags=0) OVERRIDE;
    ///
    /// @copydoc ByteStream::Close()
	virtual void            Close();

	/**
	Creates a new independent stream object referring to the same file.

	The returned stream is already open with the same privileges as the receiver.
	@result A new FileByteStream instance.
	*/
	virtual std::shared_ptr<SeekableByteStream> Clone() const OVERRIDE;
    
protected:
    virtual size_type       read_for_async(void* buf, size_type len)        { return __F::ReadBytes(buf, len); }
    virtual size_type       write_for_async(const void* buf, size_type len) { return __F::WriteBytes(buf, len); }
};
#endif /* SUPPORT_ASYNC */

EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__byte_stream__) */
