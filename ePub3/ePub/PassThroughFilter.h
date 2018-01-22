//
//  PassThroughFilter.h
//  ePub3
//
//  Created by Nelson Leme on 10/30/14.
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

#ifndef __ePub3__PassThroughFilter__
#define __ePub3__PassThroughFilter__

#include <ePub3/filter.h>
#include <ePub3/encryption.h>
#include REGEX_INCLUDE
#include <cstring>
#import <ePub3/utilities/byte_stream.h>

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
