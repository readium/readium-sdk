//
//  url_canon_cpp11.cc
//  ePub3
//
//  Created by Jim Dovey on 2013-01-16.
//  Copyright (c) 2013 The Readium Foundation. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "url_canon_cpp11.h"
#include "url_canon_internal.h"     // for _itoa_s
#include <ePub3/utilities/utfstring.h>   // for UTF8CharLen
#include REGEX_INCLUDE
#include <locale>

#if EPUB_OS(ANDROID)
# include <utf8/utf8.h>
#else
# include <codecvt>
#endif

#if EPUB_OS(WINDOWS)
# define strlcat(dst,src,sz) strcat_s(dst,sz,src)
# define strlcpy(dst,src,sz) strcpy_s(dst,sz,src)
#endif

#if USING_ICU
// the only pieces of ICU that we still need
extern "C" int32_t
uidna_IDNToASCII(const char16* src, int32_t srcLength,
                 char16* dest, int32_t destCapacity,
                 int32_t options,
                 int* parseError,
                 int* status);
extern "C" int32_t
uidna_IDNToUnicode(const char16* src, int32_t srcLength,
                   char16* dest, int32_t destCapacity,
                   int32_t options,
                   int* parseError,
                   int* status);
#endif

namespace url_canon {

namespace {

// Called when converting a character that can not be represented, this will
// append an escaped version of the numerical character reference for that code
// point. It is of the form "&#1234;" and we will escape the non-digits to
// "%26%231234%3B". Why? This is what Netscape did back in the olden days.
void appendURLEscapedChar(void *context, char* buf, int32_t bufLen,
                          const char16* code_units, int32_t length,
                          char32_t code_point, bool unassigned, int *err)
{
    if ( !unassigned )
        return;
    
    *err = 0;
    const static int prefix_len = 6;
    const static char prefix[prefix_len + 1] = "%26%23";    // "&#" percent-escaped
    
    strlcpy(buf, prefix, bufLen);
    char number[8]; // Max Unicode code point is 7 digits
    _itoa_s(code_point, number, 10);
    strlcat(buf, number, bufLen);
    
    const static int postfix_len = 3;
    const static char postfix[postfix_len + 1] = "%3B";     // ";" percent-escaped
    strlcat(buf, postfix, bufLen);
}
    
}; // anonymous namespace

#if EPUB_OS(ANDROID)
class __conv16
{
public:
    typedef std::basic_string<char>     byte_string;
    typedef std::basic_string<char16_t> wide_string;
    
    template <class _Iter>
    byte_string to_bytes(_Iter first, _Iter last) const
    {
        byte_string __r;
        utf8::utf16to8(first, last, std::back_inserter(__r));
        return __r;
    }
    byte_string to_bytes(const char16_t* s)
    {
        return to_bytes(s, s+std::char_traits<char16_t>::length(s));
    }
    byte_string to_bytes(const wide_string& s)
    {
        return to_bytes(s.begin(), s.end());
    }
    template <class _Iter>
    wide_string from_bytes(_Iter first, _Iter last) const
    {
        wide_string __r;
        utf8::utf8to16(first, last, std::back_inserter(__r));
        return __r;
    }
    wide_string from_bytes(const char* s)
    {
        return from_bytes(s, s+std::char_traits<char>::length(s));
    }
    wide_string from_bytes(const byte_string& s)
    {
        return from_bytes(s.begin(), s.end());
    }
};
    
class __conv32
{
public:
    typedef std::basic_string<char>     byte_string;
    typedef std::basic_string<char32_t> wide_string;
    
    template <class _Iter>
    byte_string to_bytes(_Iter first, _Iter last) const
    {
        byte_string __r;
        utf8::utf32to8(first, last, std::back_inserter(__r));
        return __r;
    }
    byte_string to_bytes(const char32_t* s)
    {
        return to_bytes(s, s+std::char_traits<char32_t>::length(s));
    }
    byte_string to_bytes(const wide_string& s)
    {
        return to_bytes(s.begin(), s.end());
    }
    template <class _Iter>
    wide_string from_bytes(_Iter first, _Iter last) const
    {
        wide_string __r;
        utf8::utf8to32(first, last, std::back_inserter(__r));
        return __r;
    }
    wide_string from_bytes(const char* s)
    {
        return from_bytes(s, s+std::char_traits<char>::length(s));
    }
    wide_string from_bytes(const byte_string& s)
    {
        return from_bytes(s.begin(), s.end());
    }
};
#else
typedef std::wstring_convert<std::codecvt_utf8<char16>, char16> __conv16;
typedef std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> __conv32;
#endif

CXX11CharsetConverter::CXX11CharsetConverter()
{
}
CXX11CharsetConverter::~CXX11CharsetConverter()
{
}
void CXX11CharsetConverter::ConvertFromUTF16(const char16 *input, int input_len, CanonOutput *output)
{
    int begin_offset = output->length();
    int dest_capacity = output->capacity() - begin_offset;
    output->set_length(output->length());
    
    __conv16::byte_string utf8 = __conv16().to_bytes(input, input+input_len);
    
    if ( static_cast<int>(utf8.size()) > dest_capacity )
        output->Resize(static_cast<int>(begin_offset+utf8.size()));
    
    utf8.copy(&(output->data()[begin_offset]), utf8.size());
}

// Converts the Unicode input representing a hostname to ASCII using IDN rules.
// The output must be ASCII, but is represented as wide characters.
//
// On success, the output will be filled with the ASCII host name and it will
// return true. Unlike most other canonicalization functions, this assumes that
// the output is empty. The beginning of the host will be at offset 0, and
// the length of the output will be set to the length of the new host name.
//
// On error, this will return false. The output in this case is undefined.
bool IDNToASCII(const char16* src, int src_len, CanonOutputW* output) {
    DCHECK(output->length() == 0);  // Output buffer is assumed empty.
#if USING_ICU
    while (true) {
        // Use ALLOW_UNASSIGNED to be more tolerant of hostnames that violate
        // the spec (which do exist). This does not present any risk and is a
        // little more future proof.
        int err = 0;
        int32_t num_converted = uidna_IDNToASCII(src, src_len, output->data(),
                                                 output->capacity(),
                                                 1/*UIDNA_ALLOW_UNASSIGNED*/, nullptr, &err);
        if (err == 0) {
            output->set_length(num_converted);
            return true;
        }
        if (err != 15/*U_BUFFER_OVERFLOW_ERROR*/)
            return false;  // Unknown error, give up.
        
        // Not enough room in our buffer, expand.
        output->Resize(output->capacity() * 2);
    }
#else
    static REGEX_NS::basic_regex<char> invalidCharFinder("[^a-zA-Z0-9\\-]");
    if ( REGEX_NS::regex_match(ePub3::string(src, src_len).stl_str(), invalidCharFinder) )
        return false;       // contains an invalid character somewhere, and we can't convert it
    
    if ( src_len > output->capacity() )
        output->Resize(src_len);
    string16::traits_type::copy(output->data(), src, src_len);
#endif
    return true;
}
    
bool IDNToUnicode(const char16* src, int src_len, CanonOutputW* output)
{
    DCHECK(output->length() == 0);  // Output buffer is assumed empty.
#if USING_ICU
    while (true) {
        // Use ALLOW_UNASSIGNED to be more tolerant of hostnames that violate
        // the spec (which do exist). This does not present any risk and is a
        // little more future proof.
        int err = 0;
        int32_t num_converted = uidna_IDNToUnicode(src, src_len, output->data(),
                                                   output->capacity(),
                                                   1/*UIDNA_ALLOW_UNASSIGNED*/, nullptr, &err);
        if (err == 0) {
            output->set_length(num_converted);
            return true;
        }
        if (err != 15/*U_BUFFER_OVERFLOW_ERROR*/)
            return false;  // Unknown error, give up.
        
        // Not enough room in our buffer, expand.
        output->Resize(output->capacity() * 2);
    }
#else
#if EPUB_COMPILER_SUPPORTS(CXX_UNICODE_LITERALS)
    if ( src_len > 4 && src[0] == u'x' && src[1] == u'n' && src[2] == u'-' && src[3] == u'-' )
#else
    if ( src_len > 4 && src[0] == (char16_t)'x' && src[1] == (char16_t)'n' && src[2] == (char16_t)'-' && src[3] == (char16_t)'-' )
#endif
        return false;       // it's an ASCII IDN, and we can't convert it
    
    // just copy it over
    if ( src_len > output->capacity() )
        output->Resize(src_len);
    string16::traits_type::copy(output->data(), src, src_len);
#endif
    return true;
}

bool ReadUTFChar(const char* str, int* begin, int length, unsigned* code_point_out)
{
    const char* s = str + *begin;
    int len = static_cast<int>(UTF8CharLen(*s));
    
    try
    {
        __conv32::wide_string utf32 = __conv32().from_bytes(s, s+len);
        if ( utf32.empty() )
            throw std::logic_error("No UTF-32 conversion possible for input UTF-8 character.");
        
        *code_point_out = utf32[0];
        *begin += (len-1);
    }
    catch (...)
    {
        *code_point_out = kUnicodeReplacementCharacter;
        return false;
    }
    
    return true;
}
    
bool ReadUTFChar(const char16* str, int* begin, int length, unsigned* code_point)
{
    // C++11 doesn't provide a direct UTF16<->UTF32 conversion, so we'll have to
    //  go through UTF-8
    __conv16::byte_string utf8  = __conv16().to_bytes(str+*begin, str+length);
    __conv32::wide_string utf32 = __conv32().from_bytes(utf8);
    
    if ( utf32.empty() )
    {
        *code_point = kUnicodeReplacementCharacter;
        return false;
    }
    
    *code_point = utf32[0];
    return true;
}

};  // namespace url_canon

