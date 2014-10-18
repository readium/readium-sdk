//
//  property_holder.h
//  ePub3
//
//  Created by Jim Dovey on 2013-05-06.
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

#ifndef __ePub3__property_holder__
#define __ePub3__property_holder__

#include <ePub3/utilities/basic.h>
#include <ePub3/utilities/owned_by.h>
#include <ePub3/property.h>

EPUB3_BEGIN_NAMESPACE

class PropertyHolder
{
public:
    ///
    /// An array of Metadata items, in document order.
    typedef shared_vector<Property>             PropertyList;
    
    typedef std::shared_ptr<Property>           value_type;
    typedef PropertyList::size_type             size_type;

    ///
    /// An array of concrete property IRIs.
    typedef std::vector<IRI>                    PropertyIRIList;
    
    ///
    /// A lookup table for property vocabulary IRI stubs, indexed by prefix.
    typedef std::map<string, string>            PropertyVocabularyMap;
    
    ///
    /// The list of Core Media Types from [OPF 3.0 ??5.1](http://idpf.org/epub/30/spec/epub30-publications.html#sec-core-media-types).
    static const std::map<const string, bool>   CoreMediaTypes;
    
    static const PropertyVocabularyMap          ReservedVocabularies;
    static const std::map<DCType, const IRI>    DCTypeIRIs;
    
private:
    weak_ptr<PropertyHolder>                    _parent;            ///< Parent object used to 'inherit' properties.
    PropertyList                                _properties;        ///< All properties, in document order.
    PropertyVocabularyMap                       _vocabularyLookup;  ///< A lookup table for property-prefix->IRI-stem mappings.
    
public:
                        PropertyHolder() : _parent(), _properties(), _vocabularyLookup(ReservedVocabularies) {}
    template <class _Parent>
                        PropertyHolder(const shared_ptr<_Parent>& parent) : _parent(std::dynamic_pointer_cast<PropertyHolder>(parent)), _properties(), _vocabularyLookup(ReservedVocabularies) {}
                        PropertyHolder(const PropertyHolder& o) : _parent(o._parent), _properties(o._properties), _vocabularyLookup(o._vocabularyLookup) {}
                        PropertyHolder(PropertyHolder&& o) : _parent(std::move(o._parent)), _properties(std::move(o._properties)), _vocabularyLookup(std::move(o._vocabularyLookup)) {}
    virtual             ~PropertyHolder() {}
    
    virtual PropertyHolder& operator=(const PropertyHolder& o);
    virtual PropertyHolder& operator=(PropertyHolder&& o);
    
    virtual size_type   NumberOfProperties() const                      { return _properties.size(); }
    
    
    virtual void        AddProperty(const shared_ptr<Property>& prop)   { _properties.push_back(prop); }
    virtual void        AddProperty(const shared_ptr<Property>&& prop)  { _properties.push_back(std::move(prop)); }
    virtual void        AddProperty(Property* prop)                     { _properties.emplace_back(prop); }
    
    EPUB3_EXPORT
    virtual void        AppendProperties(const PropertyHolder& properties, shared_ptr<PropertyHolder> sharedMe);
    EPUB3_EXPORT
    virtual void        AppendProperties(PropertyHolder&& properties, shared_ptr<PropertyHolder> sharedMe);
    EPUB3_EXPORT
    virtual void        RemoveProperty(const IRI& iri);
    EPUB3_EXPORT
    virtual void        RemoveProperty(const string& reference, const string& prefix="");
    
    virtual value_type  PropertyAt(size_type idx) const                 { return _properties.at(idx); }
    EPUB3_EXPORT
    virtual void        ErasePropertyAt(size_type idx);
    
    EPUB3_EXPORT
    virtual bool        ContainsProperty(DCType type, bool lookupParents) const;
    EPUB3_EXPORT
    virtual bool        ContainsProperty(const IRI& iri, bool lookupParents) const;
    EPUB3_EXPORT
    virtual bool        ContainsProperty(const string& reference, const string& prefix, bool lookupParents) const;
    
    EPUB3_EXPORT
    virtual bool        ContainsProperty(DCType type) const;
    EPUB3_EXPORT
    virtual bool        ContainsProperty(const IRI& iri) const;
    EPUB3_EXPORT
    virtual bool        ContainsProperty(const string& reference, const string& prefix="") const;
    
    EPUB3_EXPORT
    const PropertyList  PropertiesMatching(DCType type, bool lookupParents) const;
    EPUB3_EXPORT
    const PropertyList  PropertiesMatching(const IRI& iri, bool lookupParents) const;
    EPUB3_EXPORT
    const PropertyList  PropertiesMatching(const string& reference, const string& prefix, bool lookupParents) const;
    
    EPUB3_EXPORT
    const PropertyList  PropertiesMatching(DCType type) const;
    EPUB3_EXPORT
    const PropertyList  PropertiesMatching(const IRI& iri) const;
    EPUB3_EXPORT
    const PropertyList  PropertiesMatching(const string& reference, const string& prefix="") const;
    
    EPUB3_EXPORT
    PropertyPtr         PropertyMatching(DCType type, bool lookupParents) const;
    EPUB3_EXPORT
    PropertyPtr         PropertyMatching(const IRI& iri, bool lookupParents) const;
    EPUB3_EXPORT
    PropertyPtr         PropertyMatching(const string& reference, const string& prefix, bool lookupParents) const;
    
    EPUB3_EXPORT
    PropertyPtr         PropertyMatching(DCType type) const;
    EPUB3_EXPORT
    PropertyPtr         PropertyMatching(const IRI& iri) const;
    EPUB3_EXPORT
    PropertyPtr         PropertyMatching(const string& reference, const string& prefix="") const;
    
    template <class _Function>
    inline FORCE_INLINE
    _Function           ForEachProperty(_Function __f) const
    {
        return std::for_each(_properties.begin(), _properties.end(), __f);
    }
    
    EPUB3_EXPORT
    void                RegisterPrefixIRIStem(const string& prefix, const string& iriStem);
    EPUB3_EXPORT
    IRI                 MakePropertyIRI(const string& reference, const string& prefix=string::EmptyString) const;
    EPUB3_EXPORT
    IRI                 PropertyIRIFromString(const string& value) const;
    
protected:
    void                BuildPropertyList(PropertyList& output, const IRI& iri) const;
    
};

EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__property_holder__) */
