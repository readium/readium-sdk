//
//  filter.h
//  ePub3
//
//  Created by Jim Dovey on 2012-12-21.
//  Copyright (c) 2012-2013 The Readium Foundation.
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
