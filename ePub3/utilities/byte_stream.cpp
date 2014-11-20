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

EPUB3_BEGIN_NAMESPACE

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
ByteStream::size_type FileByteStream::BytesAvailable() const _NOEXCEPT
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
    Open(archive, path, flags);
}
ZipFileByteStream::~ZipFileByteStream()
{
    Close();
}
ByteStream::size_type ZipFileByteStream::BytesAvailable() const _NOEXCEPT
{
    if ( _file == nullptr )
        return 0;
    return _file->bytes_left;
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
    
    _file = zip_fopen(archive, Sanitized(path).c_str(), flags);
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

	_eof = (_file->bytes_left == 0);
    
    return numRead;
}
ByteStream::size_type ZipFileByteStream::WriteBytes(const void *buf, size_type len)
{
    // no write support at this moment
    return 0;
}
ByteStream::size_type ZipFileByteStream::Seek(size_type by, std::ios::seekdir dir)
{
    int whence = ZIP_SEEK_SET;
    switch (dir)
    {
        case std::ios::beg:
            break;
        case std::ios::cur:
            whence = ZIP_SEEK_CUR;
            break;
        case std::ios::end:
            whence = ZIP_SEEK_END;
            break;
        default:
            return Position();
    }
    
    zip_fseek(_file, long(by), whence);
	_eof = (_file->bytes_left == 0);
    return Position();
}
ByteStream::size_type ZipFileByteStream::Position() const
{
    return size_type(zip_ftell(_file));
}
std::shared_ptr<SeekableByteStream> ZipFileByteStream::Clone() const
{
	if (_file == nullptr)
		return nullptr;

	struct zip_file* newFile = zip_fopen_index(_file->za, _file->file_index, _file->flags);
	if (newFile == nullptr)
		return nullptr;
    
    zip_fseek(newFile, Position(), ZIP_SEEK_SET);

	auto result = std::make_shared<ZipFileByteStream>();
	if (bool(result))
	{
		result->_file = newFile;
		result->_mode = _mode;
	}

	return result;
}

EPUB3_END_NAMESPACE
