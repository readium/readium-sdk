//
//  manifest.h
//  ePub3
//
//  Created by Jim Dovey on 2012-11-29.
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

#ifndef __ePub3__manifest__
#define __ePub3__manifest__

#include <ePub3/epub3.h>
#include <ePub3/utilities/utfstring.h>
#include <ePub3/utilities/iri.h>
#include <ePub3/property_holder.h>
#include <ePub3/utilities/xml_identifiable.h>
#include <map>
#include <ePub3/xml/node.h>

EPUB3_BEGIN_NAMESPACE

class Package;
class ManifestItem;
class ArchiveReader;
class ByteStream;

#ifdef SUPPORT_ASYNC
class AsyncByteStream;
#endif /* SUPPORT_ASYNC */

typedef shared_ptr<Package>         PackagePtr;

typedef shared_ptr<ManifestItem>    ManifestItemPtr;

///
/// A map of item-ids to manifest items.
typedef std::map<string, shared_ptr<ManifestItem>>  ManifestTable;

// this should just be an enum, but I'm having an inordinately hard time getting the
// compiler to let me use it as such

/**
 The ItemProperties class defines an optimized representation of the applicable
 properties for a ManifestItem, as of EPUB 3.
 
 This should ideally be an `enum class`, but it seems the compiler doesn't like
 performing arithmetic or bitwise operations on those, nor does it like me defining
 those arithmetic operations as methods on an `enum class`. Therefore it's all DIY.
 
 @see http://www.idpf.org/epub/30/spec/epub30-publications.html#sec-item-property-values
 
 @ingroup epub-model
 */
class ItemProperties
#if EPUB_PLATFORM(WINRT)
	: public NativeBridge
#endif
{
public:
    ///
    /// The bit flags representing individual properties.
    enum : unsigned int {
        None                = 0,        ///< No properties defined.
        CoverImage          = 1<<0,     ///< This item is the cover image for the publication.
        ContainsMathML      = 1<<1,     ///< This item contains MathML markup.
        Navigation          = 1<<2,     ///< This item is the EPUB 3 navigation document for the publication.
        HasRemoteResources  = 1<<3,     ///< This item accesses resources located outside the EPUB container.
        HasScriptedContent  = 1<<4,     ///< This item has content which uses JavaScript or HTML5 forms.
        ContainsSVG         = 1<<5,     ///< This item contains SVG markup.
        ContainsSwitch      = 1<<6,     ///< This item contains an `epub:switch` element.
        
        AllPropertiesMask   = (1<<7)-1, ///< A mask for all standard EPUB 3 properties.
    };
    
    ///
    /// To avoid the C++ compiler's reticence for arithmetic on `enum` types, the value is an `unsigned int`.
    typedef unsigned int    value_type;
    
public:
    
    /**
     Creates a new instance from an XML `properties` attribute string.
     @param attrStr The value of a manifest `item` element's `properties` attribute.
     */
    EPUB3_EXPORT    ItemProperties(const string& attrStr);
    /**
     Creates a new instance from a canonical property IRI.
     @param iri The IRI for a single property. This *must* be a manifest item property.
     */
    EPUB3_EXPORT    ItemProperties(const IRI& iri);
    /**
     Creates a new instance with a given value (or None).
     @param v The bitfield value representing the property or properties set.
     */
                    ItemProperties(value_type v = None) : _p(v) {}
    ///
    /// Copy constructor.
                    ItemProperties(const ItemProperties& o) : _p(o._p) {}
    ///
    /// C++11 move constructor.
                    ItemProperties(ItemProperties&& o) : _p(o._p) {}
                    ~ItemProperties() {}
    
    /// @{
    /// @name Tests
    
    /**
     Checks for the presence of one or more properties.
     @param p The property bitfield against which to check.
     @result Returns `true` if all set bits in `p` are also set in `this`.
     */
    bool            HasProperty(ItemProperties::value_type p)             const   { return (_p & p) == p; }
    
    /**
     Checks for the presence of one or more properties.
     @param p The properties against which to check.
     @result Returns `true` if all properties in `p` are also in `this`.
     */
    bool            HasProperty(const ItemProperties& p)    const   { return HasProperty(p._p); }
    
    /// @}
    
    /// @{
    /// @name Arithmetic and Bitwise Operations
    
    ///
    /// Compares two ItemProperties for equality.
    bool            operator==(const ItemProperties& o)     const   { return _p == o._p; }
    ///
    /// Compares an ItemProperties against a property bitfield for equality.
    bool            operator==(value_type v)                const   { return _p == v; }
    
    ///
    /// Compares two ItemProperties for inequality.
    bool            operator!=(const ItemProperties& o)     const   { return _p != o._p; }
    ///
    /// Compares an ItemProperties against a property bitfield for inequality.
    bool            operator!=(value_type v)                const   { return _p != v; }
    
    ///
    /// Assignment from another ItemProperties instance.
    ItemProperties& operator=(const ItemProperties& o)              { _p = o._p; return *this; }
    ///
    /// Assignment from a property bitfield.
    ItemProperties& operator=(value_type v)                         { _p = v; return *this; }
    ///
    /// Assignment from an XML `properties` attribute value string.
    ItemProperties& operator=(const string& attrStr);
    
    ///
    /// Returns a new ItemProperties that is the bitwise-OR of two ItemProperties.
    ItemProperties  operator|(const ItemProperties& o)      const   { return ItemProperties(_p | o._p); }
    ///
    /// Returns a new ItemProperties that is the bitwise-OR of `this` and a property bitfield.
    ItemProperties  operator|(value_type v)                 const   { return ItemProperties(_p | v); }
    
    ///
    /// Returns a new ItemProperties that is the bitwise-AND of two ItemProperties.
    ItemProperties  operator&(const ItemProperties& o)      const   { return ItemProperties(_p & o._p); }
    ///
    /// Returns a new ItemProperties that is the bitwise-AND of `this` and a property bitfield.
    ItemProperties  operator&(value_type v)                 const   { return ItemProperties(_p & v); }
    
    ///
    /// Returns a new ItemProperties that is the bitwise inverse of `this`.
    ItemProperties  operator~()                             const   { return ItemProperties((~_p)&AllPropertiesMask); }
    ///
    /// Returns a new ItemProperties that is the bitwise-NOT of two ItemProperties.
    ItemProperties  operator^(const ItemProperties& o)      const   { return ItemProperties(_p ^ o._p); }
    ///
    /// Returns a new ItemProperties that is the bitwise-NOT of `this` and a property bitfield.
    ItemProperties  operator^(value_type v)                 const   { return ItemProperties(_p ^ v); }
    
    ///
    /// Assigns the result of a bitwise-OR with another ItemProperties.
    ItemProperties& operator|=(const ItemProperties& o)             { _p |= o._p; return *this; }
    ///
    /// Assigns the result of a bitwise-OR with a property bitfield.
    ItemProperties& operator|=(value_type v)                        { _p |= v; return *this; }
    
    ///
    /// Assigns the result of a bitwise-AND with another ItemProperties.
    ItemProperties& operator&=(const ItemProperties& o)             { _p &= o._p; return *this; }
    ///
    /// Assigns the result of a bitwise-AND with a property bitfield.
    ItemProperties& operator&=(value_type v)                        { _p &= v; return *this; }
    
    ///
    /// Assigns the result of a bitwise-NOT with another ItemProperties.
    ItemProperties& operator^=(const ItemProperties& o)             { _p ^= o._p; return *this; }
    ///
    /// Assigns the result of a bitwise-NOT with a property bitfield.
    ItemProperties& operator^=(value_type v)                        { _p ^= v; return *this; }
    
    /// @}
    
    /// @{
    /// @name Casting to Other Types
    
    ///
    /// Cast to a raw bitfield.
    operator        value_type ()                           const   { return _p; }
    ///
    /// Cast to a string value suitable for use as the `properties`
    /// attribute of an XML manifest `item` element.
    EPUB3_EXPORT
    string          str()                                   const;
    
    /// @}
    
private:
    value_type _p;                                      ///< The property bitfield.
    
    typedef std::map<string, value_type> PropertyMap;
    static const PropertyMap PropertyLookupTable;       ///< A map of known property names to bitfield values.
    
};

/**
 The ManifestItem class represents a single resource within a publication.
 
 Each publication package must list all files within the container that it plans to
 use. If an item is referenced either directly from the spine or from a navigation
 document, content document, etc. then it *must* have a corresponding ManifestItem.
 Reading systems *should not* allow use of any resources from the container which do
 not have a ManifestItem; in practice this requirement is often ignored, however.
 
 A ManifestItem defines a few important details of a resource within an EPUB 3
 publication:
 
 - A unique (within the same package) identifier for this resource.
 - Its location, relative to the manifestation's Package document.
 - Its media-type, usually a MIME type (although EPUB may define new values for items
 which do not have canonical MIME types assigned).
 - The identifier of any media-overlay resource in the same Manifest.
 - The identifier of any fallback manifest item to be displayed if this resource's
 media type is unrecognised by the reading system.
 - A set of properties providing additional information about the resource.
 
 @remarks A ManifestItem keeps a pointer to its owning Package, but does not assume
 any memory-management responsibility for that pointer. Each ManifestItem is owned by
 the Package from which it was loaded., and will be deleted when that Package is
 deallocated.
 
 @ingroup epub-model
 */
class ManifestItem : public PointerType<ManifestItem>, public OwnedBy<Package>, public PropertyHolder, public XMLIdentifiable
#if EPUB_PLATFORM(WINRT)
	, public NativeBridge
#endif
{
public:
    typedef string              MimeType;
    
private:
                                ManifestItem()                                      _DELETED_;
                                ManifestItem(const ManifestItem&)                   _DELETED_;
                        
public:
    EPUB3_EXPORT                ManifestItem(const shared_ptr<Package>& owner);
    EPUB3_EXPORT                ManifestItem(ManifestItem&&);
    virtual                     ~ManifestItem();
    
    FORCE_INLINE
    PackagePtr                  GetPackage()                        const   { return Owner(); }
    
	virtual bool                ParseXML(shared_ptr<xml::Node> node);

    EPUB3_EXPORT
    string                      AbsolutePath()                      const;
    
    const string&               Identifier()                        const   { return XMLIdentifier(); }
    const string&               Href()                              const   { return _href; }
    const MimeType&             MediaType()                         const   { return _mediaType; }
    const string&               MediaOverlayID()                    const   { return _mediaOverlayID; }
    EPUB3_EXPORT
    shared_ptr<ManifestItem>    MediaOverlay()                      const;
    const string&               FallbackID()                        const   { return _fallbackID; }
    EPUB3_EXPORT
    shared_ptr<ManifestItem>    Fallback()                          const;
    
    // strips any query/fragment from the href before returning
    EPUB3_EXPORT
    string                      BaseHref()                          const;

    // NOTE: the two "HasProperty" functions below test for predefined "_parsedProperties"
    // on the spine item, not for OPF package metadata.
    // ... unlike the other HasProperty(std::vector<IRI>& properties) method further below!
    // (TODO: very confusing API)
    bool HasProperty(const string& property) const
    {
        auto itemProps = ItemProperties(property);
        if (itemProps == ItemProperties::None)
            return false;
        return _parsedProperties.HasProperty(itemProps);
    }
    bool HasProperty(ItemProperties::value_type prop) const
    {
        if (prop == ItemProperties::value_type(ItemProperties::None))
            return false;
        return _parsedProperties.HasProperty(prop);
    }

    EPUB3_EXPORT
    bool                        HasProperty(const std::vector<IRI>& properties)  const;
    
    // fetch any relevant encryption information
    EncryptionInfoPtr           GetEncryptionInfo()                 const;

	bool						CanLoadDocument()					const;
    
    // one-shot XML document loader
    EPUB3_EXPORT
	shared_ptr<xml::Document>	ReferencedDocument()                const;
    
    // stream the data
    EPUB3_EXPORT
    unique_ptr<ByteStream>      Reader()                            const;

#ifdef SUPPORT_ASYNC
    EPUB3_EXPORT
    unique_ptr<AsyncByteStream> AsyncReader()                       const;
#endif /* SUPPORT_ASYNC */

protected:
    string                  _href;
    MimeType                _mediaType;
    string                  _mediaOverlayID;
    string                  _fallbackID;
    ItemProperties          _parsedProperties;
};

EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__manifest__) */
