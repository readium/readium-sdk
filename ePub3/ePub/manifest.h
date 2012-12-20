//
//  manifest.h
//  ePub3
//
//  Created by Jim Dovey on 2012-11-29.
//  Copyright (c) 2012 Kobo Inc. All rights reserved.
//

#ifndef __ePub3__manifest__
#define __ePub3__manifest__

#include "epub3.h"
#include <map>
#include <string>
#include <libxml/tree.h>

EPUB3_BEGIN_NAMESPACE

class Package;
class ManifestItem;
typedef std::map<std::string, ManifestItem*>    ManifestTable;

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
    ItemProperties(const std::string& attrStr);
    ItemProperties(value_type v = None) : _p(v) {}
    ItemProperties(const ItemProperties& o) : _p(o._p) {}
    ItemProperties(ItemProperties&& o) : _p(o._p) {}
    ~ItemProperties() {}
    
    bool HasProperty(unsigned int p) const { return (_p & p) == p; }
    bool HasProperty(const ItemProperties& p) const { return HasProperty(p._p); }
    
    bool operator==(const ItemProperties& o) const { return _p == o._p; }
    bool operator==(value_type v) const { return _p == v; }
    
    bool operator!=(const ItemProperties& o) const { return _p != o._p; }
    bool operator!=(value_type v) const { return _p != v; }
    
    ItemProperties& operator=(const ItemProperties& o) { _p = o._p; return *this; }
    ItemProperties& operator=(value_type v) { _p = v; return *this; }
    ItemProperties& operator=(const std::string& attrStr);
    
    ItemProperties operator|(const ItemProperties& o) const { return ItemProperties(_p | o._p); }
    ItemProperties operator|(value_type v) const { return ItemProperties(_p | v); }
    
    ItemProperties operator&(const ItemProperties& o) const { return ItemProperties(_p & o._p); }
    ItemProperties operator&(value_type v) const { return ItemProperties(_p & v); }
    
    ItemProperties operator~() const { return ItemProperties(~_p); }
    ItemProperties operator^(const ItemProperties& o) const { return ItemProperties(_p ^ o._p); }
    ItemProperties operator^(value_type v) const { return ItemProperties(_p ^ v); }
    
    ItemProperties& operator|=(const ItemProperties& o) { _p |= o._p; return *this; }
    ItemProperties& operator|=(value_type v) { _p |= v; return *this; }
    
    ItemProperties& operator&=(const ItemProperties& o) { _p &= o._p; return *this; }
    ItemProperties& operator&=(value_type v) { _p &= v; return *this; }
    
    ItemProperties& operator^=(const ItemProperties& o) { _p ^= o._p; return *this; }
    ItemProperties& operator^=(value_type v) { _p ^= v; return *this; }
    
    operator value_type () const { return _p; }
    std::string str() const;
    
private:
    value_type _p;
    
    typedef std::map<std::string, value_type> PropertyMap;
    static const PropertyMap PropertyLookupTable;
    
};

class ManifestItem
{
public:
    ManifestItem() = delete;
    ManifestItem(xmlNodePtr node, const Package* owner);
    ManifestItem(const ManifestItem&) = delete;
    ManifestItem(ManifestItem&&);
    virtual ~ManifestItem();
    
    Shared<const Package> Package() const { return _owner; }
    
    const std::string& Identifier() const { return _identifier; }
    const std::string& Href() const { return _href; }
    const std::string& MediaType() const { return _mediaType; }
    const std::string& MediaOverlayID() const { return _mediaOverlayID; }
    const ManifestItem* MediaOverlay() const;
    const std::string& FallbackID() const { return _fallbackID; }
    const ManifestItem* Fallback() const;
    
    bool HasProperty(const std::string& property) const { return _properties.HasProperty(ItemProperties(property)); }
    bool HasProperty(ItemProperties::value_type prop) const { return _properties.HasProperty(prop); }
    
    xmlDocPtr ReferencedDocument() const;
    
protected:
    Shared<const class Package> _owner;
    
    std::string     _identifier;
    std::string     _href;
    std::string     _mediaType;
    std::string     _mediaOverlayID;
    std::string     _fallbackID;
    ItemProperties  _properties;
};

EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__manifest__) */
