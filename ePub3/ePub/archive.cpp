//
//  archive.cpp
//  ePub3
//
//  Created by Jim Dovey on 2012-11-28.
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

#include "archive.h"
#include "zip_archive.h"
#include <map>

EPUB3_BEGIN_NAMESPACE

Archive::ArchiveRegistrationDomain Archive::RegistrationDomain;

void Archive::RegisterArchive(CreatorFn creator, SnifferFn sniffer)
{
    RegistrationDomain.emplace_front(creator, sniffer);
}
void Archive::Initialize()
{
    RegisterArchive([](const string& path) { return std::unique_ptr<ZipArchive>(new ZipArchive(path)); },
                    [](const string& path) { return path.rfind(".zip") == path.size()-4; });
    RegisterArchive([](const string& path) { return std::unique_ptr<ZipArchive>(new ZipArchive(path)); },
                    [](const string& path) { return path.rfind(".epub") == path.size()-5; });
}
std::unique_ptr<Archive> Archive::Open(const string& path)
{
    for ( auto& factory : RegistrationDomain )
    {
        if ( factory.CanInit(path) )
            return factory(path);
    }
    
    return nullptr;
}
bool Archive::ShouldCompress(const string &path, const string &mimeType, size_t size) const
{
    // check MIME type for known pre-compressed data formats
    if ( mimeType.find("image/", 0, 6) != string::npos && mimeType.find("bmp") == string::npos )
        return false;
    if ( mimeType.find("video/", 0, 6) != string::npos )
        return false;
    if ( mimeType.find("audio/", 0, 6) != string::npos )
        return false;
    
    // Under 1KB don't bother compressing anything
    if ( size < 1024 )
        return false;
    
    return true;
}
ArchiveItemInfo Archive::InfoAtPath(const string &path) const
{
    ArchiveItemInfo info;
    info.SetPath(path);
    return std::move(info);
}

EPUB3_END_NAMESPACE
