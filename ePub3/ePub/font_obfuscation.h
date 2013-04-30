//
//  font_obfuscation.h
//  ePub3
//
//  Created by Jim Dovey on 2012-12-21.
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

#ifndef __ePub3__font_obfuscation__
#define __ePub3__font_obfuscation__

#include <ePub3/filter.h>
#include <ePub3/encryption.h>
#include REGEX_INCLUDE
#include <cstring>

EPUB3_BEGIN_NAMESPACE

/**
 The FontObfuscator class implements font obfuscation algorithm as defined in
 Open Container Format 3.0 §4.
 
 The underlying algorithm is bidirectional, so this filter can actually be used both
 to obfuscate and de-obfuscate resources; as such, this filter may be applied when
 loading or when storing content.
 @see http://www.idpf.org/epub/30/spec/epub30-ocf.html#font-obfuscation
 */
class FontObfuscator : public ContentFilter
{
protected:
    static const size_t         KeySize = 20;       // SHA-1 key size = 20 bytes
    static const REGEX_NS::regex     TypeCheck;
    CONSTEXPR static const char * const   FontObfuscationAlgorithmID
#if EPUB_COMPILER_SUPPORTS(CXX_NONSTATIC_MEMBER_INIT)
            = "http://www.idpf.org/2008/embedding"
#endif
              ;
    
    /**
     The type-sniffer for font obfuscation applicability.
     
     The sniffer looks at two things:
     
     1. The encryption information for the item must specify the font
     obfuscation algorithm.
     2. The item must be a font resource.
     */
    static bool FontTypeSniffer(const ManifestItem* item, const EncryptionInfo* encInfo) {
        if ( encInfo == nullptr || encInfo->Algorithm() != FontObfuscationAlgorithmID )
            return false;
        return REGEX_NS::regex_match(item->MediaType().stl_str(), TypeCheck);
    }
    
private:
    ///
    /// There is no default constructor.
    FontObfuscator() _DELETED_;

private:
    /**
     Create a font obfuscation filter.
     
     The obfuscation key is built using data from every manifestation within an EPUB
     container, so the Container instance is passed in for that purpose. This is
     only used during construction.
     @see BuildKey(const Container*)
     */
    FontObfuscator(const Container* container) : ContentFilter(FontTypeSniffer), _bytesFiltered(0) {
        BuildKey(container);
    }
    ///
    /// Copy constructor.
    FontObfuscator(const FontObfuscator& o) : ContentFilter(o), _bytesFiltered(o._bytesFiltered) {
        std::memcpy(_key, o._key, KeySize);
    }
    ///
    /// Move constructor.
    FontObfuscator(FontObfuscator&& o) : ContentFilter(std::move(o)), _bytesFiltered(o._bytesFiltered) {
        std::memcpy(_key, o._key, KeySize);
    }
    
    /**
     Applies the font obfuscation algorithm to the resource data.
     @see http://www.idpf.org/epub/30/spec/epub30-ocf.html#font-obfuscation
     @param data The data to process.
     @param len The number of bytes in `data`.
     @param outputLen Storage for the count of bytes being returned.
     @result The obfuscated or de-obfuscated bytes.
     */
    virtual void * FilterData(void * data, size_t len, size_t *outputLen);
    
protected:
    uint8_t             _key[KeySize];
    size_t              _bytesFiltered;     // NOT copied
    
    /**
     Builds the obfuscaton key using data from the container.
     @param container The container for the resources to which this filter will
     apply.
     @result Always returns `true`.
     @see http://www.idpf.org/epub/30/spec/epub30-ocf.html#fobfus-keygen
     */
    bool BuildKey(const Container* container);
};

EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__font_obfuscation__) */
