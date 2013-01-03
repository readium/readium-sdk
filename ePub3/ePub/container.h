//
//  container.h
//  ePub3
//
//  Created by Jim Dovey on 2012-11-28.
//  Copyright (c) 2012 Kobo Inc. All rights reserved.
//

#ifndef __ePub3__container__
#define __ePub3__container__

#include "epub3.h"
#include "locator.h"
#include "encryption.h"
#include "package.h"
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <string>
#include <vector>

EPUB3_BEGIN_NAMESPACE

class Archive;

class Container
{
public:
    typedef std::vector<std::string>        PathList;
    typedef std::vector<Package*>           PackageList;
    typedef std::vector<EncryptionInfo*>    EncryptionList;
    
public:
    Container(const std::string& path);
    Container(Locator locator);
    Container(const Container&) = delete;
    Container(Container&& o);
    virtual ~Container();
    
    virtual PathList PackageLocations() const;
    virtual const PackageList& Packages() const { return _packages; }
    virtual std::string Version() const;
    virtual const EncryptionList& EncryptionData() const { return _encryption; }
    
    virtual const EncryptionInfo* EncryptionInfoForPath(const std::string& path) const;
    
protected:
    Archive *       _archive;
    xmlDocPtr       _ocf;
    PackageList     _packages;
    EncryptionList  _encryption;
    
    void LoadEncryption();
};

EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__container__) */
