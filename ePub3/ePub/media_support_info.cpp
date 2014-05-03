//
//  media_support_info.cpp
//  ePub3
//
//  Created by Jim Dovey on 2013-02-04.
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//  
//  This program is distributed in the hope that it will be useful, but WITHOUT ANY 
//  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
//  
//  Licensed under Gnu Affero General Public License Version 3 (provided, notwithstanding this notice, 
//  Readium Foundation reserves the right to license this material under a different separate license, 
//  and if you have done so, the terms of that separate license control and the following references 
//  to GPL do not apply).
//  
//  This program is free software: you can redistribute it and/or modify it under the terms of the GNU 
//  Affero General Public License as published by the Free Software Foundation, either version 3 of 
//  the License, or (at your option) any later version. You should have received a copy of the GNU 
//  Affero General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.

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
