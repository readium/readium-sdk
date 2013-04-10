//
//  archive.h
//  ePub3
//
//  Created by Jim Dovey on 2012-11-27.
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

#ifndef __ePub3__archive__
#define __ePub3__archive__

#include "epub3.h"
#include <iostream>
#include <list>
#include <zlib.h>
#include <sys/acl.h>

EPUB3_BEGIN_NAMESPACE

class ArchiveItemInfo;
class ArchiveReader;
class ArchiveWriter;
class ByteStream;

/**
 An abstract class representing a generic archive.
 
 @ingroup archives
 */
class Archive
{
public:
    ///
    /// Types for predefined archive compression levels.
    typedef int CompressionLevel;
    
    ///
    /// Use the default compression level for the archive type.
    static const CompressionLevel DefaultCompression = -1;
    ///
    /// Don't compress.
    static const CompressionLevel Uncompressed = 0;
    ///
    /// Fastest operation, usually the least compression.
    static const CompressionLevel FastestCompression = 1;
    ///
    /// Smallest compressed file size, usually the slowest to compress/decompress.
    static const CompressionLevel SmallestCompression = 9;
    
protected:
    ///
    /// Type of a function which creates an Archive from a file.
    typedef std::function<Archive*(const std::string&)>     CreatorFn;
    ///
    /// Type of a function which determines whether a file is a certain type of archive.
    typedef std::function<bool(const std::string&)>         SnifferFn;
    
private:
    /**
     A simple class which encapsulates a factory for Archive subclasses.
     */
    class ArchiveFactory
    {
    public:
        
    public:
        ArchiveFactory() {}
        ArchiveFactory(CreatorFn c, SnifferFn t) : _creator(c), _typeSniffer(t) {}
        ArchiveFactory(const ArchiveFactory& o) : _creator(o._creator), _typeSniffer(o._typeSniffer) {}
        ArchiveFactory(ArchiveFactory&& o) : _creator(std::move(o._creator)), _typeSniffer(std::move(o._typeSniffer)) {}
        ~ArchiveFactory() {}
        
        bool                CanInit(const std::string& path)    const   { return _typeSniffer(path); }
        Archive *           operator()(const std::string& path) const   { return _creator(path); }
        
    private:
        CreatorFn       _creator;
        SnifferFn       _typeSniffer;
    };
    
    ///
    /// A list of factories.
    typedef std::list<ArchiveFactory>           ArchiveRegistrationDomain;
    static ArchiveRegistrationDomain            RegistrationDomain;
    
protected:
    /**
     Register an archive factory.
     @param creator A function object which, when invoked with a path, will return
     an opened instance of an Archive subclass (or `nullptr` upon failure).
     @param sniffer A function object which, whin invoked with a path, will return
     `true` if it represents an archive which can be opened using the function object
     in the `creator` argument.
     */
    static void RegisterArchive(CreatorFn creator, SnifferFn sniffer);
    
public:
    ///
    /// Initialize the library, registering the default archive types.
    static void Initialize();
    
    /**
     Open an archive.
     @param path A filesystem path to an archive file.
     @result An opened instance of an Archive subclass or `nullptr`.
     */
    static Archive * Open(const std::string& path);
    
public:
    virtual ~Archive() {}
    
    ///
    /// Retrieves the filesystem path of this archive.
    virtual std::string Path() const { return _path; }
    
    /**
     Check whether an item exists within an archive.
     @param path A path within the archive to a specific item.
     @result Returns `true` if an item exists with that path, `false` otherwise.
     */
    virtual bool ContainsItem(const std::string & path) const = 0;
    /**
     Delete an item from an archive.
     @note The return value indicates whether the item is no longer present in the
     archive, *not* whether it was actually deleted by this function. If the item is
     not found, this method will return `true`.
     @param path The path within the archive of the item to remove.
     @result Returns `true` if the item is no longer in the archive, `false`
     otherwise.
     */
    virtual bool DeleteItem(const std::string & path) = 0;
    
    /**
     Creates a folder in an archive.
     @param path The path within the archive for the new folder.
     @result Returns `true` if the polder was created, `false` otherwise.
     */
    virtual bool CreateFolder(const std::string & path) = 0;
    
    /**
     Obtains a stream to read or write to a file within the archive.
     @param path The path of the item to access.
     @result Returns a (managed) pointer to the resulting byte stream, or `nullptr`.
     */
    virtual Auto<ByteStream> ByteStreamAtPath(const std::string& path) const = 0;
    
    /**
     Obtain an object used to read data from a file within the archive.
     @param path The path of the item to read.
     @result A pointer (unmanaged) to a reader object, or `nullptr`.
     @deprecated Please use ByteStreamAtPath(const std::string&)const instead.
     */
    virtual ArchiveReader* ReaderAtPath(const std::string & path) const = 0;
    
    /**
     Obtain an object used to write data to a file within the archive.
     @param path The path of the item to read.
     @param compress Whether to compress any data to the file.
     @param create Whether to create a new file if one does not yet exist.
     @result A pointer (unmanaged) to a reader object, or `nullptr`.
     @deprecated Please use ByteStreamAtPath(const std::string&)const instead.
     */
    virtual ArchiveWriter* WriterAtPath(const std::string & path, bool compress=true, bool create=true) = 0;
    
    /**
     Determines whether a given file ought to be compressed when stored in the archive.
     
     The default implementation looks at the `mediaType` to check for pre-compressed
     image, audio, or video data, for which it returns `false`. It will also return
     `false` for any file below 1024KB in size. For all other input, it returns 
     `true`.
     @param path The path of an item. Does not need to exist in the archive.
     @param mimeType The MIME type or media-type of the data to be stored.
     @param size The number of bytes expected to be written.
     @result Returns `true` if the data is considered appropriate to compress, `false`
     if the archive implementor believes compression is unnecessary.
     */
    virtual bool ShouldCompress(const std::string& path, const std::string& mimeType, size_t size) const;
    
    /**
     Apply POSIX-style permissions to a file within the archive, if supported.
     
     The default implementation does nothing.
     */
    virtual void SetPOSIXPermissions(const std::string & path, mode_t privs) {}
    /**
     Obtain POSIX-style permissions for a file, if supported.
     
     The default implementation returns zero.
     */
    virtual mode_t POSIXPermissions(const std::string & path) const { return 0; }
    
    /**
     Apply an Access Control List to a file within the archive, if supported.
     
     The default implementation does nothing.
     */
    virtual void SetACL(const std::string & path, acl_t acl) {}
    /**
     Retrieve the Access Control List for a file, if supported.
     
     The befault implementation returns `nullptr`.
     */
    virtual acl_t GetACL(const std::string & path) const { return nullptr; }
    
    /**
     Returns detailed information on a file within the archive.
     @param path The path to an existing file within the archive.
     @result An object containing detailed information on the requested item.
     */
    virtual ArchiveItemInfo InfoAtPath(const std::string & path) const;
    
    // scary Ghostbusters Zuul voice: "there is no copy, only move"
    ///
    /// Archive objects cannot be copied.
    Archive & operator = (const Archive &) = delete;
    ///
    /// Move constructor.
    Archive & operator = (Archive &&) { return *this; }
    
protected:
    ///
    /// Archives must always be created through Archive::Open(const std::string&).
    Archive() {}
    Archive(const std::string & path) : _path(path) {}
    Archive(const Archive &) = delete;  // copying is not allowed
    Archive(Archive && o) : _path(std::move(o._path)) {} // moving is allowed
    
    std::string         _path;      ///< The path to the archive file.
    
};

/**
 Encapsulates detailed information on a file within an archive.
 @ingroup archives
 */
class ArchiveItemInfo
{
public:
    ///
    /// Default constructor
    ArchiveItemInfo() = default;
    ///
    /// Copy constructor
    ArchiveItemInfo(const ArchiveItemInfo & o) : _path(o._path), _isCompressed(o._isCompressed), _compressedSize(o._compressedSize), _uncompressedSize(o._uncompressedSize), _posix(o._posix) {
        if ( o._acl != nullptr )
            _acl = acl_dup(o._acl);
    }
    ///
    /// Move constructor
    ArchiveItemInfo(ArchiveItemInfo && o) : _path(std::move(o._path)), _isCompressed(o._isCompressed), _compressedSize(o._compressedSize), _uncompressedSize(o._uncompressedSize), _posix(o._posix), _acl(o._acl) {
            o._acl = nullptr;
    }
    virtual ~ArchiveItemInfo() { if (_acl != nullptr) acl_free(_acl); }
    
    ///
    /// Retrieves the item's p4ath within an archive.
    virtual std::string Path() const { return _path; }
    ///
    /// Whether the item is compressed.
    virtual bool IsCompressed() const { return _isCompressed; }
    ///
    /// The compressed size of the item.
    virtual size_t CompressedSize() const { return _compressedSize; }
    ///
    /// The uncompressed size of the item.
    virtual size_t UncompressedSize() const { return _uncompressedSize; }
    ///
    /// POSIX-style access permissions, if supported.
    virtual mode_t POSIXPermissions() const { return _posix; }
    ///
    /// Access Control List permissions, if supported.
    virtual acl_t AccessControlList() const { return _acl; }
    
    virtual void SetPath(const std::string & path) { _path = path; }
    virtual void SetPath(std::string &&path) { _path = path; }
    virtual void SetIsCompressed(bool flag) { _isCompressed = flag;}
    virtual void SetCompressedSize(size_t size) { _compressedSize = size; }
    virtual void SetUncompressedSize(size_t size) { _uncompressedSize = size; }
    virtual void SetPOSIXPermissions(mode_t perms) { _posix = perms; }
    virtual void SetAccessControlList(acl_t acl) { _acl = acl_dup(acl); }
    
protected:
    std::string                 _path;              ///< The path to the item.
    bool                        _isCompressed;      ///< Whether the item is compressed.
    size_t                      _compressedSize;    ///< The item's compressed size.
    size_t                      _uncompressedSize;  ///< The item's uncompressed size.
    
    mode_t                      _posix;             ///< POSIX permissions, if supported.
    acl_t                       _acl;               ///< Access Control List, if supported.
    
};

/**
 A simple stream-like reader object used to fetch data from an archive.
 @deprecated This object has been superceded by the ByteStream API.
 @ingroup archives
 */
class ArchiveReader
{
public:
    virtual ~ArchiveReader() {}
    
    ///
    /// Returns `true` if reading is possible (i.e. no error has occurred).
    virtual bool operator !() const { return true; }
    /**
     Reads data from this object's target file.
     @param p A buffer into which to place data.
     @param len The size of the buffer at `p`. No more than this number of bytes will
     be read from the archive during this call.
     @result Returns the number of bytes read, or `-1` in case of error.
     */
    virtual ssize_t read(void *p, size_t len) const { return 0; }
    
protected:
    ArchiveReader() = default;
    ArchiveReader(const ArchiveReader &) = delete;
    ArchiveReader(ArchiveReader &&) = default;
};

/**
 A simple stream-like writer object used to add data to an archive.
 @deprecated This object has been superceded by the ByteStream API.
 @ingroup archives
 */
class ArchiveWriter
{
public:
    virtual ~ArchiveWriter() {}
    
    ///
    /// Returns `true` if writing is possible (i.e. no error has occurred).
    virtual bool operator !() const { return true; }
    /**
     Writes data to this object's target file.
     @param p A buffer from which to read data.
     @param len The size of the buffer at `p`. No more than this number of bytes will
     be written to the archive during this call.
     @result Returns the number of bytes written, or `-1` in case of error.
     */
    virtual ssize_t write(const void *p, size_t len) { return 0; }
    
protected:
    ArchiveWriter() = default;
    ArchiveWriter(const ArchiveWriter&) = delete;
    ArchiveWriter(ArchiveWriter&&) = default;
};

EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__archive__) */
