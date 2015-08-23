//
//  zip_archive.cpp
//  ePub3
//
//  Created by Jim Dovey on 2012-11-27.
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

#include "zip_archive.h"
#include <libzip/zipint.h>
#undef open     //undefine the macro because of further complation error
#undef close    //undefine the macro because of further complation error

#include "byte_stream.h"
#include "make_unique.h"
#include <sstream>
#include <fstream>
#include <iostream>

#if EPUB_OS(UNIX)
#include <unistd.h>
#endif
#include <fcntl.h>
#if EPUB_OS(WINDOWS)
#include <windows.h>
#endif

#if EPUB_OS(ANDROID)
extern "C" char* gAndroidCacheDir;
#endif

EPUB3_BEGIN_NAMESPACE

static string GetTempFilePath(const string& ext)
{
#if EPUB_PLATFORM(WIN)
    TCHAR tmpPath[MAX_PATH];
    TCHAR tmpFile[MAX_PATH];

    DWORD pathLen = ::GetTempPath(MAX_PATH, tmpPath);
    if ( pathLen == 0 || pathLen > MAX_PATH )
        throw std::system_error(static_cast<int>(::GetLastError()), std::system_category());

    UINT fileLen = ::GetTempFileName(tmpPath, TEXT("ZIP"), 0, tmpFile);
    if ( fileLen == 0 )
        throw std::system_error(static_cast<int>(::GetLastError()), std::system_category());

    string r(tmpFile);
    return r;
#elif EPUB_PLATFORM(WINRT)
	using ToUTF8 = std::wstring_convert<std::codecvt_utf8<wchar_t>>;

	auto tempFolder = ::Windows::Storage::ApplicationData::Current->TemporaryFolder;
	std::wstring tempFolderPath(tempFolder->Path->Data(), tempFolder->Path->Length());
	std::wstringstream ss;

	ss << tempFolderPath;
	ss << TEXT("\\epub3.");
	ss << Windows::Security::Cryptography::CryptographicBuffer::GenerateRandomNumber();
	ss << TEXT(".");
	ss << ToUTF8().from_bytes(ext.stl_str());
	
	return ToUTF8().to_bytes(ss.str());
#else
    std::stringstream ss;
#if EPUB_OS(ANDROID)
    ss << gAndroidCacheDir << "epub3.XXXXXX";
#else
    ss << "/tmp/epub3.XXXXXX." << ext;
#endif
    string path(ss.str());
    
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
    return string(buf);
#endif
}
#if 0
class ZipReader : public ArchiveReader
{
public:
    ZipReader(struct zip_file* file) : _file(file), _total_size(_file->bytes_left) {}
    ZipReader(ZipReader&& o) : _file(o._file) { o._file = nullptr; }
    virtual ~ZipReader() { if (_file != nullptr) zip_fclose(_file); }
    
    virtual bool operator !() const { return _file == nullptr || _file->bytes_left == 0; }
	virtual ssize_t read(void* p, size_t len) const { return zip_fread(_file, p, len); }

	virtual size_t total_size() const { return _total_size; }
	virtual size_t position() const { return _total_size - _file->bytes_left; }
    
private:
    struct zip_file * _file;
	size_t _total_size;
};
#endif
class ZipReader : public ArchiveReader
{
public:
    ZipReader(struct zip_file* file) : _file(file), _total_size(0), bytes_left(0)
    {
        struct zip_stat st;
        if (zip_source_stat(file->src, &st) == 0)
        {
            _total_size = st.size;
            bytes_left = _total_size;
        }
    }
    ZipReader(ZipReader&& o) : _file(o._file) { o._file = nullptr; }
    virtual ~ZipReader() { if (_file != nullptr) zip_fclose(_file); }

    virtual bool operator !() const { return _file == nullptr || bytes_left == 0; }
    virtual ssize_t read(void* p, size_t len) const {
        size_t curLen = zip_fread(_file, p, std::min(bytes_left, len));
        if (curLen != -1)
            ((ZipReader*)this)->bytes_left -= curLen;
        return curLen;
    }

    virtual size_t total_size() const { return _total_size; }
    virtual size_t position() const { return _total_size - bytes_left; }

private:
    struct zip_file * _file;
    size_t _total_size;
    size_t bytes_left;
};


class ZipWriter : public ArchiveWriter
{
    class DataBlob
    {
    public:
        DataBlob() : _tmpPath(GetTempFilePath("tmp")), _fs(_tmpPath.c_str(), std::ios::in|std::ios::out|std::ios::binary|std::ios::trunc) {}
        DataBlob(DataBlob&& o) : _tmpPath(std::move(o._tmpPath)), _fs(_tmpPath.c_str(), std::ios::in|std::ios::out|std::ios::binary|std::ios::trunc) {}
        ~DataBlob() {
            _fs.close();
#if EPUB_OS(WINDOWS)
            ::_unlink(_tmpPath.c_str());
#else
            ::unlink(_tmpPath.c_str());
#endif
        }
        
        void Append(const void * data, size_t len);
        size_t Read(void *buf, size_t len);
        
        size_t Size() { return static_cast<size_t>(_fs.tellp()); }
        size_t Size() const { return const_cast<DataBlob*>(this)->Size(); }
        size_t Avail() { return Size() - static_cast<size_t>(_fs.tellg()); }
        size_t Avail() const { return const_cast<DataBlob*>(this)->Avail(); }
        
    protected:
        string          _tmpPath;
        std::fstream    _fs;

        DataBlob(const DataBlob&) _DELETED_;
    };
    
public:
    ZipWriter(struct zip* zip, const string& path, bool compressed);
    ZipWriter(ZipWriter&& o);
    virtual ~ZipWriter() { if (_zsrc != nullptr) zip_source_free(_zsrc); }
    
    virtual bool operator !() const { return false; }
    virtual ssize_t write(const void *p, size_t len) { _data.Append(p, len); return static_cast<ssize_t>(len); }
    
    struct zip_source* ZipSource() { return _zsrc; }
	const struct zip_source* ZipSource() const { return _zsrc; }

	virtual size_t total_size() const { return _data.Size(); }
	virtual size_t position() const { return _data.Size(); }
    
protected:
    bool                _compressed;
    DataBlob            _data;
    struct zip_source*  _zsrc;
    
    static ssize_t _source_callback(void *state, void *data, size_t len, enum zip_source_cmd cmd);
    
};

ZipArchive::ZipItemInfo::ZipItemInfo(struct zip_stat & info) : ArchiveItemInfo()
{
    SetPath(info.name);
    SetIsCompressed(info.comp_method == ZIP_CM_STORE);
    SetCompressedSize(static_cast<size_t>(info.comp_size));
    SetUncompressedSize(static_cast<size_t>(info.size));
}

string ZipArchive::TempFilePath()
{
    return GetTempFilePath("zip");
}
ZipArchive::ZipArchive(const string & path)
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
void ZipArchive::EachItem(std::function<void (const ArchiveItemInfo &)> fn) const
{
    struct zip_stat zinfo = {0};
    zip_stat_init(&zinfo);
    for (int i = 0, n = zip_get_num_files(_zip); i < n; i++)
    {
        if (zip_stat_index(_zip, i, 0, &zinfo) < 0)
            continue;
        
        ZipItemInfo info(zinfo);
        fn(info);
    }
}
bool ZipArchive::ContainsItem(const string & path) const
{
    return (zip_name_locate(_zip, Sanitized(path).c_str(), 0) >= 0);
}
bool ZipArchive::DeleteItem(const string & path)
{
    int idx = zip_name_locate(_zip, Sanitized(path).c_str(), 0);
    if ( idx >= 0 )
        return (zip_delete(_zip, idx) >= 0);
    return false;
}
bool ZipArchive::CreateFolder(const string & path)
{
    return (zip_add_dir(_zip, Sanitized(path).c_str()) >= 0);
}
unique_ptr<ByteStream> ZipArchive::ByteStreamAtPath(const string &path) const
{
    /*zip_uint64_t idx = 0;
    if ((idx = zip_name_locate(_zip, path.c_str(), 0)) < 0)
        return nullptr;
    zip_error_t error;
    zip_uint64_t val = _zip_file_get_offset(_zip, idx, &error);*/


    return make_unique<ZipFileByteStream>(_zip, path);
}

#ifdef SUPPORT_ASYNC
unique_ptr<AsyncByteStream> ZipArchive::AsyncByteStreamAtPath(const string& path) const
{
    return make_unique<AsyncZipFileByteStream>(_zip, path);
}
#endif /* SUPPORT_ASYNC */

unique_ptr<ArchiveReader> ZipArchive::ReaderAtPath(const string & path) const
{
    if (_zip == nullptr)
        return nullptr;
    
    struct zip_file* file = zip_fopen(_zip, Sanitized(path).c_str(), 0);

    if (file == nullptr)
        return nullptr;
    
    return unique_ptr<ZipReader>(new ZipReader(file));
}
unique_ptr<ArchiveWriter> ZipArchive::WriterAtPath(const string & path, bool compressed, bool create)
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
    
    return unique_ptr<ZipWriter>(writer);
}
ArchiveItemInfo ZipArchive::InfoAtPath(const string & path) const
{
    struct zip_stat sbuf;
    if ( zip_stat(_zip, Sanitized(path).c_str(), 0, &sbuf) < 0 )
        throw std::runtime_error(std::string("zip_stat("+path.stl_str()+") - " + zip_strerror(_zip)));
    return ZipItemInfo(sbuf);
}

void ZipWriter::DataBlob::Append(const void *data, size_t len)
{
    _fs.write(reinterpret_cast<const std::fstream::char_type *>(data), len);
}
size_t ZipWriter::DataBlob::Read(void *data, size_t len)
{
    if ( _fs.tellg() == std::streamsize(0) )
        _fs.flush();
    return static_cast<size_t>(_fs.readsome(reinterpret_cast<std::fstream::char_type *>(data), len));
}

ZipWriter::ZipWriter(struct zip *zip, const string& path, bool compressed)
    : _compressed(compressed)
{
    // It seems that ZipWriter is not actually used in the Readium launchers, so it is temporary commented out
    assert(0);
    //_zsrc = zip_source_function(zip, &ZipWriter::_source_callback, reinterpret_cast<void*>(this));
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
            st->size = static_cast<off_t>(writer->_data.Size());
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
            r = static_cast<ssize_t>(writer->_data.Read(data, len));
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
