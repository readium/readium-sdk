//
//  zip_archive.cpp
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

#include "zip_archive.h"
#include <libzip/zipint.h>
#include "byte_stream.h"
#include <sstream>
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>

#if EPUB_OS(ANDROID)
extern "C" char* gAndroidCacheDir;
#endif

EPUB3_BEGIN_NAMESPACE

static std::string GetTempFilePath(const std::string& ext)
{
    std::stringstream ss;
#if EPUB_OS(ANDROID)
    ss << gAndroidCacheDir << "epub3.XXXXXX";
#else
    ss << "/tmp/epub3.XXXXXX." << ext;
#endif
    std::string path(ss.str());
    
    char *buf = new char[path.length()];
    std::char_traits<char>::copy(buf, ss.str().c_str(), sizeof(buf));

#if EPUB_OS(ANDROID)
    int fd = ::mkstemp(buf);
#else
    int fd = ::mkstemps(buf, static_cast<int>(ext.size()+1));
#endif
    if ( fd == -1 )
        throw std::runtime_error(std::string("mkstemp() failed: ") + strerror(errno));
    
    ::close(fd);
    return std::string(buf);
}

class ZipReader : public ArchiveReader
{
public:
    ZipReader(struct zip_file* file) : _file(file) {}
    ZipReader(ZipReader&& o) : _file(o._file) { o._file = nullptr; }
    virtual ~ZipReader() { if (_file != nullptr) zip_fclose(_file); }
    
    virtual bool operator !() const { return _file == nullptr || _file->bytes_left == 0; }
    virtual ssize_t read(void* p, size_t len) const { return zip_fread(_file, p, len); }
    
private:
    struct zip_file * _file;
};

class ZipWriter : public ArchiveWriter
{
    class DataBlob
    {
    public:
        DataBlob() : _tmpPath(GetTempFilePath("tmp")), _fs(_tmpPath, std::ios::in|std::ios::out|std::ios::binary|std::ios::trunc) {}
        DataBlob(const DataBlob&) = delete;
        DataBlob(DataBlob&& o) : _tmpPath(std::move(o._tmpPath)), _fs(_tmpPath, std::ios::in|std::ios::out|std::ios::binary|std::ios::trunc) {}
        ~DataBlob() { _fs.close(); ::unlink(_tmpPath.c_str()); }
        
        void Append(const void * data, size_t len);
        size_t Read(void *buf, size_t len);
        
        size_t Size() { return _fs.tellp(); }
        size_t Size() const { return const_cast<DataBlob*>(this)->Size(); }
        size_t Avail() { return Size() - _fs.tellg(); }
        size_t Avail() const { return const_cast<DataBlob*>(this)->Avail(); }
        
    protected:
        std::string     _tmpPath;
        std::fstream    _fs;
    };
    
public:
    ZipWriter(struct zip* zip, const std::string& path, bool compressed);
    ZipWriter(ZipWriter&& o);
    virtual ~ZipWriter() { if (_zsrc != nullptr) zip_source_free(_zsrc); }
    
    virtual bool operator !() const { return false; }
    virtual ssize_t write(const void *p, size_t len) { _data.Append(p, len); return static_cast<ssize_t>(len); }
    
    struct zip_source* ZipSource() { return _zsrc; }
    const struct zip_source* ZipSource() const { return _zsrc; }
    
protected:
    bool                _compressed;
    DataBlob            _data;
    struct zip_source*  _zsrc;
    
    static ssize_t _source_callback(void *state, void *data, size_t len, enum zip_source_cmd cmd);
    
};

ZipArchive::ZipItemInfo::ZipItemInfo(struct zip_stat & info)
{
    SetPath(info.name);
    SetIsCompressed(info.comp_method == ZIP_CM_STORE);
    SetCompressedSize(static_cast<size_t>(info.comp_size));
    SetUncompressedSize(static_cast<size_t>(info.size));
}

std::string ZipArchive::TempFilePath()
{
    return GetTempFilePath("zip");
}
ZipArchive::ZipArchive(const std::string & path)
{
    int zerr = 0;
    _zip = zip_open(path.c_str(), ZIP_CREATE, &zerr);
    if ( _zip == nullptr )
        throw std::runtime_error(std::string("zip_open() failed: ") + zError(zerr));
    _path = path;
}
ZipArchive::~ZipArchive()
{
    if ( _zip != nullptr )
        zip_close(_zip);
}
Archive & ZipArchive::operator = (ZipArchive &&o)
{
    if ( _zip != nullptr )
        zip_close(_zip);
    _zip = o._zip;
    o._zip = nullptr;
    return dynamic_cast<Archive&>(*this);
}
bool ZipArchive::ContainsItem(const std::string & path) const
{
    return (zip_name_locate(_zip, Sanitized(path).c_str(), 0) >= 0);
}
bool ZipArchive::DeleteItem(const std::string & path)
{
    int idx = zip_name_locate(_zip, Sanitized(path).c_str(), 0);
    if ( idx >= 0 )
        return (zip_delete(_zip, idx) >= 0);
    return false;
}
bool ZipArchive::CreateFolder(const std::string & path)
{
    return (zip_add_dir(_zip, Sanitized(path).c_str()) >= 0);
}
Auto<ByteStream> ZipArchive::ByteStreamAtPath(const std::string &path) const
{
    return Auto<ByteStream>(new ZipFileByteStream(_zip, path));
}
ArchiveReader* ZipArchive::ReaderAtPath(const std::string & path) const
{
    if (_zip == nullptr)
        return nullptr;
    
    struct zip_file* file = zip_fopen(_zip, Sanitized(path).c_str(), 0);
    if (file == nullptr)
        return nullptr;
    
    return new ZipReader(file);
}
ArchiveWriter* ZipArchive::WriterAtPath(const std::string & path, bool compressed, bool create)
{
    if (_zip == nullptr)
        return nullptr;
    
    int idx = zip_name_locate(_zip, Sanitized(path).c_str(), (create ? ZIP_CREATE : 0));
    if (idx == -1)
        return nullptr;
    
    ZipWriter* writer = new ZipWriter(_zip, Sanitized(path), compressed);
    if ( zip_replace(_zip, idx, writer->ZipSource()) == -1 )
    {
        delete writer;
        return nullptr;
    }
    
    return writer;
}
ArchiveItemInfo ZipArchive::InfoAtPath(const std::string & path) const
{
    struct zip_stat sbuf;
    if ( zip_stat(_zip, Sanitized(path).c_str(), 0, &sbuf) < 0 )
        throw std::runtime_error(std::string("zip_stat("+path+") - " + zip_strerror(_zip)));
    return ZipItemInfo(sbuf);
}
std::string ZipArchive::Sanitized(const std::string& path) const
{
    if ( path.find('/') == 0 )
        return path.substr(1);
    return path;
}

void ZipWriter::DataBlob::Append(const void *data, size_t len)
{
    _fs.write(reinterpret_cast<const decltype(_fs)::char_type *>(data), len);
}
size_t ZipWriter::DataBlob::Read(void *data, size_t len)
{
    if ( _fs.tellg() == 0 )
        _fs.flush();
    return _fs.readsome(reinterpret_cast<decltype(_fs)::char_type *>(data), len);
}

ZipWriter::ZipWriter(struct zip *zip, const std::string& path, bool compressed)
    : _compressed(compressed)
{
    _zsrc = zip_source_function(zip, &ZipWriter::_source_callback, reinterpret_cast<void*>(this));
}
ZipWriter::ZipWriter(ZipWriter&& o) : _compressed(o._compressed), _data(std::move(o._data)), _zsrc(o._zsrc)
{
    o._zsrc = nullptr;
    _zsrc->ud = reinterpret_cast<void*>(this);
}
ssize_t ZipWriter::_source_callback(void *state, void *data, size_t len, enum zip_source_cmd cmd)
{
    ssize_t r = 0;
    ZipWriter * writer = reinterpret_cast<ZipWriter*>(state);
    switch ( cmd )
    {
        case ZIP_SOURCE_OPEN:
        {
            if ( !(*writer) )
                return -1;
            break;
        }
        case ZIP_SOURCE_CLOSE:
        {
            break;
        }
        case ZIP_SOURCE_STAT:
        {
            if (len < sizeof(struct zip_stat))
                return -1;
            
            struct zip_stat *st = reinterpret_cast<struct zip_stat*>(data);
            zip_stat_init(st);
            st->mtime = ::time(NULL);
            st->size = writer->_data.Size();
            st->comp_method = (writer->_compressed ? ZIP_CM_DEFLATE : ZIP_CM_STORE);
            r = sizeof(struct zip_stat);
            break;
        }
        case ZIP_SOURCE_ERROR:
        default:
        {
            if ( len < sizeof(int)*2 )
                return -1;
            int *p = reinterpret_cast<int*>(data);
            p[0] = p[1] = 0;
            r = sizeof(int)*2;
            break;
        }
        case ZIP_SOURCE_READ:
        {
            r = writer->_data.Read(data, len);
            break;
        }
        case ZIP_SOURCE_FREE:
        {
            // the caller will free this
            writer->_zsrc = nullptr;
            delete writer;
            return 0;
        }
            
    }
    return r;
}

EPUB3_END_NAMESPACE
