//
//  property_holder.cpp
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

#include "property_holder.h"
#include REGEX_INCLUDE

EPUB3_BEGIN_NAMESPACE

#if EPUB_COMPILER_SUPPORTS(CXX_INITIALIZER_LISTS)
const PropertyHolder::PropertyVocabularyMap PropertyHolder::ReservedVocabularies({
    { "", "http://idpf.org/epub/vocab/package/#" },
    //{ "rendition", "http://www.idpf.org/vocab/rendition/#" }, CANNOT BE INCLUDED, BECAUSE OF OPF package@prefix attribute definition, see Package::InstallPrefixesFromAttributeValue
    { "dcterms", "http://purl.org/dc/terms/" },
    { "marc", "http://id.loc.gov/vocabulary/" },
    { "media", "http://www.idpf.org/epub/vocab/overlays/#" },
    { "onix", "http://www.editeur.org/ONIX/book/codelists/current.html#" },
    { "xsd", "http://www.w3.org/2001/XMLSchema#" }
});
const std::map<const string, bool> PropertyHolder::CoreMediaTypes({
    // Image Types
    {"image/gif", true},                            // GIF Images
    {"image/jpeg", true},                           // JPEG Images
    {"image/png", true},                            // PNG Images
    {"image/svg+xml", true},                        // SVG Documents
    
    // Application Types
    {"application/xhtml+xml", true},                // XHTML Content Documents and the EPUB Navigation Document
    {"application/x-dtbncx+xml", true},             // The superceded NCX
    {"application/vnd.ms-opentype", true},          // OpenType fonts
    {"application/font-woff", true},                // WOFF fonts
    {"application/smil+xml", true},                 // EPUB Media Overlay documents
    {"application/pls+xml", true},                  // Text-to-Speech (TTS) Pronunciation lexicons
    
    // Audio Types
    {"audio/mpeg", true},                           // MP3 audio
    {"audio/mp4", true},                            // AAC LC audio using MP4 container
    
    // Text Types
    {"text/css", true},                             // EPUB Style Sheets
    {"text/javascript", true}                       // Scripts
});
#else
typedef PropertyHolder::PropertyVocabularyMap::value_type __vmap_t;
typedef std::pair<const string, bool> __mtype_t;
static const __vmap_t __vmap_values[6] = {
    __vmap_t("", "http://idpf.org/epub/vocab/package/#"),
    //__vmap_t("rendition", "http://www.idpf.org/vocab/rendition/#"), CANNOT BE INCLUDED, BECAUSE OF OPF package@prefix attribute definition, see Package::InstallPrefixesFromAttributeValue
    __vmap_t("dcterms", "http://purl.org/dc/terms/"),
    __vmap_t("marc", "http://id.loc.gov/vocabulary/"),
    __vmap_t("media", "http://www.idpf.org/epub/vocab/overlays/#"),
    __vmap_t("onix", "http://www.editeur.org/ONIX/book/codelists/current.html#"),
    __vmap_t("xsd", "http://www.w3.org/2001/XMLSchema#")
};
static const __mtype_t __mtype_values[14] = {
    // Image Types
    __mtype_t("image/gif", true),                            // GIF Images
    __mtype_t("image/jpeg", true),                           // JPEG Images
    __mtype_t("image/png", true),                            // PNG Images
    __mtype_t("image/svg+xml", true),                        // SVG Documents
    
    // Application Types
    __mtype_t("application/xhtml+xml", true),                // XHTML Content Documents and the EPUB Navigation Document
    __mtype_t("application/x-dtbncx+xml", true),             // The superceded NCX
    __mtype_t("application/vnd.ms-opentype", true),          // OpenType fonts
    __mtype_t("application/font-woff", true),                // WOFF fonts
    __mtype_t("application/smil+xml", true),                 // EPUB Media Overlay documents
    __mtype_t("application/pls+xml", true),                  // Text-to-Speech (TTS) Pronunciation lexicons
    
    // Audio Types
    __mtype_t("audio/mpeg", true),                           // MP3 audio
    __mtype_t("audio/mp4", true),                            // AAC LC audio using MP4 container
    
    // Text Types
    __mtype_t("text/css", true),                             // EPUB Style Sheets
    __mtype_t("text/javascript", true)                       // Scripts
};
const PropertyHolder::PropertyVocabularyMap PropertyHolder::ReservedVocabularies(&__vmap_values[0], &__vmap_values[5]);
const std::map<const string, bool> PropertyHolder::CoreMediaTypes(&__mtype_values[0], &__mtype_values[13]);
#endif

PropertyHolder& PropertyHolder::operator=(const PropertyHolder& o)
{
    _parent = o._parent;
    _properties = o._properties;
    _vocabularyLookup = o._vocabularyLookup;
    return *this;
}
PropertyHolder& PropertyHolder::operator=(PropertyHolder&& o)
{
    _parent = std::move(o._parent);
    _properties = std::move(o._properties);
    _vocabularyLookup = std::move(o._vocabularyLookup);
    return *this;
}
void PropertyHolder::AppendProperties(const PropertyHolder& o, shared_ptr<PropertyHolder> sharedMe)
{
    for ( auto& prop : o._properties )
    {
        prop->SetOwner(sharedMe);
    }
    
    _properties.insert(_properties.end(), o._properties.begin(), o._properties.end());
}
void PropertyHolder::AppendProperties(PropertyHolder&& o, shared_ptr<PropertyHolder> sharedMe)
{
    for ( auto& i : o._properties )
    {
        i->SetOwner(sharedMe);
        _properties.push_back(std::move(i));
    }
}
void PropertyHolder::RemoveProperty(const IRI& iri)
{
    for ( auto pos = _properties.begin(), end = _properties.end(); pos != end; ++pos )
    {
        if ( (*pos)->PropertyIdentifier() == iri )
        {
            _properties.erase(pos);
            break;
        }
    }
}
void PropertyHolder::RemoveProperty(const string& reference, const string& prefix)
{
    IRI iri = MakePropertyIRI(reference, prefix);
    if ( iri.IsEmpty() )
        return;
    RemoveProperty(iri);
}
void PropertyHolder::ErasePropertyAt(size_type idx)
{
    if ( idx > _properties.size() )
        throw std::out_of_range("ErasePropertyAt: Index out of range");
    
    auto pos = _properties.begin();
    pos += idx;
    _properties.erase(pos);
}
bool PropertyHolder::ContainsProperty(DCType type, bool lookupParents) const
{
    IRI iri = IRIForDCType(type);
    return ContainsProperty(iri, lookupParents);
}
bool PropertyHolder::ContainsProperty(const IRI& iri, bool lookupParents) const
{
    for ( auto &i : _properties )
    {
        if ( i->PropertyIdentifier() == iri )
            return true;
    }

    if (lookupParents)
    {
        auto parent = _parent.lock();
        if ( parent )
            return parent->ContainsProperty(iri, lookupParents);
    }
    
    return false;
}
bool PropertyHolder::ContainsProperty(const string& reference, const string& prefix, bool lookupParents) const
{
    IRI iri = MakePropertyIRI(reference, prefix);
    if ( iri.IsEmpty() )
        return false;
    return ContainsProperty(iri, lookupParents);
}
bool PropertyHolder::ContainsProperty(DCType type) const
{
	return ContainsProperty(type, true);
}

bool PropertyHolder::ContainsProperty(const IRI& iri) const
{
	return ContainsProperty(iri, true);
}

bool PropertyHolder::ContainsProperty(const string& reference, const string& prefix) const
{
	return ContainsProperty(reference, prefix, true);
}

const PropertyHolder::PropertyList PropertyHolder::PropertiesMatching(DCType type, bool lookupParents) const
{
    IRI iri = IRIForDCType(type);
    return PropertiesMatching(iri, lookupParents);
}
const PropertyHolder::PropertyList PropertyHolder::PropertiesMatching(const IRI& iri, bool lookupParents) const
{
    PropertyList output;
    BuildPropertyList(output, iri);

    if (lookupParents)
    {
        auto parent = _parent.lock();
        if ( parent )
        {
            //parent->BuildPropertyList(output, iri);

            PropertyHolder::PropertyList pList = parent->PropertiesMatching(iri, lookupParents);
            output.insert(output.end(), pList.begin(), pList.end());
        }
    }

    return output;
}
const PropertyHolder::PropertyList PropertyHolder::PropertiesMatching(const string& reference, const string& prefix, bool lookupParents) const
{
    IRI iri = MakePropertyIRI(reference, prefix);
    if ( iri.IsEmpty() )
        return PropertyList();
    return PropertiesMatching(iri, lookupParents);
}


const PropertyHolder::PropertyList PropertyHolder::PropertiesMatching(DCType type) const
{
	return PropertiesMatching(type, true);
}

const PropertyHolder::PropertyList PropertyHolder::PropertiesMatching(const IRI& iri) const
{
	return PropertiesMatching(iri, true);
}

const PropertyHolder::PropertyList PropertyHolder::PropertiesMatching(const string& reference, const string& prefix) const
{
	return PropertiesMatching(reference, prefix, true);
}

PropertyPtr PropertyHolder::PropertyMatching(DCType type, bool lookupParents) const
{
    IRI iri = IRIForDCType(type);
    return PropertyMatching(iri, lookupParents);
}
PropertyPtr PropertyHolder::PropertyMatching(const IRI& iri, bool lookupParents) const
{
    // DEBUG
    auto iriString = iri.URIString();

    for ( auto &i : _properties )
    {
        if ( i->PropertyIdentifier() == iri )
            return i;
    }

    if (lookupParents)
    {
        auto parent = _parent.lock();
        if ( parent )
            return parent->PropertyMatching(iri, lookupParents);
    }

    return nullptr;
}
PropertyPtr PropertyHolder::PropertyMatching(const string& reference, const string& prefix, bool lookupParents) const
{
    IRI iri = MakePropertyIRI(reference, prefix);
    if ( iri.IsEmpty() )
        return nullptr;
    return PropertyMatching(iri, lookupParents);
}


PropertyPtr PropertyHolder::PropertyMatching(DCType type) const
{
	return PropertyMatching(type, true);
}

PropertyPtr PropertyHolder::PropertyMatching(const IRI& iri) const
{
	return PropertyMatching(iri, true);
}

PropertyPtr PropertyHolder::PropertyMatching(const string& reference, const string& prefix) const
{
	return PropertyMatching(reference, prefix, true);
}

void PropertyHolder::RegisterPrefixIRIStem(const string &prefix, const string &iriStem)
{
    auto found = _vocabularyLookup.find(prefix);
    if ( found == _vocabularyLookup.end() )
    {
        _vocabularyLookup[prefix] = iriStem;
    }
}
IRI PropertyHolder::MakePropertyIRI(const string &reference, const string& prefix) const
{
    auto found = _vocabularyLookup.find(prefix);
    if ( found == _vocabularyLookup.end() )
    {
        auto parent = _parent.lock();
        if ( parent )
            return parent->MakePropertyIRI(reference, prefix);
        
        return IRI();
    }
    return IRI(found->second + reference);
}
IRI PropertyHolder::PropertyIRIFromString(const string &attrValue) const
{
    static REGEX_NS::regex re("^(?:(.+?):)?(.+)$");
    REGEX_NS::smatch pieces;
    if ( REGEX_NS::regex_match(attrValue.stl_str(), pieces, re) == false )
        throw std::invalid_argument(_Str("Attribute '", attrValue, "' doesn't look like a property name to me"));
    
    // there are two captures, at indices 1 and 2
    return MakePropertyIRI(pieces.str(2), pieces.str(1));
}
void PropertyHolder::BuildPropertyList(PropertyList& output, const IRI& iri) const
{
    if ( iri.IsEmpty() )
        return;
    
    for ( auto& i : _properties )
    {
        if ( i->PropertyIdentifier() == iri || i->HasExtensionWithIdentifier(iri) )
            output.push_back(i);
    }
}

EPUB3_END_NAMESPACE
