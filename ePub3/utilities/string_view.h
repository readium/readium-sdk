//
//  string_view.h
//  ePub3
//
//  Created by Jim Dovey on 11/7/2013.
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

#ifndef ePub3_string_view_h
#define ePub3_string_view_h

#include <type_traits>
#include <string>
#include <limits>
#include <iostream>
#include <algorithm>
#include "__strhash.h"

#ifdef min
# undef min
#endif
#ifdef max
# undef max
#endif

/*
 
        string_view synopsis
 
    namespace std {
      // [basic.string.view], basic_string_view:
      template<class charT, class traits = char_traits<charT>>
          class basic_string_view;

      // [string.view.comparison], non-member basic_string_view comparison functions
      template<class charT, class traits>
      bool operator==(basic_string_view<charT, traits> x, basic_string_view<charT, traits> y) noexcept;
      template<class charT, class traits>
      bool operator!=(basic_string_view<charT, traits> x, basic_string_view<charT, traits> y) noexcept;
      template<class charT, class traits>
      bool operator< (basic_string_view<charT, traits> x, basic_string_view<charT, traits> y) noexcept;
      template<class charT, class traits>
      bool operator> (basic_string_view<charT, traits> x, basic_string_view<charT, traits> y) noexcept;
      template<class charT, class traits>
      bool operator<=(basic_string_view<charT, traits> x, basic_string_view<charT, traits> y) noexcept;
      template<class charT, class traits>
      bool operator>=(basic_string_view<charT, traits> x, basic_string_view<charT, traits> y) noexcept;
      // [string.view.comparison], sufficient additional overloads of comparison functions

      // [string.view.nonmem], other non-member basic_string_view functions
      template<class charT, class traits = char_traits<charT>,
               class Allocator = allocator<charT> >
        basic_string<charT, traits, Allocator> to_string(
          basic_string_view<charT, traits>,
          const Allocator& a = Allocator());

      template<class charT, class traits>
        basic_ostream<charT, traits>&
          operator<<(basic_ostream<charT, traits>& os,
                     basic_string_view<charT,traits> str);

      // basic_string_view typedef names
      typedef basic_string_view<char> string_view;
      typedef basic_string_view<char16_t> u16string_view;
      typedef basic_string_view<char32_t> u32string_view;
      typedef basic_string_view<wchar_t> wstring_view;

      // [string.view.hash], hash support:
      template <class T> struct hash;
      template <> struct hash<string_view>;
      template <> struct hash<u16string_view>;
      template <> struct hash<u32string_view>;
      template <> struct hash<wstring_view>;
    }

    namespace std {
      template<class charT, class traits = char_traits<charT>>
      class basic_string_view {
        public:
        // types
        typedef traits traits_type;
        typedef charT value_type;
        typedef const charT* pointer;
        typedef const charT* const_pointer;
        typedef const charT& reference;
        typedef const charT& const_reference;
        typedef implementation-defined const_iterator; // See [string.view.iterators]
        typedef const_iterator iterator;  // [Footnote: Because basic_string_view refers to a constant sequence, iterator and const_iterator are the same type. --end footnote]
        typedef reverse_iterator<const_iterator> const_reverse_iterator;
        typedef const_reverse_iterator reverse_iterator;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;
        static constexpr size_type npos = size_type(-1);

        // [string.view.cons], construct/copy
        constexpr basic_string_view() noexcept;
        constexpr basic_string_view(const basic_string_view&) noexcept = default;
        basic_string_view& operator=(const basic_string_view&) noexcept = default;
        template<class Allocator>
        basic_string_view(const basic_string<charT, traits, Allocator>& str) noexcept;
        basic_string_view(const charT* str);
        constexpr basic_string_view(const charT* str, size_type len);
        
        // [string.view.iterators], iterators
        constexpr const_iterator begin() const noexcept;
        constexpr const_iterator end() const noexcept;
        constexpr const_iterator cbegin() const noexcept;
        constexpr const_iterator cend() const noexcept;
        
        const_reverse_iterator rbegin() const noexcept;
        const_reverse_iterator rend() const noexcept;
        const_reverse_iterator crbegin() const noexcept;
        const_reverse_iterator crend() const noexcept;

        // [string.view.capacity], capacity
        constexpr size_type size() const noexcept;
        constexpr size_type length() const noexcept;
        constexpr size_type max_size() const noexcept;
        constexpr bool empty() const noexcept;

        // [string.view.access], element access
        constexpr const charT& operator[](size_type pos) const;
        constexpr const charT& at(size_type pos) const;
        constexpr const charT& front() const;
        constexpr const charT& back() const;
        constexpr const charT* data() const noexcept;

        // [string.view.modifiers], modifiers:
        void clear() noexcept;
        void remove_prefix(size_type n);
        void remove_suffix(size_type n);
        void swap(basic_string_view& s) noexcept;

        // [string.view.ops], string operations:
        template<class Allocator>
        explicit operator basic_string<charT, traits, Allocator>() const;

        size_type copy(charT* s, size_type n, size_type pos = 0) const;

        constexpr basic_string_view substr(size_type pos=0, size_type n=npos) const;
        int compare(basic_string_view s) const noexcept;
        int compare(size_type pos1, size_type n1, basic_string_view s) const;
        int compare(size_type pos1, size_type n1,
                    basic_string_view s, size_type pos2, size_type n2) const;
        int compare(const charT* s) const;
        int compare(size_type pos1, size_type n1, const charT* s) const;
        int compare(size_type pos1, size_type n1,
                    const charT* s, size_type n2) const;
        size_type find(basic_string_view s, size_type pos=0) const noexcept;
        size_type find(charT c, size_type pos=0) const noexcept;
        size_type find(const charT* s, size_type pos, size_type n) const;
        size_type find(const charT* s, size_type pos=0) const;
        size_type rfind(basic_string_view s, size_type pos=npos) const noexcept;
        size_type rfind(charT c, size_type pos=npos) const noexcept;
        size_type rfind(const charT* s, size_type pos, size_type n) const;
        size_type rfind(const charT* s, size_type pos=npos) const;
        size_type find_first_of(basic_string_view s, size_type pos=0) const noexcept;
        size_type find_first_of(charT c, size_type pos=0) const noexcept;
        size_type find_first_of(const charT* s, size_type pos, size_type n) const;
        size_type find_first_of(const charT* s, size_type pos=0) const;
        size_type find_last_of(basic_string_view s, size_type pos=npos) const noexcept;
        size_type find_last_of(charT c, size_type pos=npos) const noexcept;
        size_type find_last_of(const charT* s, size_type pos, size_type n) const;
        size_type find_last_of(const charT* s, size_type pos=npos) const;
        size_type find_first_not_of(basic_string_view s, size_type pos=0) const noexcept;
        size_type find_first_not_of(charT c, size_type pos=0) const noexcept;
        size_type find_first_not_of(const charT* s, size_type pos, size_type n) const;
        size_type find_first_not_of(const charT* s, size_type pos=0) const;
        size_type find_last_not_of(basic_string_view s, size_type pos=npos) const noexcept;
        size_type find_last_not_of(charT c, size_type pos=npos) const noexcept;
        size_type find_last_not_of(const charT* s, size_type pos, size_type n) const;
        size_type find_last_not_of(const charT* s, size_type pos=npos) const;
      };
    }
 
 */

EPUB3_BEGIN_NAMESPACE

// [basic.string.view], basic_string_view:
template<class _CharT, class _Traits>
class basic_string_view;

// [string.view.comparison], non-member basic_string_view comparison functions
template<class charT, class traits>
bool operator==(basic_string_view<charT, traits> x, basic_string_view<charT, traits> y) _NOEXCEPT;
template<class charT, class traits>
bool operator!=(basic_string_view<charT, traits> x, basic_string_view<charT, traits> y) _NOEXCEPT;
template<class charT, class traits>
bool operator< (basic_string_view<charT, traits> x, basic_string_view<charT, traits> y) _NOEXCEPT;
template<class charT, class traits>
bool operator> (basic_string_view<charT, traits> x, basic_string_view<charT, traits> y) _NOEXCEPT;
template<class charT, class traits>
bool operator<=(basic_string_view<charT, traits> x, basic_string_view<charT, traits> y) _NOEXCEPT;
template<class charT, class traits>
bool operator>=(basic_string_view<charT, traits> x, basic_string_view<charT, traits> y) _NOEXCEPT;
// [string.view.comparison], sufficient additional overloads of comparison functions

// [string.view.nonmem], other non-member basic_string_view functions
template <class _CharT, class _Traits = std::char_traits<_CharT>, class _Allocator = std::allocator<_CharT>>
    std::basic_string<_CharT, _Traits, _Allocator>
    to_string(basic_string_view<_CharT, _Traits>, const _Allocator& a = _Allocator());

template <class _CharT, class _Traits>
    std::basic_ostream<_CharT, _Traits>&
    operator<<(std::basic_ostream<_CharT, _Traits>& os,
               basic_string_view<_CharT, _Traits> str);

// basic_string_view typedef names
typedef basic_string_view<char, std::char_traits<char>>         string_view;
typedef basic_string_view<char16_t, std::char_traits<char16_t>> u16string_view;
typedef basic_string_view<char32_t, std::char_traits<char32_t>> u32string_view;
typedef basic_string_view<wchar_t, std::char_traits<wchar_t>>   wstring_view;

// [string.view.hash], hash support:
//template <class T> struct hash;
//template <> struct hash<string_view>;
//template <> struct hash<u16string_view>;
//template <> struct hash<u32string_view>;
//template <> struct hash<wstring_view>;

template <class _CharT, class _Traits> struct hash;

template<class _CharT, class _Traits = std::char_traits<_CharT>>
class basic_string_view {
private:
    const _CharT*   __data_;
    size_t          __size_;
    
public:
    // types
    typedef _Traits                                 traits_type;
    typedef _CharT                                  value_type;
    typedef const _CharT*                           pointer;
    typedef const _CharT*                           const_pointer;
    typedef const _CharT&                           reference;
    typedef const _CharT&                           const_reference;
    typedef const_pointer                           const_iterator;
    typedef const_iterator                          iterator;
    // Because basic_string_view refers to a constant sequence, iterator and const_iterator are the same type.
    
    typedef std::reverse_iterator<const_iterator>   const_reverse_iterator;
    typedef const_reverse_iterator                  reverse_iterator;
    typedef size_t                                  size_type;
    typedef ptrdiff_t                               difference_type;
    
    static CONSTEXPR const size_type npos = size_type(-1);
    
public:
    // [string.view.cons], construct/copy
    CONSTEXPR FORCE_INLINE
    basic_string_view() _NOEXCEPT
        : __data_(nullptr), __size_(0)
        {}
#if EPUB_COMPILER_SUPPORTS(CXX_DEFAULTED_FUNCTIONS)
    CONSTEXPR
    basic_string_view(const basic_string_view&) _NOEXCEPT = default;
    
    basic_string_view& operator=(const basic_string_view&) _NOEXCEPT = default;
#else
    CONSTEXPR FORCE_INLINE
    basic_string_view(const basic_string_view& __o) _NOEXCEPT
        : __data_(__o.__data_), __size_(__o.__size_)
        {}
    
    FORCE_INLINE
    basic_string_view& operator=(const basic_string_view&) _NOEXCEPT
        {
            __data_ = __o.__data_;
            __size_ = __o.__size_;
            return *this;
        }
#endif
    
    template<class _Allocator>
    FORCE_INLINE
    basic_string_view(const std::basic_string<_CharT, _Traits, _Allocator>& str) _NOEXCEPT
        : __data_(str.data()), __size_(str.length())
        {}
    
    FORCE_INLINE
    basic_string_view(const_pointer str)
        : __data_(str), __size_(traits_type::length(str))
        {}
    
    CONSTEXPR FORCE_INLINE
    basic_string_view(const_pointer str, size_type len)
        : __data_(str), __size_(len)
        {}
    
    // [string.view.iterators], iterators
    CONSTEXPR const_iterator begin() const _NOEXCEPT    { return __data_; }
    CONSTEXPR const_iterator end() const _NOEXCEPT      { return __data_ + __size_; }
    CONSTEXPR const_iterator cbegin() const _NOEXCEPT   { return __data_; }
    CONSTEXPR const_iterator cend() const _NOEXCEPT     { return __data_ + __size_; }
    
    const_reverse_iterator rbegin() const _NOEXCEPT     { return reverse_iterator(end()); }
    const_reverse_iterator rend() const _NOEXCEPT       { return reverse_iterator(begin()); }
    const_reverse_iterator crbegin() const _NOEXCEPT    { return reverse_iterator(cend()); }
    const_reverse_iterator crend() const _NOEXCEPT      { return reverse_iterator(cbegin()); }
    
    // [string.view.capacity], capacity
    CONSTEXPR size_type size() const _NOEXCEPT          { return __size_; }
    CONSTEXPR size_type length() const _NOEXCEPT        { return __size_; }
    CONSTEXPR size_type max_size() const _NOEXCEPT      { return std::allocator<value_type>::max_size(); }
    CONSTEXPR bool empty() const _NOEXCEPT              { return __size_ == 0; }
    
    // [string.view.access], element access
    CONSTEXPR const_reference operator[](size_type pos) const   { return __data_[pos]; }
    CONSTEXPR const_reference at(size_type pos) const;
    CONSTEXPR const_reference front() const                     { return __data_[0]; }
    CONSTEXPR const_reference back() const                      { return __data_[__size_-1]; }
    CONSTEXPR const_pointer data() const _NOEXCEPT              { return __data_; }
    
    // [string.view.modifiers], modifiers:
    void clear() _NOEXCEPT                                      { __data_ = nullptr; __size_ = 0; }
    void remove_prefix(size_type n);
    void remove_suffix(size_type n);
    void swap(basic_string_view& s) _NOEXCEPT;
    
    // [string.view.ops], string operations:
    template<class _Allocator>
    explicit operator std::basic_string<_CharT, _Traits, _Allocator>() const
        {
            return std::basic_string<_CharT, _Traits, _Allocator>(begin(), end());
        }
    
    size_type copy(_CharT* s, size_type n, size_type pos = 0) const;
    
    CONSTEXPR basic_string_view substr(size_type pos=0, size_type n=npos) const;
    
    int compare(basic_string_view s) const _NOEXCEPT
        {
            int r = traits_type::compare(__data_, s.data(), std::min(__size_, s.size()));
            if (r == 0) {
                r = int(__size_) - int(s.size());
            }
            return r;
        }
    int compare(size_type pos1, size_type n1, basic_string_view s) const
        {
            return substr(pos1, n1).compare(s);
        }
    int compare(size_type pos1, size_type n1,
                basic_string_view s, size_type pos2, size_type n2) const
        {
            return substr(pos1, n1).compare(s.substr(pos2, n2));
        }
    int compare(const_pointer s) const
        {
            return compare(basic_string_view(s));
        }
    int compare(size_type pos1, size_type n1, const pointer s) const
        {
            return compare(pos1, n1, basic_string_view(s));
        }
    int compare(size_type pos1, size_type n1,
                const_pointer s, size_type n2) const
        {
            return compare(pos1, n1, basic_string_view(s, n2));
        }
    
    size_type find(basic_string_view s, size_type pos=0) const _NOEXCEPT;
    size_type find(value_type c, size_type pos=0) const _NOEXCEPT;
    size_type find(const_pointer s, size_type pos, size_type n) const;
    size_type find(const_pointer s, size_type pos=0) const;
    
    size_type rfind(basic_string_view s, size_type pos=npos) const _NOEXCEPT;
    size_type rfind(value_type c, size_type pos=npos) const _NOEXCEPT;
    size_type rfind(const_pointer s, size_type pos, size_type n) const;
    size_type rfind(const_pointer s, size_type pos=npos) const;
    
    size_type find_first_of(basic_string_view s, size_type pos=0) const _NOEXCEPT;
    size_type find_first_of(value_type c, size_type pos=0) const _NOEXCEPT;
    size_type find_first_of(const_pointer s, size_type pos, size_type n) const;
    size_type find_first_of(const_pointer s, size_type pos=0) const;
    
    size_type find_last_of(basic_string_view s, size_type pos=npos) const _NOEXCEPT;
    size_type find_last_of(value_type c, size_type pos=npos) const _NOEXCEPT;
    size_type find_last_of(const_pointer s, size_type pos, size_type n) const;
    size_type find_last_of(const_pointer s, size_type pos=npos) const;
    
    size_type find_first_not_of(basic_string_view s, size_type pos=0) const _NOEXCEPT;
    size_type find_first_not_of(value_type c, size_type pos=0) const _NOEXCEPT;
    size_type find_first_not_of(const_pointer s, size_type pos, size_type n) const;
    size_type find_first_not_of(const_pointer s, size_type pos=0) const;
    
    size_type find_last_not_of(basic_string_view s, size_type pos=npos) const _NOEXCEPT;
    size_type find_last_not_of(value_type c, size_type pos=npos) const _NOEXCEPT;
    size_type find_last_not_of(const_pointer s, size_type pos, size_type n) const;
    size_type find_last_not_of(const_pointer s, size_type pos=npos) const;
    
};

template <class _CharT, class _Traits>
CONSTEXPR const _CharT& basic_string_view<_CharT, _Traits>::at(size_type pos) const
{
    return ((pos < __size_)
                ? __data_[pos]
                : throw std::out_of_range("basic_string_view::at"));
}

template <class _CharT, class _Traits>
void basic_string_view<_CharT, _Traits>::remove_prefix(size_type n)
{
    if (n <= __size_)
    {
        __data_ += n;
        __size_ -= n;
    }
}
template <class _CharT, class _Traits>
void basic_string_view<_CharT, _Traits>::remove_suffix(size_type n)
{
    if (n <= __size_)
    {
        __size_ -= n;
    }
}
template <class _CharT, class _Traits>
void basic_string_view<_CharT, _Traits>::swap(basic_string_view<_CharT, _Traits> &s) _NOEXCEPT
{
    std::swap(__data_, s.__data_);
    std::swap(__size_, s.__size_);
}
template <class _CharT, class _Traits>
typename basic_string_view<_CharT, _Traits>::size_type
basic_string_view<_CharT, _Traits>::copy(_CharT *s, size_type n, size_type pos) const
{
    if (pos > __size_)
        throw std::out_of_range("basic_string_view::copy");
    std::copy_n(begin()+pos, std::min(n, __size_ - pos), s);
}
template <class _CharT, class _Traits>
CONSTEXPR
basic_string_view<_CharT, _Traits>
basic_string_view<_CharT, _Traits>::substr(size_type pos, size_type n) const
{
    return ((pos > __size_)
                ? throw std::out_of_range("basic_string_view::substr")
                : basic_string_view(__data_+pos, std::min(n, __size_-pos)));
}

template <class _Traits>
struct _LIBCPP_HIDDEN __traits_eq
{
    typedef typename _Traits::char_type char_type;
    
    FORCE_INLINE
    bool operator()(const char_type& __a, const char_type& __b) _NOEXCEPT {
        return _Traits::eq(__a, __b);
    }
};

template <class _CharT, class _Traits>
typename basic_string_view<_CharT, _Traits>::size_type
basic_string_view<_CharT, _Traits>::find(const_pointer __s, size_type __pos, size_type __n) const
{
    size_type __sz = size();
    if (__pos > __sz || __sz - __pos < __n)
        return npos;
    if (__n == 0)
        return __pos;
    const_pointer __p = data();
    const_pointer __r = std::search(__p + __pos, __p + __sz, __s, __s + __n,
                                      __traits_eq<traits_type>());
    if (__r == __p + __sz)
        return npos;
    return static_cast<size_type>(__r - __p);
}
template <class _CharT, class _Traits>
typename basic_string_view<_CharT, _Traits>::size_type
basic_string_view<_CharT, _Traits>::find(basic_string_view __s, size_type __pos) const _NOEXCEPT
{
    return find(__s.data(), __pos, __s.size());
}
template <class _CharT, class _Traits>
typename basic_string_view<_CharT, _Traits>::size_type
basic_string_view<_CharT, _Traits>::find(value_type __c, size_type __pos) const _NOEXCEPT
{
    return traits_type::find(__data_+__pos, __size_-__pos, __c);
}
template <class _CharT, class _Traits>
typename basic_string_view<_CharT, _Traits>::size_type
basic_string_view<_CharT, _Traits>::find(const_pointer __s, size_type __pos) const
{
    return find(__s, __pos, traits_type::length(__s));
}
template <class _CharT, class _Traits>
typename basic_string_view<_CharT, _Traits>::size_type
basic_string_view<_CharT, _Traits>::rfind(const_pointer __s, size_type __pos, size_type __n) const
{
    size_type __sz = size();
    __pos = std::min(__pos, __sz);
    if (__n < __sz - __pos)
        __pos += __n;
    else
        __pos = __sz;
    const_pointer __p = data();
    const_pointer __r = std::find_end(__p, __p + __pos, __s, __s + __n,
                                      __traits_eq<traits_type>());
    if (__n > 0 && __r == __p + __pos)
        return npos;
    return static_cast<size_type>(__r - __p);
}
template <class _CharT, class _Traits>
typename basic_string_view<_CharT, _Traits>::size_type
basic_string_view<_CharT, _Traits>::rfind(basic_string_view __s, size_type __pos) const _NOEXCEPT
{
    return rfind(__s.data(), __pos, __s.size());
}
template <class _CharT, class _Traits>
typename basic_string_view<_CharT, _Traits>::size_type
basic_string_view<_CharT, _Traits>::rfind(value_type __c, size_type __pos) const _NOEXCEPT
{
    size_type __sz = size();
    if (__sz)
    {
        if (__pos < __sz)
            ++__pos;
        else
            __pos = __sz;
            const_pointer __p = data();
            for (const_pointer __ps = __p + __pos; __ps != __p;)
            {
                if (traits_type::eq(*--__ps, __c))
                    return static_cast<size_type>(__ps - __p);
            }
    }
    return npos;
}
template <class _CharT, class _Traits>
typename basic_string_view<_CharT, _Traits>::size_type
basic_string_view<_CharT, _Traits>::rfind(const_pointer __s, size_type __pos) const
{
    return rfind(__s, __pos, traits_type::length(__s));
}
template <class _CharT, class _Traits>
typename basic_string_view<_CharT, _Traits>::size_type
basic_string_view<_CharT, _Traits>::find_first_of(const_pointer __s, size_type __pos, size_type __n) const
{
    size_type __sz = size();
    if (__pos >= __sz || __n == 0)
        return npos;
    const_pointer __p = data();
    const_pointer __r = std::find_first_of(__p + __pos, __p + __sz, __s,
                                           __s + __n, __traits_eq<traits_type>());
    if (__r == __p + __sz)
        return npos;
    return static_cast<size_type>(__r - __p);
}
template <class _CharT, class _Traits>
typename basic_string_view<_CharT, _Traits>::size_type
basic_string_view<_CharT, _Traits>::find_first_of(basic_string_view __s, size_type __pos) const _NOEXCEPT
{
    return find_first_of(__s.data(), __pos, __s.size());
}
template <class _CharT, class _Traits>
typename basic_string_view<_CharT, _Traits>::size_type
basic_string_view<_CharT, _Traits>::find_first_of(value_type __c, size_type __pos) const _NOEXCEPT
{
    return find(__c, __pos);
}
template <class _CharT, class _Traits>
typename basic_string_view<_CharT, _Traits>::size_type
basic_string_view<_CharT, _Traits>::find_first_of(const_pointer __s, size_type __pos) const
{
    return find_first_of(__s, __pos, traits_type::length(__s));
}
template <class _CharT, class _Traits>
typename basic_string_view<_CharT, _Traits>::size_type
basic_string_view<_CharT, _Traits>::find_last_of(const_pointer __s, size_type __pos, size_type __n) const
{
    if (__n != 0)
    {
        size_type __sz = size();
        if (__pos < __sz)
            ++__pos;
        else
            __pos = __sz;
        const_pointer __p = data();
        for (const_pointer __ps = __p + __pos; __ps != __p;)
        {
            const_pointer __r = traits_type::find(__s, __n, *--__ps);
            if (__r)
                return static_cast<size_type>(__ps - __p);
        }
    }
    return npos;
}
template <class _CharT, class _Traits>
typename basic_string_view<_CharT, _Traits>::size_type
basic_string_view<_CharT, _Traits>::find_last_of(basic_string_view __s, size_type __pos) const _NOEXCEPT
{
    return find_last_of(__s.data(), __pos, __s.size());
}
template <class _CharT, class _Traits>
typename basic_string_view<_CharT, _Traits>::size_type
basic_string_view<_CharT, _Traits>::find_last_of(value_type __c, size_type __pos) const _NOEXCEPT
{
    return rfind(__c, __pos);
}
template <class _CharT, class _Traits>
typename basic_string_view<_CharT, _Traits>::size_type
basic_string_view<_CharT, _Traits>::find_last_of(const_pointer __s, size_type __pos) const
{
    return find_last_of(__s, __pos, traits_type::length(__s));
}
template <class _CharT, class _Traits>
typename basic_string_view<_CharT, _Traits>::size_type
basic_string_view<_CharT, _Traits>::find_first_not_of(const_pointer __s, size_type __pos, size_type __n) const
{
    size_type __sz = size();
    if (__pos < __sz)
    {
        const_pointer __p = data();
        const_pointer __pe = __p + __sz;
        for (const_pointer __ps = __p + __pos; __ps != __pe; ++__ps)
            if (traits_type::find(__s, __n, *__ps) == 0)
                return static_cast<size_type>(__ps - __p);
    }
    return npos;
}
template <class _CharT, class _Traits>
typename basic_string_view<_CharT, _Traits>::size_type
basic_string_view<_CharT, _Traits>::find_first_not_of(basic_string_view __s, size_type __pos) const _NOEXCEPT
{
    return find_first_not_of(__s.data(), __pos, __s.size());
}
template <class _CharT, class _Traits>
typename basic_string_view<_CharT, _Traits>::size_type
basic_string_view<_CharT, _Traits>::find_first_not_of(value_type __c, size_type __pos) const _NOEXCEPT
{
    size_type __sz = size();
    if (__pos < __sz)
    {
        const_pointer __p = data();
        const_pointer __pe = __p + __sz;
        for (const_pointer __ps = __p + __pos; __ps != __pe; ++__ps)
            if (!traits_type::eq(*__ps, __c))
                return static_cast<size_type>(__ps - __p);
    }
    return npos;
}
template <class _CharT, class _Traits>
typename basic_string_view<_CharT, _Traits>::size_type
basic_string_view<_CharT, _Traits>::find_first_not_of(const_pointer __s, size_type __pos) const
{
    return find_first_not_of(__s, __pos, traits_type::length(__s));
}
template <class _CharT, class _Traits>
typename basic_string_view<_CharT, _Traits>::size_type
basic_string_view<_CharT, _Traits>::find_last_not_of(const_pointer __s, size_type __pos, size_type __n) const
{
    size_type __sz = size();
    if (__pos < __sz)
        ++__pos;
    else
        __pos = __sz;
    const_pointer __p = data();
    for (const_pointer __ps = __p + __pos; __ps != __p;)
        if (traits_type::find(__s, __n, *--__ps) == 0)
            return static_cast<size_type>(__ps - __p);
    return npos;
}
template <class _CharT, class _Traits>
typename basic_string_view<_CharT, _Traits>::size_type
basic_string_view<_CharT, _Traits>::find_last_not_of(basic_string_view __s, size_type __pos) const _NOEXCEPT
{
    return find_last_not_of(__s.data(), __pos, __s.size());
}
template <class _CharT, class _Traits>
typename basic_string_view<_CharT, _Traits>::size_type
basic_string_view<_CharT, _Traits>::find_last_not_of(value_type __c, size_type __pos) const _NOEXCEPT
{
    size_type __sz = size();
    if (__pos < __sz)
        ++__pos;
    else
        __pos = __sz;
        const_pointer __p = data();
        for (const_pointer __ps = __p + __pos; __ps != __p;)
            if (!traits_type::eq(*--__ps, __c))
                return static_cast<size_type>(__ps - __p);
    return npos;
}
template <class _CharT, class _Traits>
typename basic_string_view<_CharT, _Traits>::size_type
basic_string_view<_CharT, _Traits>::find_last_not_of(const_pointer __s, size_type __pos) const
{
    return find_last_not_of(__s, __pos, traits_type::length(__s));
}

#if 0
#pragma mark - Comparisons
#endif

// [string.view.comparison], non-member basic_string_view comparison functions

template<class _CharT, class _Traits>
bool operator==(basic_string_view<_CharT, _Traits> __x, basic_string_view<_CharT, _Traits> __y) _NOEXCEPT
{
    return __x.compare(__y) == 0;
}
template<class _CharT, class _Traits>
bool operator!=(basic_string_view<_CharT, _Traits> __x, basic_string_view<_CharT, _Traits> __y) _NOEXCEPT
{
    return __x.compare(__y) != 0;
}
template<class _CharT, class _Traits>
bool operator< (basic_string_view<_CharT, _Traits> __x, basic_string_view<_CharT, _Traits> __y) _NOEXCEPT
{
    return __x.compare(__y) < 0;
}
template<class _CharT, class _Traits>
bool operator> (basic_string_view<_CharT, _Traits> __x, basic_string_view<_CharT, _Traits> __y) _NOEXCEPT
{
    return __y < __x;
}
template<class _CharT, class _Traits>
bool operator<=(basic_string_view<_CharT, _Traits> __x, basic_string_view<_CharT, _Traits> __y) _NOEXCEPT
{
    return !(__x > __y);
}
template<class _CharT, class _Traits>
bool operator>=(basic_string_view<_CharT, _Traits> __x, basic_string_view<_CharT, _Traits> __y) _NOEXCEPT
{
    return !(__x < __y);
}

template<class _CharT, class _Traits>
bool operator==(basic_string_view<_CharT, _Traits> __x, std::basic_string<_CharT, _Traits> __y) _NOEXCEPT
{
    return __x == basic_string_view<_CharT, _Traits>(__y);
}
template<class _CharT, class _Traits>
bool operator!=(basic_string_view<_CharT, _Traits> __x, std::basic_string<_CharT, _Traits> __y) _NOEXCEPT
{
    return __x != basic_string_view<_CharT, _Traits>(__y);
}
template<class _CharT, class _Traits>
bool operator< (basic_string_view<_CharT, _Traits> __x, std::basic_string<_CharT, _Traits> __y) _NOEXCEPT
{
    return __x < basic_string_view<_CharT, _Traits>(__y);
}
template<class _CharT, class _Traits>
bool operator> (basic_string_view<_CharT, _Traits> __x, std::basic_string<_CharT, _Traits> __y) _NOEXCEPT
{
    return __x > basic_string_view<_CharT, _Traits>(__y);
}
template<class _CharT, class _Traits>
bool operator<=(basic_string_view<_CharT, _Traits> __x, std::basic_string<_CharT, _Traits> __y) _NOEXCEPT
{
    return __x <= basic_string_view<_CharT, _Traits>(__y);
}
template<class _CharT, class _Traits>
bool operator>=(basic_string_view<_CharT, _Traits> __x, std::basic_string<_CharT, _Traits> __y) _NOEXCEPT
{
    return __x >= basic_string_view<_CharT, _Traits>(__y);
}

template<class _CharT, class _Traits>
bool operator==(std::basic_string<_CharT, _Traits> __x, basic_string_view<_CharT, _Traits> __y) _NOEXCEPT
{
    return __y == basic_string_view<_CharT, _Traits>(__x);
}
template<class _CharT, class _Traits>
bool operator!=(std::basic_string<_CharT, _Traits> __x, basic_string_view<_CharT, _Traits> __y) _NOEXCEPT
{
    return __y != basic_string_view<_CharT, _Traits>(__x);
}
template<class _CharT, class _Traits>
bool operator< (std::basic_string<_CharT, _Traits> __x, basic_string_view<_CharT, _Traits> __y) _NOEXCEPT
{
    return __y >= basic_string_view<_CharT, _Traits>(__x);
}
template<class _CharT, class _Traits>
bool operator> (std::basic_string<_CharT, _Traits> __x, basic_string_view<_CharT, _Traits> __y) _NOEXCEPT
{
    return __y <= basic_string_view<_CharT, _Traits>(__x);
}
template<class _CharT, class _Traits>
bool operator<=(std::basic_string<_CharT, _Traits> __x, basic_string_view<_CharT, _Traits> __y) _NOEXCEPT
{
    return __y > basic_string_view<_CharT, _Traits>(__x);
}
template<class _CharT, class _Traits>
bool operator>=(std::basic_string<_CharT, _Traits> __x, basic_string_view<_CharT, _Traits> __y) _NOEXCEPT
{
    return __y < basic_string_view<_CharT, _Traits>(__x);
}

template<class _CharT, class _Traits>
bool operator==(basic_string_view<_CharT, _Traits> __x, const _CharT* __y) _NOEXCEPT
{
    return __x.compare(__y) == 0;
}
template<class _CharT, class _Traits>
bool operator!=(basic_string_view<_CharT, _Traits> __x, const _CharT* __y) _NOEXCEPT
{
    return __x.compare(__y) != 0;
}
template<class _CharT, class _Traits>
bool operator< (basic_string_view<_CharT, _Traits> __x, const _CharT* __y) _NOEXCEPT
{
    return __x.compare(__y) < 0;
}
template<class _CharT, class _Traits>
bool operator> (basic_string_view<_CharT, _Traits> __x, const _CharT* __y) _NOEXCEPT
{
    return __x.compare(__y) > 0;
}
template<class _CharT, class _Traits>
bool operator<=(basic_string_view<_CharT, _Traits> __x, const _CharT* __y) _NOEXCEPT
{
    return !(__x > __y);
}
template<class _CharT, class _Traits>
bool operator>=(basic_string_view<_CharT, _Traits> __x, const _CharT* __y) _NOEXCEPT
{
    return !(__x < __y);
}

template<class _CharT, class _Traits>
bool operator==(const _CharT* __x, basic_string_view<_CharT, _Traits> __y) _NOEXCEPT
{
    return __y == __x;
}
template<class _CharT, class _Traits>
bool operator!=(const _CharT* __x, basic_string_view<_CharT, _Traits> __y) _NOEXCEPT
{
    return __y != __x;
}
template<class _CharT, class _Traits>
bool operator< (const _CharT* __x, basic_string_view<_CharT, _Traits> __y) _NOEXCEPT
{
    return __y >= __x;
}
template<class _CharT, class _Traits>
bool operator> (const _CharT* __x, basic_string_view<_CharT, _Traits> __y) _NOEXCEPT
{
    return __y <= __x;
}
template<class _CharT, class _Traits>
bool operator<=(const _CharT* __x, basic_string_view<_CharT, _Traits> __y) _NOEXCEPT
{
    return __y > __x;
}
template<class _CharT, class _Traits>
bool operator>=(const _CharT* __x, basic_string_view<_CharT, _Traits> __y) _NOEXCEPT
{
    return __y < __x;
}

#if 0
#pragma mark - Other Non-Member Functions
#endif

// [string.view.nonmem], other non-member basic_string_view functions
template <class _CharT, class _Traits, class _Allocator>
std::basic_string<_CharT, _Traits, _Allocator>
to_string(basic_string_view<_CharT, _Traits> str, const _Allocator& a)
{
    return std::basic_string<_CharT, _Traits, _Allocator>(str.data(), str.size(), a);
}

template <class _CharT, class _Traits>
std::basic_ostream<_CharT, _Traits>&
operator<<(std::basic_ostream<_CharT, _Traits>& os,
           basic_string_view<_CharT, _Traits> str)
{
    return os.write(str.data(), str.size());
}

#if 0
#pragma mark - Hashing
#endif

template <class _Ptr>
std::size_t FORCE_INLINE
__do_string_view_hash(_Ptr __p, _Ptr __e)
{
    typedef typename std::iterator_traits<_Ptr>::value_type value_type;
    return __murmur2_or_cityhash<size_t>()(__p, (__e - __p)*sizeof(value_type));
}

EPUB3_END_NAMESPACE

namespace std
{

    template <class _CharT, class _Traits>
    struct hash<ePub3::basic_string_view<_CharT, _Traits>> : public std::unary_function<::ePub3::basic_string_view<_CharT, _Traits>, std::size_t>
    {
        std::size_t operator()(const ::ePub3::basic_string_view<_CharT, _Traits>& __s) const _NOEXCEPT;
    };

    template <class _CharT, class _Traits>
    std::size_t hash<ePub3::basic_string_view<_CharT, _Traits>>::operator()(const ::ePub3::basic_string_view<_CharT, _Traits> &__s) const _NOEXCEPT
    {
        return __do_string_view_hash(__s.data(), __s.data() + __s.size());
    }

}

#endif
