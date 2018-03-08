//
//  zip_archive.h
//  ePub3
//
//  Created by Jim Dovey on 2012-11-27.
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//  
//  Redistribution and use in source and binary forms, with or without modification, 
//  are permitted provided that the following conditions are met:
//  1. Redistributions of source code must retain the above copyright notice, this 
//  list of conditions and the following disclaimer.
//  2. Redistributions in binary form must reproduce the above copyright notice, 
//  this list of conditions and the following disclaimer in the documentation and/or 
//  other materials provided with the distribution.
//  3. Neither the name of the organization nor the names of its contributors may be 
//  used to endorse or promote products derived from this software without specific 
//  prior written permission.
//  
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
//  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
//  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
//  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
//  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
//  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
//  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
//  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE 
//  OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED 
//  OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef __ePub3__zip_archive__
#define __ePub3__zip_archive__

#include <ePub3/archive.h>
#include <libzip/zip.h>
#include <list>

EPUB3_BEGIN_NAMESPACE

/**
 An Archive implementation for ZIP files, as used by the OCF 3.0 standard.
 
 @note ZIP archives do not contain any access permission information.
 @note The underlying implementation, `libzip`, writes data only when the archive
 is closed. Any data written to a zip file will therefore be kept in temporary
 storage until the archive object is closed.
 @see http://www.idpf.org/epub/30/spec/epub30-ocf.html#physical-container-zip
 @ingroup archives
 */
class ZipArchive : public Archive
{
    // a subclass that can be initialized with a zip_stat structure
    class ZipItemInfo : public ArchiveItemInfo {
    public:
        ZipItemInfo(struct zip_stat & info);
    };
    
private:
#if ENABLE_ZIP_ARCHIVE_WRITER
    static string TempFilePath();
#endif //ENABLE_ZIP_ARCHIVE_WRITER

public:
    ///
    /// Opens the ZipArchive at a given filesystem path.
    EPUB3_EXPORT
    ZipArchive(const string & path="");
    ///
    /// move constructos.
    ZipArchive(ZipArchive &&o) : _zip(o._zip) { o._zip = nullptr; }
    ///
    /// Initialize directly from a `libzip` internal structure.
    explicit ZipArchive(struct zip * aZip) : _zip(aZip) {}
    virtual ~ZipArchive();
    
    ///
    /// Move assignment.
    EPUB3_EXPORT
    Archive & operator = (ZipArchive &&o);
    
    virtual void EachItem(std::function<void(const ArchiveItemInfo&)> fn) const OVERRIDE;
    
    virtual bool ContainsItem(const string & path) const;
    virtual bool DeleteItem(const string & path);
    
    virtual bool CreateFolder(const string & path);
    
    virtual unique_ptr<ByteStream> ByteStreamAtPath(const string& path) const;

#ifdef SUPPORT_ASYNC
    virtual unique_ptr<AsyncByteStream> AsyncByteStreamAtPath(const string& path) const;
#endif /* SUPPORT_ASYNC */

    virtual unique_ptr<ArchiveReader> ReaderAtPath(const string & path) const;
#if ENABLE_ZIP_ARCHIVE_WRITER
    virtual unique_ptr<ArchiveWriter> WriterAtPath(const string & path, bool compress=true, bool create=true);
#endif //ENABLE_ZIP_ARCHIVE_WRITER
    virtual ArchiveItemInfo InfoAtPath(const string & path) const;
    
protected:
    struct zip *    _zip;           ///< Pointer to the underlying `libzip` data type.
    
    typedef std::list<zip_source*>  ZipSourceList;
    ZipSourceList   _liveSources;   ///< A list of live zip sources, which must be cleaned up upon closing.

};

EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__zip_archive__) */
