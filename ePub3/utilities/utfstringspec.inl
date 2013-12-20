//
//  utfstringspec.inl
//  ePub3
//
//  Created by Jim Dovey on 2013-04-30.
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

// this header is designed to be included directly by utfstring.h -- do
// not use it in any other fashion

#define UTFSTRING_SPECIALIZATIONS_INLINED

#ifdef min
# undef min
#endif
#ifdef max
# undef max
#endif

template <>
FORCE_INLINE string & string::assign(iterator first, iterator last)
{
    _base.assign(first.base(), last.base());
    return *this;
}
template <>
FORCE_INLINE string & string::assign(__base::const_iterator first, __base::const_iterator last)
{
    _base.assign(first, last);
    return *this;
}
template <>
FORCE_INLINE string & string::assign(const char *first, const char *last)
{
    _base.assign(first, last-first);
    return *this;
}
template <>
FORCE_INLINE string & string::append(const_iterator first, const_iterator last)
{
    _base.append(first.base(), last.base());
    return *this;
}
template <>
FORCE_INLINE string & string::append(__base::const_iterator first, __base::const_iterator last)
{
    _base.append(first, last);
    return *this;
}
template <>
FORCE_INLINE string & string::append(const char * first, const char * last)
{
    _base.append(first, last-first);
    return *this;
}
template <>
FORCE_INLINE string::iterator string::insert(iterator pos, iterator first, iterator last)
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
FORCE_INLINE string::iterator string::insert(iterator pos, __base::iterator first, __base::iterator last)
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
template <>
FORCE_INLINE string & string::replace(cxx11_const_iterator i1, cxx11_const_iterator i2, cxx11_const_iterator j1, cxx11_const_iterator j2)
{
    _base.replace(i1.base(), i2.base(), j1.base(), j2.base());
    return *this;
}
template <>
FORCE_INLINE string & string::replace(cxx11_const_iterator i1, cxx11_const_iterator i2, __base::const_iterator j1, __base::const_iterator j2)
{
    _base.replace(i1.base(), i2.base(), j1, j2);
    return *this;
}
template <>
FORCE_INLINE string & string::replace(cxx11_const_iterator i1, cxx11_const_iterator i2, std::u32string::const_iterator j1, std::u32string::const_iterator j2)
{
    auto utf8 = _Convert<value_type>::toUTF8(&(*j1), 0, std::distance(j1, j2));
    _base.replace(i1.base(), i2.base(), utf8);
    return *this;
}
template <>
FORCE_INLINE string::size_type string::find_first_of<char>(const char * s, size_type pos, size_type n) const {
    validate_utf8(s+pos, npos);
    return find_first_of(_Convert<char>::toUTF8(s), pos);
}
template <>
FORCE_INLINE string::size_type string::find_first_of<char>(const char * s, size_type pos) const  {
    validate_utf8(s+pos, npos);
    return find_first_of(_Convert<char>::toUTF8(s), pos);
}
template <>
FORCE_INLINE string::size_type string::find_first_of<xmlChar>(const xmlChar * s, size_type pos, size_type n) const {
    validate_utf8(s+pos, npos);
    return find_first_of(_Convert<xmlChar>::toUTF8(s), pos);
}
template <>
FORCE_INLINE string::size_type string::find_first_of<xmlChar>(const xmlChar * s, size_type pos) const {
    validate_utf8(s+pos, npos);
    return find_first_of(_Convert<xmlChar>::toUTF8(s), pos);
}
template <>
FORCE_INLINE string::size_type string::find_last_of<char>(const char * s, size_type pos, size_type n) const {
    validate_utf8(s+pos, npos);
    return find_last_of(_Convert<char>::toUTF8(s), pos);
}
template <>
FORCE_INLINE string::size_type string::find_last_of<char>(const char * s, size_type pos) const {
    validate_utf8(s+pos, npos);
    return find_last_of(_Convert<char>::toUTF8(s), pos);
}
template <>
FORCE_INLINE string::size_type string::find_last_of<xmlChar>(const xmlChar * s, size_type pos, size_type n) const {
    validate_utf8(s+pos, npos);
    return find_last_of(_Convert<xmlChar>::toUTF8(s), pos);
}
template <>
FORCE_INLINE string::size_type string::find_last_of<xmlChar>(const xmlChar * s, size_type pos) const {
    validate_utf8(s+pos, npos);
    return find_last_of(_Convert<xmlChar>::toUTF8(s), pos);
}
template <>
FORCE_INLINE string::size_type string::find_first_not_of<char>(const char * s, size_type pos, size_type n) const {
    validate_utf8(s+pos, npos);
    return find_first_not_of(_Convert<char>::toUTF8(s), pos);
}
template <>
FORCE_INLINE string::size_type string::find_first_not_of<char>(const char * s, size_type pos) const {
    validate_utf8(s+pos, npos);
    return find_first_not_of(_Convert<char>::toUTF8(s), pos);
}
template <>
FORCE_INLINE string::size_type string::find_first_not_of<xmlChar>(const xmlChar * s, size_type pos, size_type n) const {
    validate_utf8(s+pos, npos);
    return find_first_not_of(_Convert<xmlChar>::toUTF8(s), pos);
}
template <>
FORCE_INLINE string::size_type string::find_first_not_of<xmlChar>(const xmlChar * s, size_type pos) const {
    validate_utf8(s+pos, npos);
    return find_first_not_of(_Convert<xmlChar>::toUTF8(s), pos);
}
template <>
FORCE_INLINE string::size_type string::find_last_not_of<char>(const char * s, size_type pos, size_type n) const {
    validate_utf8(s+pos, npos);
    return find_last_not_of(_Convert<char>::toUTF8(s), pos);
}
template <>
FORCE_INLINE string::size_type string::find_last_not_of<char>(const char * s, size_type pos) const {
    validate_utf8(s+pos, npos);
    return find_last_not_of(_Convert<char>::toUTF8(s), pos);
}
template <>
FORCE_INLINE string::size_type string::find_last_not_of<xmlChar>(const xmlChar * s, size_type pos, size_type n) const {
    validate_utf8(s+pos, npos);
    return find_last_not_of(_Convert<xmlChar>::toUTF8(s), pos);
}
template <>
FORCE_INLINE string::size_type string::find_last_not_of<xmlChar>(const xmlChar * s, size_type pos) const {
    validate_utf8(s+pos, npos);
    return find_last_not_of(_Convert<xmlChar>::toUTF8(s), pos);
}
template <>
FORCE_INLINE int string::compare(const value_type * s) const _NOEXCEPT
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
FORCE_INLINE int string::compare(size_type pos1, size_type n1, const_u4pointer s) const
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
FORCE_INLINE int string::compare(size_type pos1, size_type n1, const_u4pointer s, size_type n2) const
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
FORCE_INLINE int string::compare(const std::u32string & s) const _NOEXCEPT
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
FORCE_INLINE int string::compare(size_type pos1, size_type n1, const std::u32string& str) const
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
FORCE_INLINE int string::compare(size_type pos1, size_type n1, const std::u32string& str,
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
