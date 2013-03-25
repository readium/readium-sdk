//
//  media_support_info.cpp
//  ePub3
//
//  Created by Jim Dovey on 2013-02-04.
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

#include "media_support_info.h"
#include "package.h"
#include "manifest.h"

EPUB3_BEGIN_NAMESPACE

MediaSupportInfo::MediaSupportInfo(const string& mediaType, bool supported)
  : _mediaType(mediaType),
    _support(supported ? SupportType::IntrinsicSupport : SupportType::Unsupported)
{
}
MediaSupportInfo::MediaSupportInfo(const string& mediaType, SupportType support)
  : _mediaType(mediaType),
    _support(support)
{
}
MediaSupportInfo::MediaSupportInfo(const MediaSupportInfo& o)
  : _mediaType(o._mediaType),
    _support(o._support)
{
}
MediaSupportInfo::MediaSupportInfo(MediaSupportInfo&& o)
  : _mediaType(std::move(o._mediaType)),
    _support(o._support)
{
    o._support = SupportType::Unsupported;
}
MediaSupportInfo& MediaSupportInfo::operator=(const MediaSupportInfo &o)
{
    _mediaType = o._mediaType;
    _support = o._support;
    return *this;
}
MediaSupportInfo& MediaSupportInfo::operator=(MediaSupportInfo &&o)
{
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
const MediaSupportInfo::ManifestItemList MediaSupportInfo::MatchingManifestItems(const Package* pkg) const
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
