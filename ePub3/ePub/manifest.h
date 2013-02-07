//
//  manifest.h
//  ePub3
//
//  Created by Jim Dovey on 2012-11-29.
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

#ifndef __ePub3__manifest__
#define __ePub3__manifest__

#include "epub3.h"
#include "utfstring.h"
#include "iri.h"
#include <map>
#include <libxml/tree.h>

EPUB3_BEGIN_NAMESPACE

class Package;
class ManifestItem;
class ArchiveReader;
class ByteStream;

typedef std::map<string, ManifestItem*>    ManifestTable;

// this should just be an enum, but I'm having an inordinately hard time getting the
// compiler to let me use it as such
class ItemProperties
{
public:
    enum : unsigned int {
        None                = 0,
        CoverImage          = 1<<0,
        ContainsMathML      = 1<<1,
        Navigation          = 1<<2,
        HasRemoteResources  = 1<<3,
        HasScriptedContent  = 1<<4,
        ContainsSVG         = 1<<5,
        ContainsSwitch      = 1<<6,
        
        AllPropertiesMask   = (1<<7)-1,
    };
    
    typedef unsigned int    value_type;
    
public:
                    ItemProperties(const string& attrStr);
                    ItemProperties(const IRI& iri);
                    ItemProperties(value_type v = None) : _p(v) {}
                    ItemProperties(const ItemProperties& o) : _p(o._p) {}
                    ItemProperties(ItemProperties&& o) : _p(o._p) {}
                    ~ItemProperties() {}
    
    bool            HasProperty(unsigned int p)             const   { return (_p & p) == p; }
    bool            HasProperty(const ItemProperties& p)    const   { return HasProperty(p._p); }
    
    bool            operator==(const ItemProperties& o)     const   { return _p == o._p; }
    bool            operator==(value_type v)                const   { return _p == v; }
    
    bool            operator!=(const ItemProperties& o)     const   { return _p != o._p; }
    bool            operator!=(value_type v)                const   { return _p != v; }
    
    ItemProperties& operator=(const ItemProperties& o)              { _p = o._p; return *this; }
    ItemProperties& operator=(value_type v)                         { _p = v; return *this; }
    ItemProperties& operator=(const string& attrStr);
    
    ItemProperties  operator|(const ItemProperties& o)      const   { return ItemProperties(_p | o._p); }
    ItemProperties  operator|(value_type v)                 const   { return ItemProperties(_p | v); }
    
    ItemProperties  operator&(const ItemProperties& o)      const   { return ItemProperties(_p & o._p); }
    ItemProperties  operator&(value_type v)                 const   { return ItemProperties(_p & v); }
    
    ItemProperties  operator~()                             const   { return ItemProperties(~_p); }
    ItemProperties  operator^(const ItemProperties& o)      const   { return ItemProperties(_p ^ o._p); }
    ItemProperties  operator^(value_type v)                 const   { return ItemProperties(_p ^ v); }
    
    ItemProperties& operator|=(const ItemProperties& o)             { _p |= o._p; return *this; }
    ItemProperties& operator|=(value_type v)                        { _p |= v; return *this; }
    
    ItemProperties& operator&=(const ItemProperties& o)             { _p &= o._p; return *this; }
    ItemProperties& operator&=(value_type v)                        { _p &= v; return *this; }
    
    ItemProperties& operator^=(const ItemProperties& o)             { _p ^= o._p; return *this; }
    ItemProperties& operator^=(value_type v)                        { _p ^= v; return *this; }
    
    operator        value_type ()                           const   { return _p; }
    string          str()                                   const;
    
private:
    value_type _p;
    
    typedef std::map<string, value_type> PropertyMap;
    static const PropertyMap PropertyLookupTable;
    
};

class ManifestItem
{
public:
    typedef string                  MimeType;
    
public:
                        ManifestItem()                                      = delete;
                        ManifestItem(xmlNodePtr node, const Package* owner);
                        ManifestItem(const ManifestItem&)                   = delete;
                        ManifestItem(ManifestItem&&);
    virtual             ~ManifestItem();
    
    const Package*      Package()                           const   { return _owner; }
    
    string              AbsolutePath()                      const;
    
    const string&       Identifier()                        const   { return _identifier; }
    const string&       Href()                              const   { return _href; }
    const MimeType&     MediaType()                         const   { return _mediaType; }
    const string&       MediaOverlayID()                    const   { return _mediaOverlayID; }
    const ManifestItem* MediaOverlay()                      const;
    const string&       FallbackID()                        const   { return _fallbackID; }
    const ManifestItem* Fallback()                          const;
    
    // strips any query/fragment from the href before returning
    string              BaseHref()                          const;
    
    bool                HasProperty(const string& property) const   { return _properties.HasProperty(ItemProperties(property)); }
    bool                HasProperty(ItemProperties::value_type prop)    const   { return _properties.HasProperty(prop); }
    bool                HasProperty(const std::vector<IRI>& properties)  const;
    
    // one-shot document loader
    xmlDocPtr           ReferencedDocument()                const;
    
    // stream the data
    Auto<ByteStream>    Reader()                            const;
    
protected:
    const class Package*    _owner;
    
    string                  _identifier;
    string                  _href;
    MimeType                _mediaType;
    string                  _mediaOverlayID;
    string                  _fallbackID;
    ItemProperties          _properties;
};

EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__manifest__) */
