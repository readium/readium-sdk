//
//  iri.cpp
//  ePub3
//
//  Created by Jim Dovey on 2013-01-15.
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

#include "iri.h"
#include <google-url/url_util.h>
#include "cfi.h"
#include "make_unique.h"
#include REGEX_INCLUDE

EPUB3_BEGIN_NAMESPACE

#define INCREMENT_IF_VALID(x) if ((x) != 0) { (x)++; }
#define DECREMENT_IF_VALID(x) if ((x) != 0) { (x)--; }

string IRI::gPathSeparator("/");
string IRI::gURNScheme("urn");
string IRI::gEPUBScheme("epub3");
string IRI::gReservedCharacters("!*'();:@&=+$,/?%#[]");

inline const url_parse::Component ComponentForString(const string& str)
{
    return url_parse::Component(0, str.empty() ? -1 : static_cast<int>(str.utf8_size()));
}

void IRI::AddStandardScheme(const string& scheme)
{
    url_util::AddStandardScheme(scheme.c_str());
}

IRI::IRI(const string& iriStr) : _urnComponents(), _url(ePub3::make_unique<GURL>(iriStr.stl_str())), _pureIRI(iriStr)
{
    // is it a URN?
    if ( iriStr.find("urn:", 0, 4) == 0 )
    {
        REGEX_NS::regex reg(":");
        auto components = iriStr.split(reg);
        if ( components.size() == 3 )
        {
            _urnComponents.push_back(gURNScheme);
            _urnComponents.push_back(components[1]);
            _urnComponents.push_back(components[2]);
        }
    }
}
IRI::IRI(const string& nameID, const string& namespacedString) :
#if EPUB_COMPILER_SUPPORTS(CXX_INITIALIZER_LISTS)
    _urnComponents({gURNScheme, nameID, namespacedString}),
#endif
    _pureIRI(_Str("urn:", nameID, ":", namespacedString)),
    _url(ePub3::make_unique<GURL>(_pureIRI.stl_str()))
{
#if !EPUB_COMPILER_SUPPORTS(CXX_INITIALIZER_LISTS)
    _urnComponents.push_back(gURNScheme);
    _urnComponents.push_back(nameID);
    _urnComponents.push_back(namespacedString);
#endif
}
IRI::IRI(const string& scheme, const string& host, const string& path, const string& query, const string& fragment) : _urnComponents(), _url(make_unique<GURL>())
{
    _pureIRI = _Str(scheme, "://", host);
    if ( path.empty() )
        _pureIRI += '/';
    else if ( path.find(gPathSeparator) != 0 )
        _pureIRI += ("/" + path);
    else
        _pureIRI += path;
    
    if ( !query.empty() )
        _pureIRI += _Str("?", query);
    if ( !fragment.empty() )
        _pureIRI += _Str("#", fragment);
    
    _url = ePub3::make_unique<GURL>(_pureIRI.stl_str());
}
IRI::~IRI()
{
}
IRI& IRI::operator=(const IRI& o)
{
    _urnComponents = o._urnComponents;
    _pureIRI = o._pureIRI;
    if ( _url != nullptr )
        *_url = *o._url;
    else
        _url = make_unique<GURL>(*o._url);
    return *this;
}
IRI& IRI::operator=(IRI &&o)
{
    _urnComponents = std::move(o._urnComponents);
    _pureIRI = std::move(o._pureIRI);
    _url = std::move(o._url);
    o._url = nullptr;
    return *this;
}
IRI& IRI::operator=(const string& str)
{
    // Support for URN strings
    bool isURN = false;
    if ( str.find("urn:", 0, 4) == 0 )
    {
        REGEX_NS::regex reg(":");
        auto components = str.split(reg);
        if ( components.size() == 3 )
        {
            _urnComponents.push_back(gURNScheme);
            _urnComponents.push_back(components[1]);
            _urnComponents.push_back(components[2]);
            isURN = true;
        }
    }
    
    auto newURL = ePub3::make_unique<GURL>(str.stl_str());
    if ( !newURL->is_valid() && !isURN )
        throw std::invalid_argument(_Str("IRI: '", str, "' is not a valid URL string."));
    
    _url = std::move(newURL);
    _pureIRI = str;
    if ( !isURN )
        _urnComponents.clear();
    
    return *this;
}
bool IRI::operator==(const IRI &o) const
{
    if ( IsURN() )
        return _urnComponents == o._urnComponents;
    else if ( _url == nullptr || o._url == nullptr )
        return false;
    return *_url == *o._url;
}
bool IRI::operator!=(const IRI& o) const
{
    if ( IsURN() )
        return _urnComponents != o._urnComponents;
    return *_url != *o._url;
}
bool IRI::operator<(const IRI& o) const
{
    if ( IsURN() )
        return _urnComponents < o._urnComponents;
    return *_url < *o._url;
}
IRI::IRICredentials IRI::Credentials() const
{
    string u, p;
    if ( _url->has_username() )
    {
        u = _url->username();
    }
    if ( _url->has_password() )
    {
        p = _url->password();
    }
    
    return IRICredentials(u, p);
}
const string IRI::Path(bool urlEncoded) const
{
    std::string encodedPath(_url->path());
    if ( urlEncoded )
        return encodedPath;
    
    url_canon::RawCanonOutputW<256> output;

    // note that .size() is on std::string here (equivalent to strlen(str.c_str()) ),
    // which is in fact the same as ePub3:string .utf8_size() defined in utfstring.h,
    // but not the same as ePub3:string .size() !!
    // WATCH OUT!
    int length = static_cast<int>(encodedPath.size());

    url_util::DecodeURLEscapeSequences(encodedPath.c_str(), length, &output);
    return string(output.data(), output.length());
}
const CFI IRI::ContentFragmentIdentifier() const
{
    if ( !_url->has_ref() )
        return CFI();
    
    string ref = Fragment();
    if ( ref.find("epubcfi(") != 0 )
        return CFI();
    
    return CFI(ref);
}
void IRI::SetScheme(const string& scheme)
{
    url_canon::Replacements<char> rep;
    rep.SetScheme(scheme.c_str(), ComponentForString(scheme));
    _url->ReplaceComponentsInline(rep);
    
    // can't keep the IRI up to date
    _pureIRI.clear();
}
void IRI::SetHost(const string& host)
{
    url_canon::Replacements<char> rep;
    rep.SetHost(host.c_str(), ComponentForString(host));
    _url->ReplaceComponentsInline(rep);
    
    // can't keep the IRI up to date
    _pureIRI.clear();
}
void IRI::SetPort(uint16_t port)
{
    char portStr[6];
    int len = ::snprintf(portStr, 6, "%hu", port);
    url_canon::Replacements<char> rep;
    rep.SetPort(portStr, url_parse::Component(0, len));
    _url->ReplaceComponentsInline(rep);
    
    // can't keep the IRI up to date
    _pureIRI.clear();
}
void IRI::SetCredentials(const string& user, const string& pass)
{
    url_canon::Replacements<char> rep;
    url_parse::Component invalid(0, -1);
    rep.SetUsername(user.c_str(), ComponentForString(user));
    rep.SetPassword(pass.c_str(), ComponentForString(pass));
    _url->ReplaceComponentsInline(rep);
    
    // can't keep the IRI up to date
    _pureIRI.clear();
}
void IRI::AddPathComponent(const string& component)
{
    std::string path(_url->path());
    if ( path[path.size()-1] != '/' )
        path += '/';
    path += component.stl_str();
    
    url_canon::Replacements<char> rep;
    rep.SetPath(path.c_str(), url_parse::Component(0, static_cast<int>(path.size())));
    _url->ReplaceComponentsInline(rep);
    
    if ( !_pureIRI.empty() && !_url->has_query() && !_url->has_ref() )
    {
        if ( _pureIRI[_pureIRI.size()-1] != char32_t('/') )
            _pureIRI += '/';
        _pureIRI += component;
    }
    else
    {
        // can't keep the IRI up to date
        _pureIRI.clear();
    }
}
void IRI::SetQuery(const string& query)
{
    url_canon::Replacements<char> rep;
    rep.SetQuery(query.c_str(), ComponentForString(query));
    _url->ReplaceComponentsInline(rep);
    
    if ( _pureIRI.empty() )
        return;
    
    string::size_type pos = _pureIRI.find('?');
    if ( pos != string::npos )
    {
        string::size_type end = _pureIRI.rfind('#');
        _pureIRI.replace(pos+1, (end == string::npos ? string::npos : end - pos), query);
    }
    else
    {
        pos = _pureIRI.rfind('#');
        if ( pos != string::npos )
        {
            _pureIRI.insert(pos, 1, '?');
            _pureIRI.insert(pos+1, query);
        }
        else
        {
            _pureIRI.append(1, '?');
            _pureIRI.append(query);
        }
    }
}
void IRI::SetFragment(const string& fragment)
{
    url_canon::Replacements<char> rep;
    rep.SetRef(fragment.c_str(), ComponentForString(fragment));
    _url->ReplaceComponentsInline(rep);
    
    string::size_type pos = _pureIRI.rfind('#');
    if ( pos != string::npos )
    {
        _pureIRI.replace(pos+1, string::npos, fragment);
    }
    else
    {
        _pureIRI.append(1, '#');
        _pureIRI.append(fragment);
    }
}
void IRI::SetContentFragmentIdentifier(const CFI &cfi)
{
    if ( cfi.Empty() )
        return;
    SetFragment(cfi.String());
}
string IRI::URLEncodeComponent(const string& str)
{
    url_canon::RawCanonOutput<256> output;
    url_util::EncodeURIComponent(str.c_str(), static_cast<int>(str.utf8_size()), &output);
    return string(output.data(), output.length());
}
string IRI::PercentEncodeUCS(const string& str)
{
    std::stringstream ss;
    for ( char32_t ch : str )
    {
        if ( ch > 0x7F )
        {
            // get utf8 representation and percent-encode it
            std::string utf8 = string::utf8_of(ch);
            for ( auto byte : utf8 )
            {
                char buf[3];
                snprintf(buf, 3, "%%%02X", byte);
                ss << buf;
            }
        }
        else
        {
            // single valid ASCII character
            ss << static_cast<char>(ch);
        }
    }
    
    return ss.str();
}
string IRI::IDNEncodeHostname(const string& str)
{
    url_canon::RawCanonOutputW<256> output;
    const string16 src = str.utf16string();
    if ( url_canon::IDNToASCII(src.c_str(), static_cast<int>(src.size()), &output) )
        return output.data();
    return string::EmptyString;
}
string IRI::IRIString() const
{
    if ( !_pureIRI.empty() )
        return _pureIRI;
    
    if ( !_url )
        return string::EmptyString;
    
    // we'll have to reverse-engineer it, grr
    string uri(URIString());
    std::string plainHost(_url->host());
    
    url_canon::RawCanonOutputW<256> idnDecoded;
    const string16 idnSrc = string(plainHost).utf16string();
    if ( url_canon::IDNToUnicode(idnSrc.c_str(), static_cast<int>(idnSrc.size()), &idnDecoded) && idnSrc != idnDecoded.data() )
    {
        // swap out the IDN-encoded hostname
        string::size_type pos = uri.find(plainHost);
        if ( pos != string::npos )
        {
            uri.replace(pos, plainHost.size(), idnDecoded.data());
        }
    }
    
    // have to leave it all url-encoded, sadly...
    return uri;
}
string IRI::URIString() const
{
    if ( !_url )
        return string::EmptyString;
    return _url->spec();
}

EPUB3_END_NAMESPACE
