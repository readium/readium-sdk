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

#include <ePub3/base.h>

// OpenSSL APIs are deprecated on OS X and iOS
#if EPUB_OS(DARWIN)
#define COMMON_DIGEST_FOR_OPENSSL
#include <CommonCrypto/CommonDigest.h>
#elif EPUB_PLATFORM(WIN)
#include <windows.h>
#include <Wincrypt.h>
#elif EPUB_PLATFORM(WINRT)
#include <robuffer.h>
#include <wrl.h>
#include <wrl/implements.h>
#include <windows.storage.streams.h>

using namespace ::Windows::Security::Cryptography::Core;
using namespace ::Windows::Storage::Streams;
using namespace ::Microsoft::WRL;

namespace _Internal
{
	// thanks be to: http://stackoverflow.com/questions/10520335/how-to-wrap-a-char-buffer-in-a-winrt-ibuffer-in-c
	class NativeBuffer :
		public Microsoft::WRL::RuntimeClass<Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::RuntimeClassType::WinRtClassicComMix>,
		ABI::Windows::Storage::Streams::IBuffer,
		Windows::Storage::Streams::IBufferByteAccess>
	{
	public:
		virtual ~NativeBuffer()
		{
		}

		STDMETHODIMP RuntimeClassInitialize(byte *buffer, UINT totalSize)
		{
			m_length = totalSize;
			m_buffer = buffer;

			return S_OK;
		}

		STDMETHODIMP Buffer(byte **value)
		{
			*value = m_buffer;

			return S_OK;
		}

		STDMETHODIMP get_Capacity(UINT32 *value)
		{
			*value = m_length;

			return S_OK;
		}

		STDMETHODIMP get_Length(UINT32 *value)
		{
			*value = m_length;

			return S_OK;
		}

		STDMETHODIMP put_Length(UINT32 value)
		{
			m_length = value;

			return S_OK;
		}

	private:
		UINT32 m_length;
		byte *m_buffer;
	};

	IBuffer^ CreateNativeBuffer(LPVOID lpBuffer, DWORD nNumberOfBytes)
	{
		ComPtr<NativeBuffer> nativeBuffer;
		Details::MakeAndInitialize<NativeBuffer>(&nativeBuffer, (byte *)lpBuffer, nNumberOfBytes);
		auto iinspectable = (IInspectable *)reinterpret_cast<IInspectable *>(nativeBuffer.Get());
		IBuffer ^buffer = reinterpret_cast<IBuffer ^>(iinspectable);
		return buffer;
	}

	ComPtr<IBufferByteAccess> AccessBuffer(IBuffer^ buffer)
	{
		ComPtr<IUnknown> comBuffer(reinterpret_cast<IUnknown*>(buffer));
		ComPtr<IBufferByteAccess> byteBuffer;
		comBuffer.As(&byteBuffer);
		return byteBuffer;
	}
}
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

const REGEX_NS::regex FontObfuscator::TypeCheck("(?:font/.*|application/(?:x-font-.*|vnd.ms-(?:opentype|fontobject)))");

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
#elif EPUB_PLATFORM(WINRT)
	auto inBuf = _Internal::CreateNativeBuffer(const_cast<char*>(str.data()), str.length());
	auto keyBuf = HashAlgorithmProvider::OpenAlgorithm(HashAlgorithmNames::Sha1)->HashData(inBuf);

	auto rawKeyBuf = _Internal::AccessBuffer(keyBuf);
	byte* keyBytes = nullptr;
	rawKeyBuf->Buffer(&keyBytes);
	
	memcpy(_key, keyBytes, KeySize);
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
