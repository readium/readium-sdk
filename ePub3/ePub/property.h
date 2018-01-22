//
//  property.h
//  ePub3
//
//  Created by Jim Dovey on 2013-05-03.
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

#ifndef __ePub3__property__
#define __ePub3__property__

#include <ePub3/epub3.h>
#include <ePub3/utilities/basic.h>
#include <ePub3/utilities/owned_by.h>
#include <ePub3/utilities/iri.h>
#include <ePub3/utilities/utfstring.h>
#include <ePub3/property_extension.h>
#include <ePub3/utilities/epub_locale.h>
#include <ePub3/utilities/xml_identifiable.h>
#include <ePub3/xml/node.h>
#include <vector>
#include <map>

EPUB3_BEGIN_NAMESPACE

class Package;
class PropertyHolder;

class Property;
typedef shared_ptr<Property>        PropertyPtr;

///
/// Enumerated constants for the DCMES metadata attributes used by EPUB 3.
enum class DCType : uint32_t
{
    Invalid,        ///< An invalid value.
    
    // Required DCMES Elements.
    
    Identifier,     ///< A unique identifier.
    Title,          ///< The title of the publication.
    Language,       ///< The language in which the publication is rendered.
    
    // Optional DCMES elements
    
    Contributor,    ///< The name of a secondary contributor.
    Coverage,       ///< Spatial or temporal context of the publication.
    Creator,        ///< The name of a primary creator.
    Date,           ///< The original publication date (*not* the modification date).
    Description,    ///< A description of the publication's contents.
    Format,         ///< Information on the format in which the publication is rendered.
    Publisher,      ///< Details of the publisher of the work.
    Relation,       ///< An identifier for a related resource.
    Rights,         ///< Information about rights associated with the publication.
    Source,         ///< An identifier for the source of this EPUB, e.g. the ISBN of an original print edition.
    Subject,        ///< The topic or topics which form the subject of the publication.
    Type,           ///< The type of content encoded in this publication, e.g. book, annotations, minutes, etc.
    
    Custom          = UCHAR_MAX     ///< A non-DCMES metadata value, identified only by its property IRI.
};


///
/// Enumeration for writing direction values.
enum class WritingDirection
{
    Natural,                ///< Natural direction for the text's locale.
    LeftToRight,            ///< Characters flow left to right, lines from top to bottom.
    RightToLeft,            ///< Characters flow right to left, lines from top to bottom.
    VerticalLeftToRight,    ///< Characters flow top to bottom, lines from left to right.
    VerticalRightToLeft     ///< Characters flow top to bottom, lines from right to left.
};

///
/// Enumeration for page spreads.
enum class PageSpread
{
    Automatic,              ///< No value specified by the author.
    Left,                   ///< This is the left page of a spread.
    Right,                  ///< This is the right page of a spread.
    Center,                 ///< This is a double-width page, spread across both left & right.
};

///
/// Enumeration for page progression directions.
enum class PageProgression
{
    Default,                ///< Assume based on language, etc.
    LeftToRight,            ///< Pages flow from left to right, as in English.
    RightToLeft,            ///< Pages flow from right to left, as in Japanese comics & vertical text.
};

/**
 Obtains an IRI for a DCMES metadata item.
 @note The IRIs we use for DCMES items are not canon for ePub3.  We use them
 only for the benefit of using a single method to identify metadata items
 whether defined using DCMES elements or regular `<meta>` elements with an
 IRI-based property.
 @param type A type-code for a DCMES metadata item.
 @result A constant IRI referring to the type, or an empty (invalid) IRI if the
 `type` constant does not refer to a valid DCMES element (i.e. the `Custom` or
 `Invalid` pseudo-types).
 @ingroup utilities
 */
EPUB3_EXPORT
const IRI       IRIForDCType(DCType type);
EPUB3_EXPORT
DCType          DCTypeFromIRI(const IRI& iri);
    
__private_extern__ string __lang_from_locale(const std::locale& loc);
#if EPUB_USE(LIBXML2)
__private_extern__ const xmlChar * DCMES_uri;
__private_extern__ const xmlChar * MetaTagName;
#else
__private_extern__ const TCHAR * DCMES_uri;
__private_extern__ const TCHAR * MetaTagName;
#endif

/**
 This exception is thrown when a property vocabulary prefix is unknown to a
 given Package.
 
 A prefix is considered *unknown* when it is neither in the ePub3 predefined set
 nor was it explicitly declared in the package document's `<package>` tag
 through a `prefix` attribute.
 */
class UnknownPrefix : public std::domain_error
{
public:
    EPUB3_EXPORT UnknownPrefix(const string &str)    _NOEXCEPT  : std::domain_error(str.stl_str())  {}
    EPUB3_EXPORT UnknownPrefix(const char* str)      _NOEXCEPT  : std::domain_error(str)            {}
    virtual     ~UnknownPrefix()                     _NOEXCEPT                                      {}
};

class Property : public PointerType<Property>, public OwnedBy<PropertyHolder>, public XMLIdentifiable
#if EPUB_PLATFORM(WINRT)
	, public NativeBridge
#endif
{
public:
    ///
    /// A list of Extension objects.
    typedef shared_vector<PropertyExtension>    ExtensionList;
    
private:
    DCType          _type;
    string          _value;
    string          _language;
    ExtensionList   _extensions;
    IRI             _identifier;
    
                            Property()                              _DELETED_;
    
public:
                            Property(shared_ptr<PropertyHolder>& owner) : OwnedBy(owner), _type(DCType::Invalid), _value(), _language(), _extensions(), _identifier() {}
                            Property(const Property& o) : OwnedBy(o), XMLIdentifiable(o), _type(o._type), _value(o._value), _language(o._language), _extensions(o._extensions), _identifier(o._identifier) {}
                            Property(Property&& o) : OwnedBy(std::move(o)), XMLIdentifiable(std::move(o)), _type(o._type), _value(std::move(o._value)), _language(std::move(o._language)), _extensions(std::move(o._extensions)), _identifier(std::move(o._identifier)) {}
    virtual                 ~Property() {}
    
    EPUB3_EXPORT
    bool                    ParseMetaElement(shared_ptr<xml::Node> node);
    
    
    /// @{
    /// @name Basic Attributes
    
    ///
    /// The optimized type code for this metadata element.
    DCType                  Type()                  const           { return _type; }
    
    /**
     Sets the type of this property using a DCMES type code.
     
     This also sets the IRI to match.
     @param type The new DCMES type code.
     */
    EPUB3_EXPORT
    void                    SetDCType(DCType type);
    
    ///
    /// The canonical property IRI which identifies this item's type.
    const IRI&              PropertyIdentifier()   const            { return _identifier; }
    
    /**
     Sets the type of this property using an EPUB 3 identifier IRI.
     
     The DCType value is set in concert with the given IRI.
     @param iri The EPUB 3 property identifier.
     */
    EPUB3_EXPORT
    void                    SetPropertyIdentifier(const IRI& iri);
    
    ///
    /// The value of this metadata item.
    const string&           Value()                 const           { return _value; }
    
    /**
     Sets the value of this property.
     @param value The new value, as a string.
     */
    void                    SetValue(const string& value)           { _value = value; }
    
    ///
    /// The language in which the metadata value is rendered, if specified.
    const string&           Language()              const           { return _language; }
    
    /**
     Sets an explicit language for this property, to be serialized as an xml:lang attribute.
     @param lang A language code from [IETF BCP 47](http://tools.ietf.org/html/rfc4646).
     */
    void                    SetLanguage(const string& lang)         { _language = lang; }
    
    /// @}
    
    /// @{
    /// @name Localization
    
    /**
     Obtains the localized value, if one is provided.
     
     Calls LocalizedValue(const std::locale&) passing the result of CurrentLocale().
     @result A localized version of the Metadata item's value if available, or else
     returns the non-localized value.
     */
    const string&           LocalizedValue()        const {
        return LocalizedValue(CurrentLocale());
    }
    /**
     Obtains the value according to a given locale, if one is available.
     
     Note that localized variants of a value can be specified by adding extensions
     to this property. The property's primary value may be matched to a locale by
     specifying a language value using SetLanguage(), but alternatives can *only*
     be specified using PropertyExtensions.
     @param locale A locale to use when searching for localized values.
     @result A localized version of the property's value if available, or else
     returns the non-localized value as returned from the Value() method.
     */
    EPUB3_EXPORT
    const string&           LocalizedValue(const std::locale& locale)   const;
    
    /// @}
    
    /// @{
    /// @name Extensions
    
    ///
    /// Retrieve the list of all this Metadata item's extensions.
    const ExtensionList&        Extensions()        const           { return _extensions; }
    /**
     Retrieves an extension identified by a particular property IRI.
     @param property A property IRI.
     @result An Extension with the given property IRI, if one was found. Otherwise
     returns `nullptr`.
     */
    EPUB3_EXPORT
    const shared_ptr<PropertyExtension> ExtensionWithIdentifier(const IRI& ident) const;
    /**
     Retrieves all extensions with a given type (property IRI).
     @param property A property IRI.
     @result A list of all Extensions whose property matches `property`.
     */
    EPUB3_EXPORT
    const ExtensionList         AllExtensionsWithIdentifier(const IRI& ident) const;
    
    /**
     Adds a new PropertyExtension which refines this Property's value.
     @param ext The new extension.
     */
    void                        AddExtension(const std::shared_ptr<PropertyExtension>& ext) {
        _extensions.push_back(ext);
    }
    
    EPUB3_EXPORT
    bool                        HasExtensionWithIdentifier(const IRI& ident) const;
    
    /// @}
    
public:
    // Some things to help with debugging
    typedef std::vector<std::pair<string, string>>   ValueMap;
    
    EPUB3_EXPORT
    const ValueMap              DebugValues()       const;
    
};



EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__property_holder__) */
