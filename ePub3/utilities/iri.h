//
//  iri.h
//  ePub3
//
//  Created by Jim Dovey on 2013-01-15.
//  Copyright (c) 2013 The Readium Foundation. All rights reserved.
//

#ifndef __ePub3__iri__
#define __ePub3__iri__

#include "utfstring.h"
#include "gurl.h"
#include <vector>

EPUB3_BEGIN_NAMESPACE

class IRI
{
    // would like this to contain const strings, but that proves awkward for now
    typedef std::vector<string>         ComponentList;
    typedef ComponentList::size_type    size_type;
    
    static string gPathSeparator;
    static string gURNScheme;
    static string gReservedCharacters;
    
public:
    typedef const std::pair<string, string> IRICredentials;
    
public:
    IRI() = default;
    
    // create from an IRI or URI string of any (valid) kind
    IRI(const string& iriStr);
    // create a URN
    IRI(const string& nameID, const string& namespacedString);
    // create a simple URL
    IRI(const string& scheme, const string& host, const string& path, const string& query="", const string& fragment="");
    
    IRI(const IRI& o) : _urnComponents(o._urnComponents), _url(new GURL(*o._url)) {}
    IRI(IRI&& o) : _urnComponents(std::move(o._urnComponents)), _url(o._url) { o._url = nullptr; }
    
    virtual ~IRI();
    
    IRI&            operator=(const IRI& o);
    IRI&            operator=(IRI&& o);
    
    bool            operator==(const IRI& o)                const;
    bool            operator!=(const IRI& o)                const;
    
    bool            operator<(const IRI& o)                 const;
    
    bool            IsURN() const { return _urnComponents.size() > 1; }
    bool            IsRelative() const { return !_url->has_host(); }
    const string    Scheme() const { return (IsURN() ? _urnComponents[0] : _url->scheme()); }    // simple, because it must ALWAYS be present (even if empty, as for pure fragment IRIs)
    const string&   NameID() const { if (!IsURN()) { throw std::invalid_argument("Not a URN"); } return _urnComponents[1]; }
    const string    Host() const { return _url->host(); }
    IRICredentials  Credentials() const;
    const string    NamespacedString() const { if (!IsURN()) { throw std::invalid_argument("Not a URN"); } return _urnComponents[2]; }
    int             Port() const { return _url->EffectiveIntPort(); }
    const string    Path(bool URLEncoded=true) const;
    const string    Query() const { return _url->query(); }
    const string    Fragment() const { return _url->ref(); }
    const string    LastPathComponent() const { return _url->ExtractFileName(); }
    
    void            SetScheme(const string& scheme);
    void            SetHost(const string& host);
    void            SetCredentials(const string& user, const string& pass);
    void            AddPathComponent(const string& component);
    void            SetQuery(const string& query);
    void            SetFragment(const string& query);
    
    static string   URLEncodeComponent(const string& str);
    static string   PercentEncodeUCS(const string& str);
    static string   IDNEncodeHostname(const string& host);
    
    // percent-encodes characters only where required to keep the URL correct, IDN-encodes hostname
    string          IRIString() const;
    // percent-encodes all URL characters and invalid characters as UTF-8, IDN-encodes hostname
    string          URIString() const;
    
protected:
    ComponentList   _urnComponents;
    GURL*           _url;
    string          _pureIRI;       // may be empty
    
};

EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__iri__) */
