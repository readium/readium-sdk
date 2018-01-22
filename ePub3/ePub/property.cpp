//
//  property.cpp
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
    // {DCType::Custom, "meta"}, ?
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
    {"http://www.idpf.org/vocab/rendition/#spread-auto", {"spread", "auto"}},
    {"http://www.idpf.org/vocab/rendition/#flow-auto", {"flow", "auto"}},
    {"http://www.idpf.org/vocab/rendition/#flow-paginated", {"flow", "paginated"}},
    {"http://www.idpf.org/vocab/rendition/#flow-scrolled-doc", {"flow", "scrolled-doc"}},
    {"http://www.idpf.org/vocab/rendition/#flow-scrolled-continuous", {"flow", "scrolled-continuous"}},
    {"http://idpf.org/epub/vocab/package/#page-spread-left", {"page-spread", "page-spread-left"}},
    {"http://idpf.org/epub/vocab/package/#page-spread-right", {"page-spread", "page-spread-right"}},
    {"http://www.idpf.org/vocab/rendition/#page-spread-center", {"page-spread", "page-spread-center"}}
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
static std::map<string, DCType> NameToIDMap(&__to_code_pairs[0], &__to_code_pairs[15]);

static __to_str_pair __to_str_pairs[15] = {
    //__to_code_pair(DCType::Custom, "meta"), ?
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
static std::map<DCType, string> IDToNameMap(&__to_str_pairs[0], &__to_str_pairs[14]);

static __to_rendition_pair __to_rendition_pairs[17] = {
    __to_rendition_pair("http://www.idpf.org/vocab/rendition/#orientation-landscape", RenditionPropertyBits("orientation", "landscape")),
    __to_rendition_pair("http://www.idpf.org/vocab/rendition/#orientation-portrait", RenditionPropertyBits("orientation", "portrait")),
    __to_rendition_pair("http://www.idpf.org/vocab/rendition/#orientation-auto", RenditionPropertyBits("orientation", "auto")),
    __to_rendition_pair("http://www.idpf.org/vocab/rendition/#layout-reflowable", RenditionPropertyBits("layout", "reflowable")),
    __to_rendition_pair("http://www.idpf.org/vocab/rendition/#layout-pre-paginated", RenditionPropertyBits("layout", "pre-paginated")),
    __to_rendition_pair("http://www.idpf.org/vocab/rendition/#spread-none", RenditionPropertyBits("spread", "none")),
    __to_rendition_pair("http://www.idpf.org/vocab/rendition/#spread-landscape", RenditionPropertyBits("spread", "landscape")),
    __to_rendition_pair("http://www.idpf.org/vocab/rendition/#spread-portrait", RenditionPropertyBits("spread", "portrait")),
    __to_rendition_pair("http://www.idpf.org/vocab/rendition/#spread-both", RenditionPropertyBits("spread", "both")),
    __to_rendition_pair("http://www.idpf.org/vocab/rendition/#spread-auto", RenditionPropertyBits("spread", "auto")),
    __to_rendition_pair("http://www.idpf.org/vocab/rendition/#flow-auto", RenditionPropertyBits("flow", "auto")),
    __to_rendition_pair("http://www.idpf.org/vocab/rendition/#flow-paginated", RenditionPropertyBits("flow", "paginated")),
    __to_rendition_pair("http://www.idpf.org/vocab/rendition/#flow-scrolled-doc", RenditionPropertyBits("flow", "scrolled-doc")),
    __to_rendition_pair("http://www.idpf.org/vocab/rendition/#flow-scrolled-continuous", RenditionPropertyBits("flow", "scrolled-continuous")),
    __to_rendition_pair("http://idpf.org/epub/vocab/package/#page-spread-left", RenditionPropertyBits("page-spread", "page-spread-left")),
    __to_rendition_pair("http://idpf.org/epub/vocab/package/#page-spread-right", RenditionPropertyBits("page-spread", "page-spread-right")),
    __to_rendition_pair("http://www.idpf.org/vocab/rendition/#page-spread-center", RenditionPropertyBits("page-spread", "page-spread-center"))
};

static std::map<string, RenditionPropertyBits> RenditionSplitPropertyLookup(&__to_rendition_pairs[0], &__to_rendition_pairs[16]);
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
        _identifier = IRI(string(DCMES_uri) + node->Name());
        _value = node->Content();
        _language = node->Language();
        SetXMLIdentifier(_getProp(node, "id"));
        
        return true;
    }
    else if ( node->Name() == MetaTagName )
    {
        string property = _getProp(node, "property");
        if ( property.empty() )
            return false;

        _type = DCType::Custom;
		_identifier = OwnedBy::Owner()->PropertyIRIFromString(property);
		_value = node->Content();
		_language = node->Language();
        SetXMLIdentifier(_getProp(node, "id"));

        return true;
    }
    else if ( ns != nullptr )
    {
        _type = DCType::Custom;
        _identifier = IRI(string(ns->URI()) + node->Name());
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

    auto iriString = iri.URIString();
    auto found = RenditionSplitPropertyLookup.find(iriString);
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
