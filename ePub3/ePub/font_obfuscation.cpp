//
//  font_obfuscation.cpp
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

#include "font_obfuscation.h"
#include "container.h"
#include "package.h"

// OpenSSL APIs are deprecated on OS X and iOS
#if defined(__MAC_OS_X_VERSION_MIN_REQUIRED) || defined(__IPHONE_OS_VERSION_MIN_REQUIRED)
#define COMMON_DIGEST_FOR_OPENSSL
#include <CommonCrypto/CommonDigest.h>
#else
#include <openssl/sha.h>
#endif

EPUB3_BEGIN_NAMESPACE

const std::regex FontObfuscator::TypeCheck("application/x-font-.*");

void * FontObfuscator::FilterData(void *data, size_t len, size_t *outputLen)
{
    uint8_t *buf = static_cast<uint8_t*>(data);
    for ( int i = 0; i < len && (i + _bytesFiltered) < 1040; i++)
    {
        // XOR each of the first 1040 bytes of the font with the key, circling around the keybuf
        buf[i] ^= _key[(i+_bytesFiltered)%20];
    }
    
    _bytesFiltered += len;
    *outputLen = len;
    return buf;
}
bool FontObfuscator::BuildKey()
{
    std::regex re(R"X(\s+)X");
    std::stringstream ss;
    
    for ( auto pkg : _container->Packages() )
    {
        if ( ss.tellp() > 0 )
            ss << ' ';
        
        // we use a C++11 regex to remove all whitespace in the value
        ss << regex_replace(pkg->UniqueID().stl_str(), re, "");
    }
    
    // hash the accumulated string (using OpenSSL syntax for portability)
    auto str = ss.str();
    SHA_CTX ctx;
    SHA1_Init(&ctx);
    SHA1_Update(&ctx, str.data(), str.length());
    SHA1_Final(_key, &ctx);
    
    return true;
}

EPUB3_END_NAMESPACE
