//
//  media_support_info.cpp
//  ePub3
//
//  Created by Jim Dovey on 2013-02-04.
//  Copyright (c) 2013 The Readium Foundation. All rights reserved.
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
