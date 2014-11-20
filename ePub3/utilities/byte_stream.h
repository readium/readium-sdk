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
        url_util::DecodeURLEscapeSequences(path.c_str(), static_cast<int>(path.size()), &output);
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
    virtual size_type       BytesAvailable()                        const _NOEXCEPT  { return UnknownSize; }
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
    virtual size_type       BytesAvailable()                        const _NOEXCEPT;
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
    virtual size_type       BytesAvailable()                        const _NOEXCEPT;
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

};

EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__byte_stream__) */
