//
//  zip_archive.h
//  ePub3
//
//  Created by Jim Dovey on 2012-11-27.
//  Copyright (c) 2012 Kobo Inc. All rights reserved.
//

#ifndef __ePub3__zip_archive__
#define __ePub3__zip_archive__

#include "archive.h"
#include "zip.h"
#include <list>

EPUB3_BEGIN_NAMESPACE

class ZipArchive : public Archive
{
    // a subclass that can be initialized with a zip_stat structure
    class ZipItemInfo : public ArchiveItemInfo {
    public:
        ZipItemInfo(struct zip_stat & info);
    };
    
private:
    static std::string TempFilePath();
    
public:
    ZipArchive() : ZipArchive(TempFilePath()) {}
    ZipArchive(const std::string & path);
    ZipArchive(ZipArchive &&o) : _zip(o._zip) { o._zip = nullptr; }
    explicit ZipArchive(struct zip * aZip) : _zip(aZip) {}
    virtual ~ZipArchive();
    
    Archive & operator = (ZipArchive &&o);
    
    virtual bool ContainsItem(const std::string & path) const;
    virtual bool DeleteItem(const std::string & path);;
    
    virtual bool CreateFolder(const std::string & path);
    
    virtual ArchiveReader* ReaderAtPath(const std::string & path) const;
    virtual ArchiveWriter* WriterAtPath(const std::string & path, bool compress=true, bool create=true);
        
    virtual ArchiveItemInfo InfoAtPath(const std::string & path) const;
    
protected:
    struct zip *    _zip;
    
    typedef std::list<zip_source*>  ZipSourceList;
    ZipSourceList   _liveSources;
};

EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__zip_archive__) */
