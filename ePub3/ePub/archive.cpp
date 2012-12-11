//
//  archive.cpp
//  ePub3
//
//  Created by Jim Dovey on 2012-11-28.
//  Copyright (c) 2012 Kobo Inc. All rights reserved.
//

#include "archive.h"
#include "zip_archive.h"
#include <map>

EPUB3_BEGIN_NAMESPACE

Archive::ArchiveRegistrationDomain Archive::RegistrationDomain;

void Archive::RegisterArchive(ArchiveTypeSniffer sniffer, ArchiveFactory factory)
{
    RegistrationDomain[sniffer] = factory;
}
void Archive::Initialize()
{
    RegisterArchive([](const std::string& path) { return path.find_last_of(".zip") == path.size()-4; },
                    [](const std::string& path) { return new ZipArchive(path); });
}
Archive * Archive::Open(const std::string& path)
{
    for ( auto item : RegistrationDomain )
    {
        if ( item.first(path) )
            return item.second(path);
    }
    
    return nullptr;
}
ArchiveItemInfo Archive::InfoAtPath(const std::string &path) const
{
    ArchiveItemInfo info;
    info.SetPath(path);
    return std::move(info);
}

EPUB3_END_NAMESPACE
