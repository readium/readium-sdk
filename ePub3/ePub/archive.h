//
//  archive.h
//  ePub3
//
//  Created by Jim Dovey on 2012-11-27.
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

#ifndef __ePub3__archive__
#define __ePub3__archive__

#include "epub3.h"
#include <iostream>
#include <map>
#include <zlib.h>
#include <sys/acl.h>

EPUB3_BEGIN_NAMESPACE

class ArchiveItemInfo;
class ArchiveReader;
class ArchiveWriter;
class ByteStream;

class Archive
{
protected:
    template <class T>
    class func_less : public std::binary_function<T, T, bool>
    {
    public:
        inline bool operator ()(const T& __a, const T&__b) const
        {
            return (&__a) < (&__b);
        }
    };
    
public:
    // based on the zlib compression levels
    typedef int CompressionLevel;
    static const CompressionLevel DefaultCompression = -1;
    static const CompressionLevel Uncompressed = 0;
    static const CompressionLevel FastestCompression = 1;
    static const CompressionLevel SmallestCompression = 9;
    
protected:
    typedef std::function<Archive*(const std::string&)>     ArchiveFactory;
    typedef std::function<bool(const std::string&)>         ArchiveTypeSniffer;
    typedef std::map<ArchiveTypeSniffer, ArchiveFactory, func_less<ArchiveTypeSniffer> >    ArchiveRegistrationDomain;
    
    static ArchiveRegistrationDomain RegistrationDomain;
    static void RegisterArchive(ArchiveTypeSniffer sniffer, ArchiveFactory factory);
    
public:
    static void Initialize();
    static Archive * Open(const std::string& path);
    
public:
    virtual ~Archive() {}
    
    virtual std::string Path() const { return _path; }
    
    virtual bool ContainsItem(const std::string & path) const = 0;
    virtual bool DeleteItem(const std::string & path) = 0;
    
    virtual bool CreateFolder(const std::string & path) = 0;
    
    virtual Auto<ByteStream> ByteStreamAtPath(const std::string& path) const = 0;
    
    virtual ArchiveReader* ReaderAtPath(const std::string & path) const = 0;
    virtual ArchiveWriter* WriterAtPath(const std::string & path, bool compress=true, bool create=true) = 0;
    
    virtual bool ShouldCompress(const std::string& path, const std::string& mimeType, size_t size) const;
    
    virtual void SetPOSIXPermissions(const std::string & path, mode_t privs) {}
    virtual mode_t POSIXPermissions(const std::string & path) const { return 0; }
    
    virtual void SetACL(const std::string & path, acl_t acl) {}
    virtual acl_t GetACL(const std::string & path) const { return nullptr; }
    
    virtual ArchiveItemInfo InfoAtPath(const std::string & path) const;
    
    // scary Ghostbusters Zuul voice: "there is no copy, only move"
    Archive & operator = (const Archive &) = delete;
    Archive & operator = (Archive &&) { return *this; }
    
protected:
    Archive() {}
    Archive(const std::string & path) : _path(path) {}
    Archive(const Archive &) = delete;  // copying is not allowed
    Archive(Archive && o) : _path(std::move(o._path)) {} // moving is allowed
    
    std::string         _path;
    
};

class ArchiveItemInfo
{
public:
    ArchiveItemInfo() {}
    ArchiveItemInfo(const ArchiveItemInfo & o) : _path(o._path), _isCompressed(o._isCompressed), _compressedSize(o._compressedSize), _uncompressedSize(o._uncompressedSize), _posix(o._posix) {
        if ( o._acl != nullptr )
            _acl = acl_dup(o._acl);
    }
    ArchiveItemInfo(ArchiveItemInfo && o) : _path(std::move(o._path)), _isCompressed(o._isCompressed), _compressedSize(o._compressedSize), _uncompressedSize(o._uncompressedSize), _posix(o._posix), _acl(o._acl) {
            o._acl = nullptr;
    }
    virtual ~ArchiveItemInfo() { if (_acl != nullptr) acl_free(_acl); }
    
    virtual std::string Path() const { return _path; }
    virtual bool IsCompressed() const { return _isCompressed; }
    virtual size_t CompressedSize() const { return _compressedSize; }
    virtual size_t UncompressedSize() const { return _uncompressedSize; }
    virtual mode_t POSIXPermissions() const { return _posix; }
    virtual acl_t AccessControlList() const { return _acl; }
    
    virtual void SetPath(const std::string & path) { _path = path; }
    virtual void SetPath(std::string &&path) { _path = path; }
    virtual void SetIsCompressed(bool flag) { _isCompressed = flag;}
    virtual void SetCompressedSize(size_t size) { _compressedSize = size; }
    virtual void SetUncompressedSize(size_t size) { _uncompressedSize = size; }
    virtual void SetPOSIXPermissions(mode_t perms) { _posix = perms; }
    virtual void SetAccessControlList(acl_t acl) { _acl = acl_dup(acl); }
    
protected:
    std::string                 _path;
    bool                        _isCompressed;
    size_t                      _compressedSize;
    size_t                      _uncompressedSize;
    
    mode_t                      _posix;
    acl_t                       _acl;
    
};

class ArchiveReader
{
public:
    virtual ~ArchiveReader() {}
    
    virtual bool operator !() const { return true; }
    virtual ssize_t read(void *p, size_t len) const { return 0; }
    
protected:
    ArchiveReader() = default;
    ArchiveReader(const ArchiveReader &) = delete;
    ArchiveReader(ArchiveReader &&) = default;
};

class ArchiveWriter
{
public:
    virtual ~ArchiveWriter() {}
    
    virtual bool operator !() const { return true; }
    virtual ssize_t write(const void *p, size_t len) { return 0; }
    
protected:
    ArchiveWriter() = default;
    ArchiveWriter(const ArchiveWriter&) = delete;
    ArchiveWriter(ArchiveWriter&&) = default;
};

EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__archive__) */
