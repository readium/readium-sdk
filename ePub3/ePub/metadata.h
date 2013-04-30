//
//  metadata.h
//  ePub3
//
//  Created by Jim Dovey on 2012-12-04.
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

#ifndef __ePub3__metadata__
#define __ePub3__metadata__

#include <ePub3/epub3.h>
#include <ePub3/utilities/iri.h>
#include <ePub3/utilities/utfstring.h>
#include <vector>
#include <map>
#include <string>
#include <libxml/tree.h>

EPUB3_BEGIN_NAMESPACE

class Package;

///
/// Enumeration for writing direction values.
enum class Direction
{
    Natural,                ///< Natural direction for the text's locale.
    LeftToRight,            ///< Characters flow left to right, lines from top to bottom.
    RightToLeft,            ///< Characters flow right to left, lines from top to bottom.
    VerticalLeftToRight,    ///< Characters flow top to bottom, lines from left to right.
    VerticalRightToLeft     ///< Characters flow top to bottom, lines from right to left.
};

/**
 The Metadata class encapsulates a single piece of metadata along with additional
 refinements of that metadata, as provided by the `refines` OPF metadata attribute.
 
 Each refinement is represented by the Metadata::Extension class, and any Metadata
 item can have many such extensions. The most common metadata types are those from
 the DCMES standard, and those are optimized using the Metadata::DCType enumeration.
 
 @remarks Each Metadata instance owns its Metadata::Extension instances, and is responsible
 for deleting them. The Package instance which owns a Metadata object is passed into
 the constructor to assist with property to IRI mappings, but that reference is not
 stored by either the Metadata object or its Metadata::Extension objects.
 
 @see http://www.idpf.org/epub/30/spec/epub30-publications.html#sec-meta-elem
 
 @ingroup epub-model
 */
class Metadata
{
public:
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
    /// An extension to an existing property, providing additional related metadata.
    class Extension
    {
    private:
        ///
        /// No default constructor.
                    Extension()                         _DELETED_;
        ///
        /// No copy constructor.
                    Extension(const Extension&)         _DELETED_;

    public:
        /**
         Create a new Extension.
         @param node The XML element node from which this item is to be created.
         @param owner The Package to which the metadata belongs; used for property
         IRI resolution.
         */
                    Extension(xmlNodePtr node, const Package* owner);
        ///
        /// C++11 move constructor.
                    Extension(Extension&&);
        virtual     ~Extension();
        
        ///
        /// Retrieves the extension's property IRI, declaring its type.
        const IRI&  Property()          const       { return _property; }
        ///
        /// Retrieves a scheme constant which determines how the Value() is interpreted.
        string      Scheme()            const;
        ///
        /// The extension's value.
        string      Value()             const;
        ///
        /// The XML identifier of3 this item (used to link items together).
        string      Identifier()        const;
        ///
        /// The language of the item (if applicable).
        string      Language()          const;
        
    protected:
        xmlNodePtr  _node;
        IRI         _property;
    };
    
    ///
    /// A list of Extension objects.
    typedef std::vector<Extension*>  ExtensionList;
    
private:
    ///
    /// No default constructor.
                    Metadata()                          _DELETED_;
    ///
    /// No copy constructor.
                    Metadata(const Metadata&)           _DELETED_;

public:
    /**
     Create a new Metadata item.
     @param node The XML element node describing this metadata item.
     @param owner The Package to which the metadata belongs; used for property IRI
     resolution.
     */
                    Metadata(xmlNodePtr node, const Package* owner);
    ///
    /// C++11 move constructor.
                    Metadata(Metadata&&);
    virtual         ~Metadata();
    
    /// @{
    /// @name Basic Attributes
    
    ///
    /// The optimized type code for this metadata element.
    DCType                  Type()          const           { return _type; }
    ///
    /// The canonical property IRI which identifies this item's type.
    const IRI&              Property()      const           { return _property; }
    ///
    /// The XML identifier for this item (used to associate Extensions).
    string                  Identifier()    const;
    ///
    /// The value of this metadata item.
    string                  Value()         const;
    ///
    /// The language in which the metadata value is rendered, if applicable.
    string                  Language()      const;
    
    /// @}
    
    /// @{
    /// @name Localization
    
    /**
     Obtains the localized value, if one is provided.
     
     Calls LocalizedValue(const std::locale&) passing the result of Package::Locale().
     @result A localized version of the Metadata item's value if available, or else
     returns the non-localized value.
     */
    string                  LocalizedValue()                            const;
    /**
     Obtains the value according to a given locale, if one is available.
     @param locale A locale to use when searching for localized values.
     @result A localized version of the Metadata item's value if available, or else
     returns the non-localized value.
     */
    string                  LocalizedValue(const std::locale& locale)   const;
    
    /// @}
    
    /// @{
    /// @name Extensions
    
    ///
    /// Retrieve the list of all this Metadata item's extensions.
    const ExtensionList&    Extensions()    const           { return _extensions; }
    /**
     Retrieves an extension identified by a particular property IRI.
     @param property A property IRI.
     @result An Extension with the given property IRI, if one was found. Otherwise
     returns `nullptr`.
     */
    const Extension*        ExtensionWithProperty(const IRI& property) const;
    /**
     Retrieves all extensions with a given type (property IRI).
     @param property A property IRI.
     @result A list of all Extensions whose property matches `property`.
     */
    const ExtensionList     AllExtensionsWithProperty(const IRI& property) const;
    
    /**
     Parses a new Extension from an XML element node which refines this Metadata item.
     @param node The XML element node defining additional associated metadata as a
     refinement of the base metadata item.
     @param owner The Package to which the metadata belongs; used for property IRI
     resolution.
     */
    void                    AddExtension(xmlNodePtr node, const Package* owner);
    
    /// @}
    
    /**
     Returns the canonical property IRI for a given DCType code.
     @param type The optimized type code.
     @result The canonical property IRI for the provided DCMES type.
     */
    static const IRI        IRIForDCType(DCType type);
    
public:
    // Some things to help with debugging
    typedef std::vector<std::pair<string, string>>   ValueMap;
    
    const ValueMap          DebugValues()   const;
    
protected:
    const Package*  _owner;
    DCType          _type;
    xmlNodePtr      _node;
    ExtensionList   _extensions;
    IRI             _property;
    
    bool            Decode(const Package* owner);
    
    static std::map<string, DCType> NameToIDMap;
    
};

EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__metadata__) */
