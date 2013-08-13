//
//  font_obfuscation.cpp
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

// OpenSSL APIs are deprecated on OS X and iOS
#if EPUB_OS(DARWIN)
#define COMMON_DIGEST_FOR_OPENSSL
#include <CommonCrypto/CommonDigest.h>
#elif EPUB_PLATFORM(WIN)
#include <windows.h>
#include <Wincrypt.h>
#else
#include <openssl/sha.h>
#endif

#include "font_obfuscation.h"
#include "container.h"
#include "package.h"
#include "filter_manager.h"
#include "container_constructor_parameter.h"

EPUB3_BEGIN_NAMESPACE

#if !EPUB_COMPILER_SUPPORTS(CXX_NONSTATIC_MEMBER_INIT)
const char * const FontObfuscator::FontObfuscationAlgorithmID = "http://www.idpf.org/2008/embedding";
#endif

const REGEX_NS::regex FontObfuscator::TypeCheck("(?:font/.*|application/(?:x-font-.*|vnd.ms-(?:opentype|fontobject)))");

void * FontObfuscator::FilterData(void *data, size_t len, size_t *outputLen)
{
    uint8_t *buf = static_cast<uint8_t*>(data);
    for ( size_t i = 0; i < len && (i + _bytesFiltered) < 1040; i++)
    {
        // XOR each of the first 1040 bytes of the font with the key, circling around the keybuf
        buf[i] ^= _key[(i+_bytesFiltered)%20];
    }
    
    _bytesFiltered += len;
    *outputLen = len;
    return buf;
}
bool FontObfuscator::BuildKey(const Container* container)
{
    REGEX_NS::regex re("\\s+");
    std::stringstream ss;
    
    for ( auto pkg : container->Packages() )
    {
        if ( ss.tellp() > 0 )
            ss << ' ';
        
        // we use a C++11 regex to remove all whitespace in the value
        ss << REGEX_NS::regex_replace(pkg->PackageID().stl_str(), re, "");
    }

    auto str = ss.str();
    
#if EPUB_PLATFORM(WIN)
    HCRYPTPROV csp;
    if ( ::CryptAcquireContext(&csp, NULL, NULL, PROV_DSS, CRYPT_VERIFYCONTEXT) == FALSE )
    {
        _THROW_LAST_ERROR_();
    }

    HCRYPTHASH hasher;
    if ( ::CryptCreateHash(csp, CALG_SHA, 0, 0, &hasher) == FALSE )
    {
        ::CryptReleaseContext(csp, 0);
        _THROW_LAST_ERROR_();
    }

    DWORD winerr = NO_ERROR;
    if ( ::CryptHashData(hasher, reinterpret_cast<const BYTE*>(str.data()), str.length(), 0) == TRUE )
    {
        DWORD len = KeySize;
        if ( ::CryptGetHashParam(hasher, HP_HASHVAL, _key, &len, 0) == FALSE )
            winerr = ::GetLastError();
    }
    else
    {
        winerr = ::GetLastError();
    }

    ::CryptDestroyHash(hasher);
    ::CryptReleaseContext(csp, 0);

    if ( winerr != NO_ERROR )
        _THROW_WIN_ERROR_(winerr);
#else
    // hash the accumulated string (using OpenSSL syntax for portability)
    SHA_CTX ctx;
    SHA1_Init(&ctx);
    SHA1_Update(&ctx, str.data(), str.length());
    SHA1_Final(_key, &ctx);
#endif
    return true;
}

ContentFilter *FontObfuscator::FontObfuscatorFactory(const ContentFilter::ConstructorParameters *parameters)
{
    const ContainerConstructorParameter *parameter = dynamic_cast<const ContainerConstructorParameter *>(parameters);
    if (parameter == nullptr)
    {
        return nullptr;
    }
    
    return new FontObfuscator(parameter);
}

void FontObfuscator::Register()
{
    FilterManager::Instance()->RegisterFilter(FontTypeSniffer, FontObfuscatorFactory);
}

EPUB3_END_NAMESPACE
