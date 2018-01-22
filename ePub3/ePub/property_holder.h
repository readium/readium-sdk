//
//  property_holder.h
//  ePub3
//
//  Created by Jim Dovey on 2013-05-06.
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
