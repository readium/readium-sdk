//
//  xmlstring.h
//  ePub3
//
//  Created by Jim Dovey on 2012-11-22.
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

#ifndef __ePub3_xml_string__
#define __ePub3_xml_string__

#include "base.h"
#include <string>
#include <iterator>
#include <initializer_list>
#include <locale>
#include <codecvt>

EPUB3_XML_BEGIN_NAMESPACE

class string;
typedef std::map<string, string>  NamespaceMap;

extern const size_t utf8_sizes[256];
#define UTF8CharLen(c) utf8_sizes[static_cast<xmlChar>(c)]

class string
{
public:
    typedef std::string __base;
    
    typedef __base::size_type           size_type;
    typedef __base::difference_type     difference_type;
    typedef std::char_traits<char32_t>  traits_type;
    typedef char32_t                    value_type;
    typedef value_type&                 reference;
    typedef const value_type&           const_reference;
    typedef value_type*                 u4pointer;
    typedef const value_type*           const_u4pointer;
    
    static value_type utf8_to_utf32(const xmlChar * utf8);
    static value_type utf8_to_utf32(const __base::const_iterator p);
    
    template <class _Iter>
    class _iterator
    {
    public:
        typedef std::bidirectional_iterator_tag iterator_category;
        typedef u4pointer                       pointer;
        typedef difference_type                 difference_type;
        typedef value_type                      value_type;
        typedef reference                       reference;
        
        inline _iterator() {}
        inline _iterator(const _iterator &o) : _base(o._base) {}
        inline _iterator(_iterator &&o) : _base(std::move(o._base)) {}
        inline _iterator(_Iter &i) : _base(i) {}
        inline _iterator(_Iter &&i) : _base(i) {}
        
        inline value_type operator * () const {
            return string::utf8_to_utf32(_base);
        }
        
        inline string::__base utf8char() const {
            size_type __n = utf8_sizes[(*_base)];
            __base::value_type __buf[6] = {0};
            __base::traits_type::copy(__buf, &(*_base), __n);
            return string::__base(__buf, __n);
        }
        
        inline _iterator & operator ++ () {
            _advance();
            return *this;
        }
        inline const _iterator operator ++ (int) {
            const _iterator<_Iter> tmp(*this);
            _advance();
            return tmp;
        }
        inline _iterator & operator -- () {
            _retreat();
            return *this;
        }
        inline const _iterator operator -- (int) {
            const _iterator<_Iter> tmp(*this);
            _retreat();
            return tmp;
        }
        inline _iterator & operator += (int n) {
            bool adding = (n >= 0);
            n = abs(n);
            for ( int i = 0; i < n; i++ )
            {
                if ( adding )
                    _advance();
                else
                    _retreat();
            }
            return *this;
        }
        inline _iterator & operator += (size_type n) {
            for ( int i = 0; i < n; i++ ) {
                _advance();
            }
            return *this;
        }
        inline _iterator & operator -= (int n) {
            return operator+=(-n);
        }
        inline _iterator & operator -= (size_type n) {
            for ( int i = 0; i < n; i++ ) {
                _retreat();
            }
        }
        inline _iterator operator + (int n) {
            return _iterator<_Iter>(*this).operator+=(n);
        }
        inline _iterator operator + (size_type n) {
            return _iterator<_Iter>(*this).operator+=(n);
        }
        inline _iterator operator - (int n) {
            return _iterator<_Iter>(*this).operator-=(n);
        }
        inline _iterator operator - (size_type n) {
            return _iterator<_Iter>(*this).operator-=(n);
        }
        inline difference_type operator - (_iterator i) const {
            if ( *this < i )
                return 0;
            return std::distance(i, *this);
        }
        
        inline bool operator == (const _iterator<_Iter> &o) const {
            return _base == o._base;
        }
        inline bool operator != (const _iterator<_Iter> &o) const {
            return _base != o._base;
        }
        inline bool operator > (const _iterator<_Iter> & o) const {
            return _base > o._base;
        }
        inline bool operator >= (const _iterator<_Iter> & o) const {
            return _base >= o._base;
        }
        
        // weird things happen to auto-indenting when I define these inline here...
        inline bool operator <= (const _iterator<_Iter> & o) const;
        inline bool operator < (const _iterator<_Iter> & o) const;
        
    protected:
        _Iter _base;
        
        friend class string;
        inline _Iter & __base() { return _base; }
        
        inline void _advance() {
            string::__base::value_type __c = *_base;
            string::__base::size_type __n = UTF8CharLen(__c);
            _base += __n;
        }
        inline void _retreat() { do { --_base; } while ( (static_cast<const xmlChar>(*_base) & 0xc0) == 0x80 ); }
    };
    
    class InvalidUTF8Sequence : exception {
    public:
        InvalidUTF8Sequence(const std::string & str) : exception(str) {}
        InvalidUTF8Sequence(const char * str) : exception(str) {}
    };
    
    typedef _iterator<__base::iterator>        iterator;
    typedef _iterator<__base::const_iterator>  const_iterator;
    
    static const size_type npos;
    
#if 0
#pragma mark - Construction/Destruction
#endif
    
    // Standard
    string() : _base() {}
    string(const string &o) : _base(o._base) {}
    string(string &&o) : _base(std::move(o._base)) {}
    string(const string & s, size_type i, size_type n=npos) : _base(s._base, s.to_byte_size(i), s.to_byte_size(i,n)) {}
    
    // From char32_t (value_type)
    string(const_u4pointer s);   // NUL-delimited
    string(const_u4pointer s, size_type n);
    string(size_type n, value_type c);
    string(std::initializer_list<value_type> __il) : string(__il.begin(), __il.end()) {}
    
    // From char16_t (pure UTF-16)
    string(const char16_t* s);    // NUL-delimited
    string(const char16_t* s, size_type n);
    string(size_type n, char16_t c);
    string(std::initializer_list<char16_t> __il) : string(__il.begin(), __il.end()) {}
    
    // From std::string
    string(const __base &o) : _base(o) {}
    string(__base &&o) : _base(o) {}
    string(const __base &s, size_type i, size_type n=npos);
    
    // From char
    string(const char * s) : _base(s) {}
    string(const char * s, size_type n) : _base(s, n) {}
    string(size_type n, char c) : _base(n, c) {}
    
    // From xmlChar (unsigned char)
    string(const xmlChar * s) : _base(reinterpret_cast<const char *>(s)) {}
    string(const xmlChar * s, size_type n) : _base(reinterpret_cast<const char *>(s)) {}
    string(size_type n, xmlChar c) : _base(n, static_cast<char>(c)) {}
    
    template <class InputIterator>
    string(InputIterator begin, InputIterator end);
    
    template <typename... Args>
    string(const Args&... args) : _base(_Str(args...)) {}
    
    ~string() {}
    
#if 0
#pragma mark - Length/Iteration/Indexing
#endif
    
    size_type size() const noexcept;
    size_type length() const noexcept { return size(); }
    size_type max_size() const noexcept { return _base.max_size()/sizeof(value_type); }
    size_type capacity() const noexcept { return _base.capacity(); }
    
    void resize(size_type n, value_type c);
    void resize(size_type n);
    
    void reserve(size_type res_arg = 0) { return _base.reserve(res_arg*4); } // best guess
    void shrink_to_fit() { _base.shrink_to_fit(); }
    void clear() noexcept { _base.clear(); }
    bool empty() const noexcept { return _base.empty(); }
    
    iterator begin() noexcept { return iterator(_base.begin()); }
    const_iterator begin() const { return const_iterator(_base.begin()); }
    const_iterator cbegin() const { return const_iterator(_base.begin()); }
    iterator end() noexcept { return iterator(_base.end()); }
    const_iterator end() const { return const_iterator(_base.end()); }
    const_iterator cend() const { return const_iterator(_base.end()); }
    
    const value_type at(size_type pos) const;
    value_type at(size_type pos);
    
    const value_type operator[](size_type pos) const { return at(pos); }
    value_type operator[](size_type pos) { return at(pos); }
    
    const xmlChar * xmlAt(size_type pos) const;
    xmlChar * xmlAt(size_type pos);
    
#if 0
#pragma mark - Assign
#endif
    
    template <class InputIterator>
    string & assign(InputIterator first, InputIterator last);
    
    // standard
    string & assign(const string &o) { _base.assign(o._base); return *this; }
    string & assign(const string &o, size_type i, size_type n=npos);
    string & assign(string &&o) { _base.assign(std::move(o._base)); return *this; }
    string & operator=(const string & o) { return assign(o); }
    string & operator=(string &&o) { return assign(o); }
    
    // char32_t
    string & assign(const_u4pointer s, size_type n=npos);
    string & assign(size_type n, value_type c) { clear(); resize(n, c); return *this; }
    string & assign(std::initializer_list<value_type> __il) { return assign(__il.begin(), __il.end()); }
    string & operator=(const_u4pointer s) { return assign(s, npos); }
    string & operator=(value_type c) { return assign(1, c); }
    string & operator=(std::initializer_list<value_type> l) { return assign(l); }
    
    // char16_t
    string & assign(const char16_t* s, size_type n=npos);
    string & assign(size_type n, char16_t c) { clear(); resize(n, static_cast<value_type>(c)); return *this; }
    string & assign(std::initializer_list<char16_t> __il) { return assign(__il.begin(), __il.end()); }
    string & operator=(const char16_t* s) { return assign(s, npos); }
    string & operator=(char16_t c) { return assign(1, c); }
    string & operator=(std::initializer_list<char16_t> l) { return assign(l); }
    
    // std::string
    string & assign(const __base & o) { _base.assign(o); return *this; }
    string & assign(const __base & o, size_type i, size_type n=npos)
        { _base.assign(o, i, n); return *this; }
    string & assign(__base &&o) { _base.assign(o); return *this; }
    string & operator=(const __base &o) { return assign(o); }
    string & operator=(__base &&o) { return assign(o); }
    
    // char
    string & assign(const char * s, size_type n=npos) { _base.assign(s, n); return *this; }
    string & assign(size_type n, char c) { _base.assign(n, c); return *this; }
    string & assign(std::initializer_list<__base::value_type> __il) { _base.assign(__il); return *this; }
    string & operator=(const char * s) { return assign(s, npos); }
    string & operator=(char c) { return assign(1, c); }
    string & operator=(std::initializer_list<__base::value_type> __il) { return assign(__il); }
    
    // xmlChar
    string & assign(const xmlChar * s, size_type n=npos) { _base.assign(reinterpret_cast<const char *>(s), n); return *this; }
    string & assign(size_type n, xmlChar c) { _base.assign(n, static_cast<char>(c)); return *this; }
    string & assign(std::initializer_list<xmlChar> __il) { return assign(__il.begin(), __il.end()); }
    string & operator=(const xmlChar *s) { return assign(s, npos); }
    string & operator=(xmlChar c) { return assign(1, c); }
    string & operator=(std::initializer_list<xmlChar> __il) { return assign(__il); }
    
#if 0
#pragma mark - Append
#endif
    
    template <class InputIterator>
    string & append(InputIterator first, InputIterator last);
    
    template <typename... Args>
    string & append(const Args&... args) { return append(string(args...)); }
    
    // standard
    string & append(const string &o) { _base.append(o._base); return *this; }
    string & append(const string &o, size_type i, size_type n=npos);
    string & append(string &&o) { _base.append(std::move(o._base)); return *this; }
    string & operator+=(const string & o) { return append(o); }
    string & operator+=(string &&o) { return append(o); }
    
    // char32_t
    string & append(const_u4pointer s, size_type n=npos);
    string & append(size_type n, value_type c);
    string & append(std::initializer_list<value_type> __il) { return append(__il.begin(), __il.end()); }
    string & operator+=(const_u4pointer s) { return append(s, npos); }
    string & operator+=(value_type c) { return append(1, c); }
    string & operator+=(std::initializer_list<value_type> __il) { return append(__il); }
    
    // char16_t
    string & append(const char16_t* s, size_type n=npos);
    string & append(size_type n, char16_t c);
    string & append(std::initializer_list<char16_t> __il) { return append(__il.begin(), __il.end()); }
    string & operator+=(const char16_t* s) { return append(s, npos); }
    string & operator+=(char16_t c) { return append(1, c); }
    string & operator+=(std::initializer_list<char16_t> __il) { return append(__il); }
    
    // std::string
    string & append(const __base & o) { _base.append(o); return *this; }
    string & append(const __base & o, size_type i, size_type n=npos) { _base.append(o, i, n); return *this; }
    string & append(__base &&o) { _base.append(o); return *this; }
    string & operator+=(const __base &o) { return append(o); }
    string & operator+=(__base &&o) { return append(o); }
    
    // char
    string & append(const char * s, size_type n=npos) { _base.append(s, n); return *this; }
    string & append(size_type n, char c) { _base.append(n, c); return *this; }
    string & append(std::initializer_list<__base::value_type> __il) { _base.append(__il); return *this; }
    string & operator+=(const char * s) { return append(s, npos); }
    string & operator+=(char c) { return append(1, c); }
    string & operator+=(std::initializer_list<__base::value_type> __il) { return append(__il); }
    
    // xmlChar
    string & append(const xmlChar * s, size_type n=npos) { _base.append(reinterpret_cast<const char *>(s), n); return *this; }
    string & append(size_type n, xmlChar c) { _base.append(n, static_cast<char>(c)); return *this; }
    string & append(std::initializer_list<xmlChar> __il) { return append(__il.begin(), __il.end()); }
    string & operator+=(const xmlChar *s) { return append(s, npos); }
    string & operator+=(xmlChar c) { return append(1, c); }
    string & operator+=(std::initializer_list<xmlChar> __il) { return append(__il); }
    
#if 0
#pragma mark - Insertion
#endif
    
    template <class InputIterator>
    iterator insert(iterator p, InputIterator first, InputIterator last);
    
    template <typename... Args>
    string & insert(size_type p, const Args&... args) { return insert(p, string(args...)); }
    template <typename... Args>
    iterator insert(iterator p, const Args&... args) { return insert(p, string(args...)); }
    
    // standard
    string & insert(size_type p, const string &s, size_type b=0, size_type e=npos);
    iterator insert(iterator p, const string & s, size_type b=0, size_type e=npos);
    
    // char32_t
    string & insert(size_type p, const_u4pointer s, size_type e=npos);
    string & insert(size_type p, size_type n, value_type c);
    iterator insert(iterator p, const_u4pointer s, size_type e=npos);
    iterator insert(iterator p, size_type n, value_type c);
    iterator insert(iterator p, std::initializer_list<value_type> __il) { return insert(p, __il.begin(), __il.end()); }
    
    // char16_t
    string & insert(size_type p, const char16_t* s, size_type e=npos);
    string & insert(size_type p, size_type n, char16_t c);
    iterator insert(iterator p, const char16_t* s, size_type e=npos);
    iterator insert(iterator p, size_type n, char16_t c);
    iterator insert(iterator p, std::initializer_list<char16_t> __il) { return insert(p, __il.begin(), __il.end()); }
    
    // std::string
    string & insert(size_type p, const __base &s, size_type b=0, size_type e=npos) throw (InvalidUTF8Sequence);
    string & insert(size_type p, __base::iterator b, __base::iterator e) throw (InvalidUTF8Sequence);
    iterator insert(iterator p, const __base & s, size_type b=0, size_type e=npos) throw (InvalidUTF8Sequence);
    
    // char
    string & insert(size_type p, const char * s, size_type b=0, size_type e=npos) throw (InvalidUTF8Sequence);
    string & insert(size_type p, size_type n, char c);
    iterator insert(iterator p, const char * s, size_type b=0, size_type e=npos) throw (InvalidUTF8Sequence);
    iterator insert(iterator p, size_type n, char c);
    iterator insert(iterator p, std::initializer_list<char> __il) { return insert(p, __il.begin(), __il.end()); }
    
    // xmlChar
    string & insert(size_type p, const xmlChar * s, size_type b=0, size_type e=npos) throw (InvalidUTF8Sequence)
        { return insert(p, reinterpret_cast<const char*>(s), b, e); }
    string & insert(size_type p, size_type n, xmlChar c) { return insert(p, n, static_cast<char>(c)); }
    iterator insert(iterator p, const xmlChar * s, size_type b=0, size_type e=npos) throw (InvalidUTF8Sequence)
        { return insert(p, reinterpret_cast<const char*>(s), b, e); }
    iterator insert(iterator p, size_type n, xmlChar c) { return insert(p, n, static_cast<char>(c)); }
    iterator insert(iterator p, std::initializer_list<xmlChar> __il) { return insert(p, __il.begin(), __il.end()); }
    
#if 0
#pragma mark - Erasing
#endif
    
    string & erase(size_type pos=0, size_type n=npos);
    iterator erase(const_iterator pos);
    iterator erase(const_iterator first, const_iterator last);
    
#if 0
#pragma mark - Replacements
#endif
    
    template <class InputIterator>
    string & replace(const_iterator i1, const_iterator i2, InputIterator j1, InputIterator j2);
    
    template <typename... Args>
    string & replace(size_type pos1, size_type n1, const Args&... args) {
        return replace(pos1, n1, string(args...));
    }
    template <typename... Args>
    string & replace(const_iterator i1, const_iterator i2, Args&... args) {
        return replace(i1, i2, string(args...));
    }
    
    // standard
    string & replace(size_type pos1, size_type n1, const string & str);
    string & replace(size_type pos1, size_type n1, const string & str, size_type pos2, size_type n2);
    string & replace(const_iterator i1, const_iterator i2, const string& str);
    
    // char32_t
    string & replace(size_type pos, size_type n1, const_u4pointer s, size_type n2);
    string & replace(size_type pos, size_type n1, const_u4pointer s);
    string & replace(size_type pos, size_type n1, size_type n2, value_type c);
    string & replace(const_iterator i1, const_iterator i2, const_u4pointer s, size_type n);
    string & replace(const_iterator i1, const_iterator i2, const_u4pointer s);
    string & replace(const_iterator i1, const_iterator i2, size_type n, value_type c);
    string & replace(const_iterator i1, const_iterator i2, std::initializer_list<value_type> __il) {
        return replace(i1, i2, __il.begin(), __il.end());
    }
    
    // char16_t
    string & replace(size_type pos, size_type n1, const char16_t* s, size_type n2);
    string & replace(size_type pos, size_type n1, const char16_t* s);
    string & replace(size_type pos, size_type n1, size_type n2, char16_t c);
    string & replace(const_iterator i1, const_iterator i2, const char16_t* s, size_type n);
    string & replace(const_iterator i1, const_iterator i2, const char16_t* s);
    string & replace(const_iterator i1, const_iterator i2, size_type n, char16_t c);
    string & replace(const_iterator i1, const_iterator i2, std::initializer_list<char16_t> __il) {
        return replace(i1, i2, __il.begin(), __il.end());
    }
    
    // std::string
    string & replace(size_type pos1, size_type n1, const __base & str);
    string & replace(size_type pos1, size_type n1, const __base & str, size_type pos2, size_type n2);
    string & replace(const_iterator i1, const_iterator i2, const __base & str);
    
    // char
    string & replace(size_type pos, size_type n1, const char * s, size_type n2);
    string & replace(size_type pos, size_type n1, const char * s);
    string & replace(size_type pos, size_type n1, size_type n2, char c);
    string & replace(const_iterator i1, const_iterator i2, const char * s, size_type n);
    string & replace(const_iterator i1, const_iterator i2, const char * s);
    string & replace(const_iterator i1, const_iterator i2, size_type n, char c);
    string & replace(const_iterator i1, const_iterator i2, std::initializer_list<char> __il) {
        return replace(i1, i2, __il.begin(), __il.end());
    }
    
    // xmlChar
    string & replace(size_type pos, size_type n1, const xmlChar * s, size_type n2)
        { return replace(pos, n1, reinterpret_cast<const char*>(s), n2); }
    string & replace(size_type pos, size_type n1, const xmlChar * s)
        { return replace(pos, n1, reinterpret_cast<const char*>(s)); }
    string & replace(size_type pos, size_type n1, size_type n2, xmlChar c)
        { return replace(pos, n1, n2, static_cast<char>(c)); }
    string & replace(const_iterator i1, const_iterator i2, const xmlChar * s, size_type n)
        { return replace(i1, i2, reinterpret_cast<const char*>(s), n); }
    string & replace(const_iterator i1, const_iterator i2, const xmlChar * s)
        { return replace(i1, i2, reinterpret_cast<const char*>(s)); }
    string & replace(const_iterator i1, const_iterator i2, size_type n, xmlChar c)
        { return replace(i1, i2, n, static_cast<char>(c)); }
    string & replace(const_iterator i1, const_iterator i2, std::initializer_list<xmlChar> __il) {
        return replace(i1, i2, __il.begin(), __il.end());
    }
    
#if 0
#pragma mark - Outputs
#endif
    
    size_type copy(u4pointer s, size_type n, size_type pos=0) const;
    size_type copy(char16_t* s, size_type n, size_type pos=0) const;
    size_type copyC(char * s, size_type n, size_type pos=0) const { return _base.copy(s, n, pos); }
    size_type copyXML(xmlChar * s, size_type n, size_type pos=0) const { return _base.copy(reinterpret_cast<char*>(s), n, pos); }
    
    string substr(size_type pos=0, size_type n=npos) const;
    
    void swap(string & str)
    noexcept(!__base::__alloc_traits::propagate_on_container_swap::value || std::__is_nothrow_swappable<__base::__alloc_traits>::value) {
        _base.swap(str._base);
    }
    
    std::u32string utf32string() const;
    inline const_u4pointer utf32() const { return utf32string().c_str(); }
    
    std::u16string utf16string() const;
    inline const char16_t* utf16() const { return utf16string().c_str(); }
    
    __base::const_pointer c_str() const noexcept { return _base.c_str(); }
    __base::const_pointer data() const noexcept { return _base.data(); }
    
    const __base& stl_str() const { return _base; }
    
    const xmlChar * utf8() const { return reinterpret_cast<const xmlChar *>(c_str()); }
    
    __base::allocator_type get_allocator() const noexcept { return _base.get_allocator(); }
    
#if 0
#pragma mark - Searching
#endif
    
    size_type find(const string& str, size_type pos=0) const noexcept {
        return to_utf32_size(_base.find(str._base, to_byte_size(pos)));
    }
    size_type find(const __base& str, size_type pos=0) const noexcept {
        return to_utf32_size(_base.find(str, to_byte_size(pos)));
    }
    template <typename _CharT>
    size_type find(const _CharT * s, size_type pos, size_type n) const noexcept {
        return to_utf32_size(_base.find(_Convert<_CharT>::toUTF8(s, 0, n), to_byte_size(pos)));
    }
    template <typename _CharT>
    size_type find(const _CharT * s, size_type pos = 0) const noexcept {
        return to_utf32_size(_base.find(_Convert<_CharT>::toUTF8(s), to_byte_size(pos)));
    }
    template <typename _CharT>
    size_type find(_CharT c, size_type pos = 0) const noexcept {
        return to_utf32_size(_base.find(_Convert<_CharT>::toUTF8(c), to_byte_size(pos)));
    }
    
    size_type rfind(const string& str, size_type pos=0) const noexcept {
        return to_utf32_size(_base.rfind(str._base, to_byte_size(pos)));
    }
    size_type rfind(const __base& str, size_type pos=0) const noexcept {
        return to_utf32_size(_base.rfind(str, to_byte_size(pos)));
    }
    template <typename _CharT>
    size_type rfind(const _CharT * s, size_type pos, size_type n) const noexcept {
        return to_utf32_size(_base.rfind(_Convert<_CharT>::toUTF8(s, 0, n), to_byte_size(pos)));
    }
    template <typename _CharT>
    size_type rfind(const _CharT * s, size_type pos = 0) const noexcept {
        return to_utf32_size(_base.rfind(_Convert<_CharT>::toUTF8(s), to_byte_size(pos)));
    }
    template <typename _CharT>
    size_type rfind(_CharT c, size_type pos = 0) const noexcept {
        return to_utf32_size(_base.rfind(_Convert<_CharT>::toUTF8(c), to_byte_size(pos)));
    }
    
    template <class _ForwardIterator1, class _ForwardIterator2, class _BinaryPredicate>
    _ForwardIterator1
    find_first_of(_ForwardIterator1 __first1, _ForwardIterator1 __last1,
                  _ForwardIterator2 __first2, _ForwardIterator2 __last2, _BinaryPredicate __pred) const noexcept
    {
        for (; __first1 != __last1; ++__first1)
            for (_ForwardIterator2 __j = __first2; __j != __last2; ++__j)
                if (__pred(*__first1, *__j))
                    return __first1;
        return __last1;
    }
    
    template <class _Traits>
    struct _LIBCPP_HIDDEN __traits_eq
    {
        typedef typename _Traits::char_type char_type;
        _LIBCPP_INLINE_VISIBILITY
        bool operator()(const char_type& __x, const char_type& __y) noexcept {
            return _Traits::eq(__x, __y);
        }
    };
    
    size_type find_first_of(const string& str, size_type pos=0) const noexcept {
        auto __r = find_first_of(begin()+pos, end(), str.begin(), str.end(), __traits_eq<traits_type>());
        if ( __r == end() )
            return npos;
        return __r - begin();
    }
    size_type find_first_of(const __base& str, size_type pos=0) const noexcept {
        auto __r = find_first_of(begin()+pos, end(), const_iterator(str.begin()), const_iterator(str.end()), __traits_eq<traits_type>());
        if ( __r == end() )
            return npos;
        return __r - begin();
    }
    template <typename _CharT>
    size_type find_first_of(const _CharT * s, size_type pos, size_type n) const noexcept {
        return find_first_of(_Convert<_CharT>::toUTF8(s, 0, n), pos);
    }
    template <typename _CharT>
    size_type find_first_of(const _CharT * s, size_type pos = 0) const noexcept {
        return find_first_of(_Convert<_CharT>::toUTF8(s), pos);
    }
    template <typename _CharT>
    size_type find_first_of(_CharT c, size_type pos = 0) const noexcept {
        auto __r = std::find_first_of(begin()+pos, end(), &c, ((&c) + sizeof(_CharT)));
        if ( __r == end() )
            return npos;
        return __r - begin();
    }
    
    size_type find_last_of(const string& str, size_type pos=0) const noexcept {
        size_type __sz = size();
        if ( pos < __sz )
            ++pos;
        else
            pos = __sz;
        const_iterator __p = begin();
        for ( const_iterator __ps = __p + pos; __ps != __p; )
        {
            size_type __r = str.find(*--__ps);
            if ( __r != npos )
                return __ps - begin();
        }
        return npos;
    }
    size_type find_last_of(const __base& str, size_type pos=0) const noexcept {
        size_type __sz = size();
        if ( pos < __sz )
            ++pos;
        else
            pos = __sz;
        const_iterator __p = begin();
        for ( const_iterator __ps = __p + pos; __ps != __p; )
        {
            size_type __r = str.find((--__ps).utf8char());
            if ( __r != npos )
                return __ps - begin();
        }
        return npos;
    }
    template <typename _CharT>
    size_type find_last_of(const _CharT * s, size_type pos, size_type n) const noexcept {
        return find_last_of(_Convert<_CharT>::toUTF8(s, 0, n), pos);
    }
    template <typename _CharT>
    size_type find_last_of(const _CharT * s, size_type pos = 0) const noexcept {
        return find_last_of(_Convert<_CharT>::toUTF8(s), pos);
    }
    template <typename _CharT>
    size_type find_last_of(_CharT c, size_type pos = 0) const noexcept {
        return rfind(c, pos);
    }
    
    size_type find_first_not_of(const string& str, size_type pos=0) const noexcept {
        size_type __sz = size();
        if ( pos < __sz )
        {
            const_iterator __p = begin();
            const_iterator __pe = end();
            for ( const_iterator __ps = __p + pos; __ps != __pe; ++__ps )
                if ( str.find(*__ps) == npos )
                    return static_cast<size_type>(__ps - __p);
        }
        return npos;
    }
    size_type find_first_not_of(const __base& str, size_type pos=0) const noexcept {
        size_type __sz = size();
        if ( pos < __sz )
        {
            const_iterator __p = begin();
            const_iterator __pe = end();
            for ( const_iterator __ps = __p + pos; __ps != __pe; ++__ps )
                if ( str.find(__ps.utf8char()) == npos )
                    return static_cast<size_type>(__ps - __p);
        }
        return npos;
    }
    template <typename _CharT>
    size_type find_first_not_of(const _CharT * s, size_type pos, size_type n) const noexcept {
        return find_first_not_of(_Convert<_CharT>::toUTF8(s, 0, n), pos);
    }
    template <typename _CharT>
    size_type find_first_not_of(const _CharT * s, size_type pos = 0) const noexcept {
        return find_first_not_of(_Convert<_CharT>::toUTF8(s), pos);
    }
    template <typename _CharT>
    size_type find_first_not_of(_CharT c, size_type pos = 0) const noexcept {
        return find_first_not_of(_Convert<_CharT>::toUTF8(c), pos);
    }
    
    size_type find_last_not_of(const string& str, size_type pos=0) const noexcept {
        size_type __sz = size();
        if ( pos < __sz )
            ++pos;
        else
            pos = __sz;
        const_iterator __p = begin();
        for ( const_iterator __ps = __p + pos; __ps != __p; )
            if ( str.find(*--__ps) == npos )
                return static_cast<size_type>(__ps - __p);
        return npos;
    }
    size_type find_last_not_of(const __base& str, size_type pos=0) const noexcept {
        size_type __sz = size();
        if ( pos < __sz )
            ++pos;
        else
            pos = __sz;
        const_iterator __p = begin();
        for ( const_iterator __ps = __p + pos; __ps != __p; )
            if ( str.find((--__ps).utf8char()) == npos )
                return static_cast<size_type>(__ps - __p);
        return npos;
    }
    template <typename _CharT>
    size_type find_last_not_of(const _CharT * s, size_type pos, size_type n) const noexcept {
        return find_last_not_of(_Convert<_CharT>::toUTF8(s, 0, n), pos);
    }
    template <typename _CharT>
    size_type find_last_not_of(const _CharT * s, size_type pos = 0) const noexcept {
        return find_last_not_of(_Convert<_CharT>::toUTF8(s), pos);
    }
    template <typename _CharT>
    size_type find_last_not_of(_CharT c, size_type pos = 0) const noexcept {
        return find_last_not_of(_Convert<_CharT>::toUTF8(c), pos);
    }
    
#if 0
#pragma mark - Comparisons
#endif
    
    int compare(const string& str) const noexcept {
        return _base.compare(str._base);
    }
    int compare(size_type pos1, size_type n1, const string& str) const {
        return _base.compare(to_byte_size(pos1), to_byte_size(pos1, pos1+n1), str._base);
    }
    int compare(size_type pos1, size_type n1, const string& str,
                size_type pos2, size_type n2) const {
        return _base.compare(to_byte_size(pos1), to_byte_size(pos1, pos1+n1),
                             str._base, str.to_byte_size(pos2), str.to_byte_size(pos2, pos2+n2));
    }
    
    // there exist specializations for char32_t
    template <typename _CharT>
    int compare(const _CharT * s) const noexcept {
        auto str(_Convert<_CharT>::toUTF8(s));
        return _base.compare(str);
    }
    template <typename _CharT>
    int compare(size_type pos1, size_type n1, const _CharT * s) const {
        return _base.compare(to_byte_size(pos1), to_byte_size(pos1, pos1+n1), _Convert<_CharT>::toUTF8(s));
    }
    template <typename _CharT>
    int compare(size_type pos1, size_type n1, const _CharT * s, size_type n2) const {
        return _base.compare(to_byte_size(pos1), to_byte_size(pos1, pos1+n1), _Convert<_CharT>::toUTF8(s, 0, n2));
    }
    template <typename _CharT>
    int compare(const std::basic_string<_CharT> & s) const noexcept {
        return _base.compare(_Convert<_CharT>::toUTF8(s));
    }
    template <typename _CharT>
    int compare(size_type pos1, size_type n1, const std::basic_string<_CharT>& str) const {
        return _base.compare(to_byte_size(pos1), to_byte_size(pos1, pos1+n1), _Convert<_CharT>::toUTF8(str));
    }
    template <typename _CharT>
    int compare(size_type pos1, size_type n1, const std::basic_string<_CharT>& str,
                size_type pos2, size_type n2) const {
        return _base.compare(to_byte_size(pos1), to_byte_size(pos1, pos1+n1), _Convert<_CharT>::toUTF8(str, pos2, n2));
    }
    
    bool operator == (const string & str) const noexcept { return compare(str) == 0; }
    bool operator != (const string & str) const noexcept { return compare(str) != 0; }
    bool operator > (const string & str) const noexcept { return compare(str) > 0; }
    bool operator >= (const string & str) const noexcept { return compare(str) >= 0; }
    bool operator < (const string & str) const noexcept { return compare(str) < 0; }
    bool operator <= (const string & str) const noexcept { return compare(str) <= 0; }
    
    template <typename _CharT>
    bool operator == (const _CharT * str) const noexcept { return compare<_CharT>(str) == 0; }
    template <typename _CharT>
    bool operator != (const _CharT * str) const noexcept { return compare<_CharT>(str) != 0; }
    template <typename _CharT>
    bool operator > (const _CharT * str) const noexcept { return compare<_CharT>(str) > 0; }
    template <typename _CharT>
    bool operator >= (const _CharT * str) const noexcept { return compare<_CharT>(str) >= 0; }
    template <typename _CharT>
    bool operator < (const _CharT * str) const noexcept { return compare<_CharT>(str) < 0; }
    template <typename _CharT>
    bool operator <= (const _CharT * str) const noexcept { return compare<_CharT>(str) <= 0; }
    
    bool __invariants() const { return _base.__invariants(); }
    
protected:
    __base      _base;
    
    void throw_unless_insertable(const __base &s, size_type b, size_type e) const throw (InvalidUTF8Sequence);
    void throw_unless_insertable(const char * s, size_type b, size_type e) const throw (InvalidUTF8Sequence);
    void throw_unless_insertable(const xmlChar * s, size_type b, size_type e) const throw (InvalidUTF8Sequence);
    
    __base::size_type to_byte_size(size_type __n) const noexcept;
    __base::size_type to_byte_size(size_type __b, size_type __e) const noexcept;
    size_type to_utf32_size(__base::size_type __n) const noexcept;
    size_type to_utf32_size(__base::size_type __b, __base::size_type __e) const noexcept;
    
    static inline constexpr __base::const_pointer _bchar(const xmlChar * c) noexcept { return (__base::const_pointer)(c); }
    static inline constexpr __base::pointer _bchar(xmlChar * c) noexcept { return (__base::pointer)(c); }
    
    template <class _CharT>
    class _Convert {
        typedef std::wstring_convert<std::codecvt_utf8<_CharT>, _CharT > _cvt;
    public:
        typedef typename _cvt::byte_string byte_string;
        typedef typename _cvt::wide_string wide_string;
        
        static byte_string toUTF8(const _CharT * p, size_type pos=0, size_type n=npos) {
            if ( n == npos )
                return _cvt().to_bytes(p+pos);
            return _cvt().to_bytes(p+pos, p+pos+n);
        }
        static byte_string toUTF8(const wide_string & s, size_type pos=0, size_type n=npos) {
            return _cvt().to_bytes(s.substr(pos,n));
        }
        static byte_string toUTF8(_CharT c, size_type n=1) {
            byte_string s = _cvt().to_bytes(c);
            if ( n == 1 )
                return s;
            else if ( s.length() == 1 )
                return byte_string(n, s[0]);
            byte_string r;
            r.reserve(n*s.size());
            for ( size_type i = 0; i < n; i++ )
                r.append(s);
            return r;
        }
        static wide_string fromUTF8(const char * utf8, size_type pos=0, size_type n=npos) {
            if ( n == npos )
                return _cvt().from_bytes(utf8+pos);
            return _cvt().from_bytes(utf8+pos, utf8+pos+n);
        }
        static wide_string fromUTF8(const byte_string & utf8, size_type pos=0, size_type n=npos) {
            return _cvt().from_bytes(utf8.substr(pos, n));
        }
    };
};

#if 0
#pragma mark - Template Specializations
#endif

template<>
class string:: _Convert<char> {
public:
    typedef std::string byte_string;
    typedef std::string wide_string;
    static byte_string toUTF8(const char *p, size_type pos=0, size_type n=npos) {
        if ( n == std::string::npos )
            return std::string(p+pos);
        return std::string(p+pos, n);
    }
    static byte_string toUTF8(const wide_string & s, size_type pos=0, size_type n=npos) {
        return s.substr(pos, n);
    }
    static byte_string toUTF8(char c, size_type n=1) {
        return std::string(n, c);
    }
    static wide_string fromUTF8(const char * utf8, size_type pos=0, size_type n=npos) {
        if ( n == std::string::npos )
            return std::string(utf8+pos);
        return std::string(utf8+pos, n);
    }
    static wide_string fromUTF8(const byte_string & s, size_type pos=0, size_type n=npos) {
        return s.substr(pos, n);
    }
};

template <>
class string::_Convert<xmlChar> {
public:
    typedef std::string byte_string;
    typedef std::string wide_string;
    static byte_string toUTF8(const xmlChar *p, size_type pos=0, size_type n=npos) {
        if ( n == std::string::npos )
            return std::string(reinterpret_cast<const char*>(p)+pos);
        return std::string(reinterpret_cast<const char*>(p)+pos, n);
    }
    static byte_string toUTF8(const wide_string & s, size_type pos=0, size_type n=npos) {
        return s.substr(pos, n);
    }
    static byte_string toUTF8(xmlChar c, size_type n=1) {
        return std::string(n, static_cast<char>(c));
    }
    static wide_string fromUTF8(const xmlChar * utf8, size_type pos=0, size_type n=npos) {
        if ( n == std::string::npos )
            return std::string(reinterpret_cast<const char*>(utf8)+pos);
        return std::string(reinterpret_cast<const char*>(utf8)+pos, n);
    }
    static wide_string fromUTF8(const byte_string & s, size_type pos=0, size_type n=npos) {
        return s.substr(pos, n);
    }
};

#if 0
#pragma mark - Helpers
#endif

// C++11 lets us define new literal types, so lets have "something"_xc be an xmlChar *, eh?
// Sadly, we can't define prefix forms. Boo...
constexpr inline const xmlChar * operator "" _xc(const char * __s, size_t __n) noexcept {
    return (const xmlChar *)__s;
}
static inline constexpr const xmlChar * xml(const char * __s) noexcept {
    return (const xmlChar*)(__s);
}

static inline const xmlChar * xmlChars(const std::string & str) {
    return reinterpret_cast<const xmlChar*>(str.c_str());
}
static inline string xmlString(const char * str) {
    return string(reinterpret_cast<const xmlChar*>(str));
}
static inline string xmlString(const std::string & str) {
    return string(reinterpret_cast<const xmlChar*>(str.c_str()));
}
static inline std::string asciiString(const xmlChar * s) {
    return std::string(reinterpret_cast<const char *>(s));
}
static inline constexpr const char * ascii(const xmlChar * __x) {
    return (const char *)__x;
}

////////////////////////////////////////////////////////////////////////////
// some helpers

inline string operator + (const string & lhs, const string & rhs) {
    string s(lhs);
    s.append(rhs);
    return std::move(s);
}
inline string operator + (const string & lhs, const std::string & rhs) {
    string s(lhs);
    s.append(rhs);
    return std::move(s);
}
inline string operator * (const string & lhs, const string::value_type * rhs) {
    string s(lhs);
    s.append(rhs);
    return std::move(s);
}
inline string operator * (const string & lhs, string::value_type rhs) {
    string s(lhs);
    s.append(1, rhs);
    return std::move(s);
}
inline string operator + (const string & lhs, const char * rhs) {
    string s(lhs);
    s.append(rhs);
    return std::move(s);
}
inline string operator + (const string & lhs, char rhs) {
    string s(lhs);
    s.append(1, rhs);
    return std::move(s);
}
inline string operator + (const string & lhs, const xmlChar * rhs) {
    string s(lhs);
    s.append(rhs);
    return std::move(s);
}
inline string operator + (const string & lhs, xmlChar rhs) {
    string s(lhs);
    s.append(1, rhs);
    return std::move(s);
}

// to std::string (UTF-8, signed char)
inline std::string operator + (std::string& lhs, const xmlChar rhs) {
    return std::operator+(lhs, static_cast<const char>(rhs));
}
inline std::string operator + (std::string& lhs, const xmlChar *rhs) {
    return std::operator+(lhs, reinterpret_cast<const char*>(rhs));
}
inline std::string operator + (std::string &lhs, const string &rhs) {
    return std::operator+(lhs, reinterpret_cast<const char*>(rhs.c_str()));
}

template <class _CharT, class _Traits>
inline std::basic_ostream<_CharT, _Traits>&
operator<<(std::basic_ostream<_CharT, _Traits>& __os, const string& __str) {
    return __os << __str.stl_str();
}

template <class T>
inline bool string::_iterator<T>::operator<(const _iterator<T> &o) const {
    return _base < o._base;
}
template <class T>
inline bool string::_iterator<T>::operator<=(const _iterator<T> &o) const {
    return _base <= o._base;
}

EPUB3_XML_END_NAMESPACE

#endif /* defined(__ePub3_xml_string__) */
