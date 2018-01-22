//
//  font_obfuscation.cpp
//  ePub3
//
//  Created by Jim Dovey on 2012-12-21.
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

#include <ePub3/base.h>

// OpenSSL APIs are deprecated on OS X and iOS
#if EPUB_OS(DARWIN)
#define COMMON_DIGEST_FOR_OPENSSL
#include <CommonCrypto/CommonDigest.h>
#elif EPUB_OS(ANDROID)
#include <sha1/sha1.h>
//OPEN-SSL PRE-COMPILED STATIC LIBS DEPRECATED IN READIUM-SDK ANDROID
//#include <openssl/sha.h>
#elif EPUB_PLATFORM(WIN)
#include <windows.h>
#include <Wincrypt.h>
#elif EPUB_PLATFORM(WINRT)
using namespace ::Platform;
using namespace ::Windows::Security::Cryptography;
using namespace ::Windows::Security::Cryptography::Core;
//using namespace ::Windows::Storage::Streams;
#endif

#include "font_obfuscation.h"
#include "container.h"
#include "package.h"
#include "filter_manager.h"

EPUB3_BEGIN_NAMESPACE

#if !EPUB_COMPILER_SUPPORTS(CXX_NONSTATIC_MEMBER_INIT) || EPUB_COMPILER(MSVC)
const char * const FontObfuscator::FontObfuscationAlgorithmID = "http://www.idpf.org/2008/embedding";
#endif

const REGEX_NS::regex FontObfuscator::TypeCheck("(?:font/.*|application/(?:x-font-.*|font-.*|vnd.ms-(?:opentype|fontobject)))");

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
bool FontObfuscator::BuildKey(ConstContainerPtr container, ConstPackagePtr pkg)
{
    REGEX_NS::regex re("\\s+");
    std::stringstream ss;
    
//    for ( auto pkg : container->Packages() )
//    {
        if ( ss.tellp() > 0 )
            ss << ' ';
        
        // we use a C++11 regex to remove all whitespace in the value
        std::string replacement;
        ss << REGEX_NS::regex_replace(pkg->PackageID().stl_str(), re, replacement);
//    }

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
    CryptographicBuffer::CopyToByteArray(keyBuf, &outArray);    // creates a new Array<byte>^ and returns it by reference
    memcpy_s(_key, KeySize, outArray->Data, outArray->Length);
#elif EPUB_OS(ANDROID)

    SHA1* sha1 = new SHA1();
    sha1->addBytes(str.data(), str.length());
	unsigned char* digest = sha1->getDigest();
    std::memcpy(_key, digest, KeySize); //reinterpret_cast<uint8_t*>()
	delete sha1;
	free(digest);

//    sha1_context ctx;
//    sha1_starts(&ctx);
//    sha1_update(&ctx, str.data(), str.length());
//    sha1_finish(&ctx, _key);
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
            return std::make_shared<FontObfuscator>(container, package); //New(container, package);
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
