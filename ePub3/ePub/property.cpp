//
//  property.cpp
//  ePub3
//
//  Created by Jim Dovey on 2013-05-03.
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

#include <ePub3/property.h>
#include <ePub3/utilities/iri.h>
#include <ePub3/property_holder.h>

EPUB3_BEGIN_NAMESPACE
#if EPUB_USE(LIBXML2)
const xmlChar * DCMES_uri = (const xmlChar*)"http://purl.org/dc/elements/1.1/";
const xmlChar * MetaTagName = (const xmlChar*)"meta";
#elif EPUB_USE(WIN_XML)
const TCHAR* DCMES_uri = TEXT("http://purl.org/dc/elements/1.1/");
const TCHAR* MetaTagName = TEXT("meta");
#endif

string __lang_from_locale(const std::locale& loc)
{
    std::string lname = loc.name();
    
    // handle custom locales, i.e. "LC_CTYPE=en_US.UTF-8;LC_NUMERIC=C;LC_TIME=C;LC_COLLATE=C;LC_MONETARY=C;LC_MESSAGES=C;LC_PAPER=C;LC_NAME=C;LC_ADDRESS=C;LC_TELEPHONE=C;LC_MEASUREMENT=C;LC_IDENTIFICATION=C"
    auto __p = lname.find("LC_CTYPE=");
    if ( __p != std::string::npos )
    {
        auto __e = lname.find(";", __p);
        if ( __e == std::string::npos )
            __e = lname.size();
        lname = lname.substr(__p, __e);
    }
    
    // now we have something like "en_US.UTF-8" or "en_US". Strip the ".UTF-8" part, if any
    __p = lname.find(".");
    if ( __p != std::string::npos )
        lname = lname.substr(0, __p);
    
    return string(lname);
}

typedef std::pair<string, string> RenditionPropertyBits;

#if EPUB_COMPILER_SUPPORTS(CXX_INITIALIZER_LISTS)
// C++11 FTW
static std::map<string, DCType> NameToIDMap = {
    {"meta", DCType::Custom},
    {"identifier", DCType::Identifier},
    {"title", DCType::Title},
    {"language", DCType::Language},
    {"contributor", DCType::Contributor},
    {"coverage", DCType::Coverage},
    {"creator", DCType::Creator},
    {"date", DCType::Date},
    {"description", DCType::Description},
    {"format", DCType::Format},
    {"publisher", DCType::Publisher},
    {"relation", DCType::Relation},
    {"rights", DCType::Rights},
    {"source", DCType::Source},
    {"subject", DCType::Subject},
    {"type", DCType::Type}
};

static std::map<DCType, string> IDToNameMap = {
    {DCType::Identifier, "identifier"},
    {DCType::Title, "title"},
    {DCType::Language, "language"},
    {DCType::Contributor, "contributor"},
    {DCType::Coverage, "coverage"},
    {DCType::Creator, "creator"},
    {DCType::Date, "date"},
    {DCType::Description, "description"},
    {DCType::Format, "format"},
    {DCType::Publisher, "publisher"},
    {DCType::Relation, "relation"},
    {DCType::Rights, "rights"},
    {DCType::Source, "source"},
    {DCType::Subject, "subject"},
    {DCType::Type, "type"}
};

static const std::map<string, RenditionPropertyBits> RenditionSplitPropertyLookup = {
    {"http://www.idpf.org/vocab/rendition/#orientation-landscape", {"orientation", "landscape"}},
    {"http://www.idpf.org/vocab/rendition/#orientation-portrait", {"orientation", "portrait"}},
    {"http://www.idpf.org/vocab/rendition/#orientation-auto", {"orientation", "auto"}},
    {"http://www.idpf.org/vocab/rendition/#layout-reflowable", {"layout", "reflowable"}},
    {"http://www.idpf.org/vocab/rendition/#layout-pre-paginated", {"layout", "pre-paginated"}},
    {"http://www.idpf.org/vocab/rendition/#spread-none", {"spread", "none"}},
    {"http://www.idpf.org/vocab/rendition/#spread-landscape", {"spread", "landscape"}},
    {"http://www.idpf.org/vocab/rendition/#spread-portrait", {"spread", "portrait"}},
    {"http://www.idpf.org/vocab/rendition/#spread-both", {"spread", "both"}},
    {"http://www.idpf.org/vocab/rendition/#spread-auto", {"spread", "auto"}}
};
#else
typedef std::pair<string, DCType> __to_code_pair;
typedef std::pair<DCType, string> __to_str_pair;
typedef std::pair<string, RenditionPropertyBits> __to_rendition_pair;

static __to_code_pair __to_code_pairs[16] = {
    __to_code_pair("meta", DCType::Custom),
    __to_code_pair("identifier", DCType::Identifier),
    __to_code_pair("title", DCType::Title),
    __to_code_pair("language", DCType::Language),
    __to_code_pair("contributor", DCType::Contributor),
    __to_code_pair("coverage", DCType::Coverage),
    __to_code_pair("creator", DCType::Creator),
    __to_code_pair("date", DCType::Date),
    __to_code_pair("description", DCType::Description),
    __to_code_pair("format", DCType::Format),
    __to_code_pair("publisher", DCType::Publisher),
    __to_code_pair("relation", DCType::Relation),
    __to_code_pair("rights", DCType::Rights),
    __to_code_pair("source", DCType::Source),
    __to_code_pair("subject", DCType::Subject),
    __to_code_pair("type", DCType::Type)
};
static std::map<string, DCType> NameToIDMap(&__to_code_pairs[0], &__to_code_pairs[16]);

static __to_str_pair __to_str_pairs[16] = {
    __to_str_pair(DCType::Identifier, "identifier"),
    __to_str_pair(DCType::Title, "title"),
    __to_str_pair(DCType::Language, "language"),
    __to_str_pair(DCType::Contributor, "contributor"),
    __to_str_pair(DCType::Coverage, "coverage"),
    __to_str_pair(DCType::Creator, "creator"),
    __to_str_pair(DCType::Date, "date"),
    __to_str_pair(DCType::Description, "description"),
    __to_str_pair(DCType::Format, "format"),
    __to_str_pair(DCType::Publisher, "publisher"),
    __to_str_pair(DCType::Relation, "relation"),
    __to_str_pair(DCType::Rights, "rights"),
    __to_str_pair(DCType::Source, "source"),
    __to_str_pair(DCType::Subject, "subject"),
    __to_str_pair(DCType::Type, "type")
};
static std::map<DCType, string> IDToNameMap(&__to_str_pairs[0], &__to_str_pairs[16]);

static __to_rendition_pair __to_rendition_pairs[10] = {
    __to_rendition_pair("http://www.idpf.org/vocab/rendition/#orientation-landscape", RenditionPropertyBits("orientation", "landscape")),
    __to_rendition_pair("http://www.idpf.org/vocab/rendition/#orientation-portrait", RenditionPropertyBits("orientation", "portrait")),
    __to_rendition_pair("http://www.idpf.org/vocab/rendition/#orientation-auto", RenditionPropertyBits("orientation", "auto")),
    __to_rendition_pair("http://www.idpf.org/vocab/rendition/#layout-reflowable", RenditionPropertyBits("layout", "reflowable")),
    __to_rendition_pair("http://www.idpf.org/vocab/rendition/#layout-pre-paginated", RenditionPropertyBits("layout", "pre-paginated")),
    __to_rendition_pair("http://www.idpf.org/vocab/rendition/#spread-none", RenditionPropertyBits("spread", "none")),
    __to_rendition_pair("http://www.idpf.org/vocab/rendition/#spread-landscape", RenditionPropertyBits("spread", "landscape")),
    __to_rendition_pair("http://www.idpf.org/vocab/rendition/#spread-portrait", RenditionPropertyBits("spread", "portrait")),
    __to_rendition_pair("http://www.idpf.org/vocab/rendition/#spread-both", RenditionPropertyBits("spread", "both")),
    __to_rendition_pair("http://www.idpf.org/vocab/rendition/#spread-auto", RenditionPropertyBits("spread", "auto"))
};
static std::map<string, RenditionPropertyBits> RenditionSplitPropertyLookup(&__to_rendition_pairs[0], &__to_rendition_pairs[9]);
#endif

EPUB3_EXPORT
const IRI IRIForDCType(DCType type)
{
    auto found = IDToNameMap.find(type);
    if ( found == IDToNameMap.end() )
        return IRI();
    return IRI(DCMES_uri + found->second);
}

EPUB3_EXPORT
DCType DCTypeFromIRI(const IRI& iri)
{
    if ( iri.IsEmpty() )
        return DCType::Invalid;
    auto found = NameToIDMap.find(iri.Fragment());
    if ( found == NameToIDMap.end() )
        return DCType::Custom;
    return found->second;
}

bool Property::ParseMetaElement(shared_ptr<xml::Node> node)
{
    if ( !bool(node) )
        return false;
    
    if ( !node->IsElementNode() )
        return false;
    
    auto ns = node->Namespace();
    if ( ns != nullptr && ns->URI() == DCMES_uri )
    {
        auto found = NameToIDMap.find(node->Name());
        if ( found == NameToIDMap.end() )
            return false;
        
        _type = found->second;
        
        // special property IRI, not actually in the spec, but useful for comparisons and printouts
        _identifier = IRI(string(DCMES_uri) + node->Name());
        _value = node->Content();
        _language = node->Language();
        SetXMLIdentifier(_getProp(node, "id"));
        
        return true;
    }
    else if ( node->Name() == MetaTagName )
    {
        _type = DCType::Custom;
        string property = _getProp(node, "property");
        if ( property.empty() )
            return false;
        
		_identifier = OwnedBy::Owner()->PropertyIRIFromString(property);
		_value = node->Content();
		_language = node->Language();
        SetXMLIdentifier(_getProp(node, "id"));
        return true;
    }
    
    return false;
}
void Property::SetDCType(DCType type)
{
    _type = type;
    if ( type == DCType::Invalid )
    {
        _identifier = IRI();
    }
    else if ( type != DCType::Custom )
    {
        _identifier = IRIForDCType(type);
    }
}
void Property::SetPropertyIdentifier(const IRI& iri)
{
    // HACKINESS ALERT !!
    // *Some* of the properties in the rendition namespace are boolean values whose names contain hyphens.
    // *Some* others in that namespace are simply-named with a value; the name and the value are separated by a hyphen.
    // Le sigh...
    _identifier = iri;
    _type = DCTypeFromIRI(iri);
    
    auto found = RenditionSplitPropertyLookup.find(iri.URIString());
    if ( found != RenditionSplitPropertyLookup.end() )
    {
        _identifier.SetFragment(found->second.first);
        SetValue(found->second.second);
    }
}
const string& Property::LocalizedValue(const std::locale& locale) const
{
    string llang = __lang_from_locale(locale);
    
    // does this match the main value?
    if ( llang.find(_language) == 0 || _language.find(llang) == 0 )
    {
        // main value is explicitly in this language
        return _value;
    }
    
    // alternate-script extensions
    ExtensionList scripts = AllExtensionsWithIdentifier(OwnedBy::Owner()->PropertyIRIFromString("alternate-script"));
    if ( scripts.empty() )
        return _value;      // no specializations for different languages/scripts
    
    for ( auto script : scripts )
    {
        if ( llang.find(script->Language()) == 0 || script->Language().find(llang) == 0 )
        {
            // they match
            return script->Value();
        }
    }
    
    // no values in requested localization, so use default
    return _value;
}
const shared_ptr<PropertyExtension> Property::ExtensionWithIdentifier(const IRI& ident) const
{
    for ( auto extension : _extensions )
    {
        if ( extension->PropertyIdentifier() == ident )
            return extension;
    }
    return nullptr;
}
const Property::ExtensionList Property::AllExtensionsWithIdentifier(const IRI& ident) const
{
    ExtensionList output;
    for ( auto extension : _extensions )
    {
        if ( extension->PropertyIdentifier() == ident )
            output.push_back(extension);
    }
    return output;
}
bool Property::HasExtensionWithIdentifier(const IRI& ident) const
{
    for ( auto& ext : _extensions )
    {
        if ( ext->PropertyIdentifier() == ident )
            return true;
    }
    return false;
}
const Property::ValueMap Property::DebugValues() const
{
    ValueMap values;
    values.emplace_back(_identifier.IRIString(), _value);
    for ( auto extension : _extensions )
    {
        values.emplace_back(_Str("  ", extension->PropertyIdentifier().IRIString()), extension->Value());
    }
    return values;
}

EPUB3_END_NAMESPACE
