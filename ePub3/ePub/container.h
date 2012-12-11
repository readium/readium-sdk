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
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <string>
#include <vector>

EPUB3_BEGIN_NAMESPACE

class Archive;
class Package;

class Container
{
public:
    typedef std::vector<std::string>    PathList;
    typedef std::vector<Package*>       PackageList;
    
public:
    Container(const std::string& path);
    Container(const Container&) = delete;
    Container(Container&& o);
    virtual ~Container();
    
    virtual PathList PackageLocations() const;
    virtual const PackageList& Packages();
    virtual std::string Version() const;
    
protected:
    Archive *   _archive;
    xmlDocPtr   _ocf;
    PackageList _packages;
    
    static void SetupXPath(xmlXPathContextPtr ctx);
};

EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__container__) */
