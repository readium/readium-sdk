//
//  iri.cpp
//  ePub3
//
//  Created by Jim Dovey on 2013-01-15.
//  Copyright (c) 2013 The Readium Foundation. All rights reserved.
//

#include "iri.h"
#include "url_util.h"
#include <regex>

EPUB3_BEGIN_NAMESPACE

#define INCREMENT_IF_VALID(x) if ((x) != 0) { (x)++; }
#define DECREMENT_IF_VALID(x) if ((x) != 0) { (x)--; }

string IRI::gPathSeparator('/');
string IRI::gURNScheme("urn");
string IRI::gEPUBScheme("epub3");
string IRI::gReservedCharacters("!*'();:@&=+$,/?%#[]");

inline const url_parse::Component ComponentForString(const string& str)
{
    return url_parse::Component(0, str.empty() ? -1 : static_cast<int>(str.utf8_size()));
}

IRI::IRI(const string& iriStr) : _url(new GURL(iriStr.utf16string())), _pureIRI(iriStr)
{
}
IRI::IRI(const string& nameID, const string& namespacedString) : _urnComponents{gURNScheme, nameID, namespacedString}, _url(new GURL()), _pureIRI(_Str("urn:", nameID, ":", namespacedString))
{
}
IRI::IRI(const string& scheme, const string& host, const string& path, const string& query, const string& fragment) : _urnComponents(), _url(new GURL())
{
    url_canon::Replacements<char> rep;
    rep.SetScheme(scheme.c_str(), ComponentForString(scheme));
    rep.SetHost(host.c_str(), ComponentForString(host));
    rep.SetPath(path.c_str(), ComponentForString(path));
    rep.SetQuery(query.c_str(), ComponentForString(query));
    rep.SetRef(fragment.c_str(), ComponentForString(fragment));
    
    _url->ReplaceComponents(rep);
    
    _pureIRI = _Str(scheme, "://", host, path);
    if ( !query.empty() )
        _pureIRI += _Str("?", query);
    if ( !fragment.empty() )
        _pureIRI += _Str("#", fragment);
}
IRI::~IRI()
{
    if ( _url != nullptr )
        delete _url;
}
IRI& IRI::operator=(const IRI& o)
{
    _urnComponents = o._urnComponents;
    if ( _url != nullptr )
        *_url = *o._url;
    else
        _url = new GURL(*o._url);
    return *this;
}
IRI& IRI::operator=(IRI &&o)
{
    _urnComponents = std::move(o._urnComponents);
    _url = o._url;
    o._url = nullptr;
    return *this;
}
bool IRI::operator==(const IRI &o) const
{
    if ( IsURN() )
        return _urnComponents == o._urnComponents;
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
    url_util::DecodeURLEscapeSequences(encodedPath.c_str(), static_cast<int>(encodedPath.size()), &output);
    return output.data();
}
void IRI::SetScheme(const string& scheme)
{
    url_canon::Replacements<char> rep;
    rep.SetScheme(scheme.c_str(), ComponentForString(scheme));
    _url->ReplaceComponents(rep);
    
    // can't keep the IRI up to date
    _pureIRI.clear();
}
void IRI::SetHost(const string& host)
{
    url_canon::Replacements<char> rep;
    rep.SetHost(host.c_str(), ComponentForString(host));
    _url->ReplaceComponents(rep);
    
    // can't keep the IRI up to date
    _pureIRI.clear();
}
void IRI::SetCredentials(const string& user, const string& pass)
{
    url_canon::Replacements<char> rep;
    url_parse::Component invalid(0, -1);
    rep.SetUsername(user.c_str(), ComponentForString(user));
    rep.SetPassword(pass.c_str(), ComponentForString(pass));
    _url->ReplaceComponents(rep);
    
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
    _url->ReplaceComponents(rep);
    
    if ( !_pureIRI.empty() && !_url->has_query() && !_url->has_ref() )
    {
        if ( _pureIRI[_pureIRI.size()-1] != U'/' )
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
    _url->ReplaceComponents(rep);
    
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
    _url->ReplaceComponents(rep);
    
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
string IRI::URLEncodeComponent(const string& str)
{
    std::stringstream ss;
    for ( string::size_type lastPos = 0, pos = str.find_first_of(gReservedCharacters);
          lastPos != string::npos;
          lastPos = pos, pos = str.find_first_of(gReservedCharacters, 0) )
    {
        ss << str.substr(lastPos, pos == string::npos ? pos : pos-lastPos);
        
        if ( pos == string::npos )
            break;
        
        // percent-encode each character-- we know they're all 7-bit ASCII
        char buf[3];
        string::value_type ch = str[pos];
        snprintf(buf, 3, "%%%02X", ch );
        ss << buf;
    }
    
    return ss.str();
}
string IRI::PercentEncodeUCS(const string& str)
{
    std::stringstream ss;
    for ( auto ch : str )
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
    return _url->spec();
}

EPUB3_END_NAMESPACE
