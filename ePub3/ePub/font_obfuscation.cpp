//
//  font_obfuscation.cpp
//  ePub3
//
//  Created by Jim Dovey on 2012-12-21.
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

#include <ePub3/base.h>

// OpenSSL APIs are deprecated on OS X and iOS
#if EPUB_OS(DARWIN)
#define COMMON_DIGEST_FOR_OPENSSL
#include <CommonCrypto/CommonDigest.h>
#elif EPUB_PLATFORM(WIN)
#include <windows.h>
#include <Wincrypt.h>
#elif EPUB_PLATFORM(WINRT)
using namespace ::Platform;
using namespace ::Windows::Security::Cryptography;
using namespace ::Windows::Security::Cryptography::Core;
//using namespace ::Windows::Storage::Streams;
#else
#include <openssl/sha.h>
#endif

#include "font_obfuscation.h"
#include "container.h"
#include "package.h"
#include "filter_manager.h"

EPUB3_BEGIN_NAMESPACE

#if !EPUB_COMPILER_SUPPORTS(CXX_NONSTATIC_MEMBER_INIT) || EPUB_COMPILER(MSVC)
const char * const FontObfuscator::FontObfuscationAlgorithmID = "http://www.idpf.org/2008/embedding";
#endif

const char * const kBytesFiltered = "FontObfuscator::bytesFiltered";

const REGEX_NS::regex FontObfuscator::TypeCheck("(?:font/.*|application/(?:x-font-.*|font-.*|vnd.ms-(?:opentype|fontobject)))");

bool FontObfuscator::SupportsByteRanges() const
{
    return true;
}

void * FontObfuscator::FilterData(FilterContext* context, void *data, size_t len, size_t *outputLen)
{
    FontObfuscationContext* p = dynamic_cast<FontObfuscationContext*>(context);
    size_t bytesFiltered = p->ProcessedCount();
    
    uint8_t *buf = static_cast<uint8_t*>(data);
    for ( size_t i = 0; i < len && (i + bytesFiltered) < 1040; i++)
    {
        // XOR each of the first 1040 bytes of the font with the key, circling around the keybuf
        buf[i] ^= _key[(i+bytesFiltered)%20];
    }
    
    bytesFiltered += len;
    p->SetProcessedCount(bytesFiltered);
    *outputLen = len;
    return buf;
}
bool FontObfuscator::BuildKey(ConstContainerPtr container)
{
    REGEX_NS::regex re("\\s+");
    std::stringstream ss;
    
    for ( auto pkg : container->Packages() )
    {
        if ( ss.tellp() > 0 )
            ss << ' ';
        
        // we use a C++11 regex to remove all whitespace in the value
        std::string replacement;
        ss << REGEX_NS::regex_replace(pkg->PackageID().stl_str(), re, replacement);
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
#elif EPUB_PLATFORM(WINRT)
	auto byteArray = ArrayReference<byte>(reinterpret_cast<byte*>(const_cast<char*>(str.data())), str.length());
	auto inBuf = CryptographicBuffer::CreateFromByteArray(byteArray);
	auto keyBuf = HashAlgorithmProvider::OpenAlgorithm(HashAlgorithmNames::Sha1)->HashData(inBuf);

	Array<byte>^ outArray = nullptr;
	CryptographicBuffer::CopyToByteArray(keyBuf, &outArray);	// creates a new Array<byte>^ and returns it by reference
	memcpy_s(_key, KeySize, outArray->Data, outArray->Length);
#else
    // hash the accumulated string (using OpenSSL syntax for portability)
    SHA_CTX ctx;
    SHA1_Init(&ctx);
    SHA1_Update(&ctx, str.data(), str.length());
    SHA1_Final(_key, &ctx);
#endif
    return true;
}

ContentFilterPtr FontObfuscator::FontObfuscatorFactory(ConstPackagePtr package)
{
    ConstContainerPtr container = package->GetContainer();
    for ( auto& encInfo : container->EncryptionData() )
    {
        if ( encInfo->Algorithm() == FontObfuscationAlgorithmID )
        {
            return New(container);
        }
    }
    
    // opted out, nothing for us to do here
    return nullptr;
}

void FontObfuscator::Register()
{
    FilterManager::Instance()->RegisterFilter("FontObfuscator", EPUBDecryption, FontObfuscatorFactory);
}

EPUB3_END_NAMESPACE
