//
//  utfstring.cpp
//  ePub3
//
//  Created by Jim Dovey on 2012-11-22.
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

#include "utfstring.h"
#include "integer_sequence.h"
#include <locale>

#if EPUB_PLATFORM(WINRT)
// need a converter from UTF-8 to Windows' wchar_t
#include <codecvt>
using WinRTCharConverter = std::wstring_convert<std::codecvt_utf8<wchar_t>>;
#endif

EPUB3_BEGIN_NAMESPACE

const size_t utf8_sizes[256] = {
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, // 0x00-0x1F
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, // 0x20-0x3F
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, // 0x40-0x5F
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, // 0x60-0x7F
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, // 0x80-0x9F
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, // 0xA0-0xBF
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, // 0xC0-0xDF
    3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,5,5,5,5,6,6,1,1  // 0xE0-0xFF
};

const string::size_type string::npos = string::__base::npos;
const string string::EmptyString = string();

string::string(const_u4pointer s)
{
    _base.append(_Convert<value_type>::toUTF8(s));
}
string::string(const_u4pointer s, size_type n)
{
    _base.append(_Convert<value_type>::toUTF8(s, 0, n));
}
string::string(size_type n, value_type c)
{
    if ( n != 0 )
        _base.append(_Convert<value_type>::toUTF8(c, n));
}
string::string(const char16_t* s)
{
    _base.append(_Convert<char16_t>::toUTF8(s));
}
string::string(const char16_t* s, size_type n)
{
    _base.append(_Convert<char16_t>::toUTF8(s, 0, n));
}
string::string(size_type n, char16_t c)
{
    if ( n != 0 )
        _base.append(_Convert<char16_t>::toUTF8(c, n));
}
string::string(const wchar_t* s)
{
    _base.append(_Convert<wchar_t>::toUTF8(s));
}
string::string(const wchar_t* s, size_type n)
{
    _base.append(_Convert<wchar_t>::toUTF8(s, 0, n));
}
string::string(size_type n, wchar_t c)
{
    if ( n != 0 )
        _base.append(_Convert<wchar_t>::toUTF8(c, n));
}
string::string(const std::wstring& s)
{
	_base.append(_Convert<wchar_t>::toUTF8(s));
}
#if EPUB_USE(WIN_XML)
string::string(const xml::string& s)
{
	_base.append(_Convert<wchar_t>::toUTF8(s));
}
#endif
string::string(const __base & s, size_type i, size_type n)
{
    // ensure we're looking at a valid location in the base string (not in the middle of a multi-byte character)j
    throw_unless_insertable(s, i, n);
    _base.assign(s, i, n);
}
string::string(const u16string_view& view) : _base(_Convert<char16_t>::toUTF8(view.data(), 0, view.size()))
{
}
string::string(const u32string_view& view) : _base(_Convert<char32_t>::toUTF8(view.data(), 0, view.size()))
{
}
string::string(const wstring_view& view) : _base(_Convert<wchar_t>::toUTF8(view.data(), 0, view.size()))
{
}
template <>
string::string(iterator first, iterator last) : _base(first.base(), last.base())
{
}
template <>
string::string(__base::const_iterator pos, __base::const_iterator end) : _base(pos, end)
{
}
template <>
string::string(const char * pos, const char * end) : _base(pos, end-pos)
{
}
template <>
string::string(const xmlChar * pos, const xmlChar * end) : _base(reinterpret_cast<const char*>(pos), end-pos)
{
}
string::size_type string::size() const _NOEXCEPT
{
    // note that ePub3::string .utf8_size() actually returns _base.size() (from std::string)
    // but that ePub3::string .size() does not necessarily return the same as std::string .size() !
    return to_utf32_size(_base.size());
}
void string::resize(size_type n, value_type c)
{
    size_type __s = size();
    if ( n > __s )
    {
        // get UTF-8 prepresentation of the character
        _base.append(_Convert<value_type>::toUTF8(c, n-__s));
    }
    else if ( n < __s )
    {
        // contraction-- the pad character is unneeded in this case
        resize(n);
    }
}
void string::resize(size_type n)
{
    size_type __s = size();
    if ( n > __s )
    {
        // extend with NUL chars-- one byte each in UTF-8
        size_type toAdd = n - __s;
        _base.resize(_base.size() + toAdd);
    }
    else if ( n < __s )
    {
        if ( n == 0 )
        {
            clear();
            return;
        }
        
        if ( n == 0 )
        {
            _base.resize(0);
            return;
        }
        
        // remove a certain number of UTF-8 characters
        size_type newByteSize = 0;
        const xmlChar * b = reinterpret_cast<const xmlChar*>(_base.data());
        const xmlChar * e = b + _base.length();
        for ( size_type i = 0; i < n && b < e; i++ )
        {
            // gets the byte-length of a single UTF-8 encoded character
            size_t len = UTF8CharLen(*b);
            newByteSize += len;
            b += len;
        }
        
        // resize the underlying byte string
        _base.resize(newByteSize);
    }
}
#if 0//EPUB_PLATFORM(WINRT)
::Platform::String^ string::winrt_str() const
{
	auto wstr = WinRTCharConverter().from_bytes(_base);
	return ref new ::Platform::String(wstr.data(), wstr.length());
}
#endif
const string::value_type string::at(size_type pos) const
{
    return const_cast<string*>(this)->at(pos);
}
string::value_type string::at(size_type pos)
{
    typedef _Convert<value_type> Converter;
    const char * _pos = reinterpret_cast<const char*>(xmlAt(pos));
    Converter::wide_string wstr = Converter::fromUTF8(_pos, 0, UTF8CharLen(*_pos));
    return wstr[0];
}
const xmlChar * string::xmlAt(size_type pos) const
{
    return const_cast<string*>(this)->xmlAt(pos);
}
xmlChar * string::xmlAt(size_type pos)
{
    if ( pos >= size() )
        throw std::range_error("Position beyond size of string.");
    
    __base::size_type bpos = to_byte_size(pos);
    const char * p = &_base.at(bpos);
    return const_cast<xmlChar*>(reinterpret_cast<const xmlChar *>(p));
}
string::__base string::utf8At(size_type pos) const
{
    __base::size_type bpos = to_byte_size(pos);
    size_t charLen = utf8_sizes[_base[bpos]];
    return _base.substr(bpos, charLen);
}
#ifndef UTFSTRING_SPECIALIZATIONS_INLINED
template <>
string & string::assign(iterator first, iterator last)
{
    _base.assign(first.base(), last.base());
    return *this;
}
template <>
string & string::assign(__base::const_iterator first, __base::const_iterator last)
{
    _base.assign(first, last);
    return *this;
}
template <>
string & string::assign(const char *first, const char *last)
{
    _base.assign(first, last-first);
    return *this;
}
#endif
string & string::assign(const string &o, size_type i, size_type n)
{
    // byte offset of character i
    auto pos = o._base.cbegin();
    auto end = pos + n;
    for ( size_type s = 0; s < i && pos < end; s++ )
    {
        size_t c = UTF8CharLen(*pos);
        pos += c;
    }
    
    _base.assign(pos, end);
    return *this;
}
string & string::assign(const_u4pointer s, size_type n)
{
    _base.assign(_Convert<value_type>::toUTF8(s, 0, n));
    return *this;
}
string& string::assign(const char16_t* s, size_type n)
{
    _base.assign(_Convert<char16_t>::toUTF8(s, 0, n));
    return *this;
}
#ifndef UTFSTRING_SPECIALIZATIONS_INLINED
template <>
string & string::append(const_iterator first, const_iterator last)
{
    _base.append(first.base(), last.base());
    return *this;
}
template <>
string & string::append(__base::const_iterator first, __base::const_iterator last)
{
    _base.append(first, last);
    return *this;
}
template <>
string & string::append(const char * first, const char * last)
{
    _base.append(first, last-first);
    return *this;
}
#endif
string & string::append(const string &o, size_type i, size_type n)
{
    if ( n == npos )
        return append(o.begin()+i, o.end());
    return append(o.begin()+i, o.begin()+i+n);
}
string & string::append(const_u4pointer s, size_type n)
{
    _base.append(_Convert<value_type>::toUTF8(s, 0, n));
    return *this;
}
string & string::append(size_type n, value_type c)
{
    size_type total = size() + n;
    resize(total, c);
    return *this;
}
string & string::append(const char16_t* s, size_type n)
{
    _base.append(_Convert<char16_t>::toUTF8(s, 0, n));
    return *this;
}
string & string::append(size_type n, char16_t c)
{
    append(n, static_cast<char32_t>(c));
    return *this;
}
#ifndef UTFSTRING_SPECIALIZATIONS_INLINED
template <>
string::iterator string::insert(iterator pos, iterator first, iterator last)
{
    if ( first == last )
        return pos;
    
#if CXX11_STRING_UNAVAILABLE
    _base.insert(pos.base(), first.base(), last.base());
    return iterator(pos + std::distance(first, last));
#else
    __base::iterator inserted(_base.insert(pos.base(), first.base(), last.base()));
    return iterator(inserted, _base.begin(), _base.end());
#endif
}
template <>
string::iterator string::insert(iterator pos, __base::iterator first, __base::iterator last)
{
    if ( first == last )
        return pos;
#if CXX11_STRING_UNAVAILABLE
    _base.insert(pos.base(), first, last);
    return iterator(pos + utf32_distance(first, last));
#else
    __base::iterator inserted(_base.insert(pos.base(), first, last));
    return iterator(inserted, _base.begin(), _base.end());
#endif
}
#endif
string & string::insert(size_type pos, const string &s, size_type b, size_type e)
{
    if ( b == e )
        return *this;
    
    __base::size_type bpos = to_byte_size(pos);
    if ( bpos == npos )
        return append(s, b, e);
    
    __base::size_type bb = s.to_byte_size(b);
    __base::size_type be = s.to_byte_size(b, e);
    
    if ( bb == npos )
        throw std::range_error("Position to copy from inserted string out of range");
    
    _base.insert(bpos, s._base, bb, be);
    return *this;
}
string::iterator string::insert(iterator pos, const string &s, size_type b, size_type e)
{
    if ( e == b )
        return pos;
    
    if ( pos == end() )
        return append(s, b, e).end();
    
    __base::size_type bb = s.to_byte_size(b);
    __base::size_type be = s.to_byte_size(b, e);
    
    if ( bb == npos )
        throw std::range_error("Position to copy from inserted string out of range");
    
    auto first = s._base.begin()+bb;
    auto last = (be == npos ? s._base.end() : s._base.begin()+be);
    
#if CXX11_STRING_UNAVAILABLE
    _base.insert(pos.base(), first, last);
    return iterator(pos + utf32_distance(first, last));
#else
    __base::iterator inserted(_base.insert(pos.base(), first, last));
    return iterator(inserted, _base.begin(), _base.end());
#endif
}
string & string::insert(size_type pos, const_u4pointer s, size_type e)
{
    if ( e == 0 )
        return *this;
    
    if ( pos > size() && pos != npos )
        throw std::range_error("Position outside string bounds");
    if ( pos == npos )
        pos = size();
    
    auto utf8 = _Convert<value_type>::toUTF8(s, 0, e);
    _base.insert(to_byte_size(pos), utf8);
    return *this;
}
string & string::insert(size_type pos, const char16_t* s, size_type e)
{
    if ( e == 0 )
        return *this;
    
    if ( pos > size() && pos != npos )
        throw std::range_error("Position outside string bounds");
    if ( pos == npos )
        pos = size();
    
    auto utf8 = _Convert<char16_t>::toUTF8(s, 0, e);
    _base.insert(to_byte_size(pos), utf8);
    return *this;
}
string & string::insert(size_type pos, size_type n, value_type c)
{
    size_type __s = size();
    if ( n == 0 )
        return *this;
    if ( pos == npos )
        pos = __s;
    else if ( pos > __s )
        throw std::range_error("Position outside string bounds");
    
    auto utf8 = _Convert<value_type>::toUTF8(c);
    if ( utf8.size() == 1 )
    {
        _base.insert(to_byte_size(pos), n, utf8[0]);
    }
    else
    {
        decltype(utf8) buf;
        buf.reserve(n*utf8.length());
        
        for ( size_type i = 0; i < n; i++ )
            buf.append(utf8);
        
        _base.insert(to_byte_size(pos), buf);
    }
    
    return *this;
}
string & string::insert(size_type pos, size_type n, char16_t c)
{
    size_type __s = size();
    if ( n == 0 )
        return *this;
    if ( pos == npos )
        pos = __s;
    else if ( pos > __s )
        throw std::range_error("Position outside string bounds");
    
    auto utf8 = _Convert<char16_t>::toUTF8(c);
    if ( utf8.size() == 1 )
    {
        _base.insert(to_byte_size(pos), n, utf8[0]);
    }
    else
    {
        decltype(utf8) buf;
        buf.reserve(n*utf8.length());
        
        for ( size_type i = 0; i < n; i++ )
            buf.append(utf8);
        
        _base.insert(to_byte_size(pos), buf);
    }
    
    return *this;
}
string::iterator string::insert(iterator pos, const_u4pointer s, size_type e)
{
    if ( e == 0 )
        return pos;
    auto utf8 = _Convert<value_type>::toUTF8(s, 0, e);
#if CXX11_STRING_UNAVAILABLE
    _base.insert(pos.base(), utf8.begin(), utf8.end());
    return iterator(pos + e);
#else
    __base::iterator inserted(_base.insert(pos.base(), utf8.begin(), utf8.end()));
    return iterator(inserted, _base.begin(), _base.end());
#endif
}
string::iterator string::insert(iterator pos, const char16_t* s, size_type e)
{
    if ( e == 0 )
        return pos;
    auto utf8 = _Convert<char16_t>::toUTF8(s, 0, e);
#if CXX11_STRING_UNAVAILABLE
    _base.insert(pos.base(), utf8.begin(), utf8.end());
    return iterator(pos + utf32_distance(utf8.begin(), utf8.end()));
#else
    __base::iterator inserted(_base.insert(pos.base(), utf8.begin(), utf8.end()));
    return iterator(inserted, _base.begin(), _base.end());
#endif
}
string::iterator string::insert(iterator pos, size_type n, value_type c)
{
    if ( n == 0 )
        return pos;
    if ( pos == end() )
        return append(n, c).end();
    
    auto utf8 = _Convert<value_type>::toUTF8(c);
    if ( utf8.size() == 1 )
    {
#if CXX11_STRING_UNAVAILABLE
        _base.insert(pos.base(), n, utf8[0]);
        return iterator(pos + n);
#else
        __base::iterator inserted(_base.insert(pos.base(), n, utf8[0]));
        return iterator(inserted, _base.begin(), _base.end());
#endif
    }
    
    decltype(utf8) buf;
    buf.reserve(n*utf8.size());
    
    for ( size_type i = 0; i < n; i++ )
        buf.append(utf8);
#if CXX11_STRING_UNAVAILABLE
    _base.insert(pos.base(), buf.begin(), buf.end());
    return iterator(pos + n);
#else
    auto inserted = _base.insert(pos.base(), buf.begin(), buf.end());
    return iterator(inserted, _base.begin(), _base.end());
#endif
}
string::iterator string::insert(iterator pos, size_type n, char16_t c)
{
    if ( n == 0 )
        return pos;
    if ( pos == end() )
        return append(n, c).end();
    
    auto utf8 = _Convert<char16_t>::toUTF8(c);
    if ( utf8.size() == 1 )
    {
#if CXX11_STRING_UNAVAILABLE
        _base.insert(pos.base(), n, utf8[0]);
        return iterator(pos + n);
#else
        __base::iterator inserted(_base.insert(pos.base(), n, utf8[0]));
        return iterator(inserted, _base.begin(), _base.end());
#endif
    }
    
    decltype(utf8) buf;
    buf.reserve(n*utf8.size());
    
    for ( size_type i = 0; i < n; i++ )
        buf.append(utf8);
#if CXX11_STRING_UNAVAILABLE
    _base.insert(pos.base(), buf.begin(), buf.end());
    return iterator(pos + n);
#else
    auto inserted = _base.insert(pos.base(), buf.begin(), buf.end());
    return iterator(inserted, _base.begin(), _base.end());
#endif
}
string & string::insert(size_type pos, const __base &s, size_type b, size_type e)
{
    throw_unless_insertable(s, b, e);
    _base.insert(to_byte_size(pos), s, b, e);
    return *this;
}
string & string::insert(size_type pos, __base::iterator b, __base::iterator e)
{
    throw_unless_insertable(&(*b), 0, e-b);
    _base.insert(_base.begin()+to_byte_size(pos), b, e);
    return *this;
}
string::iterator string::insert(iterator pos, const __base &s, size_type b, size_type e)
{
    throw_unless_insertable(s, b, e);
#if CXX11_STRING_UNAVAILABLE
    auto __b = s.begin()+b;
    auto __e = (e == npos ? s.end() : s.begin()+e);
    _base.insert(pos.base(), __b, __e);
    return iterator(pos + utf32_distance(__b, __e));
#else
    auto inserted(_base.insert(pos.base(), s.begin()+b, (e == npos ? s.end() : s.begin()+e)));
    return iterator(inserted, _base.begin(), _base.end());
#endif
}
string & string::insert(size_type pos, const char *s, size_type b, size_type e)
{
    throw_unless_insertable(s, b, e);
    if ( e == __base::npos )
        _base.insert(to_byte_size(pos), s+b);
    else
        _base.insert(to_byte_size(pos), s+b, e-b);
    return *this;
}
string & string::insert(size_type pos, size_type n, char c)
{
    _base.insert(to_byte_size(pos), n, c);
    return *this;
}
string::iterator string::insert(iterator pos, const char * str, size_type b, size_type e)
{
    if ( pos == end() )
        return append(str+b, e-b).end();
    
    if ( e == npos )
        e = strlen(str) - b;
#if CXX11_STRING_UNAVAILABLE
    _base.insert(pos.base(), str+b, str+e);
    return iterator(pos + utf32_distance(__base::const_iterator(str+b), __base::const_iterator(str+e)));
#else
    auto inserted(_base.insert(pos.base(), str+b, str+e));
    return iterator(inserted, _base.begin(), _base.end());
#endif
}
string::iterator string::insert(iterator pos, size_type n, char c)
{
    if ( pos == end() )
        return append(n, c).end();
#if CXX11_STRING_UNAVAILABLE
    _base.insert(pos.base(), n, c);
    return iterator(pos + n);
#else
    auto inserted(_base.insert(pos.base(), n, c));
    return iterator(inserted, _base.begin(), _base.end());
#endif
}
string & string::erase(size_type pos, size_type n)
{
    size_type __s = size();
    if ( pos == 0 && n == npos )
    {
        clear();
    }
    else if ( pos+n > __s )
    {
        throw std::range_error("Erase range outside string bounds");
    }
    else if ( n > 0 )
    {
        if ( n == npos || pos+n == __s )
        {
            _base.erase(to_byte_size(pos));
        }
        else
        {
            __base::size_type bpos = to_byte_size(pos);
            __base::size_type bend = to_byte_size(pos, pos+n);
            _base.erase(bpos, bend-bpos);
        }
    }
    
    return *this;
}
string::iterator string::erase(cxx11_const_iterator pos)
{
    auto modified(_base.erase(pos.base()));
    return iterator(modified, _base.begin(), _base.end());
}
string::iterator string::erase(cxx11_const_iterator first, cxx11_const_iterator last)
{
    auto modified(_base.erase(first.base(), last.base()));
    return iterator(modified, _base.begin(), _base.end());
}
#ifndef UTFSTRING_SPECIALIZATIONS_INLINED
template <>
string & string::replace(cxx11_const_iterator i1, cxx11_const_iterator i2, cxx11_const_iterator j1, cxx11_const_iterator j2)
{
    _base.replace(i1.base(), i2.base(), j1.base(), j2.base());
    return *this;
}
template <>
string & string::replace(cxx11_const_iterator i1, cxx11_const_iterator i2, __base::const_iterator j1, __base::const_iterator j2)
{
    _base.replace(i1.base(), i2.base(), j1, j2);
    return *this;
}
template <>
string & string::replace(cxx11_const_iterator i1, cxx11_const_iterator i2, std::u32string::const_iterator j1, std::u32string::const_iterator j2)
{
    auto utf8 = _Convert<value_type>::toUTF8(&(*j1), 0, std::distance(j1, j2));
    _base.replace(i1.base(), i2.base(), utf8);
    return *this;
}
#endif
string & string::replace(size_type pos1, size_type n1, const string & str)
{
    _base.replace(to_byte_size(pos1), to_byte_size(pos1, pos1+n1), str._base);
    return *this;
}
string & string::replace(size_type pos1, size_type n1, const string & str, size_type pos2, size_type n2)
{
    _base.replace(to_byte_size(pos1), to_byte_size(pos1, pos1+n1), str._base, str.to_byte_size(pos2), str.to_byte_size(pos2, pos2+n2));
    return *this;
}
string & string::replace(cxx11_const_iterator i1, cxx11_const_iterator i2, const string& str)
{
    _base.replace(i1.base(), i2.base(), str._base);
    return *this;
}
string & string::replace(size_type pos, size_type n1, const_u4pointer s, size_type n2)
{
    _base.replace(to_byte_size(pos), to_byte_size(pos, pos+n1), _Convert<value_type>::toUTF8(s, 0, n2));
    return *this;
}
string & string::replace(size_type pos, size_type n1, const char16_t* s, size_type n2)
{
    _base.replace(to_byte_size(pos), to_byte_size(pos, pos+n1), _Convert<char16_t>::toUTF8(s, 0, n2));
    return *this;
}
string & string::replace(size_type pos, size_type n1, const_u4pointer s)
{
    _base.replace(to_byte_size(pos), to_byte_size(pos, pos+n1), _Convert<value_type>::toUTF8(s));
    return *this;
}
string & string::replace(size_type pos, size_type n1, const char16_t* s)
{
    _base.replace(to_byte_size(pos), to_byte_size(pos, pos+n1), _Convert<char16_t>::toUTF8(s));
    return *this;
}
string & string::replace(size_type pos, size_type n1, size_type n2, value_type c)
{
    auto utf8 = _Convert<value_type>::toUTF8(c);
    if ( n2 == 1 )
    {
        _base.replace(to_byte_size(pos), to_byte_size(pos, pos+n1), utf8);
    }
    else if ( utf8.length() == 1 )
    {
        _base.replace(to_byte_size(pos), to_byte_size(pos, pos+n1), n2, utf8[0]);
    }
    else
    {
        decltype(utf8) buf;
        buf.reserve(utf8.length()*n2);
        for ( size_type i = 0; i < n2; i++ )
            buf.append(utf8);
        _base.replace(to_byte_size(pos), to_byte_size(pos, pos+n1), buf);
    }
    
    return *this;
}
string & string::replace(size_type pos, size_type n1, size_type n2, char16_t c)
{
    auto utf8 = _Convert<char16_t>::toUTF8(c);
    if ( n2 == 1 )
    {
        _base.replace(to_byte_size(pos), to_byte_size(pos, pos+n1), utf8);
    }
    else if ( utf8.length() == 1 )
    {
        _base.replace(to_byte_size(pos), to_byte_size(pos, pos+n1), n2, utf8[0]);
    }
    else
    {
        decltype(utf8) buf;
        buf.reserve(utf8.length()*n2);
        for ( size_type i = 0; i < n2; i++ )
            buf.append(utf8);
        _base.replace(to_byte_size(pos), to_byte_size(pos, pos+n1), buf);
    }
    
    return *this;
}
string & string::replace(cxx11_const_iterator i1, cxx11_const_iterator i2, const_u4pointer s, size_type n)
{
    _base.replace(i1.base(), i2.base(), _Convert<value_type>::toUTF8(s, 0, n));
    return *this;
}
string & string::replace(cxx11_const_iterator i1, cxx11_const_iterator i2, const char16_t* s, size_type n)
{
    _base.replace(i1.base(), i2.base(), _Convert<char16_t>::toUTF8(s, 0, n));
    return *this;
}
string & string::replace(cxx11_const_iterator i1, cxx11_const_iterator i2, const_u4pointer s)
{
    _base.replace(i1.base(), i2.base(), _Convert<value_type>::toUTF8(s));
    return *this;
}
string & string::replace(cxx11_const_iterator i1, cxx11_const_iterator i2, const char16_t* s)
{
    _base.replace(i1.base(), i2.base(), _Convert<char16_t>::toUTF8(s));
    return *this;
}
string & string::replace(cxx11_const_iterator i1, cxx11_const_iterator i2, size_type n, char16_t c)
{
    auto utf8 = _Convert<char16_t>::toUTF8(c);
    if ( n == 1 )
    {
        _base.replace(i1.base(), i2.base(), utf8);
    }
    else if ( utf8.length() == 1 )
    {
        _base.replace(i1.base(), i2.base(), n, utf8[0]);
    }
    else
    {
        decltype(utf8) buf;
        buf.reserve(utf8.length()*n);
        for ( size_type i = 0; i < n; i++ )
            buf.append(utf8);
        _base.replace(i1.base(), i2.base(), buf);
    }
    
    return *this;
}
string & string::replace(size_type pos1, size_type n1, const __base & str)
{
    _base.replace(to_byte_size(pos1), to_byte_size(pos1, pos1+n1), str);
    return *this;
}
string & string::replace(size_type pos1, size_type n1, const __base & str, size_type pos2, size_type n2)
{
    _base.replace(to_byte_size(pos1), to_byte_size(pos1, pos1+n1), str, pos2, n2);
    return *this;
}
string & string::replace(cxx11_const_iterator i1, cxx11_const_iterator i2, const __base & str)
{
    _base.replace(i1.base(), i2.base(), str);
    return *this;
}
string & string::replace(size_type pos, size_type n1, const char * s, size_type n2)
{
    _base.replace(to_byte_size(pos), to_byte_size(pos, pos+n1), s, n2);
    return *this;
}
string & string::replace(size_type pos, size_type n1, const char * s)
{
    _base.replace(to_byte_size(pos), to_byte_size(pos, pos+n1), s);
    return *this;
}
string & string::replace(size_type pos, size_type n1, size_type n2, char c)
{
    _base.replace(to_byte_size(pos), to_byte_size(pos, pos+n1), n2, c);
    return *this;
}
string & string::replace(cxx11_const_iterator i1, cxx11_const_iterator i2, const char * s, size_type n)
{
    _base.replace(i1.base(), i2.base(), s, n);
    return *this;
}
string & string::replace(cxx11_const_iterator i1, cxx11_const_iterator i2, const char * s)
{
    _base.replace(i1.base(), i2.base(), s);
    return *this;
}
string & string::replace(cxx11_const_iterator i1, cxx11_const_iterator i2, size_type n, char c)
{
    _base.replace(i1.base(), i2.base(), n, c);
    return *this;
}
string::size_type string::copy(u4pointer s, size_type n, size_type pos) const
{
    const_iterator cur = cbegin()+pos;
    size_type i = 0;
    for ( i = 0; i < n && cur < end(); i++, cur++ )
    {
        s[i] = *cur;
    }
    return i;
}
string::size_type string::copy(char16_t* s, size_type n, size_type pos) const
{
    return _Convert<char16_t>::fromUTF8(_base.substr(to_byte_size(pos))).copy(s, n);
}
string string::substr(size_type pos, size_type n) const
{
    if ( pos == 0 && n == npos )
        return string(*this);
    if ( n == npos )
        return string(_base.substr(to_byte_size(pos)));
    
    __base::size_type bpos = to_byte_size(pos), bn = to_byte_size(pos, pos+n) - bpos;
    return string(_base.substr(bpos, bn));
}
std::u32string string::utf32string() const
{
    return _Convert<value_type>::fromUTF8(_base);
}
std::u16string string::utf16string() const
{
    return _Convert<char16_t>::fromUTF8(_base);
}
std::wstring string::wchar_string() const
{
	return _Convert<wchar_t>::fromUTF8(_base);
}
string& string::tolower(const std::locale& loc)
{
    auto& facet = std::use_facet<std::ctype<char>>(loc);
    facet.tolower(const_cast<char*>(_base.data()), const_cast<char*>(_base.data()) + _base.size());
    return *this;
}
const string string::tolower(const std::locale& loc) const
{
    return string(*this).tolower(loc);
}
string& string::toupper(const std::locale& loc)
{
    auto& facet = std::use_facet<std::ctype<char>>(loc);
	facet.toupper(const_cast<char*>(_base.data()), const_cast<char*>(_base.data()) + _base.size());
    return *this;
}
const string string::toupper(const std::locale& loc) const
{
    return string(*this).toupper(loc);
}
#ifndef UTFSTRING_SPECIALIZATIONS_INLINED
template <>
string::size_type string::find_first_of<char>(const char * s, size_type pos, size_type n) const {
    validate_utf8(s+pos, npos);
    return find_first_of(_Convert<char>::toUTF8(s), pos);
}
template <>
string::size_type string::find_first_of<char>(const char * s, size_type pos) const  {
    validate_utf8(s+pos, npos);
    return find_first_of(_Convert<char>::toUTF8(s), pos);
}
template <>
string::size_type string::find_first_of<xmlChar>(const xmlChar * s, size_type pos, size_type n) const {
    validate_utf8(s+pos, npos);
    return find_first_of(_Convert<xmlChar>::toUTF8(s), pos);
}
template <>
string::size_type string::find_first_of<xmlChar>(const xmlChar * s, size_type pos) const {
    validate_utf8(s+pos, npos);
    return find_first_of(_Convert<xmlChar>::toUTF8(s), pos);
}
template <>
string::size_type string::find_last_of<char>(const char * s, size_type pos, size_type n) const {
    validate_utf8(s+pos, npos);
    return find_last_of(_Convert<char>::toUTF8(s), pos);
}
template <>
string::size_type string::find_last_of<char>(const char * s, size_type pos) const {
    validate_utf8(s+pos, npos);
    return find_last_of(_Convert<char>::toUTF8(s), pos);
}
template <>
string::size_type string::find_last_of<xmlChar>(const xmlChar * s, size_type pos, size_type n) const {
    validate_utf8(s+pos, npos);
    return find_last_of(_Convert<xmlChar>::toUTF8(s), pos);
}
template <>
string::size_type string::find_last_of<xmlChar>(const xmlChar * s, size_type pos) const {
    validate_utf8(s+pos, npos);
    return find_last_of(_Convert<xmlChar>::toUTF8(s), pos);
}
template <>
string::size_type string::find_first_not_of<char>(const char * s, size_type pos, size_type n) const {
    validate_utf8(s+pos, npos);
    return find_first_not_of(_Convert<char>::toUTF8(s), pos);
}
template <>
string::size_type string::find_first_not_of<char>(const char * s, size_type pos) const {
    validate_utf8(s+pos, npos);
    return find_first_not_of(_Convert<char>::toUTF8(s), pos);
}
template <>
string::size_type string::find_first_not_of<xmlChar>(const xmlChar * s, size_type pos, size_type n) const {
    validate_utf8(s+pos, npos);
    return find_first_not_of(_Convert<xmlChar>::toUTF8(s), pos);
}
template <>
string::size_type string::find_first_not_of<xmlChar>(const xmlChar * s, size_type pos) const {
    validate_utf8(s+pos, npos);
    return find_first_not_of(_Convert<xmlChar>::toUTF8(s), pos);
}
template <>
string::size_type string::find_last_not_of<char>(const char * s, size_type pos, size_type n) const {
    validate_utf8(s+pos, npos);
    return find_last_not_of(_Convert<char>::toUTF8(s), pos);
}
template <>
string::size_type string::find_last_not_of<char>(const char * s, size_type pos) const {
    validate_utf8(s+pos, npos);
    return find_last_not_of(_Convert<char>::toUTF8(s), pos);
}
template <>
string::size_type string::find_last_not_of<xmlChar>(const xmlChar * s, size_type pos, size_type n) const {
    validate_utf8(s+pos, npos);
    return find_last_not_of(_Convert<xmlChar>::toUTF8(s), pos);
}
template <>
string::size_type string::find_last_not_of<xmlChar>(const xmlChar * s, size_type pos) const {
    validate_utf8(s+pos, npos);
    return find_last_not_of(_Convert<xmlChar>::toUTF8(s), pos);
}
template <>
int string::compare(const value_type * s) const noexcept
{
    if ( s == nullptr )
        return 1;
    
    size_type sz = size();
    size_type len = traits_type::length(s);
    size_type n = std::min(sz, len);
    
    auto pos = cbegin();
    for ( ; n; pos++, s++ )
    {
        if ( traits_type::lt(*pos, *s) )
            return -1;
        if ( traits_type::lt(*s, *pos) )
            return 1;
    }
    
    if ( sz < len )
        return -1;
    if ( sz > len )
        return 1;
    return 0;
}
template <>
int string::compare(size_type pos1, size_type n1, const_u4pointer s) const
{
    if ( s == nullptr )
        return 1;
    if ( n1 == 0 && *s != 0 )
        return -1;
    
    size_type sz = (n1 == npos ? size() - pos1 : n1);
    size_type len = traits_type::length(s);
    size_type n = std::min(sz, len);
    
    auto pos = cbegin()+pos1;
    for ( ; n; pos++, s++ )
    {
        if ( traits_type::lt(*pos, *s) )
            return -1;
        if ( traits_type::lt(*s, *pos) )
            return 1;
    }
    
    if ( sz < len )
        return -1;
    if ( sz > len )
        return 1;
    return 0;
}
template <>
int string::compare(size_type pos1, size_type n1, const_u4pointer s, size_type n2) const
{
    if ( s == nullptr )
        return 1;
    if ( n1 == 0 && *s != 0 )
        return -1;
    
    size_type sz = (n1 == npos ? size() - pos1 : n1);
    size_type len = std::min(traits_type::length(s), n2);
    size_type n = std::min(sz, len);
    
    auto pos = cbegin()+pos1;
    for ( ; n; pos++, s++ )
    {
        if ( traits_type::lt(*pos, *s) )
            return -1;
        if ( traits_type::lt(*s, *pos) )
            return 1;
    }
    
    if ( sz < len )
        return -1;
    if ( sz > len )
        return 1;
    return 0;
}
template <>
int string::compare(const std::u32string & s) const noexcept
{
    size_type sz = size();
    size_type len = s.size();
    size_type n = std::min(sz, len);
    
    auto pos = cbegin();
    auto spos = s.cbegin();
    for ( ; n; pos++, spos++ )
    {
        if ( traits_type::lt(*pos, *spos) )
            return -1;
        if ( traits_type::lt(*spos, *pos) )
            return 1;
    }
    
    if ( sz < len )
        return -1;
    if ( sz > len )
        return 1;
    return 0;
}
template <>
int string::compare(size_type pos1, size_type n1, const std::u32string& str) const
{
    if ( n1 == 0 && !str.empty() )
        return -1;
    
    size_type sz = (n1 == npos ? size() - pos1 : n1);
    size_type len = str.size();
    size_type n = std::min(sz, len);
    
    auto pos = cbegin()+pos1;
    auto spos = str.cbegin();
    for ( ; n; pos++, spos++ )
    {
        if ( traits_type::lt(*pos, *spos) )
            return -1;
        if ( traits_type::lt(*spos, *pos) )
            return 1;
    }
    
    if ( sz < len )
        return -1;
    if ( sz > len )
        return 1;
    return 0;
}
template <>
int string::compare(size_type pos1, size_type n1, const std::u32string& str,
                    size_type pos2, size_type n2) const
{
    if ( n1 == 0 && n2 > 0 )
        return -1;
    
    size_type sz = (n1 == npos ? size() - pos1 : n1);
    size_type len = (n2 == npos ? str.size() - pos2 : n2);
    size_type n = std::min(sz, len);
    
    auto pos = cbegin()+pos1;
    auto spos = str.cbegin();
    for ( ; n; pos++, spos++ )
    {
        if ( traits_type::lt(*pos, *spos) )
            return -1;
        if ( traits_type::lt(*spos, *pos) )
            return 1;
    }
    
    if ( sz < len )
        return -1;
    if ( sz > len )
        return 1;
    return 0;
}
#endif
void string::validate_utf8(const __base &s) const
{
    if ( utf8::is_valid(s.begin(), s.end()) == false )
        throw InvalidUTF8Sequence(std::string("Invalid UTF-8 byte sequence: ") + s);
}
void string::validate_utf8(const char *s, size_type sz) const
{
    if ( sz == npos )
        sz = strlen(s);
    
    if ( utf8::is_valid(s, s + sz) == false )
        throw InvalidUTF8Sequence(std::string("Invalid UTF-8 byte sequence: ") + s);
}
void string::validate_utf8(const xmlChar *s, size_type sz) const
{
    validate_utf8(reinterpret_cast<const char*>(s), sz == npos ? xmlStrlen(s) : sz);
}
void string::throw_unless_insertable(const __base &s, size_type b, size_type e) const
{
    throw_unless_insertable(s.c_str(), b, e);
}
void string::throw_unless_insertable(const char *s, size_type b, size_type e) const
{
    // bounds-check the UTF8 string: is it valid?
    if ( e == npos )
        e = strlen(s);
    
    size_type t = 0;
    while ( t < b )
        t += UTF8CharLen(s[t]);
    
    // did it land directly at the starting bound?
    if ( t > b )
        throw InvalidUTF8Sequence(std::string("Invalid UTF-8 byte sequence: ") + s);
    
    // now check for the ending bound
    // if end == npos, we're checking the validity of the string (no partial multibyte characters at the end)
    while ( t < e )
        t += UTF8CharLen(s[t]);
    
    // did it land cleanly or go past the bound?
    if ( t > e )
        throw InvalidUTF8Sequence(std::string("Invalid UTF-8 byte sequence: ") + s);
}
void string::throw_unless_insertable(const xmlChar *s, size_type b, size_type e) const
{
    throw_unless_insertable(reinterpret_cast<const char*>(s), b, e);
}

string::__base::size_type string::to_byte_size(size_type __n) const _NOEXCEPT
{
    __base::size_type count = 0;
    if ( __n == npos || __n > size() )
    {
        count = __base::npos;
    }
    else if ( __n == size() )
    {
        count = _base.size();
    }
    else
    {
        auto pos = _base.cbegin();
        auto end = _base.cend();
        for ( size_type s = 0; s < __n && pos < end; s++ )
        {
            size_t c = UTF8CharLen(*pos);
            pos += c;
            count += c;
        }
    }
    
    return count;
}
string::__base::size_type string::to_byte_size(size_type __b, size_type __e) const _NOEXCEPT
{
    if ( __e == __base::npos )
        return __base::npos;
    
    __base::size_type r = to_byte_size(__b);
    if ( __e == 0 )
        return r;
    
    auto pos = _base.cbegin()+r;
    for ( size_type i = __b; i < __e; i++ )
    {
        size_t c = UTF8CharLen(*pos);
        pos += c;
        r += c;
    }
    
    return r;
}
string::size_type string::to_utf32_size(__base::size_type __n) const _NOEXCEPT
{
    size_type count = 0;
    if ( __n == __base::npos || __n > _base.size() )
    {
        count = npos;
    }
    else
    {
        auto pos = _base.cbegin();
        auto end = _base.cend();
        for ( size_type s = 0; s < __n && pos < end; count++ )
        {
            size_t c = UTF8CharLen(*pos);
            pos += c;
            s += c;
        }
    }
    
    return count;
}
string::size_type string::to_utf32_size(__base::size_type __b, __base::size_type __e) const _NOEXCEPT
{
    if ( __e == npos )
        return npos;
    
    size_type count = 0;
    auto pos = _base.cbegin()+__b;
    for ( size_type i = __b; i < __e; )
    {
        size_t c = UTF8CharLen(*pos);
        pos += c;
        count++;
    }
    return count;
}
string::size_type string::utf32_distance(__base::const_iterator first, __base::const_iterator last) _NOEXCEPT
{
    size_type __s = 0;
    for ( auto pos = first; pos < last; )
    {
        size_type __c = UTF8CharLen(*pos);
        __s += __c;
        pos += __c;
    }
    return __s;
}
string::value_type string::utf8_to_utf32(const xmlChar *utf8)
{
    if ( utf8 == nullptr )
        return 0;
    
    size_type len = UTF8CharLen(*utf8);
    return _Convert<value_type>::fromUTF8(reinterpret_cast<const char*>(utf8), len).at(0);
}
string::value_type string::utf8_to_utf32(const __base::const_iterator p)
{
    size_type len = UTF8CharLen(*p);
    return _Convert<value_type>::fromUTF8(&(*p), len).at(0);
}

EPUB3_END_NAMESPACE
