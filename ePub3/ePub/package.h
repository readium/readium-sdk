//
//  package.h
//  ePub3
//
//  Created by Jim Dovey on 2012-11-28.
//  Copyright (c) 2012 Kobo Inc. All rights reserved.
//

#ifndef __ePub3__package__
#define __ePub3__package__

#include "epub3.h"
#include <string>
#include <vector>
#include <map>
#include <libxml/tree.h>
#include "spine.h"
#include "manifest.h"
#include "cfi.h"

EPUB3_BEGIN_NAMESPACE

class Archive;
class Metadata;

class Package
{
public:
    typedef std::map<std::string, class Metadata*>      MetadataMap;
    
public:
    Package(Archive * archive, const std::string& path, const std::string& type);
    Package(const Package&) = delete;
    Package(Package&&);
    virtual ~Package();
    
    virtual const std::string & Type() const { return _type; }
    virtual std::string Version() const;
    
    // to keep these accessible in const packages, we *must* build the tree at allocation time
    // this is open to discussion, naturally
    const MetadataMap& Metadata() const { return _metadata; }
    const ManifestTable& Manifest() const { return _manifest; }
    
    const SpineItem* FirstSpineItem() const { return _spine; }
    const SpineItem* SpineItemAt(size_t idx) const;
    
    const ManifestItem* ManifestItemWithID(const std::string& ident) const;
    
    // note that the CFI is purposely non-const so the package can correct it (cf epub-cfi §3.5)
    const ManifestItem* ManifestItemForCFI(CFI& cfi, std::string& remainingCFI) const;
    
    // array-style operators: indices get spine items, identifiers get manifest items
    const SpineItem* operator[](size_t idx) const { return SpineItemAt(idx); }
    const ManifestItem* operator[](const std::string& ident) const { return ManifestItemWithID(ident); }
    
protected:
    Archive *       _archive;
    xmlDocPtr       _opf;
    std::string     _type;
    MetadataMap     _metadata;
    ManifestTable   _manifest;
    SpineItem*      _spine;
    
    bool Unpack();
};

EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__package__) */
