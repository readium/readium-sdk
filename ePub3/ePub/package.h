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
#include "nav_element.h"
#include "archive_xml.h"

EPUB3_BEGIN_NAMESPACE

class Archive;
class Metadata;
class NavigationTable;

class Package
{
public:
    typedef std::map<std::string, class Metadata*>          MetadataMap;
    typedef std::map<std::string, class NavigationTable*>   NavigationMap;
    
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
    const NavigationMap& NavigationTables() const { return _navigation; }
    
    const SpineItem* FirstSpineItem() const { return _spine; }
    const SpineItem* SpineItemAt(size_t idx) const;
    
    const ManifestItem* ManifestItemWithID(const std::string& ident) const;
    
    const class NavigationTable* NavigationTable(const std::string& title) const;
    
    // note that the CFI is purposely non-const so the package can correct it (cf. epub-cfi §3.5)
    const ManifestItem* ManifestItemForCFI(CFI& cfi, CFI* pRemainingCFI) const throw (CFI::InvalidCFI);
    xmlDocPtr DocumentForCFI(CFI& cfi, CFI* pRemainingCFI) const throw (CFI::InvalidCFI) {
        return ManifestItemForCFI(cfi, pRemainingCFI)->ReferencedDocument();
    }
    
    // array-style operators: indices get spine items, identifiers get manifest items
    const SpineItem* operator[](size_t idx) const { return SpineItemAt(idx); }
    const ManifestItem* operator[](const std::string& ident) const { return ManifestItemWithID(ident); }
    
    ArchiveXmlReader* ReaderForRelativePath(const std::string& path) const {
        return new ArchiveXmlReader(_archive->ReaderAtPath(_pathBase + path));
    }
    
protected:
    Archive *       _archive;
    xmlDocPtr       _opf;
    std::string     _pathBase;
    std::string     _type;
    MetadataMap     _metadata;
    ManifestTable   _manifest;
    NavigationMap   _navigation;
    SpineItem*      _spine;
    
    // used to verify/correct CFIs
    uint32_t        _spineCFIIndex;
    
    bool Unpack();
    const SpineItem* ConfirmOrCorrectSpineItemQualifier(const SpineItem* pItem, CFI::Component* pComponent) const;
    static NavigationList NavTablesFromManifestItem(const ManifestItem* pItem);
};

EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__package__) */
