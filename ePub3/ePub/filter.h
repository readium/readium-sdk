//
//  filter.h
//  ePub3
//
//  Created by Jim Dovey on 2012-12-21.
//  Copyright (c) 2012 Kobo Inc. All rights reserved.
//

#ifndef __ePub3__filter__
#define __ePub3__filter__

#include "base.h"
#include "../utilities/basic.h"
#include "manifest.h"
#include "encryption.h"
#include <string>

EPUB3_BEGIN_NAMESPACE

class Package;
class Container;

class ContentFilter
{
public:
    typedef std::function<bool(const ManifestItem* item, const EncryptionInfo* encInfo)> TypeSnifferFn;
    
public:
    ContentFilter() = delete;
    ContentFilter(const ContentFilter&) = delete;
    ContentFilter(ContentFilter&& o) : _sniffer(std::move(o._sniffer)), _next(std::move(o._next)), _container(std::move(o._container)) {}
    
    ContentFilter(TypeSnifferFn sniffer, const Container* container) : _sniffer(sniffer), _container(container) {}
    virtual ~ContentFilter() {}
    
    virtual TypeSnifferFn TypeSniffer() const { return _sniffer; }
    virtual void SetTypeSniffer(TypeSnifferFn fn) { _sniffer = fn; }
    
    virtual ContentFilter* Next() const { return _next.get(); }
    virtual void SetNextFilter(ContentFilter* next) { _next.reset(next); }
    
    virtual void * FilterData(void *data, size_t len) = 0;
    
protected:
    TypeSnifferFn       _sniffer;
    Auto<ContentFilter> _next;
    const Container*    _container;
};

EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__filter__) */
