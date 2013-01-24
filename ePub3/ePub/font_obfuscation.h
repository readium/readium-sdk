//
//  font_obfuscation.h
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

#ifndef __ePub3__font_obfuscation__
#define __ePub3__font_obfuscation__

#include "filter.h"
#include "encryption.h"
#include <regex>

EPUB3_BEGIN_NAMESPACE

class FontObfuscator : public ContentFilter
{
protected:
    static const size_t         KeySize = 20;       // SHA-1 key size = 20 bytes
    static const std::regex     TypeCheck;
    constexpr static const char * const   FontObfuscationAlgorithmID = "http://www.idpf.org/2008/embedding";
    
    static bool FontTypeSniffer(const ManifestItem* item, const EncryptionInfo* encInfo) {
        if ( encInfo->Algorithm() != FontObfuscationAlgorithmID )
            return false;
        return std::regex_match(item->MediaType().stl_str(), TypeCheck);
    }
    
public:
    FontObfuscator() = delete;
    FontObfuscator(const Container* container) : ContentFilter(FontTypeSniffer, container) {
        BuildKey();
    }
    FontObfuscator(FontObfuscator&& o) : ContentFilter(std::move(o)) {
        std::memcpy(_key, o._key, KeySize);
    }
    
    virtual void * FilterData(void * data, size_t len);
    
protected:
    uint8_t     _key[KeySize];
    size_t      _bytesFiltered;     // NOT copied
    
    bool BuildKey();
};

EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__font_obfuscation__) */
