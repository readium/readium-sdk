//
//  PassThroughFilter.h
//  ePub3
//
//  Created by Nelson Leme on 10/30/14.
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
//

#ifndef __ePub3__PassThroughFilter__
#define __ePub3__PassThroughFilter__

#include <ePub3/filter.h>
#include <ePub3/encryption.h>
#include REGEX_INCLUDE
#include <cstring>
#include <ePub3/utilities/byte_stream.h> //#import <ePub3/utilities/byte_stream.h>

EPUB3_BEGIN_NAMESPACE


class PassThroughFilter : public ContentFilter, public PointerType<PassThroughFilter>
{
public:
    PassThroughFilter() : ContentFilter(SniffPassThroughContent) { }
    PassThroughFilter(const PassThroughFilter &o) : ContentFilter(o) { }
    PassThroughFilter(PassThroughFilter &&o) : ContentFilter(std::move(o)) { }

    virtual void *FilterData(FilterContext *context, void *data, size_t len, size_t *outputLen) OVERRIDE;
    virtual OperatingMode GetOperatingMode() const OVERRIDE { return OperatingMode::SupportsByteRanges; }

    virtual ByteStream::size_type BytesAvailable(FilterContext *context, SeekableByteStream *byteStream) const OVERRIDE;

    static void Register();

protected:
    virtual FilterContext *InnerMakeFilterContext(ConstManifestItemPtr item) const OVERRIDE;

private:
    static bool SniffPassThroughContent(ConstManifestItemPtr item);
    static ContentFilterPtr PassThroughFactory(ConstPackagePtr package);
    
    class PassThroughContext : public RangeFilterContext
    {
    public:
        PassThroughContext() : RangeFilterContext() {}
        virtual ~PassThroughContext() { }
    }; // PassThroughContext class
    
}; // PassThroughFilter class


EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__PassThroughFilter__) */
