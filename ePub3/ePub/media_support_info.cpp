//
//  media_support_info.cpp
//  ePub3
//
//  Created by Jim Dovey on 2013-02-04.
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

#include "media_support_info.h"
#include "package.h"
#include "manifest.h"

EPUB3_BEGIN_NAMESPACE

MediaSupportInfo::MediaSupportInfo(shared_ptr<Package> owner) : OwnedBy(owner), _mediaType(""), _support(SupportType::Unsupported)
{
}
MediaSupportInfo::MediaSupportInfo(shared_ptr<Package> owner, const string& mediaType, bool supported)
  : OwnedBy(owner),
    _mediaType(mediaType),
    _support(supported ? SupportType::IntrinsicSupport : SupportType::Unsupported)
{
}
MediaSupportInfo::MediaSupportInfo(shared_ptr<Package> owner, const string& mediaType, SupportType support)
  : OwnedBy(owner),
    _mediaType(mediaType),
    _support(support)
{
}
MediaSupportInfo::MediaSupportInfo(const MediaSupportInfo& o)
  : OwnedBy(o),
    _mediaType(o._mediaType),
    _support(o._support)
{
}
MediaSupportInfo::MediaSupportInfo(MediaSupportInfo&& o)
  : OwnedBy(std::move(o)),
    _mediaType(std::move(o._mediaType)),
    _support(o._support)
{
    o._support = SupportType::Unsupported;
}
MediaSupportInfo& MediaSupportInfo::operator=(const MediaSupportInfo &o)
{
    OwnedBy::operator=(o);
    _mediaType = o._mediaType;
    _support = o._support;
    return *this;
}
MediaSupportInfo& MediaSupportInfo::operator=(MediaSupportInfo &&o)
{
    OwnedBy::operator=(std::move(o));
    _mediaType = std::move(o._mediaType);
    _support = o._support;
    o._support = SupportType::Unsupported;
    return *this;
}
bool MediaSupportInfo::HasIntrinsicSupport() const
{
    return _support == SupportType::IntrinsicSupport;
}
bool MediaSupportInfo::RequiresMediaHandler() const
{
    return _support == SupportType::SupportedWithHandler;
}
void MediaSupportInfo::SetTypeAndSupport(const string& mediaType, SupportType support)
{
    _mediaType = mediaType;
    _support = support;
}
const MediaSupportInfo::ManifestItemList MediaSupportInfo::MatchingManifestItems(shared_ptr<Package> pkg) const
{
    ManifestItemList items;
    for ( auto pair : pkg->Manifest() )
    {
        if ( pair.first == _mediaType )
            items.push_back(pair.second);
    }
    return items;
}

EPUB3_END_NAMESPACE
