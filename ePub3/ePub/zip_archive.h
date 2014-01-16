//
//  zip_archive.h
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
    static string TempFilePath();
    
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
    virtual unique_ptr<AsyncByteStream> AsyncByteStreamAtPath(const string& path) const;
    
    virtual unique_ptr<ArchiveReader> ReaderAtPath(const string & path) const;
    virtual unique_ptr<ArchiveWriter> WriterAtPath(const string & path, bool compress=true, bool create=true);
        
    virtual ArchiveItemInfo InfoAtPath(const string & path) const;
    
protected:
    struct zip *    _zip;           ///< Pointer to the underlying `libzip` data type.
    
    typedef std::list<zip_source*>  ZipSourceList;
    ZipSourceList   _liveSources;   ///< A list of live zip sources, which must be cleaned up upon closing.
    
    ///
    /// Sanitizes a path string, since `libzip` can be finnicky about them.
    string Sanitized(const string& path) const;
};

EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__zip_archive__) */
