//
//  archive.cpp
//  ePub3
//
//  Created by Jim Dovey on 2012-11-28.
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
